#!/bin/bash

# Script to resize images using ImageMagick

INP_IMAGE_DIR=/common/itlab-vision-shared/vehicles/imgs_track_10_5000-7000
OUT_IMAGE_DIR=/common/itlab-vision-shared/vehicles/resized_track_10_5000-7000
IMAGE_MAGICK_BIN_DIR=${HOME}/Documents/dependencies/ImageMagick-6.9.2-6/utilities
IMAGE_EXT=".jpg"
MAX_WIDTH=513
MAX_HEIGHT=513

export PATH=${PATH}:${IMAGE_MAGICK_BIN_DIR}
echo ${PATH}

cd ${INP_IMAGE_DIR}
for i in $( ls *${IMAGE_EXT});
do
    h=`identify "$i" | cut -f 3 -d ' ' | sed 's/.*x//'`
    w=`identify "$i" | cut -f 3 -d ' ' | sed 's/x.*//'`
    SCALE_WIDTH=1
    SCALE_HEIGHT=1
    if [ $w -gt ${MAX_WIDTH} ]
    then
        SCALE_WIDTH=$(echo "${MAX_WIDTH}/${w}" | bc -l)
    fi
    if [ $h -gt ${MAX_HEIGHT} ]
    then
        SCALE_HEIGHT=${MAX_HEIGHT}/${h}
    fi
    SCALE_FACTOR=1
    if [ $(echo "${SCALE_WIDTH}>${SCALE_HEIGHT}" | bc) -eq 1 ]
    then
        SCALE_FACTOR=$(echo "${SCALE_HEIGHT}*100" | bc)
    else
        SCALE_FACTOR=$(echo "${SCALE_WIDTH}*100" | bc);
    fi
    convert -resize ${SCALE_FACTOR}% $i ${OUT_IMAGE_DIR}/$i
done