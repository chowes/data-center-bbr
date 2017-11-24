#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <mutex>

#include "tcp_server.h"

#define NSECS_PER_SEC       1000000000
#define USECS_PER_SEC       1000000
#define NSECS_PER_USEC      1000
#define BYTES_PER_MBIT      125000

// keep a hash table of the bytes transfered by flow (lookup is by socket descriptor)
pthread_mutex_t flow_throughput_mutex = PTHREAD_MUTEX_INITIALIZER;
unordered_map<int, long> flow_throughput;
unordered_map<int, long> flow_previous;
struct timespec prev_time;

struct itimerspec timer_setting;


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


long calc_throughput(struct timespec t1, struct timespec t2, long bytes)
{
    long usecs = time_diff(t1, t2);
    long mbits = bytes / BYTES_PER_MBIT;

    long bytes_per_usec = bytes / usecs;

    cerr << usecs << " " << bytes_per_usec << endl;

    return bytes_per_usec * (USECS_PER_SEC / BYTES_PER_MBIT);
}


void *send_queries(void *conn)
{
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


void *start_senders(void *conn)
{
    TCPConnection *connection = static_cast<TCPConnection *>(conn);
    vector<string> args = connection->GetArgs();


    string server_hostname = args[2];
    long total_time = stol(args[3]);

    // we should only need to lock and unlock here since each thread only modifies its own count via reference
    pthread_mutex_lock(&flow_throughput_mutex);
    flow_throughput.emplace(connection->GetSocket(), 0);
    flow_previous.emplace(connection->GetSocket(), 0);
    auto &throughput_counter = flow_throughput.at(connection->GetSocket());
    pthread_mutex_unlock(&flow_throughput_mutex);

    char *response = new char[MSG_SIZE];

    string throughput_message = "throughput " + to_string(total_time);

    cerr << throughput_message << endl;

    connection->Send(throughput_message.c_str(), throughput_message.size());

    while (true) {
        throughput_counter += connection->Receive(response, MSG_SIZE);
    }

    return NULL;
}


int query_server(int argc, char const *argv[])
{

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

    server.StartWorkers(send_queries, argv, 0);
    server.WaitAll();

    clock_gettime(CLOCK_MONOTONIC, &t2);

    total_time = time_diff(t1, t2);
    fprintf(stdout, "got response from workers in %ld usecs\n", total_time);
    
    results_file.open(results_path, ios::app);
    results_file << num_workers << "," << total_bytes << "," << total_time << endl;

    server.Stop();

    return 0;
}


void timer_sighandler(union sigval val)
{
    struct timespec curr_time;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    long real_interval = time_diff(prev_time, curr_time);
    
    for (auto &tp : flow_throughput) {
        long &previous = flow_previous.at(tp.first);
        cout << tp.first << ": " << calc_throughput(prev_time, curr_time, tp.second - previous) << endl;
        previous = tp.second;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &prev_time);
}


int throughput_server(int argc, char const *argv[])
{

    TCPServer server;
    string results_path;
    long num_flows;
    long total_time;
    double interval;

    int status;

    if (argc < 6) {
        cerr << "usage: aggregator throughput <num flows> <total time> <interval> <results file>" << endl;
        return -1;
    }

    num_flows = strtol(argv[2], NULL, 10);
    total_time = strtol(argv[3], NULL, 10);
    interval = atof(argv[4]);
    results_path = string(argv[5]);

    // we want our signal handler to have the highest possible priority
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct sched_param monitor_param;
    monitor_param.sched_priority = 255;    

    struct sigevent timer_sig;
    timer_sig.sigev_notify = SIGEV_THREAD;
    timer_sig.sigev_notify_function = timer_sighandler;
    timer_sig.sigev_value.sival_int = 0;
    timer_sig.sigev_notify_attributes = &attr;

    timer_t timer_id;

    status = timer_create(CLOCK_REALTIME, &timer_sig, &timer_id);
    if (status != 0) {
        perror("timer_create");
        return -1;
    }

    timer_setting.it_value.tv_sec = static_cast<long>(interval * NSECS_PER_SEC) / NSECS_PER_SEC;
    timer_setting.it_value.tv_nsec = static_cast<long>(interval * NSECS_PER_SEC) % NSECS_PER_SEC;
    timer_setting.it_interval.tv_sec = static_cast<long>(interval * NSECS_PER_SEC) / NSECS_PER_SEC;
    timer_setting.it_interval.tv_nsec = static_cast<long>(interval * NSECS_PER_SEC) % NSECS_PER_SEC;

    // connect to clients
    for (int i = 0; i < num_flows; i++) {
        server.Accept();
    }

    // start the timer
    clock_gettime(CLOCK_MONOTONIC, &prev_time);
    status = timer_settime(timer_id, 0, &timer_setting, NULL);


    // start receiving data
    server.StartWorkers(start_senders, argv, 0);
    server.WaitAll();

    return 0;
}


int convergence_server(int argc, char const *argv[])
{

    TCPServer server;
    string results_path;
    long num_flows;
    long delay;
    long time_per_flow;
    double interval;

    if (argc < 6) {
        cerr << "usage: aggregator throughput <num flows> <time per flow> <interval> <results file>" << endl;
        return -1;
    }

    num_flows = strtol(argv[2], NULL, 10);
    time_per_flow = strtol(argv[3], NULL, 10);
    interval = atof(argv[4]);
    results_path = string(argv[5]);

    for (int i = 0; i < num_flows; i++) {
        server.Accept();
    }

    server.StartWorkers(start_senders, argv, time_per_flow);

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
    } else if (test_type == "converge") {
        return convergence_server(argc, argv);
    }
}
