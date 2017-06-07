#!/bin/bash

DEPENDENCIES_DIR=../../install
MATIO_LIBRARY_DIR=${DEPENDENCIES_DIR}/matio/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${MATIO_LIBRARY_DIR}
HDF5_LIBRARY_DIR=${DEPENDENCIES_DIR}/hdf5/lib
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${HDF5_LIBRARY_DIR}

###########################################
# You can either use this script to generate the DenseCRF post-processed results
# or use the densecrf_layer (wrapper) in Caffe
###########################################
DATASET=camvid          #voc12, coco

MODEL_NAME=msc_largefov
TEST_SET=test           #val, test

# the features  folder save the features computed via the model trained with the train set
# the features2 folder save the features computed via the model trained with the trainval set
FEATURE_NAME=features #features, features2

# specify the DenseCRF parameters
MAX_ITER=10

Bi_W=5
Bi_X_STD=50
Bi_Y_STD=50
Bi_R_STD=3
Bi_G_STD=3
Bi_B_STD=3

POS_W=3
POS_X_STD=3
POS_Y_STD=3

#######################################
# MODIFY THE PATH FOR YOUR SETTING
#######################################
EXP_DIR=${PWD}
SAVE_DIR=${EXP_DIR}/${DATASET}/res/${FEATURE_NAME}/${MODEL_NAME}/${TEST_SET}/fc8/post_densecrf_W${Bi_W}_XStd${Bi_X_STD}_RStd${Bi_R_STD}_PosW${POS_W}_PosXStd${POS_X_STD}

echo "SAVE TO ${SAVE_DIR}"

CRF_DIR=${EXP_DIR}/../extra/apps/densecrf2/build/install/bin

IMG_DIR=../../data/camvid/images_full

# the features are saved in .mat format
CRF_BIN=${CRF_DIR}/refine_pascal
FEATURE_DIR=${EXP_DIR}/${DATASET}/${FEATURE_NAME}/${MODEL_NAME}/${TEST_SET}/fc8

mkdir -p ${SAVE_DIR}

# run the program
${CRF_BIN} -id ${IMG_DIR} -fd ${FEATURE_DIR} -sd ${SAVE_DIR} -i ${MAX_ITER} -px ${POS_X_STD} -py ${POS_Y_STD} -pw ${POS_W} -bx ${Bi_X_STD} -by ${Bi_Y_STD} -br ${Bi_R_STD} -bg ${Bi_G_STD} -bb ${Bi_B_STD} -bw ${Bi_W}

