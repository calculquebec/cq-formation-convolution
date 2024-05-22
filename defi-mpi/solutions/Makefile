EXECUTABLE=convolution

CC=mpic++
CFLAGS=-O3 -std=c++11 -Wall
DEBUG=-g
LIBS=-lpng

all: $(EXECUTABLE)

$(EXECUTABLE): convolution.cpp Makefile
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)
