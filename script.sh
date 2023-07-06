#! /bin/bash

make clean > /dev/null
make > /dev/null 2>/dev/null
sudo insmod emucxl_kernel.ko
# sudo ./direct_queue > test_pid1.txt &
# # sleep 1
sudo ./direct_queue > result.txt
# sudo ./kvs_appl
sudo rmmod emucxl_kernel
# sudo dmesg | tail -20
python3 graph.py