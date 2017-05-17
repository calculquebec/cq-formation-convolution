SOURCES=  lodepng.cpp PACC/Tokenizer.cpp

OBJECTS=$(SOURCES:.cpp=.o) convolution.o

CC=nvcc
LIBS=
LIB_PATHS=
INCLUDE_PATHS=
CFLAGS=-O3 -g -std=c++11

EXECUTABLE=convolution

MAKE_CMD=$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIB_PATHS) $(INCLUDE_PATHS) $(LIBS) 

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MAKE_CMD)

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

convolution.o:
	$(CC) $(CFLAGS) -c convolution.cu -o convolution.o

remake:
	rm $(EXECUTABLE)
	$(MAKE_CMD)

clean:
	rm $(EXECUTABLE)
	rm $(OBJECTS)

omp: convolution_omp.o lodepng.o PACC/Tokenizer.o
	$(CC) $(CFLAGS) -o convolution_omp convolution_omp.o lodepng.o PACC/Tokenizer.o $(LIB_PATHS) $(INCLUDE_PATHS) $(LIBS) 


