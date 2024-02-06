#include "e1server.hpp"
#include <gdbm.h>

//We'll eventually need to have some sort of parameter for the gdbm
E1Server::E1Server(string nodeName) : Node(nodeName)
{
    cout << "Main: Server " << nodeName << " adding service" << endl;

    // The weak pointer created from "weak_from_this()" function will point to the server, allowing reference to the server from E1ServiceServer
    E1Service = make_shared<E1ServiceServer>(nodeName, weak_from_this()); // Creates a service on the server, and the weak pointer only exists if E1Service exists
    addService(E1Service);

}

// Since you will have multiple independent servers, each must know the gdbm file that
// will contain the data. Add a method to your server class interface that will be used to
// set the name of the file, this method in turn will set the name in the service class.
void E1Server::setGdbmFile(string gdbm_name)
{
    this->gdbm_file_name = gdbm_name;

    if (E1Service)
    {
        E1Service->setGdbmFile(gdbm_name);
    }
}
