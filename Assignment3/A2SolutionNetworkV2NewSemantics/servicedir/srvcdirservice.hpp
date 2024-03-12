

#ifndef SRVCDIRSERVICE_HPP
#define SRVCDIRSERVICE_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <map>

#include "svcdir.hpp"
#include "SvcDir.pb.h"
#include "dumpHex.hpp"
#include "network.hpp"

using namespace std;

struct SvcAddress{
   string name;
   uint32_t port;
};

class SrvDirServiceServer: public Service{
    // system state
    map<string,SvcAddress> serviceDirMap;

    
    // rpc specific values
    // version 1.0
    static const uint32_t magic = 'SVCD';
    static const uint32_t version1x = 0x0100;
    
    // network management
    int sockfd;
    in_port_t PORT = 8000;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    
    void callMethodVersion1(ServiceDirectory::svcDirRequest &receivedMsg, ServiceDirectory::svcDirResponse &replyMsg);
    bool registerService(string serviceName, string serverName, uint32_t port);
    SvcDirSearchResult searchService(string serviceName);
    bool removeService(string serviceName);
    
public:
    SrvDirServiceServer(string name, weak_ptr<Node> p):Service(name + ".SVC_DIR_RPC" ,p){ }
    ~SrvDirServiceServer(){
        stop();
    }
    void start();
    void stop();

};

#endif
