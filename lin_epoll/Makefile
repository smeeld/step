CC=c++
DST=epoll_serv
CFL=-std=c++0x -lpthread
HED=-I`pwd`
PRE=hand.o as.o serv.o hd.o
SRC=as.cpp hd.cpp serv.cpp hand.cpp
all: $(PRE)
	$(CC) -o $(DST) $(PRE) $(CFL)
$(PRE): $(SRC)
	$(CC) -c $(SRC) $(CFL) $(HED)
clean:
	rm -f $(PRE)
