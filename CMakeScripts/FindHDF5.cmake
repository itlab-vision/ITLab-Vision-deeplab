# - Try to find HDF5
#
# The following variables are optionally searched for defaults
#  HDF5_ROOT_DIR:            Base directory where all HDF5 components are found
#
# The following are set after configuration is done:
#  HDF5_FOUND
#  HDF5_INCLUDE_DIRS
#  HDF5_LIBRARIES

include(FindPackageHandleStandardArgs)

set(HDF5_ROOT_DIR "" CACHE PATH "Folder contains HDF5")
set(HDF5_INCLUDE_SEARCH_PATHS
    $ENV{HDF5_ROOT_DIR}
    $ENV{HDF5_DIR}/include)
set(HDF5_LIBRARY_SEARCH_PATHS
    $ENV{HDF5_ROOT_DIR}
    $ENV{HDF5_DIR}/lib)

find_path(HDF5_INCLUDE_DIR hdf5.h
    PATHS ${HDF5_INCLUDE_SEARCH_PATHS})

find_library(HDF5_LIBRARY hdf5
    PATHS ${HDF5_LIBRARY_SEARCH_PATHS}
    PATH_SUFFIXES
        lib)
find_library(HDF5_HL_LIBRARY hdf5_hl
    PATHS ${HDF5_LIBRARY_SEARCH_PATHS}
    PATH_SUFFIXES
        lib)


find_package_handle_standard_args(HDF5 DEFAULT_MSG
    HDF5_INCLUDE_DIR HDF5_LIBRARY HDF5_HL_LIBRARY)

MESSAGE(STATUS "HDF5 include directory: " ${HDF5_INCLUDE_DIR})
MESSAGE(STATUS "HDF5 libraries: " ${HDF5_LIBRARY} "; " ${HDF5_HL_LIBRARY})

if(HDF5_FOUND)
    set(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIR})
    set(HDF5_LIBRARIES ${HDF5_LIBRARY} ${HDF5_HL_LIBRARY})
endif()
