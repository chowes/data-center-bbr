#!/bin/bash

cong_ctl=$1

aggregator="blue00"
mb_size=1048576

filename="/home/chowes/data-center-bbr/tests/results/query/query_"$cong_ctl".csv" 
cp "/home/chowes/data-center-bbr/tests/results/query/query_template.csv" $filename

for (( i = 21; i <= 40; i++ )); do
	for ((j = 0; j < 100; j++)); do
		
		# start the aggregator and workers
		pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator query $i $mb_size $filename &
		sleep 3

		# start the workers
		
		# read the first i worker nodes from file
		input="./nodes"
		for (( k = 1; k <= $i; k++ )); do
			read -r line
			worker="$line"
			echo "starting worker: $worker"
			pdsh -w $worker /home/chowes/data-center-bbr/worker $aggregator &
		done < "$input"

		sleep 3
	done
done
