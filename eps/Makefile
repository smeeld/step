CC=c++
DST=eps
CFL=-std=c++0x -lpthread
HED=-I`pwd`
PRE=http_handler.o main.o
SRC=main.cpp http_handler.cpp
all: $(PRE)
	$(CC) -o $(DST) $(PRE) $(CFL)
$(PRE): $(SRC)
	$(CC) -c $(SRC) $(CFL) $(HED)
clean:
	rm -f $(PRE)
