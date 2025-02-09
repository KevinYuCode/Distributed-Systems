

#ifndef _KVSERVICE_HPP_
#define _KVSERVICE_HPP_

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
#include "DIRSVCrpc.h"
#include "DirSvcClientStub.hpp"

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

    // network management
    int sockfd;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    in_port_t PORT;
    string svcName;

    void callMethodVersion1(E477KV::kvRequest &receivedMsg, E477KV::kvResponse &replyMsg);
    bool kvPut(int key, const uint8_t *value, uint16_t vlen);
    kvGetResult kvGet(int key);

    // DirSvcServiceStub dirSvcClientStub;
    DirSvcServiceStub dirSvcClientStub;

public:
    KVServiceServer(string name, weak_ptr<Node> p) : Service(name, p), dirSvcClientStub(name){};

    ~KVServiceServer()
    {

        stop();

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

    void setPort(in_port_t port);
    void setSvcName(string svcName);

    void start();
    void stop();

    void setDBMFileName(string name) { DBMFileName = name; }

    bool registerService(ServerRegisterInfo serverInfo);
};

#endif
