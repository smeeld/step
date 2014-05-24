CC=c++
FL= -lX11 -lm -lGL -ljpeg -lXext -std=c++0x -I/usr/include  -DGL_GLEXT_PROTOTYPES -g
HD=-I`pwd`
DESTDIR=
SRC=main.cpp window.cpp models.cpp sphere.cpp scene.cpp image.cpp
DST=main.o window.o models.o sphere.o scene.o image.o
PRG=ball
PROG=/usr/share/prog
MODELS=/usr/share/prog/models
IMAGE=/usr/share/prog/image
SHADER=/usr/share/prog/shaders
all: $(DST)
	$(CC) -o $(PRG) $(DST) $(FL) $(HD)
$(DST): $(SRC)
	$(CC) -c $(SRC) $(FL) $(HD)
clean:
	rm -f $(DST)
install:
	if [ ! -d  $(DESTDIR)/$(PROG) ]; then mkdir -p $(DESTDIR)/$(PROG);fi
	if [ ! -d  $(DESTDIR)/$(MODELS) ]; then mkdir -p $(DESTDIR)/$(MODELS);fi
	if [ ! -d  $(DESTDIR)/$(IMAGE) ]; then mkdir -p $(DESTDIR)/$(IMAGE);fi
	if [ ! -d  $(DESTDIR)/$(SHADER) ]; then mkdir -p $(DESTDIR)/$(SHADER); fi
	if [ ! -d  $(DESTDIR)/usr/bin ]; then mkdir -p $(DESTDIR)/usr/bin; fi
	cp ./image/*.jpg $(DESTDIR)/$(IMAGE)
	cp ./models/* $(DESTDIR)/$(MODELS)
	cp ./shaders/* $(DESTDIR)/$(SHADER)
	cp ./ball $(DESTDIR)/usr/bin
 

