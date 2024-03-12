
#ifndef __CLIENT_HPP_
#define __CLIENT_HPP_
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

#include "srvcdirclientstub.hpp"
#include "network.hpp"

using namespace std;

class TstSvcDirClient: public Node {
    // for now one service stub
    SvcDirClientStub svcDirService;
    // this alive refers to client activities
    //std::atomic<bool> alive;

public:
    TstSvcDirClient(string name): Node(name), svcDirService(name){
    }
    ~TstSvcDirClient(){
        svcDirService.shutdown();
        //stop();
    }
    void start();
    void setServiceDirServerName(string name);
};


#endif

