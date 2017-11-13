#include <time.h>

#include <iostream>

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

    long num_workers = stol(args[1]);
    long total_bytes = stol(args[2]);

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


int main(int argc, char const *argv[])
{
    TCPServer server;
    long num_workers;
    long total_time;
    struct timespec t1, t2;

    if (argc < 3) {
        cerr << "usage: aggregator <num workers> <total bytes>" << endl;
        exit(1);
    }

    num_workers = strtol(argv[1], NULL, 10);

    for (int i = 0; i < num_workers; i++) {
        server.Accept();
    }


    clock_gettime(CLOCK_MONOTONIC, &t1);

    server.StartWorkers(send_queries, argv);
    server.WaitAll();

    clock_gettime(CLOCK_MONOTONIC, &t2);

    total_time = time_diff(t1, t2); // avoid dividing by 0
    fprintf(stdout, "server responded in %ld usecs\n", total_time);

    server.Stop();

    return 0;
}
