
#ifndef _SVCDIR_RPC_H_
#define _SVCDIR_RPC_H_

struct SvcDirSearchResult{
    bool status;
    std::string serverName;
    uint16_t  port;
};

//bool SvcDirRegister(std::string serviceName, std::string serverName, uint32_t port);
//SvcDirSearchResult SvcDirSearch(std::string serviceName);

#endif
