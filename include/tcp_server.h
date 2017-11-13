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
#include <string>
#include <iostream>

using namespace std;


// use this to get an OS assigned port
#define PORTNUM                     "11235"
#define MAX_WAITING_CLIENTS         10


/*
 * This helper class lets us fork a thread while keeping connetion information
 * around - we pass an arbitrary function to accept so we know how to handle
 * new connections. This lets us use the TCP client/server classes for 
 * whatever we want.
 */
class TCPConnection {
public:
    TCPConnection(int fd, sockaddr_in addr);
    
    vector<string> GetArgs();

    void Start(void *(*worker_function)(void *), const char *argv[]);

    int Send(const void *data, size_t sz);

    int Receive(void *data, size_t sz);

    void *Wait();

    void Close();

private:
    // this lets us pass any command line arguments to our worker function
    vector<string> args;
    int connection_socket;
    struct sockaddr_in client_addr;
    pthread_t connection_thread;
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
    int Accept();

    void StartWorkers(void *(*func)(void *), const char *args[]);

    /*
     * wait for our worker threads to finish and close all connections
     */
    void Stop();

private:
    int server_accept_socket;
    vector<TCPConnection *> connected_clients;
};


#endif 