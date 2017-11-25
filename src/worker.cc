#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "tcp_client.h"

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


int throughput_test(TCPClient &client, long duration)
{
    int status;

    char *msg = new char[MSG_SIZE];
    memset(msg, '%', MSG_SIZE);

    // start timer and do until duration expires
    while (send) {
        try {
            client.Send(msg, MSG_SIZE);
        } catch (const char * e) {
            delete msg;
            break;
        }
    }

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

    if (test_info.size() < 1) {
        cerr << "fatal: empty request from server" << endl;
        exit(1);        
    }

    string test_type = test_info[0];

    if (test_type == "query") {
        if (test_info.size() < 2) {
            cerr << "fatal: too few arguments from server" << endl;
            exit(1);
        }
        long query_size = stol(test_info[1]);
        query_test(client, query_size);
    } else if (test_type == "throughput") {
        if (test_info.size() < 2) {
            cerr << "fatal: too few arguments from server" << endl;
            exit(1);
        }
        long duration = stol(test_info[1]);
        throughput_test(client, duration);
    } else {
        cerr << "fatal: server requested invalid test type" << endl;
        exit(1);
    }


    client.Close();

    return 0;
}
