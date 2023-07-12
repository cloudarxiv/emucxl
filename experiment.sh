#! /bin/bash

# This script is used to run the experiment for the paper
# key value store
sudo bash kv_script.sh > results/kv.txt

echo "KV done"
# direct appl. e.g. queue
sudo bash queue_script.sh > results/queue.txt

echo "Queue done"
sudo python3 parser_table.py > results/queue_table.txt

echo "Queue table done"