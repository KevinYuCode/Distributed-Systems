#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "dumpHex.hpp"
#include "network.hpp"
#include "data.pb.h"

using namespace std;
#define close mclose
// const int MAXMSG = 1400;
// in_port_t PORT = 8080;

class ClientStub
{

public:
    static const int MAXMSG = 1400;
    in_port_t PORT = 8080;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];
    int sockfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    struct sockaddr_in servaddrreply;

    uint32_t currentSerialNumber;
    int message_id;
    int magic_number;
    char *serverAddress;
    struct getResult
    {
        bool status;
        uint8_t *value;
        uint16_t vlen;
    };
    ClientStub();
    void start();

    void createSocket();
    // {
    // }
    void closeSocket();
    // {
    // }

    Data::get_response get(uint32_t key);
    // {
    // }
    bool put(uint32_t key, const uint8_t *value, uint16_t vlen);
    // {
    // }
    void setServerAddress(char *serverAddress);
};
