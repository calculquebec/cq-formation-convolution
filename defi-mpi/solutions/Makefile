EXECUTABLE=convolution

CC=mpic++
CFLAGS=-O3 -std=c++11
DEBUG=-g -Wall
LIBS=-lpng

all: $(EXECUTABLE)

$(EXECUTABLE): convolution.cpp Makefile
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)
