#!/bin/bash

TASK=${PWD}/run_densecrf.sh
FILE=${PWD}/log_densecrf.txt
TIME=3000

CMD="sbatch -p all -t ${TIME} -N 1 -o ${FILE} ${TASK}"

echo "Command line: ${CMD}"

${CMD}