SOURCES= convolution.cpp lodepng.cpp PACC/Tokenizer.cpp

OBJECTS=$(SOURCES:.cpp=.o)

CC=pgc++
LIBS=
LIB_PATHS=
INCLUDE_PATHS=
CFLAGS=-O3 -g -std=c++11 -fast -Minfo=accel,intensity,ccff -ta=tesla,lineinfo

EXECUTABLE=convolution

MAKE_CMD=$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIB_PATHS) $(INCLUDE_PATHS) $(LIBS) 

all: $(SOURCES) $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS) 
	$(MAKE_CMD)

.cpp.o: 
	$(CC) $(CFLAGS) -c -o $@ $<

remake:
	rm $(EXECUTABLE)
	$(MAKE_CMD)

clean:
	rm $(EXECUTABLE) $(OBJECT) gmon.out

