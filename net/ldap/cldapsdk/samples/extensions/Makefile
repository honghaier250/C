
#
# Make file to compile and build sample extension programs
#

# Libraries needed to link on AIX, Linux, or Solaris systems
SOLARISLIBS = -lpthread -lrt -lresolv -lsocket 
LINUXLIBS = -lpthread -lrt -ldl 
AIXLIBS = -brtl -ldl 
HPUXLIBS =  -lpthread

#
# #### Libraries needed to link on your system ###
#
SYSLIBS=$(LINUXLIBS)

# SDK support for LDAP routines are provided by libldapsdk library
LDAPLIB = ldapsdk

# SSL support is provided by a companion library libldapssl.  
# Applications using SSL need to link with this library.  
# It provides functions for initializing SSL and creating LDAP 
# session handles that are SSL enabled. 
SLDAPLIB = ldapssl

# SDK support for LDAP extensions are provided by a companion
# library,  libldapx.  Applications using extensions
# need to link with this library.
xLDAPLIB = ldapx

# compiler to be used
OCC=cc
HPCC=aCC -Ae -Wl,+s
CC = $(OCC)

# This directory contains the header files needed to build
# your client application.
INCDIR = ../../include

# This directory contains the library files provided with the kit.
# The specific library files that you link depend on the type
# of application(s) you are building.
LIBDIR = ../../lib

LDAPLIBS = -L$(LIBDIR) -l$(LDAPLIB) $(SYSLIBS)
SSLLIBS = -L$(LIBDIR) -l$(SLDAPLIB)
XLIBS = -L$(LIBDIR) -l$(xLDAPLIB)
OPTFLAGS = -g
CFLAGS = $(OPTFLAGS) -I$(INCDIR)

SRCS =  abortpo.c addrepl.c bergetpriv.c chgrepl.c filterevents.c \
		getbinddn.c getcount.c  getpriv.c  getprivlist.c getReplFilter.c getrinfo.c \
		index.c ldaptrigger.c  listrepl.c  mergepart.c monitorevents.c \
		nmasbind.c  parsync.c recvupd.c  remrepl.c schsync.c sendupd.c \
		setReplFilter.c splitpart.c lburp.c backup.c ldapaudit.c 

OBJS =  abortpo.o addrepl.o bergetpriv.o chgrepl.o filterevents.o \
		getbinddn.o getcount.o getpriv.o  getprivlist.o getReplFilter.o getrinfo.o \
		index.o ldaptrigger.o listrepl.o  mergepart.o monitorevents.o \
		nmasbind.o parsync.o recvupd.o  remrepl.o schsync.o sendupd.o \
		setReplFilter.o splitpart.o lburp.o backup.o ldapaudit.o

PROGRAMS = abortpo addrepl bergetpriv chgrepl filterevents \
		getbinddn getcount getpriv  getprivlist getReplFilter getrinfo \
		index  ldaptrigger listrepl  mergepart monitorevents\
		nmasbind parsync recvupd remrepl schsync sendupd \
		setReplFilter splitpart lburp backup ldapaudit

all: $(PROGRAMS)

abortpo:        abortpo.o
	$(CC) -o $@ abortpo.o $(LDAPLIBS) $(XLIBS)

addrepl:        addrepl.o
	$(CC) -o $@ addrepl.o $(LDAPLIBS) $(XLIBS)

backup:       backup.o
	$(CC) -o $@ backup.o $(LDAPLIBS) $(XLIBS)

bergetpriv: 	bergetpriv.o
	$(CC) -o $@ bergetpriv.o $(LDAPLIBS) $(XLIBS)

chgrepl:        chgrepl.o
	$(CC) -o $@ chgrepl.o $(LDAPLIBS) $(XLIBS)

filterevents:	filterevents.o
	$(CC) -o $@ filterevents.o $(LDAPLIBS) $(XLIBS)

getbinddn:      getbinddn.o
	$(CC) -o $@ getbinddn.o $(LDAPLIBS) $(XLIBS)

getcount:       getcount.o
	$(CC) -o $@ getcount.o $(LDAPLIBS) $(XLIBS)

getpriv:        getpriv.o
	$(CC) -o $@ getpriv.o $(LDAPLIBS) $(XLIBS)

getprivlist:        getprivlist.o
	$(CC) -o $@ getprivlist.o $(LDAPLIBS) $(XLIBS)


getReplFilter:	getReplFilter.o
	 $(CC) -o $@ getReplFilter.o $(LDAPLIBS) $(XLIBS)

getrinfo:       getrinfo.o
	$(CC) -o $@ getrinfo.o $(LDAPLIBS) $(XLIBS)

index:       index.o
	$(CC) -o $@ index.o $(LDAPLIBS) $(XLIBS)

lburp:       lburp.o
	$(CC) -o $@ lburp.o $(LDAPLIBS) $(XLIBS)

ldaptrigger:    ldaptrigger.o
	$(CC) -o $@ ldaptrigger.o $(LDAPLIBS) $(XLIBS)

listrepl:       listrepl.o
	$(CC) -o $@ listrepl.o $(LDAPLIBS) $(XLIBS)

mergepart:      mergepart.o
	$(CC) -o $@ mergepart.o $(LDAPLIBS) $(XLIBS)

monitorevents:      monitorevents.o
	$(CC) -o $@ monitorevents.o $(LDAPLIBS) $(XLIBS)

nmasbind:        nmasbind.o
	$(CC) -o $@ nmasbind.o $(LDAPLIBS) $(XLIBS)

parsync:        parsync.o
	$(CC) -o $@ parsync.o $(LDAPLIBS) $(XLIBS)

recvupd:        recvupd.o
	$(CC) -o $@ recvupd.o $(LDAPLIBS) $(XLIBS)

remrepl:        remrepl.o
	$(CC) -o $@ remrepl.o $(LDAPLIBS) $(XLIBS)

schsync:        schsync.o
	$(CC) -o $@ schsync.o $(LDAPLIBS) $(XLIBS)

sendupd:        sendupd.o
	$(CC) -o $@ sendupd.o $(LDAPLIBS) $(XLIBS)

setReplFilter:	setReplFilter.o
	$(CC) -o $@ setReplFilter.o $(LDAPLIBS) $(XLIBS)

splitpart:      splitpart.o
	$(CC) -o $@ splitpart.o $(LDAPLIBS) $(XLIBS)

ldapaudit:      ldapaudit.o
	$(CC) -o $@ ldapaudit.o $(LDAPLIBS) $(XLIBS)

install: 
	/bin/cp $(PROGRAMS) 
clean:
	/bin/rm -f $(PROGRAMS) $(OBJS) a.out core
