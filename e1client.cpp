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


  Data::put_response dataPut = clientStub->put(7, reinterpret_cast<const uint8_t *>("kevin\0yu"), 10);
  std::cout << "E1CLIENT: status from e1client : " << dataPut.success() << std::endl;

  Data::get_response dataGet = clientStub->get(7);

  std::cout << "data from e1client get status: " << dataGet.status() << std::endl;
  std::cout << "data from e1client get value: " << dataGet.value() << std::endl;
  std::cout << "data from e1client get length: " << dataGet.value_length() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
