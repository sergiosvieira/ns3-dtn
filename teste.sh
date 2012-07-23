#!/bin/bash

for i in `seq 1 3`; do
	#RANDOM=$(date +%s)
	seed=$RANDOM
	vetor[$i]=$seed
done

for i in `seq 1 3`; do
	echo ${vetor[$i]}
done
