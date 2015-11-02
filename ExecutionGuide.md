## Execution Guide
1. Set variable DEST_DIR in scripts/deploy.sh as you need to and run that script.

2. If your dataset is PASCAL VOC and MODEL is Deeplab-largeFOV, then download next files to DEST_DIR/voc12:
  
  ```
  cd $DEST_DIR/config/deeplab-largeFOV
  wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver.prototxt
  wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/solver2.prototxt
  wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/test.prototxt
  wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train.prototxt
  
  cd $DEST_DIR/model/deeplab-largeFOV
  wget http://ccvl.stat.ucla.edu/ccvl/DeepLab-LargeFOV/train2_iter_8000.caffemodel
  
  cd $DEST_DIR/list
  wget http://www.cs.ucla.edu/~lcchen/deeplab-public/list.zip
  unzip list.zip .
  ```

3. To run system you need to configure variables in scripts/run_pascal.sh and scripts/run_densecrf.sh properly.

  ```RUN_TRAIN, RUN_TEST, RUN_TRAIN2, RUN_TEST2``` stands for train, val, trainval, test subsets accordingly.
  ```NET_ID``` stands for network name
  ```DATA_ROOT``` stands for dataset files directory
  ```EXP``` stands for 'exper' directory
  ```CAFFE_BIN``` stands for install/tools/caffe

4. If you want to learn model from scratch, you need to download initial model weights:
  
  ```
  cd $DEST_DIR/model/$MODEL
  wget http://ccvl.stat.ucla.edu/ccvl/init_models/vgg16_20M.caffemodel
  cp -s $PWD/vgg16_20M.caffemodel init.caffemodel
  ```
