all: server client

server: server.c
	gcc -g -o server server.c -L/usr/local/ssl/lib/ -lssl -lcrypt -lcurses

client: client.c
	gcc -g -o client client.c -L/usr/local/ssl/lib/ -lssl -lcrypt -lcurses

clean:
	rm server client
