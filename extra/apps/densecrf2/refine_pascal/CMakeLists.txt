set(headers "")
set(sources dense_inference.cpp)

# Build
add_executable(refine_pascal ${headers} ${sources})
target_link_libraries(refine_pascal
    denseCRF
    ${MATIO_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${OpenCV_LIBS}
    ${Util_LIBS}
)

# Install
install(TARGETS refine_pascal DESTINATION bin)
