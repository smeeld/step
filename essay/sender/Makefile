CC=c++
DIR=$(shell pwd)
DIRP=$(shell dirname $(DIR))
FLAGS=-std=c++0x -I$(DIR) -I$(DIRP)
PRG=send
SRC=main.cpp
PRGO=main.o
all: $(PRGO)
	$(CC) -o $(PRG) $(PRGO) $(FLAGS)
	mv $(PRG) ../$(PRG)
$(PRGO): $(SRC)
	$(CC) -c $(SRC) $(FLAGS)
clean:
	rm -f $(PRGO) 
 
