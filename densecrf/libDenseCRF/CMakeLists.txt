project(denseCRF)

file(GLOB headers *.h*)
file(GLOB sources *.c*)

# Build

add_library(denseCRF STATIC ${headers} ${sources})

# Install

install(TARGETS denseCRF 
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)