#!/bin/bash
# Script for building Deeplab from sources
# supposed that all dependencies were build from sources

# FIX: write correct directory with installed dependencies
INSTALL_DIR="/home/user/Documents/install"
# FIX: write correct directory with dependecies built from sources
DEPENDENCIES_DIR="/home/user/Documents/dependencies"


# include direcotories
GLOG_INCLUDE_DIR=${INSTALL_DIR}/include/glog
GFLAGS_INCLUDE_DIR=${INSTALL_DIR}/include/gflags
PROTOBUF_INCLUDE_DIR=${INSTALL_DIR}/include/google/protobuf
Boost_INCLUDE_DIR=${INSTALL_DIR}/include

HDF5_DIR=${DEPENDENCIES_DIR}/HDF518CMake/hdf5-1.8.15-patch1/hdf5
Atlas_CBLAS_INCLUDE_DIR=${DEPENDENCIES_DIR}/ATLAS-3.10.2/include
Atlas_CLAPACK_INCLUDE_DIR=${DEPENDENCIES_DIR}/ATLAS-3.10.2-build/include
LEVELDB_INCLUDE=${DEPENDENCIES_DIR}/leveldb/include
SNAPPY_INCLUDE_DIR=${DEPENDENCIES_DIR}/snappy-1.1.1/snappy-1.1.1/include
LMDB_INCLUDE_DIR=${DEPENDENCIES_DIR}/lmdb/libraries/liblmdb
MATIO_INCLUDE_DIR=${DEPENDENCIES_DIR}/matio-1.5.2/matio/include

# library directories
GLOG_LIBRARY=${INSTALL_DIR}/lib/libglog.a
GFLAGS_LIBRARY=${INSTALL_DIR}/lib/libgflags.a
PROTOBUF_LIBRARY=${INSTALL_DIR}/lib/libprotobuf.a

Atlas_CBLAS_LIBRARY=${DEPENDENCIES_DIR}/ATLAS-3.10.2-build/lib/libcblas.a
Atlas_BLAS_LIBRARY=${DEPENDENCIES_DIR}/ATLAS-3.10.2-build/lib/libatlas.a
Atlas_LAPACK_LIBRARY=${DEPENDENCIES_DIR}/ATLAS-3.10.2-build/lib/liblapack.a
LEVELDB_LIBS=${DEPENDENCIES_DIR}/leveldb/libleveldb.a
SNAPPY_LIBS=${DEPENDENCIES_DIR}/snappy-1.1.1/snappy-1.1.1/lib/libsnappy.a
LMDB_LIBRARIES=${DEPENDENCIES_DIR}/lmdb/libraries/liblmdb/liblmdb.a
OpenCV_DIR=${DEPENDENCIES_DIR}/opencv-2.4.11-6.5
MATIO_LIBRARY=${DEPENDENCIES_DIR}/matio-1.5.2/matio/lib/libmatio.a

# need to export PATH to protoc compiler
export PATH=$PATH:${INSTALL_DIR}/bin
# need to export HDF5_DIR to find hdf5 include and library directories
export HDF5_DIR=${HDF5_DIR}

cmake -DGLOG_INCLUDE_DIR=${GLOG_INCLUDE_DIR} \
      -DGFLAGS_INCLUDE_DIR=${GFLAGS_INCLUDE_DIR} \
      -DAtlas_CBLAS_INCLUDE_DIR=${Atlas_CBLAS_INCLUDE_DIR} \
      -DAtlas_CLAPACK_INCLUDE_DIR=${Atlas_CLAPACK_INCLUDE_DIR} \
      -DLEVELDB_INCLUDE=${LEVELDB_INCLUDE} \
      -DSNAPPY_INCLUDE_DIR=${SNAPPY_INCLUDE_DIR} \
      -DLMDB_INCLUDE_DIR=${LMDB_INCLUDE_DIR} \
      -DPROTOBUF_INCLUDE_DIR=${PROTOBUF_INCLUDE_DIR} \
      -DMATIO_INCLUDE_DIR=${MATIO_INCLUDE_DIR} \
      -DBoost_INCLUDE_DIR=${Boost_INCLUDE_DIR} \
      -DGLOG_LIBRARY=${GLOG_LIBRARY} \
      -DGFLAGS_LIBRARY=${GFLAGS_LIBRARY} \
      -DAtlas_CBLAS_LIBRARY=${Atlas_CBLAS_LIBRARY} \
      -DAtlas_BLAS_LIBRARY=${Atlas_BLAS_LIBRARY} \
      -DAtlas_LAPACK_LIBRARY=${Atlas_LAPACK_LIBRARY} \
      -DLEVELDB_LIBS=${LEVELDB_LIBS} \
      -DSNAPPY_LIBS=${SNAPPY_LIBS} \
      -DLMDB_LIBRARIES=${LMDB_LIBRARIES} \
      -DOpenCV_DIR=${OpenCV_DIR} \
      -DPROTOBUF_LIBRARY=${PROTOBUF_LIBRARY} \
      -DMATIO_LIBRARY=${MATIO_LIBRARY} \
     ../ITLab-Vision-deeplab
make -j 4
