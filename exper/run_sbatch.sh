#!/bin/sh
module load cuda/cuda-8.0 mkl2017

TIME=$(date -u +%s | sed "s/ /_/g")
OUTPUT_DIR=log
OUTPUT_FILE=${OUTPUT_DIR}/log_${TIME}.txt
TARGET=run_camvid_modern.sh

if [ ! -d ${OUTPUT_DIR} ]; then
    mkdir -p ${OUTPUT_DIR}
fi

sbatch --time=00:30:00 -p gpu -o ${OUTPUT_FILE} ${TARGET}