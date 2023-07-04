#! /bin/bash

make clean
make
sudo insmod emucxl_kernel.ko
# sudo ./emucxl_direct &
# sleep 1
sudo ./emucxl_direct
sudo ./emucxl_app
sudo rmmod emucxl_kernel
sudo dmesg | tail -20