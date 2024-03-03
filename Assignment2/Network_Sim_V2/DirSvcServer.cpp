
#include "DirSvcServer.hpp"

// Comments: Class is identical to a kvServer.cpp but the only difference is that it no longer implements a gdbm store for storing the values from registering a service.

DirSvcServer::DirSvcServer(string nodeName) : Node(nodeName)
{
    dirSvcService = make_shared<DirSvcService>(nodeName, weak_from_this());

    addService(dirSvcService);
}

void DirSvcServer::start()
{
    cerr << "in DIR_SVC_SERVER start" << endl;
}

// --------------------------------------- SETTING CUSTOM PORT --------------------------------------- //
void DirSvcServer::setPort(in_port_t port)
{
    dirSvcService->setPort(port);
}

// --------------------------------------- SETTING SERVICE NAME --------------------------------------- //
void DirSvcServer::setSvcName(string svcName)
{
    dirSvcService->setSvcName(svcName);
}
