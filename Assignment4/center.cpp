
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include "dds/dds.hpp"

#include "statekey.hpp"
#include "controlTransfer.hpp"

#include <cmath> //KY: added this
using namespace std;
using namespace org::eclipse::cyclonedds;

// set this to your team number
uint32_t domainID = 0;
char *programName;

inline // Function to convert degrees to radians
    double
    deg2rad(double degrees)
{
    return degrees * M_PI / 180.0;
}

inline double meterToFeet(double meters)
{
    return meters * 3.281;
}

inline // KY: Haversine formula to calculate distance between two points specified in latitude and longitude
    double
    haversineDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Earth's radius in kilometers
    const double R = 6371.0;

    // Convert latitude and longitude from degrees to radians
    lat1 = deg2rad(lat1);
    lat2 = deg2rad(lat2);
    double dLat = deg2rad(lat2 - lat1);
    double dLon = deg2rad(lon2 - lon1);

    // Apply Haversine formula
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distanceKm = R * c;

    // Convert distance from km to nautical miles
    double distanceNm = distanceKm / 1.852;

    return distanceNm;
}

int main(int argc, char *argv[])
{
    // KY: New Variables added:
    std::set<string> mySet;

    // Toronto AD coordinates
    int count = 0;
    programName = argv[0];

    // KY: Longitude and latitude of Toronto AD
    const double TORONTO_AD_LAT = 43.683;
    const double TORONTO_AD_LON = -79.633;

    // create the main DDS entities Participant, Topic, Subscriber and DataReader
    dds::domain::DomainParticipant participant(domainID);

    dds::topic::Topic<State::Update> topic(participant, "Flights");
    dds::sub::Subscriber subscriber(participant);

    // Control Transfers
    dds::topic::Topic<ControlTransfer::Update> controlTopic(participant, "ControlTransfers");

    dds::topic::ContentFilteredTopic<ControlTransfer::Update> ctopic(controlTopic, "XYZZXY", dds::topic::Filter(""));
    ctopic->filter_function([](const ControlTransfer::Update &sample)
                            { return sample.to() == ControlTransfer::FlightZone::CENTRE; }); // fix this to only get to: ad

    dds::sub::DataReader<State::Update> reader(subscriber, topic);
    dds::sub::DataReader<ControlTransfer::Update> controlReader(subscriber, ctopic);

    // set up waitsets. The conditions are that dat is availalbe, or there has been
    // a change in the numbe of publishers that are matched with our subscription.
    dds::core::cond::WaitSet waitset;
    // dds::core::cond::WaitSet controlWaitset;

    dds::core::cond::StatusCondition rsc(reader);
    dds::core::cond::StatusCondition controlRsc(controlReader);

    rsc.enabled_statuses(dds::core::status::StatusMask::data_available() | dds::core::status::StatusMask::subscription_matched());
    controlRsc.enabled_statuses(dds::core::status::StatusMask::data_available());
    waitset.attach_condition(rsc);
    waitset.attach_condition(controlRsc);

    // KY:Initalizing the publisher
    // Move these outside and before the loop
    dds::topic::Topic<ControlTransfer::Update> controlTransferTopic(participant, "ControlTransfers");
    dds::pub::Publisher controlTransferPublisher(participant);
    dds::pub::DataWriter<ControlTransfer::Update> controlTransferWriter(controlTransferPublisher, controlTransferTopic);
    // ky: Loop until subscriber is done
    while (1)
    {
        // KY: WAITING FOR PUBLISH NOTIFCATION
        try
        {
            // wait for more data or for the publisher to end.
            waitset.wait(dds::core::Duration::infinite());
        }
        catch (const dds::core::Exception &e)
        {
            cerr << programName << ": Subscriber exception while waiting for data: \"" << e.what() << "\"." << endl;
            break;
        }

        // if the publisher is gone, exit.
        // Ky: if the subscriber is waiting for the publisher to receive data, but the publisher exits for some reason
        // ky: the below method checks if the reader/subscriber has zero remaining matched publishers
        if (reader.subscription_matched_status().current_count() == 0)
        {
            cerr << "KY: Publisher disconnected" << endl;
            break;
        }

        // take the samples and print them
        dds::sub::LoanedSamples<State::Update> samples;
        dds::sub::LoanedSamples<ControlTransfer::Update> samples2;

        samples = reader.take();
        samples2 = controlReader.take();

        // Subscribed info from center
        if (samples2.length() > 0)
        {
            dds::sub::LoanedSamples<ControlTransfer::Update>::const_iterator sample_iter;
            for (sample_iter = samples2.begin(); sample_iter < samples2.end(); ++sample_iter)
            {
                const ControlTransfer::Update &msg = sample_iter->data();
                const dds::sub::SampleInfo &info = sample_iter->info();
                // note not all samples may be valid.
                if (info.valid())
                {
                    cout << "\n -------------- subscriber received -------------- " << endl;
                    cout << "    Count : \"" << ++count << "\"" << endl;
                    cout << "    callsign : \"" << msg.callsign() << "\"" << endl;
                    cout << "    TO : \"" << msg.to() << "\"" << endl;
                    cout << "    FROM : \"" << msg.from() << "\"" << endl;
                    cout << "    timestamp : \"" << msg.timestamp() << "\"\n" << endl;
                }
            }
        }

        if (samples.length() > 0)
        {
            dds::sub::LoanedSamples<State::Update>::const_iterator sample_iter;
            for (sample_iter = samples.begin(); sample_iter < samples.end(); ++sample_iter)
            {
                const State::Update &msg = sample_iter->data();
                const dds::sub::SampleInfo &info = sample_iter->info();
                // note not all samples may be valid.
                if (info.valid())
                {
                    // cout << "**** subscriber received: " << ++count << endl;
                    // cout << "    icao24  : " << msg.icao24() << endl;
                    // cout << "    baroaltitude : \"" << meterToFeet(msg.baroaltitude()) << "\"" << endl;
                    // cout << "    latitude : \"" << msg.lat() << "\"" << endl;
                    // cout << "    longitude : \"" << msg.lon() << "\"" << endl;
                    // cout << "    callsign : \"" << msg.callsign() << "\"" << endl;
                    // cout << " PUBLISH   vert rate : \"" << msg.vertrate() << "\"" << endl;

                    // cout << "    AD IS ABOUT TO PUBLISH : \"" << endl;

                    // KY: Determine whether the plane is leaving the threshold (We don't ever have to worry about planes coming)
                    double distance = haversineDistance(TORONTO_AD_LAT, TORONTO_AD_LON, msg.lat(), msg.lon());
                    // cout << "  PUBLISH  distance : \"" << distance << "\"" << endl;
                    // cout << "    CENTER IS ABOUT TO PUBLISH : \"" << endl;
                    double vertRate = msg.vertrate();

                    // // If vert rate is negative then the plane will be landing
                    // if (distance <= 8 && meterToFeet(msg.baroaltitude()) < 3000 && msg.vertrate() < 0) {
                    //     //INSIDE THRESHOLD
                    // }
                    if (msg.baroaltitude() > 0.0)
                    {
                        // If vert rate is postive or zero then the plane will leave the AD zone
                        if (distance <= 8 && meterToFeet(msg.baroaltitude()) <= 3000 && msg.vertrate() < 0)
                        {

                            if (mySet.find(msg.callsign()) != mySet.end())
                            {
                                cout << "\n DUPLICATE UPDATE FLIGHT ALREADY PUBLISHED: "
                                     << msg.callsign() << " \n"
                                     << endl;
                                continue;
                            }

                            mySet.insert(msg.callsign());

                            // ABOUT TO LEAVE THE AD THRESHOLD
                            cout << "\n-------------- PLANE LEAVING CENTER --------------"
                                 << endl;
                            cout << "  PUBLISH:  callsign : \"" << msg.callsign() << "\"" << endl;
                            cout << "  PUBLISH:  vert rate : \"" << msg.vertrate() << "\"" << endl;
                            cout << "  PUBLISH:  distance : \"" << distance << "\"" << endl;
                            cout << "  PUBLISH: baroaltitude : \"" << meterToFeet(msg.baroaltitude()) << "\"" << endl;

                            // Type variable to use when writing
                            ControlTransfer::Update CentreUpdate(
                                msg.callsign(),                  // string callsign;
                                ControlTransfer::FlightZone::AD, // FlightZone zone
                                ControlTransfer::FlightZone::CENTRE,
                                msg.timestamp() // unsigned long timestamp;
                            );

                            controlTransferWriter.write(CentreUpdate);
                        }
                    }
                }
            }
        }
        else
        {
            cout << "\n: no samples?\n"
                 << endl;
        }
    }
}
