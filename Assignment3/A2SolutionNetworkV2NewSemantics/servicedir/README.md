
Service Discovery Server
========================

The service discovery server is sort of like a Java RMI org.
Remote procedure call servers register the address and port that
they are running on, and the client stubs can query the server for the
address and port of the server.

The initial setup requires a static assignment of the server name and port
to the service disovery client and port

Files
-----
SvcDir.proto - protobuf definition of the service directory protocol

svcservice.http/cpp - implemention of the service directory rpc server stub
srvcserver.hpp/cpp - implementation of a node running a directory service serher

svcdir.hpp - header for common data struture
svcdirclientstub.hpp/cpp - rpc proxy for use by clientrs

svcdirclient.hpp/cpp - test client
svcdirmain.cpp - test progam to test the srvdir protocol
