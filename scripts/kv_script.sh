#! /bin/bash
cd ../src
make clean > /dev/null
make > /dev/null 2>/dev/null

sudo insmod emucxl_kernel.ko
cd ../scripts
echo "Policy 1: Move the object from remote memory to local memory if it is currently in remote memory." 
echo "Policy 0: Keep the object in remote memory if it is currently in remote memory."

for i in 0 10 20 30 40 50 60 70 80 90
do
    echo "$i% of total objects receive 'get' requests." 
    echo "Policy 1" 
    sudo ../src/kvs_testcase 1 1 $i # 1: policy, 1: seed value, $i: percentage of get requests
    echo "Policy 0"
    sudo ../src/kvs_testcase 0 1 $i # 0: policy, 1: seed value, $i: percentage of get requests
done
sudo rmmod emucxl_kernel