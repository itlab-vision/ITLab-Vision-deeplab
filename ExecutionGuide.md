## Execution Guide
General approach to execution is a set of scripts since there are a lot of parameters for running. There are 2 main scripts: 
 - ``` extra/scripts/run_pascal.sh ```
 - ``` extra/scripts/run_densecrf.sh ```

So, if you want to execute deeplab, you need to set up these scripts accordingly to your configuration. <br/><br/>


1. Set variable __DEST_DIR__ in extra/scripts/deploy.sh as you need to and run that script.

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

3. To run system you need to configure variables in extra/scripts/run_pascal.sh and extra/scripts/run_densecrf.sh properly.

  __RUN_TRAIN__, __RUN_TEST__, __RUN_TRAIN2__, __RUN_TEST2__ stands for train, val, trainval, test subsets accordingly,
  __NET_ID__ — for net name,
  __DATA_ROOT__ — for dataset files directory,
  __EXP__ — for 'exper' directory,
  __CAFFE_BIN__ — for install/tools/caffe binary.

4. If you want to learn model from scratch, you need to download initial model weights:
  
  ```
  cd $DEST_DIR/model/$MODEL
  wget http://ccvl.stat.ucla.edu/ccvl/init_models/vgg16_20M.caffemodel
  cp -s $PWD/vgg16_20M.caffemodel init.caffemodel
  ```
  The next time you run run_pascal.sh script, it automatically sets initial weights of your model with these values on the TRAIN step.
