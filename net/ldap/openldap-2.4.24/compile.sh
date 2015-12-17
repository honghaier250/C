#!/bin/sh

./configure CFLAGS="-g3 -O0" --prefix=/usr/local/openldap
make depend && make
