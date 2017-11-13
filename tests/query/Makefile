CXX = g++
CXXFLAGS += -std=c++11 -Iinclude
LDFLAGS += -lpthread

SRC_PATH = src
vpath %.cc $(SRC_PATH)

all: client server

client: client.o
	$(CXX) $^ -I. $(LDFLAGS) -o $@

server: server.o
	$(CXX) $^ -I. $(LDFLAGS) -o $@

clean:
	rm -f *.o client server
