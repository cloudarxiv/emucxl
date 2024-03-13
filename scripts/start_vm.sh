#! /bin/bash

TOTAL_MEMORY=16384
MEMORY_NODE0=8192
MEMORY_NODE1=8192


sudo /opt/qemu-8.2.2/bin/qemu-system-x86_64 -name emucxlVM \
-machine type=pc,accel=kvm,mem-merge=off -enable-kvm \
-cpu host -smp cpus=8 -m ${TOTAL_MEMORY}M \
-object memory-backend-ram,size=${MEMORY_NODE0}M,policy=bind,host-nodes=0,id=ram-node0,prealloc=on,prealloc-threads=8 \
-numa node,nodeid=0,cpus=0-7,memdev=ram-node0 \
-object memory-backend-ram,size=${MEMORY_NODE1}M,policy=bind,host-nodes=1,id=ram-node1,prealloc=on,prealloc-threads=8 \
-numa node,nodeid=1,memdev=ram-node1 \
-device virtio-scsi-pci,id=scsi0 \
-device scsi-hd,drive=hd0 \
-drive file=~/images/emucxl.qcow2,if=none,aio=native,cache=none,format=qcow2,id=hd0 \
-net user,hostfwd=tcp::8080-:22 \
-net nic,model=virtio \
-device virtio-net,netdev=network0 \
-netdev tap,id=network0,ifname=tap0,script=no,downscript=no \
-nographic