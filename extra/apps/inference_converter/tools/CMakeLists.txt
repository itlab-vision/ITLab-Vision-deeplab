include_directories(${Util_INCLUDE_DIRS})

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Sources
file(GLOB mat_to_png_sources mat_to_png.c*)

# Build 
add_executable(mat_to_png ${mat_to_png_sources})
target_link_libraries(mat_to_png 	
	${MATIO_LIBRARIES}
	${OpenCV_LIBS}
	${Util_LIBS}
)

# Install
install(TARGETS mat_to_png DESTINATION bin)