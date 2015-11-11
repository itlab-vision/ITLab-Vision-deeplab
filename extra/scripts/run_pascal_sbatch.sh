#!/bin/bash

TASK=${PWD}/run_pascal.sh
FILE=${PWD}/log.txt
TIME=3000

echo "Command line: sbatch -p "gpu" -N 1 -o ${FILE} ${TASK}"

sbatch -p "gpu" -t ${TIME} -N 1 -o ${FILE} ${TASK}
