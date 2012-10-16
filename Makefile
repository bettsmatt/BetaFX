CC = g++
LPATH = -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
CFLAGS=-g -Wall -Wextra -pedantic
LIBS=-lGL -lglut -lGLU -ljpeg -lpng15 -lm
IPATH= -I/usr/pkg/include

all:
	$(CC) $(CFLAGS) -o BetaFX *.cpp $(IPATH) $(LIBS) $(LDPATH) $(LPATH)
	
clean :
	rm -rf *.o
	rm BetaFX
