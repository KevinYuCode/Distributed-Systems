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
#include "srvcdirserver.hpp"
#include "tstSvcDirClient.hpp"

extern std::map<std::thread::id,shared_ptr<Node>> nodes;
extern std::map<std::thread::id,string> names;

int main(int argc, char * argv[]){
    // handle command line arguments...
    int res = network_init(argc, argv);
    std::stringstream ss;

    // start all of the servers first. This will let them get up
    // and running before the client attempts to communicste
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: starting directory server\n" << std::endl;

    // make shared broken?
    shared_ptr<SrvcDirServer> svcDirServer = make_shared<SrvcDirServer>("ServiceDirectory");

    svcDirServer->setAddress("10.0.0.2");
    svcDirServer->init();
    svcDirServer -> startServices();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: init client" << std::endl;

    shared_ptr<TstSvcDirClient> dirClient = make_shared<TstSvcDirClient>("tstClient");
    dirClient -> setServiceDirServerName("ServiceDirectory");
    dirClient->setAddress("10.0.0.3");
    dirClient -> init();

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: starting client" << std::endl;
    vector<shared_ptr<thread>> clientThreads;
    {
        // need a scope for the lock guard. 
        // if this doesn't work put it in a function
	std::lock_guard<std::mutex> guard(nodes_mutex);

	shared_ptr<thread> t = make_shared<thread>([dirClient](){
	    dirClient -> execute();
	});

	clientThreads.push_back(t);
	nodes.insert(make_pair(t->get_id(), dirClient));
	names.insert(make_pair(t->get_id(),"tstClient"));

    }

    // wait for clients to finish
    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: waiting for clients to finish" << std::endl;
    vector<shared_ptr<thread>>::iterator thit;
    for (thit = clientThreads.begin(); thit != clientThreads.end(); thit++){
	shared_ptr<thread> tmp = *thit;
	tmp->join();
    }
    std::cout << "Main: calling client stop" << std::endl;
    dirClient -> stop();
    std::cout << "Main: done calling client stop" << std::endl;

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: calling stop services on server" << std::endl;
    svcDirServer -> stopServices();

    std::cout << "Main: ************************************" << std::endl;
    std::cout << "Main: waiting for threads to complete" << std::endl;
    // wait for all server threads
    svcDirServer -> waitForServices();
 

    std::cout << "Main: all threads complete " << std::endl;

    //std::this_thread::sleep_for(std::chrono::milliseconds(10000));


    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

