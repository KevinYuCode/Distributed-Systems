#ifndef KVSERVER_HPP
#define KVSERVER_HPP

#include "network.hpp"
#include "kvservice.hpp"


class KVServer : public Node
{
private:
    shared_ptr<KVServiceServer> kvService;

public:
    KVServer(string nodeName);
    ~KVServer()
    {
    }
    void start();
    void setDBMFileName(string name);
    
    void setPort(in_port_t port);
    void setSvcName(string svcName);
};

#endif
