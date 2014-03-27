CC=c++
DST=epoll_serv
CFL=-std=c++0x -lpthread -I/home/g
PRE=as.o serv.o hd.o
SRC=as.cpp hd.cpp serv.cpp
all: $(PRE)
	$(CC) -o $(DST) $(PRE) $(CFL)
$(PRE): $(SRC)
	$(CC) -c $(SRC) $(CFL)
clean:
	rm -f $(PRE)
