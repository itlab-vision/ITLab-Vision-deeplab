#!/bin/bash

TASK=${PWD}/run_densecrf.sh
FILE=${PWD}/log_densecrf.txt
TIME=3000

echo "Command line: sbatch -p "all" -N 1 -o ${FILE} ${TASK}"

sbatch -p "all" -t ${TIME} -N 1 -o ${FILE} ${TASK}