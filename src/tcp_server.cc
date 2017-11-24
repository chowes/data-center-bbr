#include <iostream>

#include "tcp_server.h"

using namespace std;


TCPConnection::TCPConnection(int fd, sockaddr_in addr) {
    connection_socket = fd;
    client_addr = addr;
}


void TCPConnection::Start(void *(*worker_function)(void *), const char *argv[]) {
    for (int i = 0; argv[i] != NULL; i++) {
        args.push_back(string(argv[i]));
    }
    pthread_create(&connection_thread, NULL, worker_function, (void *)this);
}


vector<string> TCPConnection::GetArgs() {
    return args;
}


int TCPConnection::GetSocket() {
    return connection_socket;
}



int TCPConnection::Send(const void *data, size_t sz) {
    int bytes_sent;

    bytes_sent = send(connection_socket, data, sz, 0);
    
    if (bytes_sent < 0) {
        perror("send");
        throw strerror(errno);
    }

    return bytes_sent;
}


int TCPConnection::Receive(void *data, size_t sz) {
    int bytes_recv;

    bytes_recv = recv(connection_socket, data, sz, 0);
    
    if (bytes_recv < 0) {
        perror("recv");
        throw strerror(errno);
    }

    return bytes_recv;
}


void *TCPConnection::Wait() {
    pthread_join(connection_thread, NULL);
    return NULL;
}


void TCPConnection::Close() {
    close(connection_socket);
}


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
        cerr << gai_strerror << endl;
        throw gai_strerror(status);
    }

    // we need to traverse the list of addrinfo structs until one is valid
    for (ai_iter = server_info; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
        
        // get a socket file descriptor
        server_accept_socket = socket(ai_iter->ai_family, ai_iter->ai_socktype, 
            ai_iter->ai_protocol);

        if (server_accept_socket == -1) {
            perror("socket");
            continue;
        }

        // bind a port to our socket descriptor
        if (bind(server_accept_socket, ai_iter->ai_addr, ai_iter->ai_addrlen) == -1) {
            perror("bind");
            close(server_accept_socket);
            continue;
        }
        break; // if we get here, we have bound a usable socket
    }

    if (!ai_iter) {
        cerr << "unable to bind socket" << endl;
        throw "unable to bind socket\n";
    }

    freeaddrinfo(server_info);

    int error = listen(server_accept_socket, MAX_WAITING_CLIENTS);

    if (error == -1) {
        perror("listen");
        throw strerror(errno);
    }
}


int TCPServer::Accept() {

    int new_fd;
    struct sockaddr_in client_addr;
    socklen_t sz = sizeof client_addr;
    memset(&client_addr, 0, sz);

    new_fd = accept(server_accept_socket, (struct sockaddr*)&client_addr, &sz);
    if (new_fd <= 0) {
        throw strerror(errno);
    }

    connected_clients.push_back(new TCPConnection(new_fd, client_addr));

    return new_fd;
}


void TCPServer::StartWorkers(void *(*func)(void *), const char *argv[], int delay) {
    for (TCPConnection *conn : connected_clients) {
        conn->Start(func, argv);
        if (delay != 0) {
            sleep(delay);
        }
    }
}


void TCPServer::WaitAll() {
    for (TCPConnection *conn : connected_clients) {
        conn->Wait();
    }
}


void TCPServer::Stop() {
    for (TCPConnection *conn : connected_clients) {
        conn->Close();
    }
}
