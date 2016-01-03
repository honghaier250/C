
CC = gcc -Wall

all: tree 

tree: tree.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` tree.c -o tree

clean: 
	rm -f *.o tree
