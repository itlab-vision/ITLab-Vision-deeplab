# update the path variables
DEPENDENCIES_PATH = $(HOME)/Documents/dependencies
HDF5_LIBRARY_PATH  = $(DEPENDENCIES_PATH)/HDF518CMake/hdf5-1.8.15-patch1/hdf5/lib
MATIO_LIBRARY_PATH = $(DEPENDENCIES_PATH)/matio-1.5.2/matio/lib
MATIO_INCLUDE_PATH = $(DEPENDENCIES_PATH)/matio-1.5.2/matio/include



CC	= g++
CFLAGS	= -W -Wall -O2
CXXFLAGS = -std=c++0x


all:
	make clean
	make prog_test_densecrf
	make prog_refine_pascal
	make prog_refine_pascal_v4

clean:
	rm -f *.a
	rm -f *.o
	rm -f prog_test_densecrf
	rm -f prog_refine_pascal
	rm -f prog_refine_pascal_v4

libDenseCRF.a: libDenseCRF/bipartitedensecrf.cpp libDenseCRF/densecrf.cpp libDenseCRF/filter.cpp \
	libDenseCRF/permutohedral.cpp libDenseCRF/util.cpp libDenseCRF/densecrf.h libDenseCRF/fastmath.h \
	libDenseCRF/permutohedral.h libDenseCRF/sse_defs.h libDenseCRF/util.h
	$(CC) libDenseCRF/bipartitedensecrf.cpp libDenseCRF/densecrf.cpp libDenseCRF/filter.cpp \
	libDenseCRF/permutohedral.cpp libDenseCRF/util.cpp -c $(CFLAGS)
	ar rcs libDenseCRF.a bipartitedensecrf.o densecrf.o filter.o permutohedral.o util.o

prog_test_densecrf: test_densecrf/simple_dense_inference.cpp libDenseCRF.a
	$(CC) test_densecrf/simple_dense_inference.cpp -o prog_test_densecrf $(CFLAGS) -L. -lDenseCRF

prog_refine_pascal: refine_pascal/dense_inference.cpp refine_pascal/dense_inference.h util/Timer.h libDenseCRF.a
	$(CC) refine_pascal/dense_inference.cpp -o prog_refine_pascal $(CFLAGS) -L. -lDenseCRF -I./refine_pascal/ -I./util/

prog_refine_pascal_v4: refine_pascal_v4/dense_inference.cpp util/Timer.h libDenseCRF.a \
	$(HDF5_LIBRARY_PATH)/libhdf5_hl.a     $(HDF5_LIBRARY_PATH)/libhdf5.a \
	$(HDF5_LIBRARY_PATH)/libhdf5_hl_cpp.a $(HDF5_LIBRARY_PATH)/libhdf5_cpp.a \
	$(MATIO_LIBRARY_PATH)/libmatio.a
	$(CC) refine_pascal_v4/dense_inference.cpp -o prog_refine_pascal_v4 $(CFLAGS) $(CXXFLAGS) \
	-I./util/ -I$(MATIO_INCLUDE_PATH)/ \
	-L. -L$(MATIO_LIBRARY_PATH)/ -L$(HDF5_LIBRARY_PATH)/ -lDenseCRF -lmatio -lhdf5_hl -lhdf5

