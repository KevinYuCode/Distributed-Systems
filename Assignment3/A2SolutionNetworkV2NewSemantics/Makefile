
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CXX=g++
CXXFLAGS=-std=c++2a -g -pthread -g -I. -Iservicedir
LDFLAGS= -lprotobuf -pthread -lgdbm
endif
ifeq ($(UNAME_S),Darwin)
CXXFLAGS=-std=c++20 -g -pthread -g -I. -Iservicedir
CXX=c++
LDFLAGS= -lprotobuf -pthread
endif

SVCDIROBJS=servicedir/SvcDir.pb.o servicedir/srvcdirservice.o servicedir/srvcdirserver.o servicedir/srvcdirclientstub.o
OBJS=main.o network.o E477KV.pb.o dumpHex.o kvserver.o kvservice.o kvclient1.o kvclientstub.o   $(SVCDIROBJS)

all: bin/assign2 

test: bin/svcdirtest

bin/assign2:  $(OBJS)
	$(CXX) -g -o bin/assign2 $(OBJS)  $(LDFLAGS)
	

main.o:  E477KV.pb.h servicedir/SvcDir.pb.h

E477KV.pb.h E477KV.pb.cc: E477KV.proto
	protoc --cpp_out=. E477KV.proto

kvclient1.o: E477KV.pb.h kvclient1.hpp KVrpc.h kvclientstub.hpp network.hpp

kclientvstub.o: kvclientstub.hpp network.hpp

kvservice.o: kvservice.hpp E477KV.pb.h network.hpp servicedir/SvcDir.pb.h serverData.h
kvserver.o: kvserver.hpp network.hpp kvservice.hpp serverData.h

#service disocvery

bin/svcdirtest: $(SVCDIROBJS) servicedir/tstSvcDirMain.o servicedir/tstSvcDirClient.o network.o
	$(CXX) -g -o bin/svcdirtest $(SVCDIROBJS) servicedir/tstSvcDirMain.o servicedir/tstSvcDirClient.o network.o $(LDFLAGS)
servicedir/tstSvcDirMain.o: servicedir/tstSvcDirClient.hpp network.hpp 
servicedir/tstSvcDirClient.o: servicedir/tstSvcDirClient.hpp network.hpp
servicedir/srvcdirclientstub.o: servicedir/srvcdirclientstub.hpp network.hpp servicedir/SvcDir.pb.h

servicedir/SvcDir.pb.h servicedir/SvcDir.pb.cc: servicedir/SvcDir.proto
	protoc -I=./servicedir --cpp_out=./servicedir SvcDir.proto
servicedir/srvcdirserver.o: servicedir/srvcdirserver.hpp network.hpp servicedir/srvcdirservice.hpp
servicedir/srvcdirservice.o: servicedir/srvcdirservice.hpp network.hpp

# support files
network.o: network.hpp dumpHex.hpp
dumpHex.o: dumpHex.hpp


clean:
	rm bin/* *.o E477KV.pb.h E477KV.pb.cc servicedir/*.o servicedir/SvcDir.pb.h servicedir/SvcDir.pb.cc
