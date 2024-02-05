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
#include "e1client.hpp"

#include <gdbm.h>

// Parallel arrays? where its [id, client/server]
//                            [id, clientName/serverName]
extern std::map<std::thread::id, shared_ptr<Node>> nodes;
extern std::map<std::thread::id, string> names;

// Make multiple main files
int main(int argc, char *argv[])
{

  // handle command line arguments...
  int res = network_init(argc, argv);
  std::stringstream ss;

  // start all of the servers first. This will let them get up
  // and running before the client attempts to communicate
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: starting server" << std::endl;

  // INTIALIZING SERVER 1
  shared_ptr<E1Server> e1Server = make_shared<E1Server>("e1server");
  e1Server->setAddress("10.0.0.2");
  e1Server->setGdbmFile("dean.db");
  e1Server->startServices();

  // INTIALIZING SERVER 2
  shared_ptr<E1Server> e2Server = make_shared<E1Server>("e2server");
  e2Server->setAddress("10.0.0.4");
  e2Server->setGdbmFile("dean2.db");
  e2Server->startServices();

  // wait for servers to get up and running...
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // INTIALIZING CLIENT 1
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: init client" << std::endl;
  shared_ptr<E1Client> e1Client = make_shared<E1Client>("e1client");
  e1Client->setAddress("10.0.0.3");
  e1Client->setServerAddress("10.0.0.2");

  // INTIALIZING CLIENT 2
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: init client" << std::endl;
  shared_ptr<E1Client> e2Client = make_shared<E1Client>("e2client");
  e2Client->setAddress("10.0.0.5");
  e2Client->setServerAddress("10.0.0.4");

  // Starting Client to send messages to the server
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: starting client" << std::endl;
  shared_ptr<thread> t;
  shared_ptr<thread> t2;

  // Client 1
  {
    // need a scope for the lock guard.
    // if this doesn't work put it in a function
    std::lock_guard<std::mutex> guard(nodes_mutex);

    t = make_shared<thread>([e1Client]()
                            {
    				try{
    				  e1Client -> start();
    				} catch (exitThread & e){ } });

    // Critical section:?
    nodes.insert(make_pair(t->get_id(), e1Client));
    names.insert(make_pair(t->get_id(), "e1client"));
  }

  // Client 2
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

  // Terminatates the threads?
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: waiting for clients to finish" << std::endl;
  t->join();
  t2->join();

  // Stops all services on the servers by setting alive flag to false.
  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: calling stop services on server" << std::endl;
  e1Server->stopServices();
  e2Server->stopServices();

  std::cout << "Main: ************************************" << std::endl;
  std::cout << "Main: waiting for threads to complete" << std::endl;

  // wait for all server threads
  e1Server->waitForServices();
  e2Server->waitForServices();

  google::protobuf::ShutdownProtobufLibrary();
  std::cout << "Main: shutting down protobuf library" << std::endl;

  return 0;
}
