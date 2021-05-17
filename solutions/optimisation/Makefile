COMMON_SOURCES= ../../lodepng.cpp ../../PACC/Tokenizer.cpp
COMMON_OBJECTS=$(COMMON_SOURCES:.cpp=.o)

CC=g++
#CC=icpc
CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -march=native -fno-inline-functions
#CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -xHost -fno-inline-functions
OPT=-O2 -ftree-vectorize
PROFILE=

EXECUTABLES=convolution 1_convolution_double 2_convolution_xy_loop 3_convolution_xj_loop 4_convolution_lrgba

MAKE_CMD=$(CC) $(CFLAGS) $(OPT) $(PROFILE) -o $@ $^

all: $(EXECUTABLES)

.cpp.o:
	$(CC) $(CFLAGS) $(OPT) $(PROFILE) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) *.o gmon.out

convolution: $(COMMON_OBJECTS) convolution.o 
	$(MAKE_CMD)

1_convolution_double: $(COMMON_OBJECTS) 1_convolution_double.o
	$(MAKE_CMD)

2_convolution_xy_loop: $(COMMON_OBJECTS) 2_convolution_xy_loop.o
	$(MAKE_CMD)

3_convolution_xj_loop: $(COMMON_OBJECTS) 3_convolution_xj_loop.o
	$(MAKE_CMD)

4_convolution_lrgba: $(COMMON_OBJECTS) 4_convolution_lrgba.o
	$(MAKE_CMD)
