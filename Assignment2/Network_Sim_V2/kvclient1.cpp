#include "kvclient1.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

// std::unique_ptr<KVClient> theClient;

void KVClient1::setSvcName(string svcName)
{
    kvService.setSvcName(svcName);
}

void KVClient1::start()
{

    // // --------------------------------------- IRRELEVANT CODE????? --------------------------------------- //
    // struct addrinfo *addr_result;
    // int errcode = getaddrinfo("kvserver1", nullptr, nullptr, &addr_result); //We get the server's address (eg. 192.168.0.1) from the name of the node

    // if (errcode != 0)
    // {
    //     cout << "error finding address of kvserver: " << gai_strerror(errcode) << endl;
    // }
    // else
    // {
    //     // SUCCESSFULLY GOT THE ADDRESS INFO
    //     if (addr_result == nullptr)
    //     {
    //         cout << "getaddr info said 0, but no pointer?" << endl;
    //     }
    //     else
    //     {
    //         if (addr_result->ai_next != nullptr)
    //         {
    //             cout << "should only be one result" << endl;
    //         }
    //         else
    //         {
    //             cout << "address of kvserver is: " << inet_ntoa(((sockaddr_in *)addr_result->ai_addr)->sin_addr) << endl; //Converts the IP address into string readable form
    //         }
    //         freeaddrinfo(addr_result);
    //     }
    // }

    // PUT REQUEST
    if (name == "kvclient2")
    {

        string value1 = "LOOOOOOOOOOOOOOOOOOOOOOOOL"s;
        bool putRes = kvService.kvPut((int32_t)25, (const uint8_t *)value1.data(), (uint16_t)value1.size());
        cerr << "\n//-------------------------------------- END OF PUT ---------------------------------------//\n"
             << endl;
        cerr << "MESSAGE STORED IN DB------> LOOOOOOOOOOOOOOOOOOOOOOOOL"
             << endl;

        // GET REQUEST
        kvGetResult gres;
        gres = kvService.kvGet((int32_t)25); // Get request
        cerr << "//-------------------------------------- END OF GET ---------------------------------------//\n"
             << endl;
        cerr << "STATUS OF GET IS: " << gres.status << endl;
        cerr << "VALUE OF GET IS: " << gres.value << endl;

        // Hex Dump
        if (gres.vlen > 0)
        {
            // cerr << HexDump{gres.value, gres.vlen};
        }
    }
    else
    {
        cerr << "//-------------------------------------- KV CLIENT TEST ---------------------------------------//\n"
             << endl;

        string value1 = "This is a test! Kevin!"s;
        bool putRes = kvService.kvPut((int32_t)25, (const uint8_t *)value1.data(), (uint16_t)value1.size());
        cerr << "//-------------------------------------- END OF PUT ---------------------------------------//\n"
             << endl;
        cerr << "MESSAGE STORED IN DB------> This is a test! Kevin!"
             << endl;
        cerr << "STATUS OF PUT IS: " << putRes << "\n"
             << endl;

        // GET REQUEST
        kvGetResult gres;
        gres = kvService.kvGet((int32_t)25); // Get request
        cerr << "//-------------------------------------- END OF GET ---------------------------------------//\n"
             << endl;
        cerr << "STATUS OF GET IS: " << gres.status << endl;
        cerr << "STATUS OF GET IS: " << gres.value << endl;

        //     // Hex Dump
        //     if (gres.vlen > 0)
        //     {
        //         // cerr << HexDump{gres.value, gres.vlen};
        //     }
    }

    kvService.shutdown();
}

void KVClient1::setServerAddress(string addr) { kvService.setServerAddress(addr); }
void KVClient1::setServerName(string addr) { kvService.setServerName(addr); }
