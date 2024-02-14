#include "e1client.hpp"
#include "data.pb.h"
using namespace std;
using namespace std::string_literals;

#define close mclose

/**
 * Sets the server address attribute so the client stub can create a socket for the server
 */
void E1Client::setServerAddress(char *serverAddress)
{
  this->serverAddress = serverAddress;
}

/**
 * Defines the RPC calls that the client will invoke
 */
void E1Client::start()
{
  // Create an instance of the client stub
  clientStub = make_shared<ClientStub>();
  clientStub->setServerAddress(serverAddress);
  // Call the put method from the client stub
  Data::put_response dataPut = clientStub->put(13, reinterpret_cast<const uint8_t *>("kevin\0yu"), 10);
  std::cout << "E1CLIENT's PUT status: " << dataPut.success() << std::endl;

  // Call the get method from the client stub
  Data::get_response dataGet = clientStub->get(13);

  // Print out the results form the get request
  std::cout << "Client's GET status: " << dataGet.status() << std::endl;
  std::cout << "Client's GET value: " << dataGet.value() << std::endl;
  std::cout << "Client's GET length: " << dataGet.value_length() << std::endl;
}
