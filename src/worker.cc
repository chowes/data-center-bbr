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


int throughput_test(TCPClient &client, string hostname, long duration)
{
    char *iperf_argv[6];

    iperf_argv[0] = new char[strlen("iperf")];
    strcpy(iperf_argv[0], "iperf");
    iperf_argv[1] = new char[strlen("-c")];
    strcpy(iperf_argv[1], "-c");
    iperf_argv[2] = new char[strlen(hostname.c_str())];
    strcpy(iperf_argv[2], hostname.c_str());
    iperf_argv[3] = new char[strlen("-t")];
    strcpy(iperf_argv[3], "-t");
    iperf_argv[4] = new char[strlen(to_string(duration).c_str())];
    strcpy(iperf_argv[4], to_string(duration).c_str());
    iperf_argv[5] = NULL;



    int status;

    pid_t pid = fork();
    if (pid == 0) {
        execv("/usr/bin/iperf", iperf_argv);

        // we should never get here...
        perror("execv");
        exit(1);
    } else {
        waitpid(pid, &status, 0);
    
        for (int i = 0; iperf_argv[i] != NULL; i++) {
            delete iperf_argv[i];
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
        string server_hostname = string(argv[1]);
        long duration = stol(test_info[1]);
        throughput_test(client, server_hostname, duration);
    } else {
        cerr << "fatal: server requested invalid test type" << endl;
        exit(1);
    }


    client.Close();

    return 0;
}
