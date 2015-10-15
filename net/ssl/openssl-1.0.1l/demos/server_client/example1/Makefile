all: server client

server: server.c
	gcc -g -o server server.c -lssl -lcrypt -lcurses

client: client.c
	gcc -g -o client client.c -lssl -lcrypt -lcurses

clean:
	rm server client
