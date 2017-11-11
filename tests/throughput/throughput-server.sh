#!/bin/bash

# start the iperf server, output results in Mbits/second
iperf -s -f m -y --logfile server_throughput.csv
