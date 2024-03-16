#include "kvclient1.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

// std::unique_ptr<KVClient> theClient;

void KVClient1::start()
{
    // test getaddrinfo
    // store a value;
    stringstream ss;
    ss << "\nAbout to call put" << endl;
    cerr << ss.str();

    // string value1 = "This is \0 a test!!"s;
    string value1 = "This is a test!!"s;
    cout << "KVCLIENT: This is the key: " << (int32_t)25 << endl;
    cout << "KVCLIENT: This is the value: " << (const uint8_t *)value1.data() << endl;
    cout << "KVCLIENT: This is the vlue size: " << (uint16_t)value1.size() << endl;
    bool putRes = kvService.kvPut((int32_t)25, (const uint8_t *)value1.data(), (uint16_t)value1.size());
    cerr << "put status is " << putRes << "\n"
         << endl;

    // value1 = "WHYYYYYYYYYYYYYYYYYYYYYYY!!"s;
    // cout << "KVCLIENT: This is the key: " << (int32_t)25 << endl;
    // cout << "KVCLIENT: This is the value: " << (const uint8_t *)value1.data() << endl;
    // cout << "KVCLIENT: This is the vlue size: " << (uint16_t)value1.size() << endl;
    // putRes = kvService.kvPut((int32_t)25, (const uint8_t *)value1.data(), (uint16_t)value1.size());
    // cerr << "put status is " << putRes << "\n"
    //      << endl;

    // get the same value back.
    ss = stringstream();
    ss << "\nKVCLIENT: About to call get" << endl;
    cerr << ss.str();

    kvGetResult gres;
    gres = kvService.kvGet((int32_t)25);
    cerr << "KVCLIENT: get status is " << gres.status
         << endl;
    cerr << "KVCLIENT: get value is " << gres.value << "\n"
         << endl;

    // if (gres.vlen > 0){
    // cerr << HexDump{gres.value, gres.vlen};
    // }
}
