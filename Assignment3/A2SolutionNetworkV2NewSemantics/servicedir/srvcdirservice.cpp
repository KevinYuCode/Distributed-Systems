
#include "srvcdirservice.hpp"


#ifdef __APPLE__
#define MSG_CONFIRM 0
#endif

using namespace std;
using namespace string_literals;

#define close mclose
void mclose(int fd);

void SrvDirServiceServer::stop(){
    alive = false;
}

void SrvDirServiceServer::start(){
    struct sockaddr_in servaddr, cliaddr;
    
    // get a socket to recieve messges
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return;// this will exit the service thread
    }

    // clear variables before initializing
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Port and Interfact information for binding
    // the socket
    servaddr.sin_family    = AF_INET;        // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;   // whatever interface is available
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        return;// this will exit the service thread
    }

    socklen_t len;
    int n;
    //cerr << "alive = " << alive << endl;
    while(alive){ 
	//cerr << "waiting for call from client" << endl;

        // wait for a mesage from a client
	len = sizeof(cliaddr);  //len is value/result
	n = recvfrom(sockfd, (uint8_t *)udpMessage, MAXMSG, 
		    MSG_WAITALL, ( struct sockaddr *) &cliaddr,
		    &len);
        //cerr << "server received " << n << " bytes." << endl;
	//std::cerr << HexDump{udpMessage,(uint32_t)n} << endl;


	ServiceDirectory::svcDirRequest receivedMsg; 
	ServiceDirectory::svcDirResponse replyMsg;

	if (!receivedMsg.ParseFromArray(udpMessage,n)){
	    cerr << "Couild not parse message" << endl;
	    // ignore
	}

	//cerr << "----->message parsed" << endl;
        if ((receivedMsg.magic()) != magic){
                cerr << "service unrecognized message" << endl;
        } else {
            // start by copying version and serial to reply
            replyMsg.set_magic(receivedMsg.magic());
            replyMsg.set_version(receivedMsg.version());
            replyMsg.set_serial(receivedMsg.serial());
            
            if ((receivedMsg.version() & 0xFF00)  == version1x){
                // dispatch version 1.x
                callMethodVersion1(receivedMsg, replyMsg);
            } else {
                cerr << "unrecognized version" << endl;
                // For now ignore, message doesn't have a wrong version reply
            }
            // at this point in time the reply is complete
            // send response back
            uint32_t msglen = replyMsg.ByteSizeLong();
            // double check size
            replyMsg.SerializeToArray(udpMessage, msglen);
            //cerr << "reply message" << HexDump{udpMessage,msglen} ;
            
            int servern = sendto(sockfd, udpMessage, msglen,
                                 MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
            //cerr << "server sent " << servern << " bytes" << endl;
        }
    }

    close(sockfd);

}


void SrvDirServiceServer::callMethodVersion1(ServiceDirectory::svcDirRequest &receivedMsg, ServiceDirectory::svcDirResponse &replyMsg){
    if (receivedMsg.has_registerargs()){
	//stringstream ss;
	//ss << "register message requested" << endl;
	//cerr << ss.str();
        
        const ServiceDirectory::registerRequest& rReq = receivedMsg.registerargs();

	ServiceDirectory::registerResponse *rResp = replyMsg.mutable_registerres();
	rResp->set_status(registerService(rReq.servicename(), rReq.servername(), rReq.port()));
    }
    if (receivedMsg.has_searchargs()){
	//stringstream ss;
	//ss << "search message requested" << endl;
	//cerr << ss.str();
        
        const ServiceDirectory::searchRequest& sReq = receivedMsg.searchargs();

        SvcDirSearchResult result = searchService(sReq.servicename());

	ServiceDirectory::searchResponse *sr = replyMsg.mutable_searchres();
        sr->set_status(result.status);
        sr->set_servername(result.serverName);
        sr->set_port(result.port);
    }
    if (receivedMsg.has_removeargs()){
	//stringstream ss;
	//ss << "search message requested" << endl;
	//cerr << ss.str();
        
        const ServiceDirectory::removeRequest& rReq = receivedMsg.removeargs();

        bool result = removeService(rReq.servicename());

	ServiceDirectory::removeResponse *rr = replyMsg.mutable_removeres();
        rr->set_status(result);
    }
}

bool SrvDirServiceServer::registerService(string serviceName, string serverName, uint32_t port){
    //stringstream ss;
    //ss = stringstream();
    //ss << "In registerService, serviceName is " << serviceName << ", server is " << serverName << ":" << port << endl;
    //cerr << ss.str();

        
    serviceDirMap[serviceName] = SvcAddress{serverName,port};

    //ss = stringstream();
    //ss << "leaving registerService" << endl;
    //cerr << ss.str();

    return true;
}


SvcDirSearchResult SrvDirServiceServer::searchService(string serviceName){
    //stringstream ss;
    //ss = stringstream();
    //ss << "in serchService, serviceName is " << serviceName << endl;
    //cerr << ss.str();
    
    SvcDirSearchResult res;
    res.status = false;

    map<string,SvcAddress>::iterator it;
    it =   serviceDirMap.find(serviceName);
    if (it != serviceDirMap.end()){
	res.status = true;
        res.serverName = it->second.name;
        res.port = it->second.port;
    }
    return res;
}
bool SrvDirServiceServer::removeService(string serviceName){
    return serviceDirMap.erase(serviceName);
}
