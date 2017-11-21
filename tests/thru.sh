#!/bin/bash

cong_ctl=$1

aggregator="blue00"
mb_size=1048576

for (( i = 1; i <= 20; i++ )); do

	# create a results file template
	filename="results/throughput/throughput_test_"$cong_ctl"_"$i".csv"
	cp results/throughput/iperf_template.csv $filename

	# start the aggregator
	pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator throughput $i 0 30 .1 $filename


	# build the worker list
	worker_list=""
	# read the first i worker nodes from file
	input="./nodes"
	for (( j = 1; j <= $i; j++ )); do
		read -r line
		worker_list+="$line"
		worker_list+=","
	done < "$input"

	
	# start the workers
	pdsh -w $worker_list /home/chowes/data-center-bbr/worker $aggregator
done