all: ssl_session

ssl_session:
	gcc -Wall -g ssl_session.c -o ssl_session -L/usr/local/ssl/lib -lssl -lcrypto

clean:
	rm ssl_session
