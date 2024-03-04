#include "DirSvcService.hpp"

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

using namespace std;
using namespace string_literals;

#define close mclose
void mclose(int fd);

void DirSvcService::stop()
{
    alive = false;
}

// Sets the port that the dirSvcService will listen on 
void DirSvcService::setPort(in_port_t port)
{
    PORT = port;
}

// Note this method is not needed since the DNS server doesn't request a service 
void DirSvcService::setSvcName(string svcName)
{
    this->svcName = svcName;
}

/**
 * DNS server configures socket information and starts listening for incoming requests from the DirSvcClientStub
*/
void DirSvcService::start()
{

    // --------------------------------------- REGISTER SERVICE --------------------------------------- //
    cerr << "\n-------------------- Registering DNS --------------------" << endl;
    struct sockaddr_in servaddr, cliaddr;

    // searchService a socket to recieve messges
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return; // this will exit the service thread and stop the server
    }

    // clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interfact information for binding
    // the socket
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // whatever interface is available
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        return; // this will exit the service thread and stop the server
    }

    socklen_t len;
    int n;

    while (alive)
    {
        cerr << "\nwaiting for call from client\n"
             << endl;

        // wait for a mesage from a client
        len = sizeof(cliaddr); // len is value/result
        n = recvfrom(sockfd, (uint8_t *)udpMessage, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);

        // Received message
        cerr << "----------------- DNS SERVER RECEIVED SOMETHING -----------------\n"
             << endl;

        DIRSVC::dirSvcRequest receivedMsg;
        DIRSVC::dirSvcResponse replyMsg;

        if (!receivedMsg.ParseFromArray(udpMessage, n))
        {
            cerr << "Could not parse message" << endl;
            // ignore
        }

        // cerr << "message parsed" << endl;

        // Check the magic number
        if ((receivedMsg.magic()) != magic)
        {
            cerr << "service unrecognized message" << endl;
        }
        else
        {
            // start by copying version and serial to reply
            replyMsg.set_magic(magic);
            replyMsg.set_version(receivedMsg.version());
            replyMsg.set_serial(receivedMsg.serial());

            // POSSSIBLE PROBLEM
            if ((receivedMsg.version()) == version1x)
            {
                // dispatch version 1.x
                callMethodVersion1(receivedMsg, replyMsg);
            }
            else
            {
                cerr << "-------------------- Unrecognized Version --------------------" << endl;
                // For now ignore, message doesn't have a wrong version reply
            }

            // at this point in time the reply is complete send response back
            uint32_t msglen = replyMsg.ByteSizeLong();

            // double check size
            replyMsg.SerializeToArray(udpMessage, msglen);

            // Send message back to the client stub
            int server = sendto(sockfd, udpMessage, msglen,
                                MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
}

/**
 * Determines what message was requested by the DirSvcClientStub (either search, register, delete)
*/
void DirSvcService::callMethodVersion1(DIRSVC::dirSvcRequest &receivedMsg, DIRSVC::dirSvcResponse &replyMsg)
{
    if (receivedMsg.has_registerargs())
    {
        stringstream ss;
        ss << "DIR_SVC_SERVICE: register message requested" << endl;
        cerr << ss.str();

        const DIRSVC::registerRequest &preq = receivedMsg.registerargs();

        // STRUCT DEF:
        // struct ServerRegisterInfo
        // {
        //     string serviceName;
        //     string serverName;
        //     int serverPort;
        // };
        ServerRegisterInfo value;
        value.serviceName = preq.service_name();
        value.serverName = preq.server_name();
        value.serverPort = preq.server_port();

        bool registerRes = registerService(value);

        DIRSVC::registerResponse *presp = replyMsg.mutable_registerres();
        presp->set_status(registerRes);

        cerr << "\nDIR_SVC_SERVICE: END OF REGISTER SERVICE" << endl;
    }
    if (receivedMsg.has_searchargs())
    {
        stringstream ss;
        ss << "DIR_SVC_SERVICE: Search message requested" << endl;
        cerr << ss.str();

        const DIRSVC::searchRequest &searchReq = receivedMsg.searchargs();

        // STRUCT DEF:
        // struct ServerSearchInfo
        // {
        //     bool status;
        //     string serverName;
        //     int serverPort;
        // };

        cerr << "DIR_SVC_SERVICE: Searching for following service -------> " << searchReq.service_name() << "\n"
             << endl;
        ServerSearchInfo result = searchService(searchReq.service_name());

        cerr << "DIR_SVC_SERVICE: Search Resolved Status -------> " << result.status << endl;
        cerr << "DIR_SVC_SERVICE: Search Resolved Server Name -------> " << result.serverName << endl;
        cerr << "DIR_SVC_SERVICE: Search Resolved Server Port-------> " << result.serverPort << endl;

        DIRSVC::searchResponse *gr = replyMsg.mutable_searchres();
        gr->set_status(result.status);
        gr->set_server_name(result.serverName);
        gr->set_server_port(result.serverPort);

        cerr << "\nDIR_SVC_SERVICE: END OF SEARCH SERVICE" << endl;
    }
    if (receivedMsg.has_deleteargs())
    {
        stringstream ss;
        ss << "DIR_SVC_SERVICE: delete message requested" << endl;
        cerr << ss.str();

        const DIRSVC::deleteRequest &deleteReq = receivedMsg.deleteargs();

        // STRUCT DEF:
        // struct ServerSearchInfo
        // {
        //     bool status;
        //     string serverName;
        //     int serverPort;
        // };
        cerr << "DIR_SVC_SERVICE: deleting following service ------->" << deleteReq.service_name() << endl;
        bool status = deleteService(deleteReq.service_name());

        DIRSVC::deleteResponse *gr = replyMsg.mutable_deleteres();

        gr->set_status(status);

        cerr << "\nDIR_SVC_SERVICE: END OF DELETE SERVICE" << endl;
    }
}


/**
 * Searches through the unordered map to find the corresponding server name and server port based on the service name
*/
DirSvcService::ServerSearchInfo DirSvcService::searchService(string key)
{
    // STRUCT DEF:
    //  struct ServerSearchInfo
    //  {
    //      bool status;
    //      string serverName;
    //      int serverPort;
    //  };
    ServerSearchInfo res;

    auto it = servicesMap.find(key);

    if (it == servicesMap.end())
    {
        res.status = false;
        res.serverName = "";
        res.serverPort = 0;
    }
    else
    {
        res.status = true;
        res.serverName = it->second.serverName;
        res.serverPort = it->second.serverPort;
    }

    return res;
}

/**
 * Registers service into unordered map and stores the service name as the key and the server name and port as the value.
*/
bool DirSvcService::registerService(ServerRegisterInfo &value)
{
    bool status = false;
    ServerMapData serverMapData;
    serverMapData.serverName = value.serverName;
    serverMapData.serverPort = value.serverPort;

    // STRUCT DEF:
    // struct ServerMapData
    // {
    //     string serverName;
    //     int serverPort;
    // };
    std::cout << "\n"
              << "DIR_SVC_SERVICE: THIS IS THE SERVICE KEY------------------>: " << value.serviceName << std::endl;
    std::cout << "DIR_SVC_SERVICE: THIS IS THE SERVER KEY------------------>: " << value.serverName << std::endl;
    std::cout << "DIR_SVC_SERVICE: THIS IS THE SERVER PORT------------------>: " << value.serverPort << "\n"
              << std::endl;

    servicesMap[value.serviceName] = serverMapData;

    // You can directly return the result of std::unordered_map::insert
    return servicesMap.find(value.serviceName) != servicesMap.end() ? 1 : 0;
}


/**
 * Deletes service from unordered map to remove the service from the DNS.
*/
bool DirSvcService::deleteService(string key)
{
    auto it = servicesMap.find(key);

    if (it != servicesMap.end())
    {
        // If the key is found erase using the iterator
        servicesMap.erase(it);
        auto newIt = servicesMap.find(key);
        std::cout << "DIR_SVC_SERVICE: Deleted the following service -------->" << key
                  << endl;

        std::cout << "\n\nDIR_SVC_SERVICE: Remaining names in service server: " << endl;

        // Iterate over the unordered_map using a range-based for loop
        for (const auto &pair : servicesMap)
        {
            std::cout << "Key: " << pair.first << ", Server Name: " << pair.second.serverName << ", Server Port: " << pair.second.serverPort << "\n\n"
                      << std::endl;
        }

        return true;
    }

    std::cout << "DIR_SVC_SERVICE: Can't find following service to delete" << key
              << endl;

    // Return false if the key was not found in the map
    return false;
}
