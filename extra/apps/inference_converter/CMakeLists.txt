cmake_minimum_required(VERSION 2.8)

set(project inference_converter)
project(${project})

add_compile_options(-std=c++0x -O2 -Wall -W)

if (NOT DEFINED WITH_OPENMP)
    set(WITH_OPENMP Off CACHE BOOL "Enables to use OpenMP" FORCE)
endif()

if (WITH_OPENMP)
    add_definitions(-DWITH_OPENMP)
    add_compile_options(-fopenmp)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Modules
include_directories(include)

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIR})

find_package(HDF5)
include_directories(${HDF5_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Sources
set(Util_LIBS util)
set(InferenceConverter_LIBS ${project})

add_subdirectory(util)
add_subdirectory(src)
add_subdirectory(tools)
