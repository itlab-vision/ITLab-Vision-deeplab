project(${Util_LIBS})

# Sources
file(GLOB headers *.h*)
file(GLOB sources *.c*)

# Build
add_library(util STATIC ${headers} ${sources})
