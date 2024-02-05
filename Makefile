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

all: bin/main bin/mainTest3 bin/mainTest4
OBJS= main.o network.o dumpHex.o e1server.o e1service.o e1client.o data.pb.o clientStub.o 

bin/main:  $(OBJS)
	$(CXX) -g -o bin/main $(OBJS) $(LDFLAGS)
	
OBJS2= mainTest3.o network.o dumpHex.o e1server.o e1service.o e1client3.o data.pb.o clientStub.o

# New target for bin/mainTest3
bin/mainTest3: $(OBJS2)
	$(CXX) -g -o bin/mainTest3 $(OBJS2) $(LDFLAGS)

OBJS3= mainTest4.o network.o dumpHex.o e1server.o e1service.o e1client4.o data.pb.o clientStub.o

# New target for bin/mainTest4
bin/mainTest4: $(OBJS3)
	$(CXX) -g -o bin/mainTest4 $(OBJS3) $(LDFLAGS)



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

mainTest3.o: network.hpp e1server.hpp e1service.hpp e1client3.hpp

e1client3.o: e1client3.hpp network.hpp clientStub.hpp

mainTest4.o: network.hpp e1server.hpp e1service.hpp e1client4.hpp

e1client4.o: e1client4.hpp network.hpp clientStub.hpp

clean:
	rm bin/* *.o 
