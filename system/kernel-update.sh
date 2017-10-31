#!/bin/bash

if ! [ $(id -u) = 0 ]; then
	echo "This script needs to be run as root"
	exit 1
fi

echo "Downloading Linux kernel 4.13.10"
wget --directory-prefix=/home/chowes/ https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.13.10.tar.xz

echo "Extracting kernel..."
tar -xf /home/chowes/linux-4.13.10.tar.xz

echo "Copying config file into compile directory"
cp /home/chowes/data-center-bbr/system/.config /home/chowes/linux-4.13.10/


echo "Compiling..."
make -j 12 -C /home/chowes/linux-4.13.10/

echo "Installing modules..."
make -j 12 -C /home/chowes/linux-4.13.10/ modules_install

echo "Installing new kernel..."
make -j 12 -C /home/chowes/linux-4.13.10/ install
update-initramfs -c -k 4.13.10
update grub

echo "Rebooting into new kernel..."
reboot

