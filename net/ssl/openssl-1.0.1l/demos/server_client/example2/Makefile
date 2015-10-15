all: sslserver sslclient


sslserver: sslserver.c
	gcc -g -o sslserver sslserver.c -L/usr/local/lib64 -lcrypto -lssl -lpthread -ldl

sslclient: sslclient.c
	gcc -g -o sslclient sslclient.c -L/usr/local/lib64 -lcrypto -lssl -lpthread -ldl

clean:
	rm sslserver sslclient 
