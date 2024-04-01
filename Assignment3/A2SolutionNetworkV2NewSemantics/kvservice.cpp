
#include "kvservice.hpp"

#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

using namespace std;
using namespace string_literals;

#define close mclose
void mclose(int fd);

void KVServiceServer::stop()
{
    alive = false;
}

void KVServiceServer::start()
{
    stringstream ss;
    struct sockaddr_in servaddr, cliaddr;

    // ss << "in kvserviceServer::start" << endl;
    // ss << "Service Name is " << name << endl;
    // ss << "Node Name is " << nodeName() << endl;
    // cerr << ss.str();

    // open the GDBM file.
    if (DBMFileName.empty())
    {
        cerr << "Name of DB file not specified" << endl;
        return;
    }

    if (svcName.empty())
    {
        cerr << "Service name has not been defined" << endl;
        return;
    }

#ifdef __APPLE__
    dataFile = dbm_open(DBMFileName.c_str(), (O_RDWR | O_CREAT), 0644);
    if (!dataFile)
    {
        cerr << "NDBM Error: could not open database file" << endl;
        return;
    }
#else
    dataFile = gdbm_open(DBMFileName.c_str(), 0, GDBM_WRCREAT, 0644, NULL);
    if (!dataFile)
    {
        cerr << "GDBM Error: " << gdbm_strerror(gdbm_errno) << endl;
        return;
    }
#endif

    // get a socket to recieve messges
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return; // this will return from the service thread
    }

    // clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interfact information for binding
    // the socket
    servaddr.sin_family = AF_INET;         // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // whatever interface is available
    servaddr.sin_port = htons(port);

    // ss = stringstream();
    // ss << "Service " << name << "binding to port " << port << endl;
    // cerr <<  ss.str();

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        cerr << "xxxxx" << endl;
        return; // this will return from the service thread
    }

    // port is open and bound, register with service server.
    ss = stringstream();
    ss << "Registering server " << nodeName() << ", port " << port << " as service " << svcName
       << endl;
    cerr << ss.str();
    bool svcRes = svcDirService.registerService(svcName, nodeName(), port);
    ss = stringstream();
    ss << "Register result is " << svcRes << endl;
    cerr << ss.str();

    socklen_t len;
    int n;
    // cerr << "alive = " << alive << endl;
    while (alive)
    {
        // cerr << "waiting for call from client" << endl;

        // wait for a mesage from a client
        len = sizeof(cliaddr); // len is value/result
        n = recvfrom(sockfd, (uint8_t *)udpMessage, MAXMSG,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        string replicasPrimaryServerAddress;
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(cliaddr.sin_port);
        // Source Address
        if (n > 0 && !isPrimary)
        {

            // Logging client IP and port
            cout << "KVSERVICE: Received message from " << client_ip << ":" << client_port << endl;

            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(primaryServer->portNumber);
            struct addrinfo *addr_result;
            int errcode = getaddrinfo(primaryServer->name.c_str(), nullptr, nullptr, &addr_result);
            if (errcode != 0)
            {
                cout << "error finding address of " << primaryServer->name << gai_strerror(errcode) << endl;
                return;
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
                        replicasPrimaryServerAddress = inet_ntoa(((sockaddr_in *)addr_result->ai_addr)->sin_addr);
                        cout << "KVSERVICE: The primary server name is: " << primaryServer->name << endl;
                        cout << "KVSERVICE: The primary server address is: " << replicasPrimaryServerAddress << endl;
                        cout << "KVSERVICE: The source ip address is coming from: " << client_ip << endl;
                        servaddr.sin_addr = ((sockaddr_in *)addr_result->ai_addr)->sin_addr;
                    }
                    freeaddrinfo(addr_result);
                }
            }
        }

        ss = stringstream();
        ss << "KVSERVICE: " << name << " kv server received " << n << " bytes." << endl;
        // ss << HexDump{udpMessage, (uint32_t)n} << endl;
        cerr << ss.str();

        E477KV::kvRequest receivedMsg;
        E477KV::kvResponse replyMsg;

        if (!receivedMsg.ParseFromArray(udpMessage, n))
        {
            ss = stringstream();
            ss << "KVService for node " << nodeName() << " couild not parse message" << endl;
            cerr << ss.str();
            // ignore
        }

        // cerr << "message parsed" << endl;

        if ((receivedMsg.magic()) != 'E477')
        {
            cerr << "service unrecognized magic number" << endl;
        }
        else
        {
            // start by copying version and serial to reply
            replyMsg.set_magic('E477');
            replyMsg.set_version(receivedMsg.version());
            replyMsg.set_serial(receivedMsg.serial());

            if (!isPrimary && replicasPrimaryServerAddress != client_ip)
            {
                cerr << "KVSERVICE REPLICA: INVALID SOURCE ADDRESS!" << endl;
                return;
            }
            if ((receivedMsg.version() & 0xFF00) == version1x)
            {
                if (!isPrimary)
                    cerr << "KVSERVICE REPLICA: SOURCE ADDRESS IS FROM PRIMARY SERVER!" << endl;

                // dispatch version 1.x
                // AKA DO THE RESPECTIVE PUT OR GET ACTION
                callMethodVersion1(receivedMsg, replyMsg);
            }
            else
            {
                cerr << "unrecognized version" << endl;
                // For now ignore, message doesn't have a wrong version reply
            }
            // at this point in time the reply is complete
            // send response back
            uint32_t msglen = replyMsg.ByteSizeLong();
            // double check size
            replyMsg.SerializeToArray(udpMessage, msglen);
            // cerr << "reply message" << HexDump{udpMessage,msglen} ;

            int servern = sendto(sockfd, udpMessage, msglen,
                                 MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            // cerr << "server sent " << servern << " bytes" << endl;
        }
    }

    // imfprtimnat;eu this is not reached because of bug
    // with alive flag
    close(sockfd);
    bool remRes = svcDirService.removeService(svcName);

