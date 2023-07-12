#! /bin/bash

make clean > /dev/null
make > /dev/null 2>/dev/null
sudo insmod emucxl_kernel.ko
echo "Policy 1: Fetch object to local if it is in remote"
echo "Policy 0: Keep object in remote if it is in remote"
for i in 0 10 20 30 40 50 60 70 80 90
do
    echo "Policy 1 and 0, $i% of objects getting get requests"
    sudo ./kvs_testcase 1 1 $i
    sudo ./kvs_testcase 0 1 $i
done
sudo rmmod emucxl_kernel