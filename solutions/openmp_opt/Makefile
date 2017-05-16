COMMON_SOURCES= ../../lodepng.cpp ../../PACC/Tokenizer.cpp
COMMON_OBJECTS=$(COMMON_SOURCES:.cpp=.o)

CC=g++
CC=icpc
CFLAGS=-g -std=c++11 -Wall -fopenmp
CFLAGS=-g -std=c++11 -Wall -fopenmp -xHost -pg -fno-inline-functions
OPT=-O3

EXECUTABLES=convolution_double_openmp

MAKE_CMD=$(CC) $(CFLAGS) $(OPT) -o $@ $^

all: $(EXECUTABLES)

.cpp.o:
	$(CC) $(CFLAGS) $(OPT) -c -o $@ $<

clean:
	rm $(EXECUTABLE) $(OBJECT) gmon.out

convolution_double_openmp: $(COMMON_OBJECTS) convolution_double_openmp.o 
	$(MAKE_CMD)

