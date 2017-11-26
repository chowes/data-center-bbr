#!/bin/bash

cong_ctl=$1

aggregator="blue00"
test_length=10

filename="/home/chowes/data-center-bbr/tests/results/throughput/thru_"$cong_ctl".csv" 
cp "/home/chowes/data-center-bbr/tests/results/throughput/thru_template.csv" $filename

for (( i = 1; i <= 20; i++ )); do
    for (( j = 0; j < 100; j++ )); do            

	echo "flows: $i - iter: $j"

        # start the aggregator and workers
    	pdsh -w $aggregator /home/chowes/data-center-bbr/aggregator throughput $i $test_length $testlength $filename &
    	aggregator_pid=$!
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

    	wait
    	sleep 3
    done
done
