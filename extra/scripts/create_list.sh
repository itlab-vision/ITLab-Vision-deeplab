#!/bin/bash

EXP_DIR=${PWD}/../../exper
DATASET=voc12
IMGS_DIR=/common/itlab-vision-shared/vehicles
IMGS_SUB_DIR=imgs_track_10_5000-7000
TEST_SET=test

cd ${IMGS_DIR}/${IMGS_SUB_DIR}
(ls | sed "s|^|${IMGS_SUB_DIR}/|") > ${EXP_DIR}/${DATASET}/list/${TEST_SET}.txt
(ls -1 | sed "s|\(.*\)\..*|\1|")  > ${EXP_DIR}/${DATASET}/list/${TEST_SET}_id.txt