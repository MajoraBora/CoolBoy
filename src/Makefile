CC=gcc
LIBS= -lSDL -lGL
CFLAGS = -Wall -g -std=c11 -D_POSIX_C_SOURCE=199309L 
SRC=$(wildcard *.c)
make: main.c
	$(CC) $(SRC) -o main $(CFLAGS) $(LIBS)
