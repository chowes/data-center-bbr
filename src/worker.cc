#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "tcp_client.h"

using namespace std;


#define NSECS_PER_SEC       1000000000
#define USECS_PER_SEC       1000000
#define NSECS_PER_USEC      1000
#define BYTES_PER_MBIT      125000


bool time_expired;
struct itimerspec timer_setting;



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


void timer_sighandler(union sigval val)
{
    time_expired = true;
}


int convergence_test(TCPClient &client, long duration)
{
    int status;

    char *msg = new char[MSG_SIZE];
    memset(msg, '%', MSG_SIZE);

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

    timer_setting.it_value.tv_sec = static_cast<long>(duration * NSECS_PER_SEC) / NSECS_PER_SEC;
    timer_setting.it_value.tv_nsec = static_cast<long>(duration * NSECS_PER_SEC) % NSECS_PER_SEC;
    timer_setting.it_interval.tv_sec = static_cast<long>(duration * NSECS_PER_SEC) / NSECS_PER_SEC;
    timer_setting.it_interval.tv_nsec = static_cast<long>(duration * NSECS_PER_SEC) % NSECS_PER_SEC;

    status = timer_settime(timer_id, 0, &timer_setting, NULL);

    // start timer and do until duration expires
    while (!time_expired) {
        try {
            client.Send(msg, MSG_SIZE);
        } catch (const char * e) {
            break;
        }
    }

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
    } else if (test_type == "converge") {
        if (test_info.size() < 2) {
            cerr << "fatal: too few arguments from server" << endl;
            exit(1);
        }
        long duration = stol(test_info[1]);
        convergence_test(client, duration);
    } else {
        cerr << "fatal: server requested invalid test type" << endl;
        exit(1);
    }


    client.Close();

    return 0;
}
