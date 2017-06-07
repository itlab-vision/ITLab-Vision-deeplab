#!/bin/sh

# ATTENTION: if you run testing existing model (when only RUN_TEST2=1)
# you need to put files run_pascal.sh, run_pascal_sbatch.sh, sub.sed
# into directory exper/ (at the same level as voc12 directory)

# general parameters
SCRIPTS_DIR=${PWD}
DEST_DIR=${SCRIPTS_DIR}/..
DATASET=voc12
NET_ID=deeplab_resnet_res1_original
DATA_ROOT=${HOME}/data/pascal/all

# parameters for run_pascal.sh
CAFFE_BIN=install/bin/caffe
NUM_LABELS=21
TRAIN_SET_SUFFIX=_aug
TRAIN_SET_STRONG=train
DEV_ID=0
RUN_TRAIN=0
RUN_TEST=1
RUN_TRAIN2=0
RUN_TEST2=0
RUN_TRAIN3=0
RUN_TEST3=0
RUN_SAVE=0

# directory hierarchy
EXP_DIR=${DEST_DIR}/exper/${DATASET}
EXP=${DATASET}
CONFIG_DIR=${EXP_DIR}/config/${NET_ID}
LIST_DIR=${EXP_DIR}/list
LOG_DIR=${EXP_DIR}/log/${NET_ID}
MODEL_DIR=${EXP_DIR}/model/${NET_ID}

if [ ! -d ${LOG_DIR} ]; then
    mkdir -p ${LOG_DIR}
fi

export GLOG_log_dir=${LOG_DIR}

# Training #1 (on official train)

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
    SNAPSHOT=`ls -t ${MODEL_DIR}/train_iter_*.solverstate | head -n 1`
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
        if [ ! -f "${SNAPSHOT}" ] && [ -f "${MODEL}" ]; then
            CMD="${CMD} --weights=${MODEL}"
        fi
        if [ -f "${SNAPSHOT}" ]; then
            CMD="${CMD} --snapshot=${SNAPSHOT}"
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
    mkdir -p ${FEATURE_DIR}/${TEST_SET}/fc1
    mkdir -p ${FEATURE_DIR}/${TEST_SET}/crf
    mkdir -p ${FEATURE_DIR}/${TEST_SET}/pred
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
    SNAPSHOT=`ls -t ${MODEL_DIR}/train2_iter_*.solverstate | head -n 1`
    MODEL=${MODEL_DIR}/init2.caffemodel
    if [ ! -f ${MODEL} ]; then
        MODEL=`ls -t ${MODEL_DIR}/train_iter_*.caffemodel | head -n 1`
    fi
    #
    echo Training2 net ${MODEL_DIR}
    FEATURE_DIR=${EXP_DIR}/features2/${NET_ID}
    mkdir -p ${FEATURE_DIR}/${TRAIN_SET}/fc8
    mkdir -p ${FEATURE_DIR}/${TRAIN_SET}/crf
    mkdir -p ${FEATURE_DIR}/${TRAIN_SET}/pred
    for pname in trainval solver2; do
       sed "$(eval echo $(cat sub.sed))" \
           ${CONFIG_DIR}/${pname}.prototxt > ${CONFIG_DIR}/${pname}_${TRAIN_SET}.prototxt
    done
    CMD="${CAFFE_BIN} train \
        --solver=${CONFIG_DIR}/solver2_${TRAIN_SET}.prototxt \
        --gpu=${DEV_ID}"
        if [ ! -f "${SNAPSHOT}" ] && [ -f "${MODEL}" ]; then
            CMD="${CMD} --weights=${MODEL}"
        fi
        if [ -f "${SNAPSHOT}" ]; then
            CMD="${CMD} --snapshot=${SNAPSHOT}"
        fi
    echo Running ${CMD} && ${CMD}
fi

# Test #2 on official val set

if [ ${RUN_TEST2} -eq 1 ]; then
    for TEST_SET in val; do
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
        mkdir -p ${FEATURE_DIR}/${TEST_SET}/pred
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

