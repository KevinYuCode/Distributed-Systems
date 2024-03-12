#include "tstSvcDirClient.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

void TstSvcDirClient::start(){

    string serverName="exmaple.com";
    uint32_t port=1024;

    cerr << "calling registerService" << endl;
    bool registerRes = svcDirService.registerService("kv",serverName,port);
    cerr << "register status is " << registerRes << endl;

    // get the same value back.
    cerr << "calling searchService" << endl;
    SvcDirSearchResult sRes;
    sRes = svcDirService.searchService("kv");
    cerr << "ssearch status is " << sRes.status << endl;
    if (sRes.status){
        cerr << "address is " << sRes.serverName << ":" << sRes.port << endl;
    }
    // remove
    bool remRes = svcDirService.removeService("kv");
    cerr << "remove status is " << remRes << endl;

    // is it still there?
    cerr << "calling searchService" << endl;
    sRes = svcDirService.searchService("kv");
    cerr << "ssearch status is " << sRes.status << endl;
    if (sRes.status){
        cerr << "address is " << sRes.serverName << ":" << sRes.port << endl;
    }

}

void TstSvcDirClient::setServiceDirServerName(string name) {svcDirService.setServerName(name);}
