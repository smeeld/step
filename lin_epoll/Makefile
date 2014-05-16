CC=c++
DST=eps
CFL=-std=c++0x -lpthread
HED=-I`pwd`
PRE=main.o serv.o hd.o
SRC=serv.cpp hd.cpp main.cpp
all: $(PRE)
	$(CC) -o $(DST) $(PRE) $(CFL)
$(PRE): $(SRC)
	$(CC) -c $(SRC) $(CFL) $(HED)
clean:
	rm -f $(PRE)
