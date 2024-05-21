CC=g++
#CC=icpc
CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -march=native -fno-inline-functions
#CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -xHost -fno-inline-functions
OPT=-O2 -ftree-vectorize
PROFILE=
LIBS=-lpng

EXECUTABLES=convolution 1_convolution_double 2_convolution_soa 3_convolution_omp_simd

MAKE_CMD=$(CC) $(CFLAGS) $(OPT) $(PROFILE) $(LIBS) -o $@ $^

all: $(EXECUTABLES)

.cpp.o:
	$(CC) $(CFLAGS) $(OPT) $(PROFILE) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) *.o gmon.out

convolution: convolution.o
	$(MAKE_CMD)

1_convolution_double: 1_convolution_double.o
	$(MAKE_CMD)

2_convolution_soa: 2_convolution_soa.o
	$(MAKE_CMD)

3_convolution_omp_simd: 3_convolution_omp_simd.o
	$(MAKE_CMD)
