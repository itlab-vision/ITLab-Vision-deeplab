#!/bin/bash
# Script to compile Dense CRF implementation
cd ../../../ITLab-Vision-deeplab-build
mkdir densecrf
cp -r ../ITLab-Vision-deeplab/densecrf/ ./
cd densecrf
make