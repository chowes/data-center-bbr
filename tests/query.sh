#!/bin/bash

cong_ctl=$1

aggregator="blue00"
mb_size=1048576

filename="/home/chowes/data-center-bbr/tests/results/query/query_"$cong_ctl".csv" 
cp "/home/chowes/data-center-bbr/tests/results/query/query_template.csv" $filename

for (( i = 1; i <= 20; i++ )); do
	for ((j = 0; j < 100; j++)); do
		
		# build the worker list
		worker_list=""
		# read the first i worker nodes from file
		input="./nodes"
		for (( k = 1; k <= $i; k++ )); do
			read -r line
			worker_list+="$line"
			worker_list+=","
		done < "$input"
		
		echo $j
		
		# start the aggregator and workers
		pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator query $i $mb_size $filename &
		sleep 3
		pdsh -w $worker_list /home/chowes/data-center-bbr/worker $aggregator
		sleep 3

	done
done
