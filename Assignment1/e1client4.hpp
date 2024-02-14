
#ifndef __CLIENT_HPP_
#define __CLIENT_HPP_
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "clientStub.hpp"
#include "e1service.hpp"

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

#include "dumpHex.hpp"
#include "network.hpp"

using namespace std;

class E1Client : public Node
{
public:
    shared_ptr<ClientStub> clientStub;
    char *serverAddress;
    E1Client(string name) : Node(name), clientStub() {}
    ~E1Client()
    {
        stop();
    }
    void start();
    void setServerAddress(char *serverAddress);
};

#endif
