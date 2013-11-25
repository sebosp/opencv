CC=g++ -std=c++11
LDLIBS=-lglut -lSOIL -lGLEW -lGL -lm -lboost_regex -lfreetype
CFLAGS=-Wall -ggdb
LIBS=-L/usr/lib
INC=-I/opt/local/include -I/usr/include/freetype2
OBJ=shader_utils.o wostat.o
mtastat: shader_utils.o wostat.o
	$(CC) $(LIBS) $(INC) $(CFLAGS) $(LDLIBS) $(OBJ) mtastat.cpp -o mtastat
all: mtastat
clean: 
	rm -f *.o mtastat
.PHONY: all clean
