#!/bin/bash

if ! [ $(id -u) = 0 ]; then
	echo "This script needs to be run as root"
	exit 1
fi

echo "Enabling TCP BBR module"

modprobe tcp_bbr

echo "Rebooting into new kernel..."
reboot

