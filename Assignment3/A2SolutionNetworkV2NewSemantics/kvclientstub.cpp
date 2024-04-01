#include "kvclientstub.hpp"
#include "servicedir/srvcdirclientstub.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

bool KVServiceStub::kvPut(int32_t key, const uint8_t *value, uint16_t vlen)
{
    stringstream ss;
    // init if needed
    if (!ready)
        init();

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG]; // to serialize into
    struct sockaddr_in servaddrreply;

    // get the current value of serial for this request.
    uint32_t serial = this->serial++;

    // marshal parameters to send.
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

    ss = stringstream();
    // ss << "In put, message is " << endl;
    // ss << HexDump{buffer,blen} << endl;
    ss << "\nKVCLIENTSTUB: Sending to " << inet_ntoa(servaddr.sin_addr) << endl;
    cerr << ss.str();
    // std::cout << "blen = " << dec << blen << endl;

    // send the message to the server.
    bool completed = false;
    uint32_t numTries = 0;
    E477KV::kvResponse putRespMsg;
    while (!completed)
    {
        n = sendto(sockfd, (const char *)buffer, blen,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        // ss = stringstream();
        // ss << "put client sendto n = " << n << endl;
        // cerr << ss.str();

        bool gotMessage = true;
        do
        {
            len = sizeof(struct sockaddr_in);
            n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                         MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

            // check for timeout here..
            if (n == -1)
            {
                numTries++;
                gotMessage = false;
                if (numTries > maxTries)
                    return false;
            }
            else if (!putRespMsg.ParseFromArray(buffer, n))
            {
                cerr << "Couild not parse message" << endl;
                // wait for another mesage
            }
            else if (putRespMsg.magic() != 'E477')
            {
                cerr << "Magic Mismatch" << endl;
                gotMessage = false;
            }
            else if (msg.version() != putRespMsg.version())
            {
                cerr << "Version Mismatch" << endl;
                gotMessage = false;
            }
            else if (msg.serial() != putRespMsg.serial())
            {
                // wait for another message is the serial number is wrong.
                cerr << "Serial Numnbers Mismatch" << endl;
                gotMessage = false;
            }
            else
            {
                // have a reasonble message
                completed = true;
            }
        } while (!gotMessage);
    }

    bool returnRes = false;
    if (putRespMsg.has_putres())
    {
        returnRes = putRespMsg.putres().status();
    }
    else
    {
        cerr << "wrong message type" << endl;
        returnRes = false;
    }

    return returnRes;
}

kvGetResult KVServiceStub::kvGet(int32_t key)
{
    // init if needed
    stringstream ss;

    if (!ready)
        init();

    kvGetResult retVal;
    retVal.status = false;
    retVal.value = nullptr;
    retVal.vlen = 0;

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

    // ss= strinstream();
    // ss << HexDump{buffer,blen} << endl;
    // ss << "blen = " << dec << blen << endl;
    // std::cout << ss.str();

    // send the messa
    bool completed = false;
    uint32_t numTries = 0;
    E477KV::kvResponse getRespMsg;
    while (!completed)
    {
        n = sendto(sockfd, (const char *)buffer, blen,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        // std::cout << "client sendto n = " << n << endl;

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
                numTries++;
                gotMessage = false;
                if (numTries > maxTries)
                    return retVal;
            }
            else if (!getRespMsg.ParseFromArray(buffer, n))
            {
                cerr << "Couild not parse message" << endl;
                // wait for another mesage
            }
            else if (getRespMsg.magic() != magic)
            {
                gotMessage = false;
            }
            else if (msg.version() != getRespMsg.version())
            {
                cerr << "Version Mismatch" << endl;
                gotMessage = false;
            }
            else if (msg.serial() != getRespMsg.serial())
            {
                // wait for another message is the serial number is wrong.
                cerr << "Serial Numnbers Mismatch" << endl;
                gotMessage = false;
            }
            else
            {
                // have a reasonable message
                completed = true;
            }
        } while (!gotMessage);
    }

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

    stringstream ss;
    ss << "\nKVServiceStub: Entering init" << endl;
    cerr << ss.str();

    if (svcName.empty())
    {
        cerr << "Does not know service name!! " << endl;
        return false;
    }

    SvcDirClientStub svcDirService(name + ".svcDirstub");
    svcDirService.setServerName(svcDirServer);

    ss = stringstream();
    ss << "Client Stub " << name << " querying svcDirServer:" << svcDirServer << " for the serverice: " << svcName << endl;
    cerr << ss.str();

    SvcDirSearchResult svcRes = svcDirService.searchService(svcName);
    if (!svcRes.status)
    {
        cerr << "Service server not found!! " << endl;
        return false;
    }
    ss = stringstream();
    ss << "Client Stub " << name << " answer is server " << svcRes.serverName << ", port " << svcRes.port << endl;
    cerr << ss.str();

    // Filling server information
    // TODO - need to add service lookup concept
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(svcRes.port);

    struct addrinfo *addr_result;
    int errcode = getaddrinfo(svcRes.serverName.c_str(), nullptr, nullptr, &addr_result);
    if (errcode != 0)
    {
        cout << "error finding address of " << svcRes.serverName << gai_strerror(errcode) << endl;
        return false;
    }
    else
    {
        if (addr_result == nullptr)
        {
            cout << "getaddr info said 0, but no pointer?" << endl;
        }
        else
        {
            if (addr_result->ai_next != nullptr)
            {
                cout << "should only be one result" << endl;
            }
            else
            {
                cout << "CLIENT STUB: address of kvserver is: " << inet_ntoa(((sockaddr_in *)addr_result->ai_addr)->sin_addr) << endl;
                servaddr.sin_addr = ((sockaddr_in *)addr_result->ai_addr)->sin_addr;
            }
            freeaddrinfo(addr_result);
        }
    }

    // don't really need to implement this one in network as it doesn't use a socket
    // inet_aton(serverAddrString.c_str(), &servaddr.sin_addr);// = inet_aton("127.0.0.1");//INADDR_ANY;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // set socket to timeout in 1 second if reply not received
    // TODO this should be a parameter somehwere.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error");
        exit(1);
    }
    ready = true;
    return true;
}

void KVServiceStub::shutdown()
{
    if (!ready)
        return;
    close(sockfd);
}
