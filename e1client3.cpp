#include "e1client3.hpp"
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

  // Make another instance of e1client

  std::cout << "E1CLIENT: name of the client: " << name << std::endl;
  std::cout << "E1CLIENT: name of the ADDY: " << addr_str << std::endl;

  if (addr_str == "10.0.0.3")
  {
    Data::put_response dataPut = clientStub->put(4, reinterpret_cast<const uint8_t *>("69"), strlen("69"));

    std::cout << "E1CLIENT (test 3): status from e1client put: " << dataPut.success() << std::endl;
    std::cout << "Client is starting... INSIDE CLIENT #2" << std::endl;
    Data::get_response dataGet = clientStub->get(4);

    std::cout << "data from e1client get status: " << dataGet.status() << std::endl;
    std::cout << "data from e1client get value: " << dataGet.value() << std::endl;
    std::cout << "data from e1client get length: " << dataGet.value_length() << std::endl;
    // std::cout << "Client is starting... INSIDE CLIENT #3" << std::endl;
  }

  else if (addr_str == "10.0.0.5")
  {
    // bool status = clientStub->put(69, "420");
    Data::put_response dataPut = clientStub->put(17, reinterpret_cast<const uint8_t *>("420"), strlen("420"));

    std::cout << "E1CLIENT (test 3): status from e2client put: " << dataPut.success() << std::endl;
    std::cout << "Client is starting... INSIDE CLIENT #2" << std::endl;
    Data::get_response dataGet = clientStub->get(17);

    std::cout << "data from e2client get status: " << dataGet.status() << std::endl;
    std::cout << "data from e2client get value: " << dataGet.value() << std::endl;
    std::cout << "data from e2client get length: " << dataGet.value_length() << std::endl;
    // std::cout << "Client is starting... INSIDE CLIENT #3" << std::endl;
  }
  else
  {
    std::cout << "No clients with address" << addr_str << " found " << std::endl;
  }
}
