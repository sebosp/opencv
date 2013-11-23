CC=g++
LDLIBS=-lglut -lSOIL -lGLEW -lGL -lm
CFLAGS=-Wall -ggdb
OBJ=shader_utils.o
tunnel: shader_utils.o NGon.o
ch01: shader_utils.o
	$(CC) $(CFLAGS) $(LDLIBS) $(OBJ) ch01.cpp -o ch01
all: tunnel
clean: 
	rm -f *.o tunnel ch01
.PHONY: all clean
