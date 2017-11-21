#!/bin/bash

cong_ctl=$1

aggregator="blue00"
mb_size=1048576

for (( i = 1; i <= 20; i++ )); do

	echo pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator throughput $i 0 30 .1 "throughput_test_"$cong_ctl"_"$i".csv"


	# build the worker list

	worker_list=""
	# read the first i worker nodes from file
	input="./nodes"
	for (( j = 1; j <= $i; j++ )); do
		read -r line
		worker_list+="$line"
		worker_list+=","
		echo $worker_list
	done < "$input"

	
	# start each worker

	echo pdsh -w $worker_list /home/chowes/data-center-bbr/worker $aggregator
done