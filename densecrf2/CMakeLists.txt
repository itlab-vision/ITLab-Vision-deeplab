cmake_minimum_required(VERSION 2.8)
project(refine_pascal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" -O2 -Wall -W)

if (NOT CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX install)
endif()
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Packages

set(DenseCRF_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libDenseCRF)
set(Util_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/util)

# Sources

add_subdirectory(libDenseCRF)
add_subdirectory(util)
add_subdirectory(refine_pascal)
