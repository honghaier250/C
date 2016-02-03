#!/bin/sh

groupadd mysql
useradd -r -g mysql mysql
mkdir -p /usr/local/mysql
mkdir -p /data/mysqldb
