
#include "srvcdirserver.hpp"


SrvcDirServer::SrvcDirServer(string nodeName): Node(nodeName){
            // note this is the same as installng the service software on the server
            // it doesn't run yet until the node is started.

            cout << "Main: Server " << nodeName << " adding key value service" << endl;
	    srvDirService = make_shared<SrvDirServiceServer>(nodeName,weak_from_this());

            addService(srvDirService);
        }

void SrvcDirServer::start(){
   cerr << "in SrvcDirServer start" << endl;
}

void SrvcDirServer::stop(){
}

