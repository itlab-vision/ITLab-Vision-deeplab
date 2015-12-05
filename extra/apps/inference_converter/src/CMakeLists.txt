include_directories(${Util_INCLUDE_DIRS})

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -fopenmp)

# Sources
file(GLOB headers *.h*)
file(GLOB sources *.c*)

# Build
add_library(${project} STATIC ${headers} ${sources})
target_link_libraries(${project} 
    ${MATIO_LIBRARIES}
    ${OpenCV_LIBS}
    ${Util_LIBS})