#ifdef __APPLE__
    dbm_close(dataFile);
    dataFile = nullptr;
#else
    gdbm_close(dataFile);
    dataFile = nullptr;
#endif
}

void KVServiceServer::callMethodVersion1(E477KV::kvRequest &receivedMsg, E477KV::kvResponse &replyMsg)
{
    if (receivedMsg.has_putargs())
    {
        stringstream ss;
        ss << "KVSERVICE: put message requested" << endl;
        cerr << ss.str();

        const E477KV::putRequest &preq = receivedMsg.putargs();

        int key = preq.key();
        string valueAsStr = preq.value();

        bool putRes = kvPut(key, (uint8_t *)valueAsStr.c_str(), valueAsStr.length());

        E477KV::putResponse *presp = replyMsg.mutable_putres();
        presp->set_status(putRes);

        cout << "\nKVSERVICE: Server Name: " << name << endl;
        cout << "KVSERVICE: This is the key: " << (int32_t)key << endl;
        cout << "KVSERVICE: This is the value: " << valueAsStr << endl;
        cout << "KVSERVICE: This is the value size: " << (uint16_t)valueAsStr.size() << endl;
        cout << "KVSERVICE: put response is: " << putRes << endl;
        cout << "KVSERVICE: Is it a Primary server: " << isPrimary << endl;

        // Assuming this is the primary server and the put response succeeded
        if (isPrimary)
        {

            bool replicasUpdated = true;
            for (auto it = replicas->begin(); it != replicas->end(); ++it)
            {
                cout << "KVSERVICE: replica name: " << it->name << endl;
                cout << "KVSERVICE: replica port: " << it->portNumber << endl;

                // Updating all of the replicas
                kvPutReplica((int32_t)key, (const uint8_t *)valueAsStr.data(), (uint16_t)valueAsStr.size(), it->name, it->portNumber);
            }
        }
        else
        {
            kvGetResult result = kvGet(key);
            cout << "KVSERVICE REPLICA GET: Key used --> " << key << endl;
            cout << "KVSERVICE REPLICA GET: Status --> " << result.status << endl;
            cout << "KVSERVICE REPLICA GET: Value --> " << result.value << endl;
            cout << "KVSERVICE REPLICA GET: Value Length --> " << result.vlen << endl;

            cout << "KVSERVICE: END OF REPLICA\n"
                 << endl;
        }
    }
    if (receivedMsg.has_getargs())
    {
        stringstream ss;
        ss << "KVSERVICE: get message requested" << endl;
        cerr << ss.str();

        const E477KV::getRequest &greq = receivedMsg.getargs();

        int key = greq.key();

        kvGetResult result = kvGet(key);

        E477KV::getResponse *gr = replyMsg.mutable_getres();
        gr->set_status(result.status);
        gr->set_value(string((char *)result.value, result.vlen));
    }
}

