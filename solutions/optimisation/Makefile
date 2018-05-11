COMMON_SOURCES= ../../lodepng.cpp ../../PACC/Tokenizer.cpp
COMMON_OBJECTS=$(COMMON_SOURCES:.cpp=.o)

CC=g++
#CC=icpc
CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -march=native -pg -fno-inline-functions
#CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -xHost -pg -fno-inline-functions
OPT=-O3

EXECUTABLES=1_convolution_func 2_convolution_elim_xy 3_convolution_elim_ij 4_convolution_colorfunc 5_convolution_transpose \
6_convolution_double convolution

MAKE_CMD=$(CC) $(CFLAGS) $(OPT) -o $@ $^

all: $(EXECUTABLES)

.cpp.o:
	$(CC) $(CFLAGS) $(OPT) -c -o $@ $<

clean:
	rm $(EXECUTABLE) $(OBJECT) gmon.out

convolution: $(COMMON_OBJECTS) convolution.o 
	$(MAKE_CMD)

1_convolution_func: $(COMMON_OBJECTS) 1_convolution_func.o 
	$(MAKE_CMD)

2_convolution_elim_xy: $(COMMON_OBJECTS) 2_convolution_elim_xy.o 
	$(MAKE_CMD)

3_convolution_elim_ij: $(COMMON_OBJECTS) 3_convolution_elim_ij.o 
	$(MAKE_CMD)

4_convolution_colorfunc: $(COMMON_OBJECTS) 4_convolution_colorfunc.o 
	$(MAKE_CMD)

5_convolution_transpose: $(COMMON_OBJECTS) 5_convolution_transpose.o 
	$(MAKE_CMD)

6_convolution_double: $(COMMON_OBJECTS) 6_convolution_double.o 
	$(MAKE_CMD)


