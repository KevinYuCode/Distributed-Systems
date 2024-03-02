#include "DirSvcClientStub.hpp"

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

void DirSvcServiceStub::setSvcName(string svcName)
{
    this->svcName = svcName;
}

// -------------------------------------------------- Register Service -------------------------------------------------- //
bool DirSvcServiceStub::registerService(string svcName, string serverName, int svcPort)
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

    const int maxRetries = 1; // Maximum number of retries
    int retries = 0;
    bool success = false;

    while (retries < maxRetries && !success)
    {
        DIRSVC::dirSvcRequest msg;
        msg.set_magic(magic);
        msg.set_version(version1x);
        msg.set_serial(serial);

        DIRSVC::registerRequest *pr = msg.mutable_registerargs();
        pr->set_service_name(svcName);
        pr->set_server_name(serverName);
        pr->set_server_port(svcPort);
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
        std::cout << "blen = " << dec << blen << endl;

        int n = sendto(sockfd, (const char *)buffer, blen, MSG_CONFIRM,
                       (const struct sockaddr *)&servaddr, sizeof(servaddr));

        cerr << "after sendto in dirsvcclienstub" << n << endl;

        if (n < 0)
        {
            // Handle send error
            ++retries;
            continue;
        }

        DIRSVC::dirSvcResponse regRespMsg;
        socklen_t len = sizeof(servaddrreply);
        cerr << "Before" << n << endl;

        n = recvfrom(sockfd, (char *)buffer, MAXMSG, MSG_WAITALL,
                     (struct sockaddr *)&servaddrreply, &len);

        cerr << "after recv from in dirsvcclienstub" << n << endl;

        if (n == -1)
        {
            // Handle recvfrom error (e.g., timeout)
            ++retries;
            continue;
        }

        if (regRespMsg.ParseFromArray(buffer, n) &&
            regRespMsg.magic() == 'DNS' &&
            regRespMsg.version() == version1x &&
            regRespMsg.serial() == serial &&
            regRespMsg.has_registerres())
        {

            success = regRespMsg.registerres().status();
        }
        else
        {
            // Parse error or wrong message received, retry
            ++retries;
        }
    }

    if (!success)
    {
        cerr << "Failed to put service name after " << maxRetries << " attempts." << endl;
    }
    cerr << "success in dirsvcclient register service:" << success << endl;

    return success;
}

ServerSearchInfo DirSvcServiceStub::searchService(string svcName)
{
    // init if needed
    ServerSearchInfo retVal;
    retVal.status = false;
    retVal.serverName = "";
    retVal.serverPort = 0;
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

    DIRSVC::dirSvcRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial);

    DIRSVC::searchRequest *gr = msg.mutable_searchargs();

    gr->set_service_name(svcName);
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
    n = sendto(sockfd, (const char *)buffer, blen,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    // std::cout << "client sendto n = " << n << endl;

    DIRSVC::dirSvcResponse searchRespMsg;
    bool gotMessage = true;
    do
    {
        len = sizeof(struct sockaddr_in);
    std::cout << "\n\nDID IT SEND------------------------------------" << dec << blen << endl;

        n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);
        
        std::cout << "AFTER RECV FROM \n" << dec << blen << endl;

        // std::stringstream ss;
        // ss << "client recieved = " << n << std::endl;
        // std::cout << ss.str();
        // std::cout << HexDump{buffer,(uint32_t)n} << endl;
        //  check for timeout here..
        if (n == -1)
        {
            return retVal;
        } // null ptrs and falls status };

        if (!searchRespMsg.ParseFromArray(buffer, n))
        {
            cerr << "Couild not parse message" << endl;
            // wait for another mesage
        }
        else
        {
            if (searchRespMsg.magic() != magic)
            {
                gotMessage = false;
            }
            else
            {
                if (msg.version() != searchRespMsg.version())
                {
                    cerr << "Version Mismatch" << endl;
                    gotMessage = false;
                }
                else
                {
                    // wait for another message is the serial number is wrong.
                    if (msg.serial() != searchRespMsg.serial())
                    {
                        cerr << "Serial Numnbers Mismatch" << endl;
                        gotMessage = false;
                    }
                }
            }
        }
    } while (!gotMessage);

    if (searchRespMsg.has_searchres())
    {
        retVal.status = searchRespMsg.searchres().status();
        retVal.serverName = searchRespMsg.searchres().server_name();
        retVal.serverPort = searchRespMsg.searchres().server_port();
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
bool DirSvcServiceStub::init()
{

    // Filling server information
    // TODO - need to add service lookup concept

    // --------------------------------------- DOING SERVER LOOKUP FOR SERVICE NAME --------------------------------------- //

    // SOCKET CONFIGURATION
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8081); // We don't know what the port is... until we search it in the service directory

    // look up address by name (Version 2 of network.cpp)
    struct addrinfo *res; // This will store the IPv4 address representation

    // DNS RESOLUTION
    // return one record in the res parameter with the address. The nice thing is that the address is already in a IPv4 address representation and can be used directly.
    int numAddr = getaddrinfo("dns_server", nullptr, nullptr, &res); // Does the DNS resolution meaning it converts the "kvserver" to its equivalent ip address e.g "192.116.123.12"

    cerr << "number of address results is " << numAddr << endl;
    servaddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
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

void DirSvcServiceStub::shutdown()
{
    if (!ready)
        return;
    close(sockfd);
    ready = false;
}
