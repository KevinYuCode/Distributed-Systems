#include "e1client4.hpp"
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

  // Conditionally call specific RPC methods based on what server the client is trying to call
  if (addr_str == "10.0.0.3")
  {
    // Call the put method from the client stub
    Data::put_response dataPut = clientStub->put(7, reinterpret_cast<const uint8_t *>("123"), strlen("123"));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    std::cout << "-------------------------- CLIENT 1 RESULTS --------------------------" << std::endl;

    // Call the get method from the client stub
    Data::get_response dataGet = clientStub->get(7);

    // Print out the results form the get request
    std::cout << "Client1's GET status: " << dataGet.status() << std::endl;
    std::cout << "Client1's GET value: " << dataGet.value() << std::endl;
    std::cout << "Client1's GET length: " << dataGet.value_length() << std::endl;
  }
  else if (addr_str == "10.0.0.5")
  {
    // Call the put method from the client stub
    Data::put_response dataPut = clientStub->put(7, reinterpret_cast<const uint8_t *>("586"), strlen("586"));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "-------------------------- CLIENT 2 RESULTS --------------------------" << std::endl;

    // Call the get method from the client stub
    Data::get_response dataGet = clientStub->get(7);

    // Print out the results form the get request
    std::cout << "Client2's GET status: " << dataGet.status() << std::endl;
    std::cout << "Client2's GET value: " << dataGet.value() << std::endl;
    std::cout << "Client2's GET length: " << dataGet.value_length() << std::endl;
  }
}