bool KVServiceServer::kvPut(int key, const uint8_t *value, uint16_t vlen)
{
    stringstream ss;
    // ss = stringstream();
    // ss << "In kvPut, key is " << key << endl;
    // cerr << ss.str();

#ifdef __APPLE__
    datum dbkey = {(void *)&key, sizeof(int)};
    datum dbvalue = {(void *)value, vlen};
    bool result = dbm_store(dataFile, dbkey, dbvalue, DBM_REPLACE);
#else
    datum dbkey = {(char *)&key, sizeof(int)};
    datum dbvalue = {(char *)value, vlen};
    bool result = gdbm_store(dataFile, dbkey, dbvalue, GDBM_REPLACE);
#endif

    return !result;
}

kvGetResult KVServiceServer::kvGet(int key)
{
    stringstream ss;
    // ss = stringstream();
    // ss << "in kvGet, key is " << key << endl;
    // cerr << ss.str();

    kvGetResult res;

#ifdef __APPLE__
    datum dbkey = {(void *)&key, sizeof(int)};
    datum value = dbm_fetch(dataFile, dbkey);
#else
    datum dbkey = {(char *)&key, sizeof(int)};
    datum value = gdbm_fetch(dataFile, dbkey);
#endif
    if (value.dptr == NULL)
    {
        res.status = false;
        res.value = nullptr;
        res.vlen = 0;
    }
    else
    {
        res.status = true;
        res.value = (uint8_t *)value.dptr;
        res.vlen = value.dsize;
    }

    return res;
}

void KVServiceServer::setServiceDirServer(string name) { svcDirService.setServerName(name); }
void KVServiceServer::setServiceName(string svcName) { this->svcName = svcName; }

// New methods:

/**
 * Will send back a list of server addresses and file descriptors (which are integers)
 */
KVServiceServer::sendToAddress KVServiceServer::init(string serverName, int replicaPort)
{
    int replicaSockfd;
    sendToAddress res;
    struct sockaddr_in servaddr; // Added this local variable here

    // Filling server information of the replicas. The primary thing we are interested here is the servaddr struct which will hold the destination server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(replicaPort);

    struct addrinfo *addr_result;
    int errcode = getaddrinfo(serverName.c_str(), nullptr, nullptr, &addr_result);
    if (errcode != 0)
    {
        cout << "error finding address of " << serverName << gai_strerror(errcode) << endl;
        return res;
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
                cout << "KVSERVICE: address of kvserver is: " << inet_ntoa(((sockaddr_in *)addr_result->ai_addr)->sin_addr) << endl;
                servaddr.sin_addr = ((sockaddr_in *)addr_result->ai_addr)->sin_addr;
            }
            freeaddrinfo(addr_result);
        }
    }

    // don't really need to implement this one in network as it doesn't use a socket
    // inet_aton(serverAddrString.c_str(), &servaddr.sin_addr);// = inet_aton("127.0.0.1");//INADDR_ANY;

    // Creating socket file descriptor
    if ((replicaSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // set socket to timeout in 1 second if reply not received
    // TODO this should be a parameter somehwere.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(replicaSockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error");
        exit(1);
    }

    // Save the real server address and the socket file descriptor
    res.servaddr = servaddr;
    res.sockfd = replicaSockfd;
    // ready = true;

    return res;
}

bool KVServiceServer::kvPutReplica(int32_t key, const uint8_t *value, uint16_t vlen, string serverName, int serverPort)
{
    stringstream ss;

    // init if needed
    sendToAddress serverAddressInfo;
    // if (!ready)
    // {
    serverAddressInfo = init(serverName, serverPort);
    // }

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

    // ss = stringstream();
    // ss << "In put, message is " << endl;
    // ss << HexDump{buffer,blen} << endl;

    ss << "\nKVSERVICE: Sending to " << inet_ntoa(serverAddressInfo.servaddr.sin_addr) << endl;
    cerr << ss.str();
    // std::cout << "blen = " << dec << blen << endl;

    // send the message to the server.
    bool completed = false;
    uint32_t numTries = 0;
    E477KV::kvResponse putRespMsg;
    while (!completed)
    {
        n = sendto(serverAddressInfo.sockfd, (const char *)buffer, blen,
                   MSG_CONFIRM, (const struct sockaddr *)&serverAddressInfo.servaddr, sizeof(serverAddressInfo.servaddr));
        // ss = stringstream();
        // ss << "put client sendto n = " << n << endl;
        // cerr << ss.str();

        bool gotMessage = true;
        do
        {
            len = sizeof(struct sockaddr_in);
            n = recvfrom(serverAddressInfo.sockfd, (char *)buffer, MAXMSG,
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
