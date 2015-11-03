##Installation Guide

This document is Deeplab installation guide. Here you can find step-by-step
instructions to install dependencies and Deeplab itself:
- [CUDA](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#cuda)
- [Boost](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#boost)
- [Glog](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#glog)
- [Gflags](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#gflags)
- [Atlas](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#atlas)
- [HDF5](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#hdf5)
- [leveldb](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#leveldb)
- [lmdb](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#lmdb)
- [Snappy](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#snappy)
- [OpenCV](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#opencv)
- [ProtoBuf](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#protobuf)
- [MATIO](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#matio)
- [Deeplab](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#deeplab)
- [Dense CRF](https://github.com/ITLab-Vision/ITLab-Vision-deeplab/blob/installation-guide/InstallationGuide.md#dense-crf)

###CUDA

1. Download run-file of CUDA Toolkit (version >= 5.0, for example,
   [CUDA Toolkit 7.5](https://developer.nvidia.com/cuda-toolkit)).
2. Execute run-file.

Note: CUDA Toolkit is required to build Deeplab. Nowadays Deeplab doesn't
support `CPU_ONLY` mode.

###BOOST
From Sources:
```bash
wget http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.gz
tar -xzvf boost_1_54_0.tar.gz
cd boost_1_54_0
./bootstrap.sh --prefix=/home/<username>/Documents/install/boost
./b2 install --with=all
```

From Packages:
```bash
sudo apt-get install libboost-all-dev
```

###GLOG
From Sources:
```bash
wget https://google-glog.googlecode.com/files/glog-0.3.3.tar.gz
tar -xzvf glog-0.3.3.tar.gz
cd glog-0.3.3
./configure --prefix=/home/<username>/Documents/install
make
make install DESTDIR=/home/<username>/Documents/install
```

From Packages:
```bash
sudo apt-get install libgoogle-glog-dev
```

###GFLAGS
From Sources:
```bash
git clone https://github.com/gflags/gflags
mv gflags gflags-2.1.2
mkdir gflags-2.1.2-build
cd gflags-2.1.2-build
cmake -DCMAKE_INSTALL_PREFIX=/home/<username>/Documents/install -g make ../gflags-2.1.2
make
make install
```

From Packages:
```bash
sudo apt-get install libgflags-dev
```

###ATLAS
From Sources:

1. Download [ATLAS](http://sourceforge.net/projects/math-atlas/files/Stable/3.10.2/atlas3.10.2.tar.bz2/download)
2. Download [LAPACK](http://www.netlib.org/lapack/#_lapack_version_3_4_1)

```bash
tar -zxvf lapack-3.4.1.tgz
cd lapack-3.4.1
cp INSTALL/make.inc.gfortran make.inc
// correct make.inc:
// 1. OPTS = -O2 -fPIC
// 2. NOOPT = -O0 -fPIC
cd SRC
make

tar -xvf atlas3.10.2.tar.bz2
mv ATLAS ATLAS-3.10.2
mkdir atlas_build
cd atlas_build
// call cpufreq-set -g performance (or cpufreq-selector -g performance),
// if configure procedure failed
../ATLAS-3.10.2/configure -b 64 \
      --prefix=../../../install \
      --with-netlib-lapack-tarfile=../../lapack-3.4.1.tgz
```

From Packages:
```bash
sudo apt-get install libatlas-base-dev
```

###HDF5 
[From Sources](https://www.hdfgroup.org/ftp/HDF5/current/src/unpacked/release_docs/INSTALL):
```bash
wget http://www.hdfgroup.org/ftp/HDF5/current/src/CMake/hdf5-1.8.15-patch1-CMake.tar.gz
tar -zxvf hdf5-1.8.15-patch1-CMake.tar.gz
cd HDF518CMake/hdf5-1.8.15-patch1
./configure --prefix=/home/<username>/Documents/install/hdf5/ \
            --enable-build-all=yes --enable-fortran=yes --enable-cxx=yes
make
make check
make install
```

From Packages:
```bash
sudo apt-get install libhdf5-serial-dev
```

###LEVELDB
From Sources:
```bash
git clone https://github.com/google/leveldb
cd leveldb
make
```

From Packages:
```bash
sudo apt-get install libleveldb-dev
```

###LMDB
From Sources:
```bash
git clone https://github.com/LMDB/lmdb
cd lmdb/libraries/liblmdb
make
```

From Packages:
```bash
sudo apt-get install liblmdb-dev
```

###SNAPPY
From Sources:
```bash
wget https://snappy.googlecode.com/files/snappy-1.1.1.tar.gz
tar -xzvf snappy-1.1.1.tar.gz
cd snappy-1.1.1
 ./configure --prefix=/home/<username>/Documents/dependencies/snappy-1.1.1/snappy-1.1.1/
make
make install
```

From Packages:
```bash
sudo apt-get install libsnappy-dev
```

###OPENCV
[From Sources](http://docs.opencv.org/3.0-last-rst/doc/tutorials/introduction/linux_install/linux_install.html):
```bash
wget https://github.com/Itseez/opencv/archive/2.4.11.zip
unzip 2.4.11
cd opencv-2.4.11
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_INSTALL_PREFIX=/home/<username>/Documents/install ..
make
make install
```

From Packages:
```bash
sudo apt-get install libopencv-dev
```

###PROTOBUF
[From Sources](https://github.com/google/protobuf):
```bash
git clone https://github.com/google/protobuf
cd protobuf
./autogen.sh
./configure --prefix=/home/<username>/Documents/install
make
make install
export PATH=$PATH:/home/<username>/Documents/install/bin
```

From Packages:
```bash
sudo apt-get install libprotobuf-dev protobuf-compiler
```

###MATIO
```bash
wget http://sourceforge.net/projects/matio/files/latest/download
tar -zxvf matio-1.5.2.tar.gz
cd matio-1.5.2
// if configure procedure failed with relative path to HDF5 try absolute path
 ./configure --with-hdf5=/home/<username>/Documents/install../HDF518CMake/hdf5-1.8.15-patch1/hdf5 \
             --prefix=/home/<username>/Documents/dependencies/matio-1.5.2/matio
make
make check
make install
```

###DEEPLAB
```bash
git clone https://github.com/ITLab-Vision/ITLab-Vision-deeplab.git
mkdir ITLab-Vision-deeplab-build
cd ITLab-Vision-deeplab-build
export PATH=$PATH:/home/<username>/Documents/install/bin

cmake -DGLOG_INCLUDE_DIR=<path> \
      -DGFLAGS_INCLUDE_DIR=<path> \
      -DAtlas_CBLAS_INCLUDE_DIR=<path> \
      -DAtlas_CLAPACK_INCLUDE_DIR=<path> \
      -DLEVELDB_INCLUDE=<path> \
      -DSNAPPY_INCLUDE_DIR=<path> \
      -DLMDB_INCLUDE_DIR=<path> \
      -DPROTOBUF_INCLUDE_DIR=<path> \
      -DMATIO_INCLUDE_DIR=<path> \
      -DBoost_INCLUDE_DIR=<path> \
      -DGLOG_LIBRARY=<path>/libglog.a \
      -DGFLAGS_LIBRARY=<path>/libgflags.a \
      -DAtlas_CBLAS_LIBRARY=<path>/libcblas.a \
      -DAtlas_BLAS_LIBRARY=<path>/libatlas.a \
      -DAtlas_LAPACK_LIBRARY=<path>/liblapack.a \
      -DLEVELDB_LIBS=<path>/libleveldb.a \
      -DSNAPPY_LIBS=<path>/libsnappy.a \
      -DLMDB_LIBRARIES=<path>/liblmdb.a \
      -DOpenCV_DIR=<path>/opencv-2.4.11/build \
      -DPROTOBUF_LIBRARY=<path>/libprotobuf.a \
      -DMATIO_LIBRARY=<path>/libmatio.a \
     ../ITLab-Vision-deeplab
make
make install
```

###Dense CRF
```bash
cd ITLab-Vision-deeplab/densecrf
# correct Makefile before make
make
# You will see 4 binary files:
# library libDenseCRF.a,
# applications - prog_refine_pascal, prog_refine_pascal_v4, prog_test_dence_crf
```