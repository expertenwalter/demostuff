CC=gcc
CFLAGS=-O3 -Wall -std=c99
#CFLAGS=-g -Wall -ansi -D_DEBUG_

LIBS=-lm -lSDL -lGL
EXECNAME=opengl
EXT=c

SOURCES  := $(wildcard *.$(EXT))         
OBJECTS  := $(patsubst %.$(EXT), %.o, $(SOURCES))

$(EXECNAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECNAME) $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJECTS) $(EXECNAME) *.o
