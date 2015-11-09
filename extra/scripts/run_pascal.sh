#!/bin/sh

# general parameters
SCRIPTS_DIR=${PWD}
DEST_DIR=${SCRIPTS_DIR}/../..
DATASET=voc12
NET_ID=deeplab_largeFOV
DATA_ROOT=/common/itlab-vision-shared/pascal/pascal2012devkit

# parameters for run_pascal.sh
CAFFE_DIR=../../src/caffe
CAFFE_BIN=../../../ITLab-Vision-deeplab-build/tools/caffe
NUM_LABELS=21
TRAIN_SET_SUFFIX=_aug
TRAIN_SET_STRONG=train
DEV_ID=0
RUN_TRAIN=0
RUN_TEST=0
RUN_TRAIN2=0
RUN_TEST2=1
RUN_SAVE=0

# directory hierarchy
EXP_DIR=${DEST_DIR}/exper/${DATASET}
EXP=$EXP_DIR
CONFIG_DIR=${EXP_DIR}/config/${NET_ID}
LIST_DIR=${EXP_DIR}/list
LOG_DIR=${EXP_DIR}/log/${NET_ID}
MODEL_DIR=${EXP_DIR}/model/${NET_ID}

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/common/cuda-6.5/lib64
export GLOG_log_dir=${LOG_DIR}

# Training #1 (on train_aug)

if [ ${RUN_TRAIN} -eq 1 ]; then
    #
    TRAIN_SET=train${TRAIN_SET_SUFFIX}
    if [ -z ${TRAIN_SET_WEAK_LEN} ]; then
	TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}
	comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
    else
	TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}_head${TRAIN_SET_WEAK_LEN}
	comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt | head -n ${TRAIN_SET_WEAK_LEN} > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
    fi
    #
    MODEL=${MODEL_DIR}/init.caffemodel
    #
    echo Training net ${MODEL_DIR}
    for pname in train solver; do
	sed "$(eval echo $(cat sub.sed))" \
	    ${CONFIG_DIR}/${pname}.prototxt > ${CONFIG_DIR}/${pname}_${TRAIN_SET}.prototxt
    done
        CMD="${CAFFE_BIN} train \
         --solver=${CONFIG_DIR}/solver_${TRAIN_SET}.prototxt \
         --gpu=${DEV_ID}"
	if [ -f ${MODEL} ]; then
	    CMD="${CMD} --weights=${MODEL}"
	fi
	echo Running ${CMD} && ${CMD}
fi

# Test #1 specification (on val or test)

if [ ${RUN_TEST} -eq 1 ]; then
    #
    for TEST_SET in val; do
	TEST_ITER=`cat ${LIST_DIR}/${TEST_SET}.txt | wc -l`
	MODEL=${MODEL_DIR}/test.caffemodel

	if [ ! -f ${MODEL} ]; then
	    MODEL=`ls -t ${MODEL_DIR}/train_iter_*.caffemodel | head -n 1`
	fi
	#
	echo Testing net ${MODEL_DIR}
	FEATURE_DIR=${EXP_DIR}/features/${NET_ID}
	mkdir -p ${FEATURE_DIR}/${TEST_SET}/fc8
	mkdir -p ${FEATURE_DIR}/${TEST_SET}/crf
	sed "$(eval echo $(cat sub.sed))" \
	    ${CONFIG_DIR}/test.prototxt > ${CONFIG_DIR}/test_${TEST_SET}.prototxt
	CMD="${CAFFE_BIN} test \
             --model=${CONFIG_DIR}/test_${TEST_SET}.prototxt \
             --weights=${MODEL} \
             --gpu=${DEV_ID} \
             --iterations=${TEST_ITER}"
	echo Running ${CMD} && ${CMD}
    done
fi

# Training #2 (finetune on trainval_aug)

if [ ${RUN_TRAIN2} -eq 1 ]; then
    #
    TRAIN_SET=trainval${TRAIN_SET_SUFFIX}
    if [ -z ${TRAIN_SET_WEAK_LEN} ]; then
	TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}
	comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
    else
	TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}_head${TRAIN_SET_WEAK_LEN}
	comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt | head -n ${TRAIN_SET_WEAK_LEN} > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
    fi
    #
    MODEL=${MODEL_DIR}/init2.caffemodel
    if [ ! -f ${MODEL} ]; then
	MODEL=`ls -t ${MODEL_DIR}/train_iter_*.caffemodel | head -n 1`
    fi
    #
    echo Training2 net ${MODEL_DIR}
    for pname in train solver2; do
	sed "$(eval echo $(cat sub.sed))" \
	    ${CONFIG_DIR}/${pname}.prototxt > ${CONFIG_DIR}/${pname}_${TRAIN_SET}.prototxt
    done
        CMD="${CAFFE_BIN} train \
         --solver=${CONFIG_DIR}/solver2_${TRAIN_SET}.prototxt \
         --weights=${MODEL} \
         --gpu=${DEV_ID}"
	echo Running ${CMD} && ${CMD}
fi

# Test #2 on official test set

if [ ${RUN_TEST2} -eq 1 ]; then
    #
    for TEST_SET in val test; do
	TEST_ITER=`cat ${LIST_DIR}/${TEST_SET}.txt | wc -l`
	MODEL=${MODEL_DIR}/test2.caffemodel
	if [ ! -f ${MODEL} ]; then
	    MODEL=`ls -t ${MODEL_DIR}/train2_iter_*.caffemodel | head -n 1`
	fi
	#
	echo Testing2 net ${MODEL_DIR}
	FEATURE_DIR=${EXP_DIR}/features2/${NET_ID}
	mkdir -p ${FEATURE_DIR}/${TEST_SET}/fc8
	mkdir -p ${FEATURE_DIR}/${TEST_SET}/crf
	sed "$(eval echo $(cat sub.sed))" \
	    ${CONFIG_DIR}/test.prototxt > ${CONFIG_DIR}/test_${TEST_SET}.prototxt
	CMD="${CAFFE_BIN} test \
             --model=${CONFIG_DIR}/test_${TEST_SET}.prototxt \
             --weights=${MODEL} \
             --gpu=${DEV_ID} \
             --iterations=${TEST_ITER}"
	echo Running ${CMD} && ${CMD}
    done
fi

# Translate and save the model

if [ ${RUN_SAVE} -eq 1 ]; then
    #
    MODEL=${MODEL_DIR}/test2.caffemodel
    if [ ! -f ${MODEL} ]; then
	MODEL=`ls -t ${MODEL_DIR}/train*_iter_*.caffemodel | head -n 1`
    fi
    MODEL_DEPLOY=${MODEL_DIR}/deploy.caffemodel
    #
    echo Translating net ${MODEL_DIR}
        CMD="${CAFFE_BIN} save \
         --model=${CONFIG_DIR}/deploy.prototxt \
         --weights=${MODEL} \
         --out_weights=${MODEL_DEPLOY}"
	echo Running ${CMD} && ${CMD}
fi
