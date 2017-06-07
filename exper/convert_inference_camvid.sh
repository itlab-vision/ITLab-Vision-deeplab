#!/bin/sh

# Task parameters
MAKE_ARCHIVE=1
FEATURES=features # ...2
MODEL=deeplab_resnet_res1
FEATURE_TYPE=fc1 # pred, fc8, crf
SUBSETS=("trainval")

# Dataset parameters
DATASET=camvid
IMAGE_SIZE=960
CLASS_COUNT=32
DATASET_DIR="${HOME}/data/camvid/images"

# Environment
CONVERTER_BIN="${HOME}/distrib/ITLab-Vision-deeplab/extra/apps/inference_converter/build/install/bin/mat_to_png"
PYTHON_BIN="python"
CONVERTER_SCRIPT="./mat2png.py"
PAINTER_SCRIPT="./paint_camvid.py"
EXPER_DIR="${PWD}"
OUTPUT_DIR="${EXPER_DIR}/${DATASET}/res/${FEATURES}"
ZIP_BIN=zip
FREN_SCRIPT=./fren.py

for SUBSET in ${SUBSETS}; do
    INPUT_DIR="${EXPER_DIR}/${DATASET}/${FEATURES}/${MODEL}/${SUBSET}/${FEATURE_TYPE}"
    SAVE_DIR="${OUTPUT_DIR}/${MODEL}/${SUBSET}/${FEATURE_TYPE}/raw"
    mkdir -p "${SAVE_DIR}"
    ${PYTHON_BIN} "${FREN_SCRIPT}" "${INPUT_DIR}" "${INPUT_DIR}" "(\w+)_blob_0.mat" "\1.mat"

    if [ "$?" != "0" ]; then
        exit
    fi

    ${PYTHON_BIN} "${CONVERTER_SCRIPT}" "${INPUT_DIR}" "${SAVE_DIR}" "${DATASET_DIR}"
    #${CONVERTER_BIN} -i="${EXPER_DIR}/${DATASET}/${FEATURES}/${MODEL}/${SUBSET}/${FEATURE_TYPE}" -d="${DATASET_DIR}" -o="${SAVE_DIR}" -s=${IMAGE_SIZE} -c=${CLASS_COUNT} -b=true

    if [ "$?" != "0" ]; then
        exit
    fi
    
    if [ MAKE_ARCHIVE ] && [ "$?" == "0" ]; then
        INPUT_DIR_="${SAVE_DIR}"
        SAVE_DIR_="${OUTPUT_DIR}/${MODEL}/${SUBSET}/${FEATURE_TYPE}"
        ${ZIP_BIN} -q -b "${INPUT_DIR_}/../" "${SAVE_DIR_}/${MODEL}_${SUBSET}_${FEATURE_TYPE}_raw.zip" ${INPUT_DIR_}/*
        echo "Created archive: '${SAVE_DIR_}/${MODEL}_${SUBSET}_${FEATURE_TYPE}_raw.zip'"
    fi

    INPUT_DIR="${SAVE_DIR}"
    SAVE_DIR="${OUTPUT_DIR}/${MODEL}/${SUBSET}/${FEATURE_TYPE}/segm"
    mkdir -p "${SAVE_DIR}"
    ${PYTHON_BIN} "${PAINTER_SCRIPT}" "${INPUT_DIR}" "${SAVE_DIR}"

    if [ "$?" != "0" ]; then
        exit
    fi

    if [ MAKE_ARCHIVE ] && [ "$?" == "0" ]; then
        INPUT_DIR="${SAVE_DIR}"
        SAVE_DIR="${OUTPUT_DIR}/${MODEL}/${SUBSET}/${FEATURE_TYPE}"
        ${ZIP_BIN} -q -b "${INPUT_DIR}/../" "${SAVE_DIR}/${MODEL}_${SUBSET}_${FEATURE_TYPE}_segm.zip" ${INPUT_DIR}/*
        echo "Created archive: '${SAVE_DIR}/${MODEL}_${SUBSET}_${FEATURE_TYPE}_segm.zip'"
    fi
done