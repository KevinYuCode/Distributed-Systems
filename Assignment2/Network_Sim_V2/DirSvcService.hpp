

#ifndef _DIRSVCSERVICE_HPP_
#define _DIRSVCSERVICE_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "KVrpc.h"
#include "DIRSVC.pb.h"
#include "dumpHex.hpp"
#include "network.hpp"

using namespace std;

class DirSvcService : public Service
{
    // ------------------------------------- Data Structure Definitions ------------------------------------- //
    // The data structure for getting the server info
    struct ServerSearchInfo
    {
        bool status;
        string serverName;
        int serverPort;
    };

    // The data structure for registering the server info (important for unordered map)
    struct ServerRegisterInfo
    {
        string serviceName;
        string serverName;
        int serverPort;
    };

    struct ServerMapData
    {
        string serverName;
        int serverPort;
    };

    unordered_map<string, ServerMapData>
        servicesMap;

    // rpc specific values
    // version 1.0
    static const uint32_t magic = 'DNS';      // Changed this for DNS protocol
    static const uint32_t version1x = 0x0101; // Changed this for DNS protocol

    // network management
    int sockfd;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    in_port_t PORT;
    string svcName;

    void callMethodVersion1(DIRSVC::dirSvcRequest &receivedMsg, DIRSVC::dirSvcResponse &replyMsg);
    bool registerService(ServerRegisterInfo &value);
    ServerSearchInfo searchService(string key);
    bool deleteService(string key);

public:
    DirSvcService(string name, weak_ptr<Node> p) : Service(name, p){};
    ~DirSvcService()
    {
        stop();
    }

    void setPort(in_port_t port);
    void setSvcName(string svcName);

    void start();
    void stop();
};

#endif
