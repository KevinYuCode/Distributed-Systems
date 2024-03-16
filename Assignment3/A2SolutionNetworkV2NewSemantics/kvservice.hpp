

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef __APPLE__
#include <ndbm.h>
#else
#include <gdbm.h>
#endif

#include "KVrpc.h"
#include "E477KV.pb.h"
#include "dumpHex.hpp"
#include "network.hpp"
#include "servicedir/srvcdirclientstub.hpp"
#include <vector>
#include "serverData.h"

using namespace std;

class KVServiceServer : public Service
{
    // system state
    string DBMFileName;
#ifdef __APPLE__
    DBM *dataFile = NULL;
#else
    GDBM_FILE dataFile;
#endif

    // rpc specific values
    // version 1.0
    static const uint32_t magic = 'E477';
    static const uint32_t version1x = 0x0100;

    // service Dir Management
    string svcName;
    SvcDirClientStub svcDirService;

    // network management
    int sockfd;
    in_port_t port = 8080;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];

    void callMethodVersion1(E477KV::kvRequest &receivedMsg, E477KV::kvResponse &replyMsg);
    bool kvPut(int key, const uint8_t *value, uint16_t vlen);
    kvGetResult kvGet(int key);

    // NEW VARIABLES
    int isPrimary = 0;
    vector<ServerData::ServerInfo> *replicas;
    ServerData::ServerInfo *primaryServer;
    bool ready;
    atomic<uint32_t> serial;
    const uint32_t maxTries = 3;



public:
    struct sendToAddress
    {
        sockaddr_in servaddr;
        int sockfd;
    };
    KVServiceServer(string name, weak_ptr<Node> p, vector<ServerData::ServerInfo> *replicas, ServerData::ServerInfo *primaryServer, int isPrimary) : Service(name + ".KV_RPC", p), svcDirService(name + ".svcDirstub"), serial(1)
    {
        this->isPrimary = isPrimary; // Keeps track of whether the service is a primary service/server

        // If Primary
        if (isPrimary)
        {
            this->replicas = replicas;
            cout
                << "Name of primary Server: " << name << " The replica server is: " << (*replicas)[0].name << endl;
        }

        // If Replica
        else
        {
            this->primaryServer = primaryServer;
            cout
                << "Name of replica server: " << name << " The primary server is: " << primaryServer->name << endl;
        }
    }

    ~KVServiceServer()
    {
        stop();

        // bug in alive flag prevents this from being reached.
#ifdef __APPLE__
        if (dataFile != nullptr)
        {
            dbm_close(dataFile);
            dataFile = nullptr;
        }
#else
        if (dataFile != nullptr)
        {
            gdbm_close(dataFile);
            dataFile = nullptr;
        }
#endif
    }

    void start();
    void stop();

    void setDBMFileName(string name) { DBMFileName = name; }
    void setServiceDirServer(string addr);
    void setServiceName(string svcName);
    void setPort(in_port_t p) { port = p; }
    sendToAddress init(string serverName, int port);
    bool kvPutReplica(int32_t, const uint8_t *value, uint16_t vlen, string serverName, int serverPort);
};

#endif
