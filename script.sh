#! /bin/bash

make clean
make
sudo insmod memcxlib_mmap.ko
# sudo ./memcxlib_app &
# sleep 1
sudo ./memcxlib_app
sudo rmmod memcxlib_mmap
sudo dmesg | tail -20