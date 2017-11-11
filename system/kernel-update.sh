#!/bin/bash

if ! [ $(id -u) = 0 ]; then
	echo "This script needs to be run as root"
	exit 1
fi


echo "Compiling..."
# make -j 12 -C /home/chowes/linux-4.13.10/

echo "Installing modules..."
make -j 12 -C /home/chowes/linux-4.13.10/ modules_install

echo "Installing new kernel..."
make -j 12 -C /home/chowes/linux-4.13.10/ install
update-initramfs -c -k 4.13.10
update-grub

echo "Rebooting into new kernel..."
reboot

