#ifndef __CHOWES_TCP_SERVER__
#define __CHOWES_TCP_SERVER__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>

using namespace std;


// use this to get an OS assigned port
#define PORTNUM                     "11235"
#define MAX_WAITING_CLIENTS         10


class TCPConnection {
    int connection_socket;
    struct sockaddr_in client_addr;
    pthread_t connection_thread;

public:
    TCPConnection(int fd, sockaddr_in addr, void *(*worker_function)(void *)) {
        connection_socket = fd;
        client_addr = addr;
        pthread_create(&connection_thread, NULL, worker_function, NULL);
    }

    int Send(const void *data, size_t sz) {

    }

    int Receive(void *data, size_t sz) {

    }

    void *Wait() {
        pthread_join(connection_thread, NULL);
        return NULL;
    }

    void Close() {
        close(connection_socket);
    }
};


class TCPServer {

public:

    /*
     * constructor - create a TCPServer and bind to a port (see PORTNUM)
     * 
     * on success, we store a socket descriptor in socket_fd and listen for
     * incoming connections
     */
    TCPServer();

    /*
     * accept an incoming connection and createa new worker thread
     *
     * each new connection is added to connected_clients, we can wait on these 
     * threads by calling stop
     */
    int Accept(void *(*func)(void *));

    /*
     * wait for our worker threads to finish and close all connections
     */
    void Stop();

private:
    int server_accept_socket;
    vector<TCPConnection> connected_clients;
};


#endif 