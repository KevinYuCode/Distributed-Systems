#ifndef DIRSVCSERVER_HPP
#define DIRSVCSERVER_HPP

#include "network.hpp"
#include "DirSvcService.hpp"


class DirSvcServer : public Node
{
private:
    shared_ptr<DirSvcService> dirSvcService;

public:
    DirSvcServer(string nodeName);
    ~DirSvcServer()
    {
    }
    void start();
    
    void setPort(in_port_t port);
    void setSvcName(string svcName);
};

#endif
