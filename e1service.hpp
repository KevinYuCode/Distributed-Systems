

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "data.pb.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <atomic>
#include <mutex>

#include "dumpHex.hpp"
#include "network.hpp"
#include <gdbm.h>

using namespace std;

class E1ServiceServer : public Service
{
    // network management
    mutex gdbm_mutex;
    int sockfd;
    in_port_t PORT = 8080;
    static const int MAXMSG = 1400;
    uint8_t udpMessage[MAXMSG];
    GDBM_FILE gdbm_database;
    struct sockaddr_in servaddr, cliaddr;
    string gdbm_file; // Each service must have its own gdbm file name (key) to retrieve the data

public:
    // nodeName = serverName, p = weak pointer pointing to server
    E1ServiceServer(string nodeName, weak_ptr<Node> p) : Service(nodeName + ".ex_1_service", p) {}
    ~E1ServiceServer()
    {
        stop();
    }
    void start();
    void setGdbmFile(string gdbm_name);
    void test();

    void checkHeader(Data::key_value_message *kv_message);
    void stop();
    void dispatch(Data::key_value_message *kv_message);
    void get_request(Data::key_value_message *kv_message);
    void put_request(Data::key_value_message *kv_message);
};

#endif
