#!bin/bash
# Install Deeplab and its dependencies (Ubuntu 14.04)

# install dependencies excluding CUDA Driver and CUDA Toolkit
sudo apt-get install libboost-all-dev libgoogle-glog-dev libgflags-dev \
                     libatlas-base-dev libhdf5-serial-dev libleveldb-dev \
                     liblmdb-dev libsnappy-dev libopencv-dev \
                     libprotobuf-dev protobuf-compiler

# install MATIO library
wget http://downloads.sourceforge.net/project/matio/matio/1.5.2/matio-1.5.2.tar.gz
tar -zxvf matio-1.5.2.tar.gz
cd matio-1.5.2
 ./configure
make
make check
make install
cd ..
rm -r matio-1.5.2

# install deeplab from sources using CMake
USR_INCLUDE_DIR = /usr/include
USR_LIBRARY_DIR = /usr/lib/x86_64-linux-gnu

USR_LOCAL_INCLUDE_DIR = /usr/local/include
USR_LOCAL_LIBRARY_DIR = /usr/local/lib

USR_SHARE_DIR = /usr/share

git clone https://github.com/ITLab-Vision/ITLab-Vision-deeplab.git
mkdir ITLab-Vision-deeplab-build
cd ITLab-Vision-deeplab-build
cmake -DGLOG_INCLUDE_DIR = $USR_INCLUDE_DIR/glog \
      -DGFLAGS_INCLUDE_DIR = $USR_INCLUDE_DIR/gflags \
      -DAtlas_CBLAS_INCLUDE_DIR = $USR_INCLUDE_DIR/atlas \
      -DAtlas_CLAPACK_INCLUDE_DIR = $USR_INCLUDE_DIR/atlas \
      -DLEVELDB_INCLUDE = $USR_INCLUDE_DIR/leveldb \
      -DSNAPPY_INCLUDE_DIR = $USR_INCLUDE_DIR \
      -DLMDB_INCLUDE_DIR = $USR_INCLUDE_DIR \
      -DPROTOBUF_INCLUDE_DIR = $USR_INCLUDE_DIR/google/protobuf \
      -DMATIO_INCLUDE_DIR = $USR_LOCAL_INCLUDE_DIR \
      -DBoost_INCLUDE_DIR = $USR_INCLUDE_DIR/boost \
      -DGLOG_LIBRARY = $USR_LIBRARY_DIR/libglog.a \
      -DGFLAGS_LIBRARY = $USR_LIBRARY_DIR/libgflags.a \
      -DAtlas_CBLAS_LIBRARY = $USR_LIBRARY_DIR/libcblas.a \
      -DAtlas_BLAS_LIBRARY = $USR_LIBRARY_DIR/libatlas.a \
      -DAtlas_LAPACK_LIBRARY = $USR_LIBRARY_DIR/liblapack.a \
      -DLEVELDB_LIBS = $USR_LIBRARY_DIR/libleveldb.a \
      -DSNAPPY_LIBS = $USR_LIBRARY_DIR/libsnappy.a \
      -DLMDB_LIBRARIES = $USR_LIBRARY_DIR/liblmdb.a \
      -DOpenCV_DIR = $USR_SHARE_DIR/opencv-2.4.11/build \
      -DPROTOBUF_LIBRARY = $USR_LIBRARY_DIR/libprotobuf.a \
      -DMATIO_LIBRARY = $USR_LOCAL_LIBRARY_DIR/libmatio.a \
     ../ITLab-Vision-deeplab
make
make install