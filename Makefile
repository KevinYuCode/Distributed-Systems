UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
CXX=g++
CXXFLAGS=-std=c++2a -g -pthread -g
LDFLAGS=-pthread -lprotobuf -lgdbm
endif
ifeq ($(UNAME_S),Darwin)
CXXFLAGS=-std=c++20 -g -pthread -g
CXX=c++
LDFLAGS=-pthread -lprotobuf -lgdbm
endif

all: bin/main
OBJS= main.o network.o dumpHex.o e1server.o e1service.o e1client.o data.pb.o clientStub.o

bin/main:  $(OBJS)
	$(CXX) -g -o bin/main $(OBJS) $(LDFLAGS)
	
data.pb.h data.pb.cc: data.proto
	protoc --cpp_out=. data.proto

main.o:  network.hpp e1server.hpp e1service.hpp e1client.hpp

e1client.o: e1client.hpp network.hpp clientStub.hpp

clientStub.o: clientStub.hpp network.hpp 

e1service.o: e1service.hpp network.hpp
e1server.o: e1server.hpp e1service.hpp network.hpp

network.o: network.hpp
dumpHex.o: dumpHex.hpp

data.o: data.pb.cc data.pb.h

clean:
	rm bin/* *.o 
