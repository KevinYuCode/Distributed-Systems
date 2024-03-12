#ifndef SRVCDIRSERVER_HPP
#define SRVCDIRSERVER_HPP
#include "network.hpp"
#include "srvcdirservice.hpp"

using namespace std;

class SrvcDirServer: public Node{
    private:
        shared_ptr<SrvDirServiceServer> srvDirService;
    public:
        SrvcDirServer(string nodeName);
	~SrvcDirServer(){
	}

	void start();
	void stop();
};

#endif
