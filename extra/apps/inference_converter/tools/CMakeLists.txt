set(headers "")
set(sources mat_to_png.cpp)

add_executable(mat_to_png ${headers} ${sources})
target_link_libraries(mat_to_png 	
    ${MATIO_LIBRARIES}
    ${OpenCV_LIBS}
    ${Util_LIBS}
    ${InferenceConverter_LIBS}
    ${HDF5_LIBRARIES}
)

# Install
install(TARGETS mat_to_png DESTINATION bin)