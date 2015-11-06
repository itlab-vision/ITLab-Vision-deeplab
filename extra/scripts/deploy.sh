#!/bin/bash
# Script helps to deploy Deeplab on a local machine

# general parameters
SCRIPTS_DIR=${PWD}
DEST_DIR=${PWD}/../..
DATASET=voc12
NET_ID=deeplab_largeFOV
DATA_ROOT=/common/itlab-vision-shared

# URLs to download solver and network parameters
SOLVER_URL=http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver.prototxt
SOLVER2_URL=http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver2.prototxt
TEST_NET_URL=http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/test.prototxt
TRAIN_NET_URL=http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train.prototxt
CAFFE_MODEL_URL=http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train2_iter_8000.caffemodel
LIST_URL=http://www.cs.ucla.edu/~lcchen/deeplab-public/list.zip

# parameters for run_pascal.sh
CAFFE_DIR=../../src/caffe
CAFFE_BIN=../../../ITLab-Vision-deeplab-build/tools/caffe
NUM_LABELS=21
TRAIN_SET_SUFFIX=_aug
TRAIN_SET_STRONG=train
DEV_ID=0
RUN_TRAIN=1
RUN_TEST=1
RUN_TRAIN2=1
RUN_TEST2=1
RUN_SAVE=0

if [ ! -d ${DEST_DIR} ]; then
  mkdir -p ${DEST_DIR}
fi

# create directory hierarchy
EXP_DIR=${DEST_DIR}/exper/${DATASET}
CONFIG_DIR=${EXP_DIR}/config/${NET_ID}
LIST_DIR=${EXP_DIR}/list
LOG_DIR=${EXP_DIR}/log/${NET_ID}
MODEL_DIR=${EXP_DIR}/model/${NET_ID}
mkdir -p ${EXP_DIR}/model
mkdir -p ${EXP_DIR}/res
mkdir -p ${EXP_DIR}/features
mkdir -p ${EXP_DIR}/features2
mkdir -p ${EXP_DIR}/log
mkdir -p ${EXP_DIR}/config
mkdir -p ${LIST_DIR}

mkdir -p ${CONFIG_DIR}
mkdir -p ${MODEL_DIR}
mkdir -p ${LOG_DIR}

# download solver and network description (.prototxt)
cd ${CONFIG_DIR}
wget ${SOLVER_URL}
wget ${SOLVER2_URL}
wget ${TEST_NET_URL}
wget ${TRAIN_NET_URL}

# download model
cd ${MODEL_DIR}
wget ${CAFFE_MODEL_URL}

# download train and test lists
cd ${LIST_DIR}
wget ${LIST_URL}
unzip -j list.zip
rm list.zip

# run experiment
cd ${SCRIPTS_DIR}
source run_pascal.sh
# source run_densecrf.sh