#include <iostream>

#include "tcp_server.h"


using namespace std;


TCPServer::TCPServer() {

    int status;
    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *ai_iter; // used to traverse address info list

    // we have to make sure the hints is 0 filled
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // use IPv4
    hints.ai_socktype = SOCK_STREAM; // use TCP
    hints.ai_flags = AI_PASSIVE; // let the OS give us a suitable socket

    // get a linked list of addrinfo structs
    status = getaddrinfo(NULL, PORTNUM, &hints, &server_info);
    if (status != 0) {
        throw gai_strerror(status);
    }

    // we need to traverse the list of addrinfo structs until one is valid
    for (ai_iter = server_info; ai_iter != NULL; ai_iter = ai_iter->ai_next) {

        // get a socket file descriptor
        server_accept_socket = socket(ai_iter->ai_family, ai_iter->ai_socktype, 
            ai_iter->ai_protocol);

        if (server_accept_socket == -1) {
            continue;
        }

        // bind a port to our socket descriptor
        if (bind(server_accept_socket, ai_iter->ai_addr, ai_iter->ai_addrlen) == -1) {
            close(server_accept_socket);
            continue;
        }
        break; // if we get here, we have bound a usable socket
    }

    if (!ai_iter) {
        throw "unable to bind socket\n";
    }

    // done with this
    freeaddrinfo(server_info);
}


void TCPServer::Listen() {
    int error = listen(server_accept_socket, MAX_WAITING_CLIENTS);

    if (error == -1) {
        throw strerror(errno);
    }
}


int TCPServer::Accept(void *(*func)(void *)) {

    int new_fd;
    struct sockaddr_in client_addr;
    socklen_t sz = sizeof client_addr;
    memset(&client_addr, 0, sz);

    new_fd = accept(server_accept_socket, (struct sockaddr*)&client_addr, &sz);
    if (new_fd <= 0) {
        throw strerror(errno);
    }

    TCPConnection new_connection(new_fd, client_addr, func);
    connected_clients.push_back(new_connection);

    return new_fd;
}


void TCPServer::Stop() {
    for (TCPConnection conn : connected_clients) {
        conn.Wait();
        conn.Close();
    }
}


void *test(void *args) {
    cout << "hello world" << endl;
}



int main(int argc, char const *argv[])
{
    TCPServer server;

    server.Listen();

    for (int i = 0; i < 10; i++) {
        server.Accept(test);
    }

    server.Stop();

    return 0;
}