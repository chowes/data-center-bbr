#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "tcp_client.h"

#define MESSAGE_SZ      64*1024

using namespace std;


int query_test(TCPClient &client, long query_size)
{
    cout << "query test - message size: " << query_size << endl;

    char *msg = new char[query_size];
    memset(msg, '%', query_size);

    client.Send(msg, query_size);

    delete msg;

    return 0;
}


int throughput_test(TCPClient &client, long duration, long delay)
{
    cout << "throughput test - duration: " << duration << " delay: " << delay << endl;

    char *msg = new char[MESSAGE_SZ];
    memset(msg, '%', MESSAGE_SZ);

    client.Send(msg, MESSAGE_SZ);

    delete msg;

    return 0;
}


int main(int argc, char const *argv[])
{
    char *data;
    vector<string> test_info;

    if (argc < 2) {
        cerr << "usage: client <server hostname>" << endl;
        exit(1);
    }

    TCPClient client(argv[1]);


    data = new char[256];
    memset(data, '\0', 256);

    client.Receive(data, 256);

    string buf; 
    stringstream ss(data); 

    while (ss >> buf) {
        test_info.push_back(buf);
    }

    for (string s : test_info) {
        cout << s << endl;
    }


    string test_type = test_info[0];

    if (test_type == "query") {
        long query_size = stol(test_info[1]);
        query_test(client, query_size);
    } else if (test_type == "throughput") {
        long duration = stol(test_info[1]);
        long delay = stol(test_info[2]);
    } else {
        cerr << "fatal: server requested invalid test type" << endl;
        exit(1);
    }


    client.Close();

    return 0;
}
