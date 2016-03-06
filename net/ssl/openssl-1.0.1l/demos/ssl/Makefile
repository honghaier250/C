all: serv cli

serv:
	gcc -Wall -g serv.c -o serv -L/usr/local/ssl/lib -lcrypto -lssl

cli:
	gcc -Wall -g cli.c -o cli -L/usr/local/ssl/lib -lcrypto -lssl

clean:
	rm -rf serv cli
