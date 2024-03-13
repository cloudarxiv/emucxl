#!/usr/bin/env python3

import re
import statistics

# Read text from file
with open("results/queue_results.txt", "r") as file:
    text = file.read()

# Extract experiment number, enqueue time, and dequeue time using regular expressions
experiments = re.findall(r'Run (\d+)', text)
local_enqueue_times = re.findall(r'Local_Enque_Time: ([\d.]+)', text)
local_dequeue_times = re.findall(r'Local_Deque_Time: ([\d.]+)', text)
remote_enqueue_times = re.findall(r'Remote_Enque_Time: ([\d.]+)', text)
remote_dequeue_times = re.findall(r'Remote_Deque_Time: ([\d.]+)', text)

# Create table
table = []
for i in range(len(experiments)):
    row = [int(experiments[i]), float(local_enqueue_times[i]), float(local_dequeue_times[i]), float(remote_enqueue_times[i]), float(remote_dequeue_times[i])]
    table.append(row)

# Calculate mean and standard deviation
mean_local_enqueue = round(statistics.mean([row[1] for row in table]), 2)
mean_local_dequeue = round(statistics.mean([row[2] for row in table]), 2)
mean_remote_enqueue = round(statistics.mean([row[3] for row in table]), 2)
mean_remote_dequeue = round(statistics.mean([row[4] for row in table]), 2)

std_dev_local_enqueue = round(statistics.stdev([row[1] for row in table]), 2)
std_dev_local_dequeue = round(statistics.stdev([row[2] for row in table]), 2)
std_dev_remote_enqueue = round(statistics.stdev([row[3] for row in table]), 2)
std_dev_remote_dequeue = round(statistics.stdev([row[4] for row in table]), 2)

print(f"Mean & {mean_local_enqueue} & {mean_remote_enqueue} & {mean_local_dequeue} & {mean_remote_dequeue}\\\\")

print(f"Standard Deviation & {std_dev_local_enqueue} & {std_dev_remote_enqueue} & {std_dev_local_dequeue} & {std_dev_remote_dequeue}\\\\")

