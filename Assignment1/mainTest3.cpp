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
#include "e1server.hpp"
// #include "e1client.hpp"
#include "e1client3.hpp"

#include <gdbm.h>

// Global Variables to store the nodes and names of the threads
extern std::map<std::thread::id, shared_ptr<Node>> nodes;
extern std::map<std::thread::id, string> names;

int main(int argc, char *argv[])
{

  // Setup network configuration using command line args
  int res = network_init(argc, argv);
  std::stringstream ss;

  // Initializing the server with its own address
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: starting server" << std::endl;

  // INTIALIZING SERVER 1
  shared_ptr<E1Server> e1Server = make_shared<E1Server>("e1server");
  e1Server->setAddress("10.0.0.2");
  e1Server->setGdbmFile("dean3.db");
  e1Server->startServices();

  // INTIALIZING SERVER 2
  shared_ptr<E1Server> e2Server = make_shared<E1Server>("e2server");
  e2Server->setAddress("10.0.0.4");
  e2Server->setGdbmFile("dean4.db");
  e2Server->startServices();

  // Wait for the servers to setup before initalizing the client
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Initialize the client 1 with unique address and unique server address
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: init client" << std::endl;
  shared_ptr<E1Client3> e1Client = make_shared<E1Client3>("e1client");
  e1Client->setAddress("10.0.0.3");
  e1Client->setServerAddress("10.0.0.2");

  // Initialize the client 2 with unique address and unique server address
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: init client" << std::endl;
  shared_ptr<E1Client3> e2Client = make_shared<E1Client3>("e2client");
  e2Client->setAddress("10.0.0.5");
  e2Client->setServerAddress("10.0.0.4");

  // Starting Client to send messages to the server
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: starting client" << std::endl;
  shared_ptr<thread> t;
  shared_ptr<thread> t2;

  // Create a guard scope to add client1 to global variables and call its start method
  {
    // need a scope for the lock guard.
    // if this doesn't work put it in a function
    std::lock_guard<std::mutex> guard(nodes_mutex);

    // Spin off a new thread and call the start() method on the client
    t = make_shared<thread>([e1Client]()
                            {
    				try{
    				  e1Client -> start();
    				} catch (exitThread & e){ } });

    // Critical section
    nodes.insert(make_pair(t->get_id(), e1Client));
    names.insert(make_pair(t->get_id(), "e1client"));
  }

  // Create a guard scope to add client2 to global variables and call its start method
  {
    // need a scope for the lock guard.
    // if this doesn't work put it in a function
    std::lock_guard<std::mutex> guard(nodes_mutex);

    t2 = make_shared<thread>([e2Client]()
                             {
              try{
                e2Client -> start();
              } catch (exitThread & e){ } });

    // Critical section:?
    nodes.insert(make_pair(t2->get_id(), e2Client));
    names.insert(make_pair(t2->get_id(), "e2client"));
  }

  // Wait for the threads to finish its execution before proceeding with the main thread
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: waiting for clients to finish" << std::endl;
  t->join();
  t2->join();

  // Stops all services on the servers by setting alive flag to false.
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: calling stop services on server" << std::endl;
  e1Server->stopServices();
  e2Server->stopServices();

  // Wait for all server threads to complete
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: waiting for threads to complete" << std::endl;

  e1Server->waitForServices();
  e2Server->waitForServices();

  google::protobuf::ShutdownProtobufLibrary();
  std::cout << "Main: shutting down protobuf library" << std::endl;

  return 0;
}
