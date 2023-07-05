#! /bin/bash

make clean > /dev/null
make > /dev/null 2>/dev/null
sudo insmod emucxl_kernel.ko
# sudo ./direct_appl &
# sleep 1
sudo ./direct_appl
sudo ./kvs_appl
sudo rmmod emucxl_kernel
# sudo dmesg | tail -20