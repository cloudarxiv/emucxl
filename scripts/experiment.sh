#! /bin/bash

# This script is used to run the experiment for the paper
# key value store
echo "KV start"
sudo bash ../src/kv_script.sh > ../results/kv_results.txt

echo "KV done"
# direct appl. e.g. queue
echo "Queue start"
sudo bash ../src/queue_script.sh > ../results/queue_results.txt

echo "Queue done"
# parse the results
echo "Queue stats start"
sudo python3 ../experiments/parser_table.py > ../results/queue_stats.txt

echo "Queue stats done"