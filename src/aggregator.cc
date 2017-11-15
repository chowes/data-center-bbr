#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>

#include "tcp_server.h"


#define USECS_PER_SEC       1000000
#define NSECS_PER_USEC      1000
#define BYTES_PER_MBIT      125000


long time_diff(struct timespec t1, struct timespec t2)
{
    long secs;
    long nsecs;
    long usecs;

    secs = t2.tv_sec - t1.tv_sec;
    nsecs = t2.tv_nsec - t1.tv_nsec;

    usecs = (secs * USECS_PER_SEC) + (nsecs / NSECS_PER_USEC);

    return usecs;
}


void *send_queries(void *conn) {
    TCPConnection *connection = static_cast<TCPConnection *>(conn);
    vector<string> args = connection->GetArgs();

    long num_workers = stol(args[2]);
    long total_bytes = stol(args[3]);

    string query_message = "query " + to_string(total_bytes / num_workers);
    char *response = new char[total_bytes / num_workers];

    connection->Send(query_message.c_str(), query_message.size());
   
    int bytes = 0; 
    while (bytes < total_bytes / num_workers) {
        bytes += connection->Receive(response, total_bytes / num_workers);
    }

    cout << "received bytes from worker: " << bytes << endl;

    return NULL;
}


void *start_iperf(void *conn) {
    TCPConnection *connection = static_cast<TCPConnection *>(conn);
    vector<string> args = connection->GetArgs();



    return NULL;
}


int query_server(int argc, char const *argv[]) {

    TCPServer server;
    long num_workers;
    long total_bytes;
    long total_time;
    string results_path;
    struct timespec t1, t2;

    ofstream results_file;

    if (argc < 4) {
        cerr << "usage: aggregator query <num workers> <total request size (bytes)> <results_file>" << endl;
        return -1;
    }

    num_workers = strtol(argv[2], NULL, 10);
    total_bytes = strtol(argv[3], NULL, 10);
    results_path = string(argv[4]);

    for (int i = 0; i < num_workers; i++) {
        server.Accept();
    }


    clock_gettime(CLOCK_MONOTONIC, &t1);

    server.StartWorkers(send_queries, argv);
    server.WaitAll();

    clock_gettime(CLOCK_MONOTONIC, &t2);

    total_time = time_diff(t1, t2);
    fprintf(stdout, "got response from workers in %ld usecs\n", total_time);
    
    results_file.open("query_results.txt", ios::app);
    results_file << num_workers << "," << total_bytes << "," << total_time << endl;

    server.Stop();

    return 0;
}


int throughput_server(int argc, char const *argv[]) {

    TCPServer server;
    string results_path;
    long num_flows;
    long delay;
    long total_time;
    double interval;

    if (argc < 5) {
        cerr << "usage: aggregator throughput <num flows> <delay> <total time> <interval> <results file>" << endl;
        return -1;
    }

    num_flows = strtol(argv[2], NULL, 10);
    delay = strtol(argv[3], NULL, 10);    
    total_time = strtol(argv[4], NULL, 10);
    interval = atof(argv[5]);
    results_path = string(argv[6]);

    for (int i = 0; i < num_flows; i++) {
        server.Accept();
    }


    // create argument vector for iperf arguments
    // iperf -s -t <client timeout + e> -y c -i <interval> -o <results file>

    vector<char*> iperf_argv;
    char *arg = new char[strlen("-%")];
    char *val = new char[256];
    
    strcpy(arg, "-s");
    iperf_argv.push_back(arg);

    strcpy(arg, "-t");
    iperf_argv.push_back(arg);
    strcpy(val, to_string(total_time).c_str());
    iperf_argv.push_back(val);

    strcpy(arg, "-y");
    iperf_argv.push_back(arg);
    strcpy(arg, "c");
    iperf_argv.push_back(arg);

    strcpy(arg, "-i");
    iperf_argv.push_back(arg);
    strcpy(val, to_string(interval).c_str());
    iperf_argv.push_back(val);

    strcpy(arg, "-o");
    iperf_argv.push_back(arg);
    strcpy(val, results_path.c_str());
    iperf_argv.push_back(val);

    iperf_argv.push_back(NULL);


    int status;

    pid_t pid = fork();
    if (pid == 0) {
        execv("iperf", &iperf_argv[0]);
    } else {
        waitpid(pid, &status, 0);
    }

    delete val;
    delete arg;

    return 0;
}


int main(int argc, char const *argv[])
{
    TCPServer server;
    long num_workers;
    long total_bytes;
    long total_time;
    struct timespec t1, t2;

    if (argc < 2) {
        cerr << "usage: aggregator test <args ...>" << endl;
        exit(1);
    }

    string test_type = argv[1];

    if (test_type == "query") {
        return query_server(argc, argv);
    } else if (test_type == "throughput") {
        return throughput_server(argc, argv);
    }
}
