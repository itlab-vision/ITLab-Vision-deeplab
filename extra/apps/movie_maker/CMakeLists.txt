cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME movie_maker)
project(${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

set(MOVIE_MAKER_LIBRARY ${PROJECT_NAME})

find_package(OpenCV REQUIRED)

set(MOVIE_MAKER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${MOVIE_MAKER_INCLUDE}")

set(LIBRARY_DEPS ${OpenCV_LIBS})

add_subdirectory(src)
add_subdirectory(samples)

message( STATUS "")
message( STATUS "General configuration for ${PROJECT_NAME}")
message( STATUS "======================================")
message( STATUS "")
message( STATUS "   Configuration:        ${CMAKE_BUILD_TYPE}")
message( STATUS "   OpenCV build path:    ${OpenCV_DIR}")
message( STATUS "")