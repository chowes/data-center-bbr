#ifndef __CHOWES_TCP_CLIENT__
#define __CHOWES_TCP_CLIENT__

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

#define PORTNUM                     "11235"
#define MSG_SIZE                    65536

using namespace std;


class TCPClient {

public:

    /*
     * constructor - connect to a server (see PORTNUM)
     * 
     * on success, we store a socket descriptor in socket_fd
     */
    TCPClient(const string hostname);

    // send data
    int Send(const void *data, size_t sz);

    // receive data
    int Receive(void *data, size_t sz);

    // close the connection
    void Close();

private:
    int client_socket;
    struct sockaddr_in server_addr;
};


#endif 