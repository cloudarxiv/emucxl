#! /bin/bash

make clean > /dev/null
make > /dev/null 2>/dev/null
sudo insmod emucxl_kernel.ko
for i in {1..10}
do
    echo "Running experiments $i"
    sudo ./direct_queue
done

sudo rmmod emucxl_kernel