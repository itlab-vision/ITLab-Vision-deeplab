set(target ${MOVIE_MAKER_LIBRARY})

FILE(GLOB hdrs "*.h*" "${MOVIE_MAKER_INCLUDE}/*.h*")
FILE(GLOB srcs "*.cpp")

ADD_LIBRARY(${target} STATIC ${srcs} ${hdrs})
TARGET_LINK_LIBRARIES(${target} ${LIBRARY_DEPS})