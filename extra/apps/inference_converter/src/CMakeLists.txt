# Sources
set(headers "")
set(sources pascal_processing.cpp)

# Build
add_library(${project} STATIC ${headers} ${sources})
target_link_libraries(${project}
    ${MATIO_LIBRARIES}
    ${OpenCV_LIBS}
    ${Util_LIBS}
    ${HDF5_LIBRARIES}
)