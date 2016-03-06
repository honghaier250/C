all: builtin dynamic

builtin:
	gcc -Wall -g builtin.c -o builtin -L/usr/local/ssl/lib -lcrypto

dynamic:
	gcc -Wall -g dynamic.c -o dynamic -L/usr/local/ssl/lib -lcrypto

clean:
	rm -rf builtin dynamic