# Training #3 (sequence of finetune steps)
if [ ${RUN_TRAIN3} -eq 1 ]; then
    # Required sequence of solvers named: solver2_step_<step from 1>.prototxt
    # Required sequence of models named: trainval_step_<step from 1>.prototxt
    # Each next step uses initialization from previous step last snapshot
    # or from ${MODEL_DIR}/'_seq_prev.caffemodel'

    for STEP in 1 2 3; do
        TRAIN_SET=trainval${TRAIN_SET_SUFFIX}
        if [ -z ${TRAIN_SET_WEAK_LEN} ]; then
            TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}
            comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
        else
            TRAIN_SET_WEAK=${TRAIN_SET}_diff_${TRAIN_SET_STRONG}_head${TRAIN_SET_WEAK_LEN}
            comm -3 ${LIST_DIR}/${TRAIN_SET}.txt ${LIST_DIR}/${TRAIN_SET_STRONG}.txt | head -n ${TRAIN_SET_WEAK_LEN} > ${LIST_DIR}/${TRAIN_SET_WEAK}.txt
        fi

        MODEL=${MODEL_DIR}/_seq_prev.caffemodel
        if [ ! -f ${MODEL} ]; then
            if [ ${STEP} != 1 ]; then
                MODEL=`ls -t ${MODEL_DIR}/train2_step_$((${STEP}-1))_iter_*.caffemodel | head -n 1`
            else
                MODEL=`ls -t ${MODEL_DIR}/train_iter_*.caffemodel | head -n 1`
                if [ ! -f "${MODEL}" ]; then
                    MODEL=${MODEL_DIR}/init2.caffemodel
                fi
            fi
        fi
        if [ ! -f "${MODEL}" ]; then
            echo Unable to find initial weights for training3 step ${STEP}
            break
        fi
        
        echo Training3 \(step ${STEP}\) net ${MODEL_DIR}
        for pname in trainval_step_${STEP} solver2_step_${STEP}; do
            sed "$(eval echo $(cat sub.sed))" \
                ${CONFIG_DIR}/${pname}.prototxt > ${CONFIG_DIR}/${pname}_${TRAIN_SET}.prototxt
        done
        CMD="${CAFFE_BIN} train \
            --solver=${CONFIG_DIR}/solver2_step_${STEP}_${TRAIN_SET}.prototxt \
            --gpu=${DEV_ID}"
            if [ ! -f "${SNAPSHOT}" && -f "${MODEL}" ]; then
                CMD="${CMD} --weights=${MODEL}"
            fi
            if [ -f "${SNAPSHOT}" ]; then
                CMD="${CMD} --snapshot=${SNAPSHOT}"
            fi
        echo Running ${CMD} && ${CMD}
        cp -s `ls -t ${MODEL_DIR}/train2_step_${STEP}_iter_*.caffemodel | head -n 1` ${MODEL_DIR}/_seq_prev.caffemodel \
            && echo ${MODEL_DIR}/_seq_prev.caffemodel is set to `ls -t ${MODEL_DIR}/train2_step_${STEP}_iter_*.caffemodel | head -n 1`
    done
fi

# Test #3
if [ ${RUN_TEST3} -eq 1 ]; then
    for TEST_SET in train val test; do
        TEST_ITER=`cat ${LIST_DIR}/${TEST_SET}.txt | wc -l`
        MODEL=${MODEL_DIR}/test3.caffemodel
        if [ ! -f ${MODEL} ]; then
            MODEL=`ls -t ${MODEL_DIR}/train2_step_*_iter_*.caffemodel | head -n 1`
        fi
        #
        echo Testing3 net ${MODEL_DIR}
        FEATURE_DIR=${EXP_DIR}/features3/${NET_ID}
        mkdir -p ${FEATURE_DIR}/${TEST_SET}/fc8
        mkdir -p ${FEATURE_DIR}/${TEST_SET}/crf
        mkdir -p ${FEATURE_DIR}/${TEST_SET}/pred
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
