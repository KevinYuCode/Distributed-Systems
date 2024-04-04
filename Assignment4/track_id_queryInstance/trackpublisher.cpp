/*
 * Track publisher eample
 * two aircaft flying towards the airport
 * publish 10 samples each
 */

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "track.hpp"

using namespace org::eclipse::cyclonedds;
using namespace std;

int main()
{
    try
    {
        cout << "*** trackpublisher starting" << endl;

        // create a participant in domain 1
        dds::domain::DomainParticipant participant(1);

        // topic with type Radar::Track, name TRACK_DATA and default QoS
        dds::topic::Topic<Radar::Track> topic(participant, "TRACK_DATA");

        // add a publisher to te particpant (could go before the topic)
        dds::pub::Publisher publisher(participant);

        // add a DataWriter for the topic to the publisher
        dds::pub::DataWriter<Radar::Track> writer(publisher, topic);

        // The two aircraft from the slide
        Radar::Track plane1(1, 10034, 22.5, 23.5, 20);
        Radar::Track plane2(2, 10036, 23, 24, 40);

        // current_count() of matched_status() tells us how many subscribers
        // are subscribed to our topic with compatiable QoS
        std::cout << "*** trackpublisher Waiting for subscriber." << std::endl;
        dds::core::cond::WaitSet waitset;
        dds::core::cond::StatusCondition wsc(writer);
        wsc.enabled_statuses(dds::core::status::StatusMask::publication_matched());
        waitset.attach_condition(wsc);
        try
        {
            waitset.wait(dds::core::Duration::infinite()); // wait forever
        }
        catch (const dds::core::TimeoutError &)
        {
            cerr << "Timeout" << endl;
            exit(1);
        }
        catch (const dds::core::Exception &e)
        {
            cerr << "psr_publisher  error: \"" << e.what() << "\"." << endl;
            exit(1);
        }

        // 10 samples for each plane
        int increment = -2;
        for (int i = 0; i < 10; i++)
        {
            cout << "** trackpublisher write plane." << endl;
            // publish the radar returns for the two planes 30 milliseconds apart
            writer.write(plane1);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            writer.write(plane2);

            std::this_thread::sleep_for(std::chrono::seconds(3));

            // the planes fly 2 NM towards the radar, and the timestamp changes by 20
            plane1.distance(plane1.distance() + increment);
            if (plane1.distance() < 0)
            {
                increment *= -1;
            }
            plane1.timestamp(plane1.timestamp() + 20);
            plane2.distance(plane2.distance() + increment);
            plane2.timestamp(plane2.timestamp() + 20);
        }

        // unless otherwise specified, deleting a writer will dispose
        // of all the messages. Wait a short period of time to ensure that
        // any subscribers have recieved the messages
        cout << "*** trackpublisher wait 1 second for all samples to be read" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    catch (const dds::core::Exception &e)
    {
        cerr << "*** trackpublisher Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    std::cout << "*** trackpublisher finished." << std::endl;

    return EXIT_SUCCESS;
}
