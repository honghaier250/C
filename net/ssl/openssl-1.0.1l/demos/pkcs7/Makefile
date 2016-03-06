all: p7sign p7verify p7encode p7decode

p7sign: sign.c util.c util.h
	@gcc -Wall -g -o p7sign sign.c util.c $(shell pkg-config --cflags --libs openssl)

p7verify: verify.c util.c util.h
	@gcc -Wall -g -o p7verify verify.c util.c $(shell pkg-config --cflags --libs openssl)

p7encode: encode.c util.c util.h
	@gcc -Wall -g -o p7encode encode.c util.c $(shell pkg-config --cflags --libs openssl)

p7decode: decode.c util.c util.h
	@gcc -Wall -g -o p7decode decode.c util.c $(shell pkg-config --cflags --libs openssl)

test:
	@rm -f sign.c.new sign.c.openssl test.p7b
	@echo -n "run: "
	./p7sign
	@echo -n "run: "
	./p7verify
	@echo -n "run :"
	openssl smime -verify -in test.p7b -inform PEM -noverify -out sign.c.openssl

dist: clean
	tar -zcf ../pkcs7.tar.gz *

clean:
	@rm -f p7verify p7sign p7encode p7decode sign.c.new sign.c.openssl test.p7b
