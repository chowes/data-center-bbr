#!/bin/bash

if ! [ $(id -u) = 0 ]; then
	echo "This script needs to be run as root"
	exit 1
fi

dpkg -i /home/chowes/linux-headers-4.13.0-041300_4.13.0-041300.201709031731_all.deb
dpkg -i /home/chowes/linux-headers-4.13.0-041300-generic_4.13.0-041300.201709031731_amd64.deb
dpkg -i /home/chowes/linux-image-4.13.0-041300-generic_4.13.0-041300.201709031731_amd64.deb

update-grub

echo "Rebooting into new kernel..."
reboot

