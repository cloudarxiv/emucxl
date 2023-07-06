#!/usr/bin/env python3
import matplotlib.pyplot as plt

local_enque_time = []
remote_enque_time = []

num_enque = []

if __name__ == "__main__":
    # read data from "results.txt"
    with open('result.txt', 'r') as f:
        for line in f:
            line = line.split()
            if line[0] == 'Test:':
                num_enque.append(str(line[1] + 'E + ' + line[2] + 'D + ' + line[3] + 'E' ))
            elif line[0] == 'Local_Time:':
                local_enque_time.append(float(line[1]))
            elif line[0] == 'Remote_Time:':
                remote_enque_time.append(float(line[1]))

    # plot graphs
    fig = plt.figure()
    plt.plot(num_enque, local_enque_time,'-s' , label='Local')
    plt.plot(num_enque, remote_enque_time, '-s', label='Remote')
    plt.xlabel('Number of Operations', fontsize = 8)
    plt.ylabel('Time (s)')
    plt.xticks(rotation = 5, fontsize = 6)
    plt.title('Time vs Number of Operations')
    plt.legend()
    plt.savefig('graph.png')