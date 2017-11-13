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
        cerr << gai_strerror(error) << endl;
        throw gai_strerror(error);
    }

    for (ai = servinfo; ai != NULL; ai = ai->ai_next) {
        client_socket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (socket_fd == -1) {
            perror("socket");
            continue;
        }

        error = connect(client_socket, ai->ai_addr, ai->ai_addrlen);
        if (error == -1) {
            close(socket_fd);
            perror("connect");
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
    int bytes_sent;

    bytes_sent = send(client_socket, data, sz, 0);
    
    if (bytes_sent < 0) {
        perror("send");
        throw strerror(errno);
    }

    return bytes_sent;
}


int TCPClient::Receive(void *data, size_t sz) {
    int bytes_recv;

    bytes_recv = recv(client_socket, data, sz, 0);
    
    if (bytes_recv < 0) {
        perror("recv");
        throw strerror(errno);
    }

    return bytes_recv;
}


void TCPClient::Close() {
    close(client_socket);
}
