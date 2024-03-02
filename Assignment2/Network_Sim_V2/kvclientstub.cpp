#include "kvclientstub.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

void KVServiceStub::setSvcName(string svcName)
{
    this->svcName = svcName;
}

bool KVServiceStub::kvPut(int32_t key, const uint8_t *value, uint16_t vlen)
{

    // init if needed
    if (!ready)
    {
        if (!init())
        {
            // init returned false, so problem setting up the
            // socket onnection
            return false;
        }
    }

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG]; // to serialize into
    struct sockaddr_in servaddrreply;

    // get the current value of serial for this request.
    uint32_t serial = this->serial++;

    const int maxRetries = 5; // Maximum number of retries
    int retries = 0;
    bool success = false;

    while (retries < maxRetries && !success)
    {
        E477KV::kvRequest msg;
        msg.set_magic(magic);
        msg.set_version(version1x);
        msg.set_serial(serial++);
        E477KV::putRequest *pr = msg.mutable_putargs();
        pr->set_key(key);
        pr->set_value(std::string((const char *)value, vlen));
        blen = msg.ByteSizeLong();
        if (blen > MAXMSG)
        {
            // too long??
            std::cerr << " *** msg too long" << std::endl;
            // errno = ???
            return false;
        }
        msg.SerializeToArray(buffer, blen);
        // std::cout << HexDump{buffer,blen} << endl;

        // send the message to the server.
        // std::cout << "blen = " << dec << blen << endl;
        int n = sendto(sockfd, (const char *)buffer, blen, MSG_CONFIRM,
                       (const struct sockaddr *)&servaddr, sizeof(servaddr));

        if (n < 0)
        {
            // Handle send error
            ++retries;
            continue;
        }

        E477KV::kvResponse putRespMsg;
        socklen_t len = sizeof(servaddrreply);
        n = recvfrom(sockfd, (char *)buffer, MAXMSG, MSG_WAITALL,
                     (struct sockaddr *)&servaddrreply, &len);

        if (n == -1)
        {
            // Handle recvfrom error (e.g., timeout)
            ++retries;
            continue;
        }

        if (putRespMsg.ParseFromArray(buffer, n) &&
            putRespMsg.magic() == 'E477' &&
            putRespMsg.version() == version1x &&
            putRespMsg.serial() == serial &&
            putRespMsg.has_putres())
        {

            success = putRespMsg.putres().status();
        }
        else
        {
            // Parse error or wrong message received, retry
            ++retries;
        }
    }

    if (!success)
    {
        cerr << "Failed to put key after " << maxRetries << " attempts." << endl;
    }

    return success;
}

kvGetResult KVServiceStub::kvGet(int32_t key)
{
    // init if needed
    kvGetResult retVal;
    retVal.status = false;
    retVal.value = nullptr;
    retVal.vlen = 0;
    if (!ready)
    {
        if (!init())
        {
            // init returned false, so problem setting up the
            // socket onnection
            return retVal;
        }
    }

    struct sockaddr_in servaddrreply;

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];
    E477KV::kvRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial++);
    E477KV::getRequest *gr = msg.mutable_getargs();

    gr->set_key(key);
    blen = msg.ByteSizeLong();
    if (blen > MAXMSG)
    {
        // too long??
        std::cerr << " *** msg too long" << std::endl;
        // errno = ???
        retVal.status = false;
        return retVal;
    }
    msg.SerializeToArray(buffer, blen);
    // std::cout << HexDump{buffer,blen} << endl;

    // send the message3
    // std::cout << "blen = " << dec << blen << endl;
    n = sendto(sockfd, (const char *)buffer, blen,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    // std::cout << "client sendto n = " << n << endl;

    E477KV::kvResponse getRespMsg;
    bool gotMessage = true;
    do
    {
        len = sizeof(struct sockaddr_in);
        n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        // std::stringstream ss;
        // ss << "client recieved = " << n << std::endl;
        // std::cout << ss.str();
        // std::cout << HexDump{buffer,(uint32_t)n} << endl;
        //  check for timeout here..
        if (n == -1)
        {
            return retVal;
        } // null ptrs and falls status };

        if (!getRespMsg.ParseFromArray(buffer, n))
        {
            cerr << "Couild not parse message" << endl;
            // wait for another mesage
        }
        else
        {
            if (getRespMsg.magic() != magic)
            {
                gotMessage = false;
            }
            else
            {
                if (msg.version() != getRespMsg.version())
                {
                    cerr << "Version Mismatch" << endl;
                    gotMessage = false;
                }
                else
                {
                    // wait for another message is the serial number is wrong.
                    if (msg.serial() != getRespMsg.serial())
                    {
                        cerr << "Serial Numnbers Mismatch" << endl;
                        gotMessage = false;
                    }
                }
            }
        }
    } while (!gotMessage);

    if (getRespMsg.has_getres())
    {
        retVal.status = getRespMsg.getres().status();
        retVal.vlen = getRespMsg.getres().value().size();
        retVal.value = new uint8_t[retVal.vlen];
        memcpy(retVal.value, (uint8_t *)getRespMsg.getres().value().data(), retVal.vlen);
    }
    else
    {
        cerr << "wrong message type" << endl;
        retVal.status = false;
    }
    // cout << "leaving get stub" << endl;
    return retVal;
}

// set up socket for calls to server
bool KVServiceStub::init()
{

    // Filling server information
    // TODO - need to add service lookup concept

    // --------------------------------------- DOING SERVER LOOKUP FOR SERVICE NAME --------------------------------------- //
    // ServerSearchInfo ServerSearchInfo = dnsLookup(svcName);

    // cerr << "THE SERVER NAME IS:.................................\n\n\n\n"
    //      << endl;
    // cerr << ServerSearchInfo.serverName << endl;
    // cerr << "THE SERVER NAME SET IN KV CLIENT STUB IS --------> "<<PORT << endl;
    // cerr << "THE SERVER PORT SET IN KV CLIENT STUB IS -------->"<<serverName << endl;

    // SOCKET CONFIGURATION
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT); // We don't know what the port is... until we search it in the service directory

    // look up address by name (Version 2 of network.cpp)
    struct addrinfo *res; // This will store the IPv4 address representation

    cerr << "------------------- BEFORE HERE --------------------" << endl;
    // DNS RESOLUTION
    // return one record in the res parameter with the address. The nice thing is that the address is already in a IPv4 address representation and can be used directly.
    int numAddr = getaddrinfo("", nullptr, nullptr, &res); // Does the DNS resolution meaning it converts the "kvserver" to its equivalent ip address e.g "192.116.123.12"
    cerr << "------------------- HERE --------------------" << endl;


    cerr << "number of address results is " << numAddr << endl;
    servaddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    cerr << "------------------- AFTER --------------------" << endl;
    freeaddrinfo(res);

    // old version that used address directly
    // inet_aton(serverAddrString.c_str(), &servaddr.sin_addr);

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return false;
    }

    // set socket to timeout in 1 second if reply not received
    // TODO this should be a parameter somehwere.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error");
        return false;
    }
    ready = true;
    return true;
}

void KVServiceStub::shutdown()
{
    if (!ready)
        return;
    close(sockfd);
    ready = false;
}

// ServerSearchInfo KVServiceStub::dnsLookup(string serviceName)
// {

//     ServerSearchInfo searchResult = dirSvcClientStub.searchService(svcName);

//     if (searchResult.status)
//     {

//         serverName = searchResult.serverName;
//         PORT = searchResult.serverPort;
//         }
//     return searchResult;
// }