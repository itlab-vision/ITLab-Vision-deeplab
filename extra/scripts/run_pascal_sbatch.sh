#!/bin/bash

TASK=${PWD}/run_pascal.sh
FILE=${PWD}/log.txt
TIME=3000

CMD="sbatch -p \"gpu\" -t ${TIME} -N 1 -o ${FILE} ${TASK}"

echo "Command line: ${CMD}"

${CMD}
