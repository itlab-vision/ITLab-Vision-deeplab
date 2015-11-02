# Script helps to deploy Deeplab on a local machine

DEST_DIR = $PWD/..
DATASET = voc12
MODELS = deeplab_largeFOV


if [ ! -d ${DEST_DIR} ]; then
  mkdir -p ${DEST_DIR}
fi

DATASET_DIR = ${DEST_DIR}/exper/${DATASET}
mkdir -p ${DATASET_DIR}/model
mkdir -p ${DATASET_DIR}/res
mkdir -p ${DATASET_DIR}/features
mkdir -p ${DATASET_DIR}/features2
mkdir -p ${DATASET_DIR}/log
mkdir -p ${DATASET_DIR}/config
mkdir -p ${DATASET_DIR}/list

for MODEL in ${MODELS}; do
  mkdir -p ${DATASET_DIR}/config/${MODEL}
  mkdir -p ${DATASET_DIR}/model/${MODEL}
done
