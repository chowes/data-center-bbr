#include <iostream>

#include "tcp_client.h"


using namespace std;


TCPClient::TCPClient(const string hostname) {

    int socket_fd;
    int error;
    struct addrinfo hints, *servinfo, *ai;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    error = getaddrinfo(hostname.c_str(), PORTNUM, &hints, &servinfo);
    if (error != 0) {
        cerr << "getaddrinfo" << endl;
        throw gai_strerror(error);
    }

    for (ai = servinfo; ai != NULL; ai = ai->ai_next) {
        client_socket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (socket_fd == -1) {
            perror("client: socket");
            continue;
        }

        error = connect(client_socket, ai->ai_addr, ai->ai_addrlen);
        if (error == -1) {
            close(socket_fd);
            throw strerror(errno);
            continue;
        }

        break;
    }

    /* we can't get a connection... */
    if (ai == NULL) {
        throw "socket error: unable to connect to server";
    }

    freeaddrinfo(servinfo);
}

int TCPClient::Send(const void *data, size_t sz) {

}


int TCPClient::Receive(void *data, size_t sz) {

}


void TCPClient::Close() {

}



int main(int argc, char const *argv[])
{
    try {
        TCPClient client(argv[1]);
    } catch (const char *err) {
        cerr << err << endl;
    }

    return 0;
}