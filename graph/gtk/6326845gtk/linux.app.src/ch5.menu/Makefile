
CC = gcc

all: frontend

frontend: frontend.c combobox.c misc.c
	$(CC) `pkg-config --libs --cflags gtk+-2.0` frontend.c combobox.c misc.c -o frontend 

clean: 
	rm -f *.o frontend
