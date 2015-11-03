# update the path variables 

CC	= g++
CFLAGS	= -W -Wall -O2
CXXFLAGS = -std=c++0x


all:
	make clean
	make prog_test_densecrf
	make prog_refine_pascal
	make prog_refine_pascal_v5

clean:
	rm -f *.a
	rm -f *.o
	rm -f prog_test_densecrf
	rm -f prog_refine_pascal
	rm -f prog_refine_pascal_v5

libDenseCRF.a: libDenseCRF/bipartitedensecrf.cpp libDenseCRF/densecrf.cpp libDenseCRF/filter.cpp libDenseCRF/permutohedral.cpp libDenseCRF/util.cpp libDenseCRF/densecrf.h libDenseCRF/fastmath.h libDenseCRF/permutohedral.h libDenseCRF/sse_defs.h libDenseCRF/util.h
	$(CC) libDenseCRF/bipartitedensecrf.cpp libDenseCRF/densecrf.cpp libDenseCRF/filter.cpp libDenseCRF/permutohedral.cpp libDenseCRF/util.cpp -c $(CFLAGS)
	ar rcs libDenseCRF.a bipartitedensecrf.o densecrf.o filter.o permutohedral.o util.o

prog_test_densecrf: test_densecrf/simple_dense_inference.cpp libDenseCRF.a
	$(CC) test_densecrf/simple_dense_inference.cpp -o prog_test_densecrf $(CFLAGS) -L. -lDenseCRF

prog_refine_pascal: refine_pascal/dense_inference.cpp refine_pascal/dense_inference.h util/Timer.h libDenseCRF.a
	$(CC) refine_pascal/dense_inference.cpp -o prog_refine_pascal $(CFLAGS) -L. -lDenseCRF -I./refine_pascal/ -I./util/

prog_refine_pascal_v5: refine_pascal_v5/dense_inference_cpp0x.cpp util/Timer.h libDenseCRF.a refine_pascal_v5/dense_inference_cpp0x.hxx
	$(CC) refine_pascal_v5/dense_inference_cpp0x.cpp -o prog_refine_pascal_v5 $(CFLAGS) $(CXXFLAGS) -L. -lDenseCRF -lmatio -I./util/

