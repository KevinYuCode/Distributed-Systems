#include "e1client3.hpp"
#include "data.pb.h"
using namespace std;
using namespace std::string_literals;

#define close mclose

/**
 * Sets the server address attribute so the client stub can create a socket for the server
 */
void E1Client3::setServerAddress(char *serverAddress)
{
  this->serverAddress = serverAddress;
}

/**
 * Defines the RPC calls that the client will invoke
 */
void E1Client3::start()
{
  // Create an instance of the client stub
  clientStub = make_shared<ClientStub>();
  clientStub->setServerAddress(serverAddress);
  
  // Conditionally call specific RPC methods based on what server the client is trying to call
  if (addr_str == "10.0.0.3")
  {

    // Call the put method from the client stub
    Data::put_response dataPut = clientStub->put(69, reinterpret_cast<const uint8_t *>("111"), strlen("111"));
    std::cout << "Client1's PUT status: " << dataPut.success() << std::endl;

    std::cout << "-------------------------- CLIENT 1 RESULTS --------------------------" << std::endl;

    // Call the get method from the client stub
    Data::get_response dataGet = clientStub->get(69);

    // Print out the results form the get request
    std::cout << "Client1's GET status: " << dataGet.status() << std::endl;
    std::cout << "Client1's GET value: " << dataGet.value() << std::endl;
    std::cout << "Client1's GET length: " << dataGet.value_length() << std::endl;
  }
  else if (addr_str == "10.0.0.5")
  {
    shared_ptr<ClientStub> clientStub2 = make_shared<ClientStub>();
    clientStub2->setServerAddress(serverAddress);

    std::cout << "INSIDE CLIENT 2: " << std::endl;

    // Call the put method from the client stub
    Data::put_response dataPut = clientStub2->put(69, reinterpret_cast<const uint8_t *>("420"), strlen("420"));
    std::cout << "Client2's PUT status: " << dataPut.success() << std::endl;

    std::cout << "-------------------------- CLIENT 2 RESULTS --------------------------" << std::endl;

    // Call the get method from the client stub
    Data::get_response dataGet = clientStub2->get(69);

    // Print out the results form the get request
    std::cout << "Client2's GET status: " << dataGet.status() << std::endl;
    std::cout << "Client2's GET value: " << dataGet.value() << std::endl;
    std::cout << "Client2's GET length: " << dataGet.value_length() << std::endl;
  }
  // Incase the conditional doesn't work for finding what RPC to calls
  else
  {
    std::cout << "No clients with address" << addr_str << " found " << std::endl;
  }
}
