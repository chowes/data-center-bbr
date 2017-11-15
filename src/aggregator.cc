#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

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

    string server_hostname = args[2];
    long total_time = stol(args[4]);

    string throughput_message = "throughput " + to_string(total_time);

    connection->Send(throughput_message.c_str(), throughput_message.size());

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

    if (argc < 7) {
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

    char *iperf_argv[10];

    iperf_argv[0] = new char[strlen("iperf")];
    strcpy(iperf_argv[0], "iperf");
    iperf_argv[1] = new char[strlen("-s")];
    strcpy(iperf_argv[1], "-s");
    iperf_argv[2] = new char[strlen("-t")];
    strcpy(iperf_argv[2], "-t");
    iperf_argv[3] = new char[strlen(to_string(total_time).c_str())];
    strcpy(iperf_argv[3], to_string(total_time).c_str());
    iperf_argv[4] = new char[strlen("-i")];
    strcpy(iperf_argv[4], "-i");
    iperf_argv[5] = new char[strlen(to_string(interval).c_str())];
    strcpy(iperf_argv[5], to_string(interval).c_str());
    iperf_argv[6] = new char[strlen("-y")];
    strcpy(iperf_argv[6], "-y");
    iperf_argv[7] = new char[strlen("a")];
    strcpy(iperf_argv[7], "a");
    iperf_argv[8] = NULL;


    int status;

    pid_t pid = fork();
    if (pid == 0) {
        // since iperf doesn't output to file properly we have to redirect stdout
        int fd = open(results_path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 1);
        close(fd);

        execv("/usr/bin/iperf", iperf_argv);

        // we should never get here...
        perror("execv");
        exit(1);
    } else {
        
        // we don't want to do this until the server starts, but i can't think of a good way to signal this...
        sleep(1);

        server.StartWorkers(start_iperf, argv);
        waitpid(pid, &status, 0);

        for (int i = 0; iperf_argv[i] != NULL; i++) {
            delete iperf_argv[i];
        }
    }

    return 0;
}


int main(int argc, char const *argv[])
{
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
