
#include "network.hpp"
#include "kvservice.hpp"
#include "serverData.h"

class KVServer : public Node
{

private:
        shared_ptr<KVServiceServer> kvService;

public:
        KVServer(string nodeName, vector<ServerData::ServerInfo> *replicas, ServerData::ServerInfo *primaryServer, int isPrimary);
        ~KVServer()
        {
        }

        void start();

        void setDBMFileName(string name);
        void setSvcName(string name);
        void setSvcDirServer(string name);
        void setPort(in_port_t p) { kvService->setPort(p); }
};
