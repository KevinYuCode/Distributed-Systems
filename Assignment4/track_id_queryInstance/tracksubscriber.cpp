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

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "track.hpp"

using namespace org::eclipse::cyclonedds;
using namespace std;

int main(int argc, char * argv[]) {

    

    
    try {
        cout << "*** track_subscriber started " << endl;

        // participant in Domain 1
        dds::domain::DomainParticipant participant(1);

        // participant in Domain 1
        dds::topic::Topic<Radar::Track> topic(participant, "TRACK_DATA");

        // add a subscriber to the participant (could be done before te topic)
	dds::sub::Subscriber subscriber(participant);

        // add a track specific DataReader to the subscriber with custom DataReader QoS
        dds::sub::DataReader<Radar::Track> reader(subscriber, topic);


        std::cout << "*** tracksubscriber Waiting for publisher." << std::endl;
        dds::core::cond::WaitSet waitset;
        dds::core::cond::StatusCondition rsc(reader);
        rsc.enabled_statuses(dds::core::status::StatusMask::subscription_matched());
        waitset.attach_condition(rsc);
        try{
            waitset.wait(dds::core::Duration::infinite()); // wait forever
        }
        catch (const dds::core::TimeoutError &){
            cerr << "Timeout" << endl;
            exit(1);
        }
        catch (const dds::core::Exception &e){
            cerr << "tracksubscirber  error: \"" << e.what() << "\"." << endl;
            exit(1);
        }

        while (1) {
            // prompt user for id number
            cout << "Track id: " << flush;
            string response;
            cin >> response;
            //cout << "You typed: " << response << endl;
            int trackid = stoi(response);
            //cout << "As integer: " << trackid << endl;
            // Create an instance with the key
            Radar::Track planeq(trackid,0, 0, 0, 0);
            // search the reader for the instance of the key
            // each instance has it's own queue because of keys
            auto handle = reader.lookup_instance(planeq);
            if (handle.is_nil()){
                // no such instance hs been received
                cout << "no such track" << endl;
            } else {
                // retrieve the sample associated with the key.
                dds::sub::LoanedSamples<Radar::Track> samples;
                samples = reader.select().instance(handle).read();
                cout << "Samples legnth is " << samples.length() << endl;
                if (samples.length()> 0){
                    dds::sub::LoanedSamples<Radar::Track>::const_iterator first = samples.begin();
                    cout << "timestamp  : " << first->data().timestamp() << endl;
                    cout << "start_angle: " << first->data().strt_angle() << endl;
                    cout << "end_angle  : " << first->data().end_angle() << endl;
                    cout << "distance   : " << first->data().distance() << endl;
                }

            }


        }
        
    } catch (const dds::core::Exception& e) {
        cerr << "*** tracksubscriber DDS exception: " << e.what() << endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        cerr << "*** tracksubscriber C++ exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "*** tracksubscriber Done." << endl;

    return EXIT_SUCCESS;
}
