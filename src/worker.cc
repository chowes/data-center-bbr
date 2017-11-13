#include <iostream>
#include <fstream>

#include "tcp_client.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        cerr << "usage: client <hostname>" << endl;
        exit(1);
    }

    TCPClient client(argv[1]);

    char *query_data = new char[256];
    client.Receive(query_data, 256);

    long query_size = stol(query_data);

    cerr << query_size << endl;

    char *msg = new char[query_size];

    client.Send(msg, query_size);
    client.Close();

    return 0;
}
