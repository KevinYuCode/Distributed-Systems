#include "e1client.hpp"
#include "data.pb.h"
// #include "clientStub.hpp"
using namespace std;
using namespace std::string_literals;

#define close mclose

// const int MAXMSG = 1400;
// in_port_t PORT = 8080;

void E1Client::setServerAddress(char *serverAddress)
{
  serverAddress = serverAddress;
}
void E1Client::start()
{
  clientStub = make_shared<ClientStub>();

  std::cout << "Client is starting... INSIDE CLIENT #1" << std::endl;

  //Make another instance of e1client 


  // bool status = clientStub->put(69, "420");
  bool status = clientStub->put(69, reinterpret_cast<const uint8_t *>("420"), 5);

  //


  // std::cout << "Client is starting... INSIDE CLIENT #2" << std::endl;
  // Data::get_response data = clientStub->get(69);
  // std::cout << "Client is starting... INSIDE CLIENT #3" << std::endl;

  // CAUSING PROBLEMS

  std::cout << "E1CLIENT: status from e1client put: " << status <<std::endl;
  // std::cout << "data from e1client get: " << data.status() << std::endl;
  // std::cout << "data from e1client get: " << data.value() << std::endl;
  // std::cout << "data from e1client get: " << data.value_length() << std::endl;
  // std::cout << data.status() << data.value() << data.value_length() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
