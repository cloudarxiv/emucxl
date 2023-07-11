#! /bin/bash

make clean > /dev/null
make > /dev/null 2>/dev/null
sudo insmod emucxl_kernel.ko
# sudo ./direct_queue > test_pid1.txt &
# # sleep 1
# sudo ./direct_queue > result.txt
# # sudo ./kvs_appl
# sudo rmmod emucxl_kernel
# # sudo dmesg | tail -20
# python3 graph.py
# for i in 1 10 25 37 45
# do
#     echo "Running seed $i"
#     sudo ./kvs_testcase 1 $i
#     sudo ./kvs_testcase 0 $i
# done

echo "Policy 1: Fetch object to local if it is in remote"
echo "Policy 0: Keep object in remote if it is in remote"

for i in 0 10 20 30 40 50 60 70 80 90
do
    echo "x = $i"
    sudo ./kvs_testcase 1 1 $i
    sudo ./kvs_testcase 0 1 $i
done

sudo rmmod emucxl_kernel