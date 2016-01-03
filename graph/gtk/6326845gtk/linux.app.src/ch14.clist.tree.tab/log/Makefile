
CC = gcc -Wall

all: webtraffic 

bitmaps.o: bitmaps.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g bitmaps.c 

filesel.o: filesel.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g filesel.c 

progressbar.o: progressbar.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g progressbar.c 

parselog.o: parselog.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g parselog.c 

misc.o: misc.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g misc.c 

frontend.o: frontend.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g frontend.c 

notebook.o: notebook.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -c -g notebook.c 

#bitmaps: bitmaps.o 
#	$(CC) `pkg-config --libs --cflags gtk+-2.0` -g bitmaps.o -o bitmaps

webtraffic: parselog.o misc.o frontend.o notebook.o progressbar.o filesel.o bitmaps.o 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` -g bitmaps.o parselog.o misc.o progressbar.o filesel.o frontend.o notebook.o -o webtraffic

clean: 
	rm -f *.o webtraffic
