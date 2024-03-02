#ifndef DIRSVCCLIENTSTUB_HPP_
#define DIRSVCCLIENTSTUB_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

#include "DIRSVCrpc.h"
#include "DIRSVC.pb.h"
#include "dumpHex.hpp"

using namespace std;

class DirSvcServiceStub
{
    // system managment values
    string name;
    bool ready;
    string serverAddrString;
    string serverName;
    // rpc specific values
    // version 1.0

    // DNS CONFIGURATION
    static const uint32_t magic = 'DNS';
    static const uint32_t version1x = 0x0101;
    atomic<uint32_t> serial;

    // network management
    int sockfd;
    static const in_port_t PORT;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    struct sockaddr_in servaddr;

    bool init();

    string svcName;

public:
    DirSvcServiceStub(string name) : name(name), ready(false), serial(1)
    {
    }
    ~DirSvcServiceStub()
    {
        shutdown();
    }

    void setSvcName(string svcName);
    void shutdown();
    void setServerAddress(string address)
    {
        serverAddrString = address;
    }
    void setServerName(string name)
    {
        serverName = name;
    }
    bool registerService(string svcName, string serverName, int svcPort);
    ServerSearchInfo searchService(string svcName);
};

#endif
