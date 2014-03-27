CC=gcc
FL=-lX11 -lGL -ljpeg -lXext -lm
HD=-I`pwd`
SRC=xx.c XX.c sh.c
DST=xx.o XX.o sh.o
PROG=3d
all: $(DST)
	$(CC) -o $(PROG) $(DST) $(FL) $(HD)
$(DST): $(SRC)
	$(CC) -c $(SRC) $(FL) $(HD)
clean:
	rm -f $(DST)
