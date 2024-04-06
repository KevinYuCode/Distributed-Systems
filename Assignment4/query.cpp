/*
 * Track Subscriber
 * - waits until interrupted with Ctrl C
 */

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"
#include "controlTransfer.hpp"
#include "statekey.hpp"

using namespace org::eclipse::cyclonedds;
using namespace std;

int main(int argc, char *argv[])
{

    try
    {
        cout << "*** track_subscriber started " << endl;

        // participant in Domain 1
        dds::domain::DomainParticipant participant(0);

        // participant in Domain 1
        dds::topic::Topic<State::Update> topic(participant, "Flights");

        // add a subscriber to the participant (could be done before te topic)
        dds::sub::Subscriber subscriber(participant);

        // add a track specific DataReader to the subscriber with custom DataReader QoS
        dds::sub::DataReader<State::Update> reader(subscriber, topic);

        std::cout << "*** tracksubscriber Waiting for publisher." << std::endl;
        dds::core::cond::WaitSet waitset;
        dds::core::cond::StatusCondition rsc(reader);
        rsc.enabled_statuses(dds::core::status::StatusMask::data_available());
        waitset.attach_condition(rsc);
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
            cerr << "tracksubscirber  error: \"" << e.what() << "\"." << endl;
            exit(1);
        }

        while (1)
        {
            // prompt user for id number
            cout << "Enter the flight ID: "; /*<< flush;*/
            string flightID;
            cin >> flightID;
            cout << "You typed: " << flightID << endl;
            // string flightID
            // cout << "As integer: " << trackid << endl;
            //  Create an instance with the key
            State::Update planeQuery(
                "abc", // icao24
                0,     // timestamp.
                0.0,   // lat
                0.0,   // lon
                0.0,   // vel
                0.0,   // heading
                0.0,   // vertrat
                flightID, // calsign
                7400,  // squawk
                0.0,   // baroaltitude
                0.0);  // geoaltigude
            // search the reader for the instance of the key
            // each instance has it's own queue because of keys
            auto handle = reader.lookup_instance(planeQuery);
            if (handle.is_nil())
            {
                // no such instance hs been received
                cout << "no such flight" << endl;
            }
            else
            {
                // retrieve the sample associated with the key.
                dds::sub::LoanedSamples<State::Update> samples;
                samples = reader.select().instance(handle).read();
                cout << "Samples length is " << samples.length() << endl;
                if (samples.length() > 0)
                {
                    dds::sub::LoanedSamples<State::Update>::const_iterator first = samples.begin();
                    cout << "timestamp  : \"" << first->data().timestamp() << endl;
                    cout << "icao24  : \"" << first->data().icao24() << endl;
                    cout << "baroaltitude : \"" << first->data().baroaltitude() * 3.281 << "\"" << endl;
                    cout << "latitude : \"" << first->data().lat() << "\"" << endl;
                    cout << "longitude : \"" << first->data().lon() << "\"" << endl;
                    cout << "vert rate : \"" << first->data().vertrate() << "\"" << endl;
                }
            }
        }
    }
    catch (const dds::core::Exception &e)
    {
        cerr << "*** tracksubscriber DDS exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        cerr << "*** tracksubscriber C++ exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "*** tracksubscriber Done." << endl;

    return EXIT_SUCCESS;
}
