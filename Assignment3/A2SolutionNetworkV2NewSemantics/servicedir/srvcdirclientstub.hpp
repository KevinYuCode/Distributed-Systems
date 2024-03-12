#ifndef __SRVCDIRSTUB_HPP_
#define __SRVCDIRSTUB_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>


#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

#include "svcdir.hpp"
#include "SvcDir.pb.h"
#include "dumpHex.hpp"


using namespace std;

class SvcDirClientStub{
    // system managment values
    string name;
    bool ready;
    string serverName;


    // rpc specific values
    // version 1.0
    const uint32_t maxTries = 3;
    static const uint32_t magic = 'SVCD';
    static const uint32_t version1x = 0x0100;
    atomic<uint32_t> serial;

     // network management
    int sockfd;
    static const in_port_t PORT = 8000;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    struct sockaddr_in	 servaddr;

    void init();

public:
    SvcDirClientStub(string name ): name(name),ready(false), serial(1){
    }
    ~SvcDirClientStub(){
        shutdown();
    }
    void shutdown();
    void setServerName(string name){
        serverName = name;
    }
    bool registerService(std::string serviceName, std::string serverName, uint32_t port);
    SvcDirSearchResult searchService(std::string serviceName);
    bool removeService(std::string serviceName);
};

#endif

