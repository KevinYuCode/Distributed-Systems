#include "kvclient1.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);


//std::unique_ptr<KVClient> theClient;

void KVClient1::start(){

    // store a value;
    string value1="This is \0 a test!!"s;
    bool putRes = kvService.kvPut((int32_t)25,(const uint8_t*)value1.data(), (uint16_t)value1.size());
    cerr << "status is " << putRes << endl;

    // get the same value back.
    kvGetResult gres;
    gres = kvService.kvGet((int32_t)25);
    cerr << "status is " << gres.status << endl;
    if (gres.vlen > 0){
	cerr << HexDump{gres.value, gres.vlen};
    }
    kvService.shutdown();
}

void KVClient1::setServerAddress(string addr) {kvService.setServerAddress(addr);}
