
#ifndef __CLIENT_HPP3_
#define __CLIENT_HPP3_
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "clientStub.hpp"
// #include "e1service.hpp"
#include "dumpHex.hpp"
#include "network.hpp"

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif



using namespace std;

class E1Client3 : public Node
{
public:
    shared_ptr<ClientStub> clientStub;
    char *serverAddress;
    E1Client3(string name) : Node(name), clientStub() {}
    ~E1Client3()
    {
        stop();
    }
    void start();
    void setServerAddress(char *serverAddress);
};

#endif
