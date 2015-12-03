# Packages
include_directories(${DenseCRF_INCLUDE_DIRS})

include_directories(${Util_INCLUDE_DIRS})

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Sources
file(GLOB dense_inference_sources dense_inference.c*)

# Build
add_executable(refine_pascal ${dense_inference_sources})
add_dependencies(refine_pascal denseCRF)
target_link_libraries(refine_pascal
	denseCRF
	${MATIO_LIBRARIES}
	${OpenCV_LIBS}
)

# Install
install(TARGETS refine_pascal mat_to_png DESTINATION bin)