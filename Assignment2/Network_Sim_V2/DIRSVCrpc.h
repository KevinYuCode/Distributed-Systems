
#ifndef _DIRSVCRPC_H_
#define _DIRSVCRPC_H_
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
using namespace std;

struct ServerSearchInfo
{
    bool status;
    string serverName;
    int serverPort;
};

struct ServerRegisterInfo{
    string serviceName;
    string serverName;
    int serverPort;
};

#endif
