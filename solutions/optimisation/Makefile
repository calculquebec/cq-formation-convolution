COMMON_SOURCES= ../../lodepng.cpp ../../PACC/Tokenizer.cpp
COMMON_OBJECTS=$(COMMON_SOURCES:.cpp=.o)

CC=g++
#CC=icpc
CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -march=native -fno-inline-functions
#CFLAGS=-I../.. -g -std=c++11 -Wall -fopenmp -xHost -fno-inline-functions
OPT=-O3
PROFILE=

EXECUTABLES=1_convolution_func 2_convolution_double 3_convolution_transpose

MAKE_CMD=$(CC) $(CFLAGS) $(OPT) $(PROFILE) -o $@ $^

all: $(EXECUTABLES)

.cpp.o:
	$(CC) $(CFLAGS) $(OPT) $(PROFILE) -c -o $@ $<

clean:
	rm -f $(EXECUTABLE) *.o gmon.out

convolution: $(COMMON_OBJECTS) convolution.o 
	$(MAKE_CMD)

1_convolution_func: $(COMMON_OBJECTS) 1_convolution_func.o
	$(MAKE_CMD)

2_convolution_double: $(COMMON_OBJECTS) 2_convolution_double.o
	$(MAKE_CMD)

3_convolution_transpose: $(COMMON_OBJECTS) 3_convolution_transpose.o
	$(MAKE_CMD)
