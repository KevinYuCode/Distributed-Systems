/*+
 *  File:   main.cpp
 *
 *  Purpose:
 *      This module is the start driver for several of the ELEC 477 assignments.
 *      It initializes the
-*/
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <string>

#include "network.hpp"
#include "kvserver.hpp"
#include "kvclient1.hpp"
#include "DirSvcServer.hpp"

extern std::map<std::thread::id, shared_ptr<Node>> nodes;
extern std::map<std::thread::id, string> names;

int main(int argc, char *argv[])
{

    // handle command line arguments...
    int res = network_init(argc, argv);

    // KY: -------------------------------------- KVSERVER1 -------------------------------------- //
    // start all of the servers first. This will let them get up
    // and running before the client attempts to communicste
    std::cout << "\nMain: ************************************\n" << std::endl;
    std::cout << "\nMain: starting server\n" << std::endl;

    // KY: -------------------------------------- DNS SERVER -------------------------------------- //

    shared_ptr<DirSvcServer> dnsServer = make_shared<DirSvcServer>("dns_server");

    dnsServer->setAddress("10.0.0.69");
    dnsServer->setPort(8081);
    dnsServer->setSvcName("DNS");
    dnsServer->init();          // Initializes the simulated DNS config and other activities??
    dnsServer->startServices(); // Starts up the list of services by creating a new thread for each service so aka one service for each server.

    // std::this_thread::sleep_for(std::chrono::milliseconds(50));


    // KY: -------------------------------------- KVSERVER1 -------------------------------------- //
    // KY: Every instance of a server that is created, a new service is added to the node's "services" list
    shared_ptr<KVServer> kvServer = make_shared<KVServer>("kvserver");

    kvServer->setAddress("10.0.0.12");
    kvServer->setPort(8080);
    kvServer->setSvcName("email");
    kvServer->setDBMFileName("server1");
    kvServer->init();          // Initializes the simulated DNS config and other activities??
    kvServer->startServices(); // Starts up the list of services by creating a new thread for each service so aka one service for each server.



    // KY: -------------------------------------- INITIALIZING CLIENT -------------------------------------- //
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: init client" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    shared_ptr<KVClient1> kvClient = make_shared<KVClient1>("kvclient");
    kvClient->setAddress("10.0.0.14");
    kvClient->setSvcName("email");
    // kvClient->setServerName("kvserver1"); //We don't need this right?????????
    kvClient->init();

    // std::cout << "Main: ************************************" << std::endl;
    // std::cout << "Main: starting client" << std::endl;
    // vector<shared_ptr<thread>> clientThreads;
    // {
    //     // need a scope for the lock guard.
    //     // if this doesn't work put it in a function
    //     std::lock_guard<std::mutex> guard(nodes_mutex);

    //     shared_ptr<thread> t = make_shared<thread>([kvClient]()
    //                                                { kvClient->execute(); });

    //     clientThreads.push_back(t);
    //     nodes.insert(make_pair(t->get_id(), kvClient));
    //     names.insert(make_pair(t->get_id(), "kvclient"));
    // }

    // // wait for clients to finish
    // std::cout << "Main: ************************************" << std::endl;
    // std::cout << "Main: waiting for clients to finish" << std::endl;
    // vector<shared_ptr<thread>>::iterator thit;
    // for (thit = clientThreads.begin(); thit != clientThreads.end(); thit++)
    // {
    //     shared_ptr<thread> tmp = *thit;
    //     tmp->join();
    // }

    // when clients finish, shut down the servers
    // TODO - combine into node stop? that is node stop should
    // shut down all services and the client.

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "\nMain: calling stop services on server" << std::endl;
    kvServer->stopServices();
    dnsServer->stopServices();

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "\nMain: waiting for threads to complete" << std::endl;
    // wait for all server threads
    kvServer->waitForServices();
    dnsServer->waitForServices();

    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
