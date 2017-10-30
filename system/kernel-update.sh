#!/bin/bash

echo "This needs to be run as root"

echo "Downloading Linux kernel 4.13.10"

wget --directory-prefix=~/ https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.13.10.tar.xz
tar -xf ~/linux-4.13.10.tar.xz
cp .config ~/linux-4.13.10/

echo "Compiling Linux kernel..."

make -j 12 -C ~/linux-4.13.10/

echo "Installing modules..."

make -j 12 -C ~/linux-4.13.10/ modules_install

echo "Installing new kernel..."

make -j 12 -C ~/linux-4.13.10/ install

update-initramfs -c -k 4.13.10
update grub

echo "Rebooting into new kernel..."

