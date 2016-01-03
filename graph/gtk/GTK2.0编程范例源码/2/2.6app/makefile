CC=gcc
all:main.o callbacks.o interface.o
	$(CC) -o app main.o callbacks.o interface.o `pkg-config --libs gtk+-2.0` 
main.o:main.c interface.h callbacks.h
	$(CC) -c main.c `pkg-config --cflags gtk+-2.0`
interface.o:interface.c interface.h callbacks.h
	$(CC) -c interface.c `pkg-config --cflags gtk+-2.0`
callbacks.o:callbacks.c callbacks.h
	$(CC) -c callbacks.c `pkg-config --cflags gtk+-2.0`
clean:
	rm -f *.o

