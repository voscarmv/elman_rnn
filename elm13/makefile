# Unix make file for pink noise program

CC = gcc
CFLAGS = -O2 -g

all: pink

pink: pink.o fftc.o wav.o lrand.o
	$(CC) -o pink pink.o fftc.o wav.o lrand.o -lm

pink.o: pink.c

fftc.o: fftc.c
	$(CC) $(CFLAGS) -DREALTR=1 -DCONJ=0 -c fftc.c

wav.o: wav.c wav.h

lrand.o: lrand.c
