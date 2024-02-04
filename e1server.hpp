
#include "network.hpp"
#include "e1service.hpp"

class E1Server : public Node
{
    string gdbm_file = "";
    shared_ptr<E1ServiceServer> E1Service;
    
public:
    E1Server(string nodeName);
    ~E1Server(){};
    void setGdbmFile(string gdbm_name);
};
