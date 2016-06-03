cmake_minimum_required(VERSION 2.8)
project(refine_pascal)

add_compile_options(-std=c++0x -O2 -Wall -W)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()


set(DenseCRF_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libDenseCRF)

# Packages
include_directories(${DenseCRF_INCLUDE_DIRS})
include_directories(include)

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIR})

find_package(HDF5)
include_directories(${HDF5_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Sources
set(Util_LIBS util)

add_subdirectory(libDenseCRF)
add_subdirectory(util)
add_subdirectory(refine_pascal)
