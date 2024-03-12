#include "srvcdirclientstub.hpp"
#include <netdb.h>

#include <chrono>

using namespace std;
using namespace std::string_literals;

#define close mclose
void mclose(int fd);

bool SvcDirClientStub::registerService(std::string serviceName, std::string serverName, uint32_t port){
    // init if needed
    if (!ready) init();

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];	 // to serialize into
    struct sockaddr_in	 servaddrreply;

    // get the current value of serial for this request.
    uint32_t serial = this->serial++;

    // marshal parameters to send.
    ServiceDirectory::svcDirRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial++);
    ServiceDirectory::registerRequest * regReq = msg.mutable_registerargs();
    regReq -> set_servicename(serviceName);
    regReq -> set_servername(serverName);
    regReq -> set_port(port);
    blen = msg.ByteSizeLong();
    if (blen > MAXMSG){
        // too long??
	std::cerr << " *** msg too long" << std::endl;
	// errno = ???
	return false;
    }
    msg.SerializeToArray(buffer, blen);

    // send the message to the server.
    bool completed = false;
    int numTries = 0;
    ServiceDirectory::svcDirResponse regRespMsg;

    while (!completed){
	n = sendto(sockfd, (const char *)buffer, blen,
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

	// wait for the reply
	bool gotMessage = true;
	do {
	    len = sizeof(struct sockaddr_in);
	    n = recvfrom(sockfd, (char *)buffer, MAXMSG,
				MSG_WAITALL, (struct sockaddr *) &servaddrreply, &len);
	    //std::stringstream ss;
	    //ss << "client recieved = " << n << std::endl;
	    //std::cout << ss.str();

	    if (n==-1) { 
		numTries++;
		bool gotMessage = false;
		if (numTries > maxTries) return false;
	    }  else if (!regRespMsg.ParseFromArray(buffer,n)){
		cerr << "Could not parse message register" << endl;
		bool gotMessage = false;
	    } if (regRespMsg.magic() != magic){
		cerr << "Magic number is wrong" << endl;
		bool gotMessage = false;
	    } else  if (msg.serial() != regRespMsg.serial()){
		// wait for another message is the serial number is wrong.
		cerr << "Serial Numnbers do not match" << endl;
		bool gotMessage = false;
	    } else if (msg.version() != regRespMsg.version()){
		cerr << "Version do not match" << endl;
		bool gotMessage = false;
	    } else {
	        completed = true;
	    }
	} while(!gotMessage);
    }

    bool returnRes = false;
    if (regRespMsg.has_registerres()){
      returnRes = regRespMsg.registerres().status();
    } else {
      cerr << "wrong message type" << endl;
	returnRes  = false;
    }

    return returnRes;
}

SvcDirSearchResult SvcDirClientStub::searchService(std::string serviceName){
    // init if needed
    if (!ready) init();
    
    SvcDirSearchResult retVal;
    retVal.status=false;
    retVal.serverName = "";
    retVal.port = 0;

    struct sockaddr_in	 servaddrreply;

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];
    ServiceDirectory::svcDirRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial++);
    ServiceDirectory::searchRequest * searchReq = msg.mutable_searchargs();

    searchReq -> set_servicename(serviceName);
    blen = msg.ByteSizeLong();
    if (blen > MAXMSG){
        // too long??
	std::cerr << " *** msg too long" << std::endl;
	// errno = ???
	retVal.status = false;
	return retVal;
    }
    msg.SerializeToArray(buffer, blen);
    //std::cout << HexDump{buffer,blen} << endl;
    //std::cerr << "******blen = " << dec << blen << endl;


    // send the message
    bool completed = false;
    int numTries = 0;
    ServiceDirectory::svcDirResponse searchRespMsg;

    while (!completed){
	n = sendto(sockfd, (const char *)buffer, blen,
		MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
	//std::cout << "client x sendto n = " << n << endl;

	bool gotMessage = true;
	do {
	    std::cout << "client waiting " << endl;
	    len = sizeof(struct sockaddr_in);
	    n = recvfrom(sockfd, (char *)buffer, MAXMSG,
				MSG_WAITALL, (struct sockaddr *) &servaddrreply, &len);
	    std::stringstream ss;
	    //ss << "client recieved = " << n << std::endl;
	    //std::cout << ss.str();
	    //std::cout << HexDump{buffer,(uint32_t)n} << endl;

	    if (n == -1){
		//std::cout << "**** TIMEOUT client sendto n = " << n << endl;
		numTries ++;
		bool gotMessage = false;
		if (numTries == maxTries) return retVal; // already false
	    } else if (!searchRespMsg.ParseFromArray(buffer,n)){
		cerr << "Could not parse message (search)" << endl;
		bool gotMessage = false;
	    } else if (msg.magic() != magic){
		cerr << "Magic Numbers Do Not Match" << endl;
		bool gotMessage = false;
	    } else if (msg.serial() != searchRespMsg.serial()){
		cerr << "Serial numnbers do not match" << endl;
		bool gotMessage = false;
	    } else if (msg.version() != searchRespMsg.version()){ 
		cerr << "Version do not match" << endl;
		bool gotMessage = false;
	    } else {
	        completed = true;
	    }
	} while(!gotMessage);
    }

    if (searchRespMsg.has_searchres()){
        retVal.status = searchRespMsg.searchres().status();
        retVal.serverName = searchRespMsg.searchres().servername();
        retVal.port = searchRespMsg.searchres().port();
    } else {
      cerr << "wrong message type" << endl;
	retVal.status=false;
    }
    //cout << "leaving search stub" << endl;
    return retVal;
}

