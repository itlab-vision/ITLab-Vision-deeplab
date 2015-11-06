#!/bin/bash
# Script helps to deploy Deeplab on a local machine

SCRIPTS_DIR=${PWD}
DEST_DIR=${PWD}/../..
DATASET=voc12
MODEL=deeplab_largeFOV

if [ ! -d ${DEST_DIR} ]; then
  mkdir -p ${DEST_DIR}
fi

# create directory hierarchy
EXPERIMENT_DIR=${DEST_DIR}/exper
DATASET_DIR=${EXPERIMENT_DIR}/${DATASET}
mkdir -p ${DATASET_DIR}/model
mkdir -p ${DATASET_DIR}/res
mkdir -p ${DATASET_DIR}/features
mkdir -p ${DATASET_DIR}/features2
mkdir -p ${DATASET_DIR}/log
mkdir -p ${DATASET_DIR}/config
mkdir -p ${DATASET_DIR}/list

mkdir -p ${DATASET_DIR}/config/${MODEL}
mkdir -p ${DATASET_DIR}/model/${MODEL}

# download solver and network description (.prototxt)
cd ${DATASET_DIR}/config/${MODEL}
wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver.prototxt
wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver2.prototxt
wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/test.prototxt
wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train.prototxt

# download model
cd ${DATASET_DIR}/model/${MODEL}
wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train2_iter_8000.caffemodel

# download train and test lists
cd ${DATASET_DIR}/list
wget http://www.cs.ucla.edu/~lcchen/deeplab-public/list.zip
unzip -j list.zip
rm list.zip

# execute experiment
cd ${SCRIPTS_DIR}
# source run_pascal.sh
# source run_densecrf.sh