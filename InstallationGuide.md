##Installation Guide

This document is Deeplab installation guide. Here you can find step-by-step
instructions to install dependencies and Deeplab itself.

###BOOST
From Sources:
```
wget http://sourceforge.net/projects/boost/files/boost/1.54.0/boost_1_54_0.tar.gz
tar -xzvf boost_1_54_0.tar.gz
cd boost_1_54_0
./bootstrap.sh --prefix=/home/<username>/Documents/install/boost
./b2 install --with=all
```

From Packages:
```
sudo apt-get install libboost-all-dev
```

###GLOG
From Sources:
```
wget https://google-glog.googlecode.com/files/glog-0.3.3.tar.gz
tar -xzvf glog-0.3.3.tar.gz
cd glog-0.3.3
./configure --prefix=/home/<username>/Documents/install
make
make install DESTDIR=/home/<username>/Documents/install
```

From Packages:
```
sudo apt-get install libgoogle-glog-dev
```

###GFLAGS
From Sources:
```
git clone https://github.com/gflags/gflags
mv gflags gflags-2.1.2
mkdir gflags-2.1.2-build
cd gflags-2.1.2-build
cmake -DCMAKE_INSTALL_PREFIX=../../install -g make ../gflags-2.1.2
make
make install
```

From Packages:
```
sudo apt-get install libgflags-dev
```

###ATLAS
From Sources:
```
Скачать ATLAS http://sourceforge.net/projects/math-atlas/files/Stable/3.10.2/atlas3.10.2.tar.bz2/download
Скачать LAPACK http://www.netlib.org/lapack/#_lapack_version_3_4_1

tar -zxvf lapack-3.4.1.tgz
cd lapack-3.4.1
cp INSTALL/make.inc.gfortran make.inc
// в файле make.inc
// 1. OPTS = -O2 -fPIC
// 2. NOOPT = -O0 -fPIC
cd SRC
make

tar -xvf atlas3.10.2.tar.bz2
mv ATLAS ATLAS-3.10.2
mkdir atlas_build
cd atlas_build
// вызвать cpufreq-set -g performance (или cpufreq-selector -g performance),
// если конфигурирование было прервано с ошибкой
../ATLAS-3.10.2/configure -b 64 \
                                          --prefix=../../../install \
                                          --with-netlib-lapack-tarfile=../../lapack-3.4.1.tgz
```

From Packages:
```
sudo apt-get install libatlas-base-dev
```

###HDF5 
[From Sources]((https://www.hdfgroup.org/ftp/HDF5/current/src/unpacked/release_docs/INSTALL)):
```
wget http://www.hdfgroup.org/ftp/HDF5/current/src/CMake/hdf5-1.8.15-patch1-CMake.tar.gz
tar -zxvf hdf5-1.8.15-patch1-CMake.tar.gz
cd HDF518CMake/hdf5-1.8.15-patch1
./configure --prefix=/home/<user>/Documents/install/hdf5/ --enable-build-all=yes --enable-fortran=yes --enable-cxx=yes
make
make check
make install
```

From Packages:
```
sudo apt-get install libhdf5-serial-dev
```

###LEVELDB
From Sources:
```
git clone https://github.com/google/leveldb
cd leveldb
make
```

From Packages:
```
sudo apt-get install libleveldb-dev
```

###LMDB
From Sources:
```
git clone https://github.com/LMDB/lmdb
cd lmdb/libraries/liblmdb
make
```

From Packages:
```
sudo apt-get install liblmdb-dev
```

###SNAPPY
From Sources:
```
wget https://snappy.googlecode.com/files/snappy-1.1.1.tar.gz
tar -xzvf snappy-1.1.1.tar.gz
cd snappy-1.1.1
 ./configure --prefix=/home/<username>/Documents/dependencies/snappy-1.1.1/snappy-1.1.1/
make
make install
```

From Packages:
```
sudo apt-get install libsnappy-dev
```

###OPENCV
[From Sources](http://docs.opencv.org/3.0-last-rst/doc/tutorials/introduction/linux_install/linux_install.html):
```
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
```
sudo apt-get install libopencv-dev
```

###PROTOBUF
[From Sources](https://github.com/google/protobuf):
```
git clone https://github.com/google/protobuf
cd protobuf
./autogen.sh
./configure --prefix=/home/<username>/Documents/install
make
make install
export PATH=$PATH:/home/<username>/Documents/install/bin
```

From Packages:
```
sudo apt-get install libprotobuf-dev protobuf-compiler
```

###MATIO
```
wget http://sourceforge.net/projects/matio/files/latest/download
tar -zxvf matio-1.5.2.tar.gz
cd matio-1.5.2
// при конфигурировании могут возникнуть проблемы с относительным путем до библиотеки HDF5
// решение: использовать абсолютный путь
 ./configure --with-hdf5=/home/<username>/Documents/install../HDF518CMake/hdf5-1.8.15-patch1/hdf5 \
                  --prefix=/home/<username>/Documents/dependencies/matio-1.5.2/matio
make
make check
make install
```

###DEEPLAB
```
git clone https://bitbucket.org/deeplab/deeplab-public.git
mkdir deeplab-public-build
cd deeplab-public-build
export PATH=$PATH:/home/<username>/Documents/install/bin
// требуется предварительное изменение CMakeLists.txt, т.к. в коде явно подключается
// библиотека matio (!не опционально), см. примечания с деталями вносимых изменений
// командная строка cmake находится в скрипте 	

cmake -DGLOG_INCLUDE_DIR=../../install/include/glog \
      -DGFLAGS_INCLUDE_DIR=../../install/include/gflags \
      -DAtlas_CBLAS_INCLUDE_DIR=../../dependencies/ATLAS-3.10.2/include \
      -DAtlas_CLAPACK_INCLUDE_DIR=../../dependencies/ATLAS-3.10.2-build/include \
      -DLEVELDB_INCLUDE=../../dependencies/leveldb/include \
      -DSNAPPY_INCLUDE_DIR=../../dependencies/snappy-1.1.1/snappy-1.1.1/include \
      -DLMDB_INCLUDE_DIR=../../dependencies/lmdb/libraries/liblmdb \
      -DPROTOBUF_INCLUDE_DIR=../../install/include/google/protobuf \
      -DMATIO_INCLUDE_DIR=../../dependencies/matio-1.5.2/matio/include \
      -DBoost_INCLUDE_DIR=../../install/include/boost \
      -DGLOG_LIBRARY=../../install/lib/libglog.a \
      -DGFLAGS_LIBRARY=../../install/lib/libgflags.a \
      -DAtlas_CBLAS_LIBRARY=../../dependencies/ATLAS-3.10.2-build/lib/libcblas.a \
      -DAtlas_BLAS_LIBRARY=../../dependencies/ATLAS-3.10.2-build/lib/libatlas.a \
      -DAtlas_LAPACK_LIBRARY=../../dependencies/ATLAS-3.10.2-build/lib/liblapack.a \
      -DLEVELDB_LIBS=../../dependencies/leveldb/libleveldb.a \
      -DSNAPPY_LIBS=../../dependencies/snappy-1.1.1/snappy-1.1.1/lib/libsnappy.a \
      -DLMDB_LIBRARIES=../../dependencies/lmdb/libraries/liblmdb/liblmdb.a \
      -DOpenCV_DIR=../../dependencies/opencv-2.4.11/build \
      -DPROTOBUF_LIBRARY=../../install/lib/libprotobuf.a \
      -DMATIO_LIBRARY=/home/<username>/Documents/dependencies/matio-1.5.2/matio/lib/libmatio.a \
     ../deeplab-public
make
make install
```
