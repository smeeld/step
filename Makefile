CC=gcc
CFLAGS=-lX11 -lm -lXext -lGL -I/home/g -ljpeg

all: sh.c xx.c XX.c
	$(CC) -c sh.c xx.c XX.c $(CFLAGS)
clean:
	rm -f sh.o xx.o XX.o
