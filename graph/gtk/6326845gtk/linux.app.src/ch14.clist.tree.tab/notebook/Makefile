
CC = gcc -Wall

notebook: notebook.c notebook.c
	$(CC) `pkg-config --libs --cflags gtk+-2.0` notebook.c -o notebook

clean: 
	rm -f *.o notebook
