#!/bin/bash

cong_ctl=$1

aggregator="blue00"
mb_size=1048576

for (( i = 1; i <= 20; i++ )); do
	for ((j = 0; j < 10; j++)); do
		
		# start the aggregator
		echo pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator query $i $mb_size "query_test_"$cong_ctl".csv"


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

		
		# start the workers
		echo pdsh -w $worker_list /home/chowes/data-center-bbr/worker $aggregator

	done
done
