## DenseCRF 2.0

This updated implementation for denseCRF module is intended to fix some issues with original implementation.

### Code

The code is modified from the publicly available code by Philipp Krähenbühl and Vladlen Koltun.
See their project [website](http://www.philkr.net/home/densecrf) for more information 

If you also use this part of code, please cite their [paper](http://googledrive.com/host/0B6qziMs8hVGieFg0UzE0WmZaOW8/papers/densecrf.pdf):
Efficient Inference in Fully Connected CRFs with Gaussian Edge Potentials, Philipp Krähenbühl and Vladlen Koltun, NIPS 2011.

## Installation

This tool uses some external libraries:
- [OpenCV](http://opencv.org/) (tested on version 3.0)
- [MatIO](https://sourceforge.net/projects/matio/) (tested on version 1.5.2)

There are usual steps to build and install this tool:
```
mkdir build && cd build 
cmake ..
make && make install
```

Please see run_densecrf.sh for examples of input arguments or see the dense_inference.cpp.

### Caffe wrapper

There also a wrapper for original implementation in Caffe (see the layer densecrf_layer.cpp).