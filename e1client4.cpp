#include "e1client4.hpp"
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

  // Make another instance of e1client

  if (addr_str == "10.0.0.3")
  {
    // FIRST CLIENT
    Data::put_response dataPut = clientStub->put(7, reinterpret_cast<const uint8_t *>("123"), strlen("123"));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    std::cout << "-------------------------- CLIENT 1 RESULTS --------------------------" << std::endl;

    Data::get_response dataGet = clientStub->get(7);
    std::cout << "data from e1client get status: " << dataGet.status() << std::endl;
    std::cout << "data from e1client get value: " << dataGet.value() << std::endl;
    std::cout << "data from e1client get length: " << dataGet.value_length() << std::endl;
  }
  else if (addr_str == "10.0.0.5")
  {
    // SECOND CLIENT
    Data::put_response dataPut = clientStub->put(7, reinterpret_cast<const uint8_t *>("586"), strlen("586"));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "-------------------------- CLIENT 2 RESULTS --------------------------" << std::endl;
    Data::get_response dataGet = clientStub->get(7);
    std::cout << "data from e2client get status: " << dataGet.status() << std::endl;
    std::cout << "data from e2client get value: " << dataGet.value() << std::endl;
    std::cout << "data from e2client get length: " << dataGet.value_length() << std::endl;
  }
}
