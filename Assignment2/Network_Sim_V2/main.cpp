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

    // start all of the servers first. This will let them get up
    // and running before the client attempts to communicste
    std::cout << "\nMain: ************************************"
              << std::endl;
    std::cout << "Main: starting server\n"
              << std::endl;

    // KY: -------------------------------------- DNS SERVER -------------------------------------- //
    // Initalizing the DNS server
    shared_ptr<DirSvcServer> dnsServer = make_shared<DirSvcServer>("dns_server");

    // Configuring DNS server's address, port number, service name. And then initializing and starting its services.
    dnsServer->setAddress("10.0.0.69");
    dnsServer->setPort(8081);
    dnsServer->setSvcName("DNS"); //Note I don't think a service name is necessary since the DNS doesn't request service name
    dnsServer->init();          // Initializes the simulated DNS config and other activities??
    dnsServer->startServices(); // Starts up the list of services by creating a new thread for each service so aka one service for each server.

    // KY: -------------------------------------- KVSERVER1 -------------------------------------- //
    // Initalizing the DNS server
    shared_ptr<KVServer> kvServer = make_shared<KVServer>("kvserver");

    // Configuring KV server 1's address, port number, service name. And then initializing and starting its services.
    kvServer->setAddress("10.0.0.12");
    kvServer->setPort(8080);
    kvServer->setSvcName("email");
    kvServer->setDBMFileName("server");
    kvServer->init();          // Initializes the simulated DNS config and other activities??
    kvServer->startServices(); // Starts up the list of services by creating a new thread for each service so aka one service for each server.

    // KY: -------------------------------------- KVSERVER2 -------------------------------------- //
    // KY: Every instance of a server that is created, a new service is added to the node's "services" list
    shared_ptr<KVServer> kvServer2 = make_shared<KVServer>("kvserver2");

    // Configuring KV server 2's address, port number, service name. And then initializing and starting its services.
    kvServer2->setAddress("10.0.0.10");
    kvServer2->setPort(8089);
    kvServer2->setSvcName("taxi");
    kvServer2->setDBMFileName("server2");
    kvServer2->init();          // Initializes the simulated DNS config and other activities??
    kvServer2->startServices(); // Starts up the list of services by creating a new thread for each service so aka one service for each server.

    // KY: -------------------------------------- INITIALIZING CLIENT 1-------------------------------------- //
    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "Main: init client\n"
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    shared_ptr<KVClient1> kvClient = make_shared<KVClient1>("kvclient");
    kvClient->setAddress("10.0.0.14");
    kvClient->setSvcName("email");
    kvClient->init();

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: starting client" << std::endl;
    vector<shared_ptr<thread>> clientThreads;
    {
        // need a scope for the lock guard.
        // if this doesn't work put it in a function
        std::lock_guard<std::mutex> guard(nodes_mutex);

        shared_ptr<thread> t = make_shared<thread>([kvClient]()
                                                   { kvClient->execute(); });

        clientThreads.push_back(t);
        nodes.insert(make_pair(t->get_id(), kvClient));
        names.insert(make_pair(t->get_id(), "kvclient"));
    }

    // // KY: -------------------------------------- INITIALIZING CLIENT 2 -------------------------------------- //
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: init client" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    shared_ptr<KVClient1> kvClient2 = make_shared<KVClient1>("kvclient2");
    kvClient2->setAddress("10.0.0.11");
    kvClient2->setSvcName("taxi");
    kvClient2->init();

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: starting client 2" << std::endl;
    vector<shared_ptr<thread>> clientThreads2;
    {
        // need a scope for the lock guard.
        // if this doesn't work put it in a function
        std::lock_guard<std::mutex> guard(nodes_mutex);

        shared_ptr<thread> t = make_shared<thread>([kvClient2]()
                                                   { kvClient2->execute(); });

        clientThreads2.push_back(t);
        nodes.insert(make_pair(t->get_id(), kvClient2));
        names.insert(make_pair(t->get_id(), "kvclient2"));
    }

    // wait for client 2 to finish
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: waiting for clients to finish" << std::endl;
    vector<shared_ptr<thread>>::iterator thit2;
    for (thit2 = clientThreads2.begin(); thit2 != clientThreads2.end(); thit2++)
    {
        shared_ptr<thread> tmp = *thit2;
        tmp->join();
    }

    // // wait for client 1 to finish
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: waiting for clients to finish" << std::endl;
    vector<shared_ptr<thread>>::iterator thit;
    for (thit = clientThreads.begin(); thit != clientThreads.end(); thit++)
    {
        shared_ptr<thread> tmp = *thit;
        tmp->join();
    }

    // when clients finish, shut down the servers
    // TODO - combine into node stop? that is node stop should
    // shut down all services and the client.

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "Main: calling stop services on server" << std::endl;
    kvServer->stopServices();
    dnsServer->stopServices();

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "Main: waiting for threads to complete" << std::endl;
    // wait for all server threads
    kvServer->waitForServices();
    dnsServer->waitForServices();

    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
