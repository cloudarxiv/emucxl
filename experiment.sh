#! /bin/bash

# This script is used to run the experiment for the paper
# key value store
echo "KV start"
sudo bash kv_script.sh > results/kv_results.txt

echo "KV done"
# direct appl. e.g. queue
echo "Queue start"
sudo bash queue_script.sh > results/queue_results.txt

echo "Queue done"
# parse the results
echo "Queue stats start"
sudo python3 parser_table.py > results/queue_stats.txt

echo "Queue stats done"