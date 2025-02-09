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

/**
 * Same functionality as KVPUT but instead of storing a key and a value we are using a service name as the key and the value is the server name and server port.
 * The DirSvcClientStub is interface for how clients and kv servers will interact with the DNS server.
 * This method is used for registering a service to the DNS server.
 */
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

    const int maxRetries = 5; // Maximum number of retries
    int retries = 0;
    bool success = false;

    // At least once semantics
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

        int n = sendto(sockfd, (const char *)buffer, blen, MSG_CONFIRM,
                       (const struct sockaddr *)&servaddr, sizeof(servaddr));

        if (n < 0)
        {
            // Handle send error
            ++retries;
            continue;
        }

        DIRSVC::dirSvcResponse regRespMsg;
        socklen_t len = sizeof(servaddrreply);

        n = recvfrom(sockfd, (char *)buffer, MAXMSG, MSG_WAITALL,
                     (struct sockaddr *)&servaddrreply, &len);

        if (n == -1)
        {
            // Handle recvfrom error (e.g., timeout)
            ++retries;
            continue;
        }

        // Verify header details and correct message type
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

    return success;
}

/**
 * Same functionality as KVGET but instead of getting a value (arbitrary string) we are fetching the server name and server port.
 * The DirSvcClientStub is interface for how clients and kv servers will interact with the DNS server.
 * This method is used for searching for a service based on a service name.
 */
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

    DIRSVC::dirSvcResponse searchRespMsg;
    const int maxRetries = 5; // Maximum number of retries
    int retries = 0;
    bool success = false;

    // At least once semantics
    while (retries < maxRetries && !success)
    {
        len = sizeof(struct sockaddr_in);

        n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        if (n == -1)
        {
            std::cerr << " n == -1" << std::endl;
            // return retVal;

            // Handle recvfrom error (e.g., timeout)
            ++retries;
            continue;
        } // null ptrs and falls status };

        // Verify header details and correct message type
        if (searchRespMsg.ParseFromArray(buffer, n) &&
            searchRespMsg.magic() == 'DNS' &&
            searchRespMsg.version() == version1x &&
            searchRespMsg.serial() == serial &&
            searchRespMsg.has_searchres())
        {

            success = searchRespMsg.searchres().status();
        }
        else
        {
            // Parse error or wrong message received, retry
            std::cerr << " retrying" << std::endl;
            ++retries;
        }
    }

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

// Configures the directory service client stub to talk to the DNS server by configuring the destination IP address and port
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

// Shuts down service
void DirSvcServiceStub::shutdown()
{
    if (!ready)
        return;
    close(sockfd);
    ready = false;
}

/**
 * This method is used for deleting a service from the DNS based on a provided service name.
 */
bool DirSvcServiceStub::deleteService(string key)
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

    struct sockaddr_in servaddrreply;

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];

    DIRSVC::dirSvcRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial);

    DIRSVC::deleteRequest *gr = msg.mutable_deleteargs();

    gr->set_service_name(key);
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
    std::cout << "\n\nDIR_SVC_CLIENT_STUB: Key sent to delete ---------> " << dec << key << endl;

    // send the message3
    n = sendto(sockfd, (const char *)buffer, blen,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    DIRSVC::dirSvcResponse deleteRespMsg;
    const int maxRetries = 5; // Maximum number of retries
    int retries = 0;
    bool success = false;

    // At least once semantics
    while (retries < maxRetries && !success)
    {
        len = sizeof(struct sockaddr_in);

        n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&servaddrreply, &len);

        if (n == -1)
        {
            // Handle recvfrom error (e.g., timeout)
            ++retries;
            continue;
        }

        if (deleteRespMsg.ParseFromArray(buffer, n) &&
            deleteRespMsg.magic() == 'DNS' &&
            deleteRespMsg.version() == version1x &&
            deleteRespMsg.serial() == serial &&
            deleteRespMsg.has_deleteres())
        {

            success = deleteRespMsg.deleteres().status();
        }
        else
        {
            // Parse error or wrong message received, retry
            ++retries;
        }
    }

    if (deleteRespMsg.has_deleteres())
    {
        success = deleteRespMsg.deleteres().status();
    }
    else
    {
        cerr << "wrong message type" << endl;
        success = false;
    }
    return success;
}