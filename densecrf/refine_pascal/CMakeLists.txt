# Packages

include_directories(${DenseCRF_INCLUDE_DIRS})

find_package(MATIO REQUIRED)
include_directories(${MATIO_INCLUDE_DIRS})

find_package(OpenCV COMPONENTS core highgui imgproc REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Sources

file(GLOB headers *.h*)
file(GLOB sources *.c*)

# Build

add_executable(refine_pascal ${headers} ${sources})
add_dependencies(refine_pascal denseCRF)
target_link_libraries(refine_pascal
	denseCRF
	${MATIO_LIBRARIES}
	${OpenCV_LIBS}
)

# Install

install(TARGETS refine_pascal DESTINATION bin)