CXX = g++
CXXFLAGS += -std=c++11 -Iinclude -g
LDFLAGS += -lpthread

SRC_PATH = src
vpath %.cc $(SRC_PATH)


all: aggregator worker

aggregator: aggregator.o tcp_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

worker: worker.o tcp_client.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o aggregator worker
