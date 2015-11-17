## Execution Guide
General approach to execution is a set of scripts since there are a lot of parameters for running. There are 2 main scripts: 
 - ``` extra/scripts/run_pascal.sh ```
 - ``` extra/scripts/run_densecrf.sh ```

So, if you want to execute Deeplab, you need to set up these scripts accordingly to your configuration. <br/><br/>


1. Set variable __DEST_DIR__ in extra/scripts/deploy.sh as you need to and run that script.

2. To run system you need to configure variables in extra/scripts/run_pascal.sh and extra/scripts/run_densecrf.sh properly.

  __RUN_TRAIN__, __RUN_TEST__, __RUN_TRAIN2__, __RUN_TEST2__ stands for train, val, trainval, test subsets respectively,
  __NET_ID__ — for net name,
  __DATA_ROOT__ — for dataset files directory,
  __EXP__ — for 'exper' directory,
  __CAFFE_BIN__ — for install/tools/caffe binary.

3. If you want to learn model from a scratch, you need to download initial model weights:
  
  ```
  cd $DEST_DIR/model/$MODEL
  wget http://ccvl.stat.ucla.edu/ccvl/init_models/vgg16_20M.caffemodel
  cp -s $PWD/vgg16_20M.caffemodel init.caffemodel
  ```
  The next time you run run_pascal.sh script, it automatically sets initial weights of your model with these values on the TRAIN step.
