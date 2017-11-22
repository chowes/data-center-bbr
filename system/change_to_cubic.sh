#!/bin/bash

if ! [ $(id -u) = 0 ]; then                 
	echo "This script must be run as root"
	exit 1
fi 

# change congestion control algorithm to TCP BBR
echo cubic > /proc/sys/net/ipv4/tcp_congestion_control
echo "currently using: "
cat /proc/sys/net/ipv4/tcp_congestion_control

