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
#include "servicedir/srvcdirserver.hpp"

extern std::map<std::thread::id, shared_ptr<Node>> nodes;
extern std::map<std::thread::id, string> names;

int main(int argc, char *argv[])
{
    // handle command line arguments...
    int res = network_init(argc, argv);
    std::stringstream ss;

    // start all of the servers first. This will let them get up
    // and running before the client attempts to communicste
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: starting server" << std::endl;

    std::cout << "Main: starting directory server" << std::endl;
    shared_ptr<SrvcDirServer> svcDirServer = make_shared<SrvcDirServer>("ServiceDirectory");
    svcDirServer->setAddress("10.0.0.2");
    svcDirServer->init();
    svcDirServer->startServices();

    ServerData::ServerInfo replica1 = {"kvserver2", 5000};
    vector<ServerData::ServerInfo> *replicas = new vector<ServerData::ServerInfo>;
    ;
    replicas->push_back(replica1);

    ServerData::ServerInfo *primaryServer = new ServerData::ServerInfo{"kvserver1", 5193};

    // Primary Server
    shared_ptr<KVServer> kvServer1 = make_shared<KVServer>("kvserver1", replicas, primaryServer, 1);

    kvServer1->setAddress("10.0.0.3");
    kvServer1->setDBMFileName("server1");
    kvServer1->setSvcDirServer("ServiceDirectory");
    kvServer1->setSvcName("kv1");
    kvServer1->setPort(5193);
    kvServer1->init();

    // Replica Server
    shared_ptr<KVServer> kvServer2 = make_shared<KVServer>("kvserver2", nullptr, primaryServer, 0);
    kvServer2->setAddress("10.0.0.4");
    kvServer2->setDBMFileName("server2");
    kvServer2->setSvcDirServer("ServiceDirectory");
    kvServer2->setSvcName("kv1");
    kvServer2->setPort(5000);
    kvServer2->init();

    // After Servers have been initialized:
    kvServer1->startServices();
    kvServer2->startServices();

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "Main: init client" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    shared_ptr<KVClient1> kvClient = make_shared<KVClient1>("kvclient");
    kvClient->setAddress("10.0.0.10");
    kvClient->setSvcDirServer("ServiceDirectory");
    kvClient->setSvcName("kv1");

    kvClient->init();

    std::cout << "\nMain: ************************************" << std::endl;
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

    // wait for clients to finish
    std::cout << "\nMain: ************************************" << std::endl;
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

    kvServer1->stopServices();
    svcDirServer->stopServices();

    std::cout << "\nMain: ************************************" << std::endl;
    std::cout << "Main: waiting for threads to complete" << std::endl;
    // wait for all server threads
    kvServer1->waitForServices();
#ifdef NOTNOW
    kvServer2->waitForServices();
#endif
    svcDirServer->waitForServices();

    // std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
