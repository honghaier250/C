# You can do "make SUB=blah" to make only a few, or edit here, or both
# You can also run make directly in the subdirs you want.

SUB =   lib tftp tftpd

%.build: MCONFIG acconfig.h version.h
	$(MAKE) -C $(patsubst %.build, %, $@)

%.install: MCONFIG acconfig.h version.h
	$(MAKE) -C $(patsubst %.install, %, $@) install

%.clean:
	$(MAKE) -C $(patsubst %.clean, %, $@) clean

%.distclean:
	$(MAKE) -C $(patsubst %.distclean, %, $@) distclean

all:      MCONFIG $(patsubst %, %.build, $(SUB))

tftp.build: lib.build
tftpd.build: lib.build

install:  MCONFIG $(patsubst %, %.install, $(SUB))

clean:	localclean $(patsubst %, %.clean, $(SUB))

localclean:
	rm -f version.h

distclean: localdistclean $(patsubst %, %.distclean, $(SUB))

localdistclean: localclean
	rm -f MCONFIG config.status config.log acconfig.h *~ \#*
	rm -rf *.cache
	find . -type f \( -name \*.orig -o -name \*.rej \) | xargs rm -f

spotless: distclean
	rm -f configure acconfig.h.in

autoconf: configure acconfig.h.in

config:	MCONFIG acconfig.h

release:
	$(MAKE) autoconf
	$(MAKE) distclean

MCONFIG: configure MCONFIG.in acconfig.h.in
	./configure

acconfig.h: MCONFIG
	: Generated by side effect

# Adding "configure" to the dependencies serializes this with running
# autoconf, because there are apparently race conditions between
# autoconf and autoheader.
acconfig.h.in: configure.in configure aclocal.m4
	rm -f acconfig.h.in acconfig.h
	autoheader

configure: configure.in aclocal.m4
	rm -f MCONFIG configure config.log acconfig.h config.cache
	autoconf

version.h: version
	echo \#define VERSION \"tftp-hpa `cat version`\" > version.h
