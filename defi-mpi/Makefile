EXECUTABLE=convolution

CC=g++
CFLAGS=-O3 -std=c++11 -Wall
DEBUG=-g -Wall
LIBS=-lpng

all: $(EXECUTABLE)

$(EXECUTABLE): convolution.cpp Makefile
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)
