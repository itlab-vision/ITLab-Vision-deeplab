cmake_minimum_required(VERSION 2.8)

set(project inference_converter)
project(${project})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" -O2 -Wall -W)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Packages

set(Util_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/util)
set(Util_LIBS util)

set(InferenceConverter_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(InferenceConverter_LIBS ${project})

# Sources

add_subdirectory(util)
add_subdirectory(src)
add_subdirectory(tools)
