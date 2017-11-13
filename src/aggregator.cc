#include "tcp_server.h"

#include <iostream>

void *send_queries(void *conn) {
    TCPConnection *connection = static_cast<TCPConnection *>(conn);
    vector<string> args = connection->GetArgs();

    long num_workers = stol(args[1]);
    long total_bytes = stol(args[2]);

    string query_message = to_string(total_bytes / num_workers);
    char *response = new char[total_bytes / num_workers];

    connection->Send(query_message.c_str(), query_message.size());
   
    int bytes = 0; 
    while (bytes < total_bytes / num_workers) {
        bytes += connection->Receive(response, total_bytes / num_workers);
        cerr << bytes << endl;
    }

    cout << response << ": " << bytes << endl;

    return NULL;
}


int main(int argc, char const *argv[])
{
    TCPServer server;
    long num_workers;

    if (argc < 3) {
        cerr << "usage: aggregator <num workers> <total bytes>" << endl;
        exit(1);
    }

    num_workers = strtol(argv[1], NULL, 10);

    for (int i = 0; i < num_workers; i++) {
        server.Accept();
    }


    server.StartWorkers(send_queries, argv);

    server.Stop();

    return 0;
}
