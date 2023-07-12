#! /bin/bash

# This script is used to run the experiment for the paper
# direct appl. e.g. queue
sudo bash queue_script.sh > results/queue.txt
sudo python3 parser_table.py > results/queue_table.txt

# key value store
sudo bash kv_script.sh > results/kv.txt