bool SvcDirClientStub::removeService(std::string serviceName){
    // init if needed
    if (!ready) init();
    
    bool retVal=false;

    struct sockaddr_in	 servaddrreply;

    int n;
    socklen_t len;
    uint32_t blen = MAXMSG;
    uint8_t buffer[MAXMSG];
    ServiceDirectory::svcDirRequest msg;
    msg.set_magic(magic);
    msg.set_version(version1x);
    msg.set_serial(serial++);
    ServiceDirectory::removeRequest * remReq = msg.mutable_removeargs();

    remReq -> set_servicename(serviceName);
    blen = msg.ByteSizeLong();
    if (blen > MAXMSG){
        // too long??
	std::cerr << " *** msg too long" << std::endl;
	// errno = ???
	retVal = false;
	return retVal;
    }
    msg.SerializeToArray(buffer, blen);
    //std::cout << HexDump{buffer,blen} << endl;
    //std::cout << "blen = " << dec << blen << endl;
    

    // send the message3
    
    bool completed = false;
    int numTries = 0;
    ServiceDirectory::svcDirResponse removeRespMsg;

    while (!completed){
        n = sendto(sockfd, (const char *)buffer, blen,
                   MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        //std::cout << "client sendto n = " << n << endl;
        
        bool gotMessage = true;
        do {
            len = sizeof(struct sockaddr_in);
            n = recvfrom(sockfd, (char *)buffer, MAXMSG,
                         MSG_WAITALL, (struct sockaddr *) &servaddrreply, &len);
            
            //std::stringstream ss;
            //ss << "client recieved = " << n << std::endl;
            //std::cout << ss.str();
            //std::cout << HexDump{buffer,(uint32_t)n} << endl;
            if (n == -1){
                //std::cout << "**** TIMEOUT client sendto n = " << n << endl;
                numTries ++;
                bool gotMessage = false;
                if (numTries == maxTries) return retVal;
            } else if (!removeRespMsg.ParseFromArray(buffer,n)){
                cerr << "Could not parse message remove" << endl;
                bool gotMessage = false;
            } else if (msg.magic() != magic){
                cerr << "Magic Numbers Do Not Match" << endl;
                bool gotMessage = false;
            } else if (msg.serial() != removeRespMsg.serial()){
                cerr << "Serial Numnbers do not Match" << endl;
                bool gotMessage = false;
            } else if (msg.version() != removeRespMsg.version()){
                cerr << "Version Mismatch" << endl;
                bool gotMessage = false;
            }
            } while(!gotMessage);
    }
    if (removeRespMsg.has_removeres()){
        retVal = removeRespMsg.removeres().status();
    } else {
        cerr << "wrong message type" << endl;
        retVal=false;
    }
    return retVal;
}

// set up socket for calls to server
void SvcDirClientStub::init(){
    //cerr << "entering client stub init" << endl;
// TODO - this should really be protected with a mutex
    if (serverName.empty()){
        cerr << "Does not know server address!! " << endl;
        exit(EXIT_FAILURE);
    }
    // cerr << "name of DirService Server is " << serverName << "\n" << endl;

    // Filling server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    
    // look up address by name (Version 2 of network.cpp)
    struct addrinfo * res;
    int numAddr = getaddrinfo(serverName.c_str(), nullptr, nullptr, &res);
    //cerr << "number of address results svcdir server is " << numAddr << endl;
    servaddr.sin_addr = ((struct sockaddr_in*)res -> ai_addr) -> sin_addr;
    freeaddrinfo(res);

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
	perror("socket creation failed");
	exit(EXIT_FAILURE);
    }

    // set socket to timeout in 1 second if reply not received
    // TODO this should be a parameter somehwere.
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv)) < 0) {
        perror("Error");
        exit(1);
    }

    // init done
    ready = true;

    //cerr << "exiting client stub init" << endl;
}

void SvcDirClientStub::shutdown(){
    if (!ready) return;
}

