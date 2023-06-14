#! /bin/bash

make clean
make
sudo insmod emucxl_mmap.ko
sudo ./emucxl_app &
sleep 1
sudo ./emucxl_app
sudo rmmod emucxl_mmap
sudo dmesg | tail -20