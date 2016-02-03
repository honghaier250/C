#
# Make file to compile and build sample programs for Unix platforms.
#

#uname = $(shell uname -p)
uname = `uname -p`

# Libraries needed to link on AIX, Linux, or Solaris systems
SOLARISLIBS = -lpthread -lrt -lresolv -lsocket 
LINUXLIBS = -lpthread -lresolv -ldl 
AIXLIBS = -brtl -ldl -liconv -lpthread
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

# GSS (Generic Security Services ) support is porvided by a 
# companion library ldapgss
GSSAPILIB = ldapgss

# SDK support for LDAP extensions are provided by a companion
# library,  libldapx.  Applications using extensions
# need to link with this library.
xLDAPLIB = ldapx 

# compiler to be used 
OCC=cc
HPCC=aCC -Ae -Wl,+s
CC = $(OCC)

# This directory contains the header files needed to built
# your client application. 
INCDIR = ../include

# This directory contains the library files provided with the kit.
# The specific library files that you link depend on the type
# of application(s) you are building.
#ifeq($(uname),x86_64)
LIBDIR = ../lib64
#else
#LIBDIR = ../lib
#endif

LDAPLIBS = -L$(LIBDIR) -l$(LDAPLIB) $(SYSLIBS)
SSLLIBS = -L$(LIBDIR)  -l$(SLDAPLIB)
XLIBS = -L$(LIBDIR) -l$(xLDAPLIB)

# This make file assumes system GSS API library is libgss.
GSSLIBS = -L$(LIBDIR) -l$(GSSAPILIB) -lgss

OPTFLAGS = -g
CFLAGS = $(OPTFLAGS) -I$(INCDIR) -DUNIX 


SRCS =  CheckBind.c addUserToGroup.c addentry.c addentry1.c bind.c \
        cmpattrs.c delentry.c dyngroup.c extmatch.c getdse.c gracelog.c list.c \
        listgroup.c mkcontainer.c modattrs.c modifyACL.c modifyTimeStamp.c \
        modpass.c mutual.c nameandoid.c operationalAttrs.c rebind.c renamerdn.c \
        schema.c search.c searchAsynch.c searchBinary.c searchPersist.c \
        searchmsg.c setpass.c sortcntl.c sslbind.c sslbindi.c \
        starttls.c verpass.c vlvcntl.c saslExternal.c md5bind.c \
        multiSort.c searchURL.c connpool.c utf8towc.c simplepass.c utf8bind.c \
		debug.c threadSafe.c gssbind.c 


OBJS =  CheckBind.o addUserToGroup.o addentry.o addentry1.o bind.o \
        cmpattrs.o delentry.o dyngroup.o extmatch.o getdse.o gracelog.o list.o \
        listgroup.o mkcontainer.o modattrs.o modifyACL.o modifyTimeStamp.o \
        modpass.o mutual.o nameandoid.o operationalAttrs.o rebind.o renamerdn.o \
        schema.o search.o searchAsynch.o searchBinary.o searchPersist.o \
        searchmsg.o setpass.o sortcntl.o sslbind.o sslbindi.o \
        starttls.o verpass.o vlvcntl.o saslExternal.o md5bind.o \
        multiSort.o searchURL.o connpool.o utf8towc.o simplepass.o utf8bind.o \
		debug.o threadSafe.c gssbind.o 


PROGRAMS =  CheckBind addUserToGroup addentry addentry1 bind \
        cmpattrs debug delentry dyngroup extmatch getdse gracelog list \
        listgroup mkcontainer modattrs modifyACL modifyTimeStamp \
        modpass mutual nameandoid operationalAttrs rebind renamerdn \
        schema search searchAsynch searchBinary searchPersist \
        searchmsg setpass sortcntl sslbind sslbindi \
        starttls verpass vlvcntl saslExternal md5bind \
        multiSort searchURL connpool utf8towc simplepass utf8bind \
		threadSafe gssbind 


all: $(PROGRAMS)


CheckBind:       CheckBind.o
	$(CC) -o $@ CheckBind.o $(LDAPLIBS)

addUserToGroup:       addUserToGroup.o
	$(CC) -o $@ addUserToGroup.o $(LDAPLIBS)

addentry:       addentry.o
	$(CC) -o $@ addentry.o $(LDAPLIBS)

addentry1:       addentry1.o
	$(CC) -o $@ addentry1.o $(LDAPLIBS)

bind:       bind.o
	$(CC) -o $@ bind.o $(LDAPLIBS)

cmpattrs:       cmpattrs.o
	$(CC) -o $@ cmpattrs.o $(LDAPLIBS)

connpool:       connpool.o connpool.h
	$(CC) -o $@ connpool.o $(LDAPLIBS)

debug:       debug.o
	$(CC) -o $@ debug.o $(LDAPLIBS)

delentry:       delentry.o
	$(CC) -o $@ delentry.o $(LDAPLIBS)

dyngroup:       dyngroup.o
	$(CC) -o $@ dyngroup.o $(LDAPLIBS)

extmatch:       extmatch.o
	$(CC) -o $@ extmatch.o $(LDAPLIBS)

getdse:       getdse.o
	$(CC) -o $@ getdse.o $(LDAPLIBS)

gracelog:       gracelog.o
	$(CC) -o $@ gracelog.o $(LDAPLIBS)

list:       list.o
	$(CC) -o $@ list.o $(LDAPLIBS)

listgroup:       listgroup.o
	$(CC) -o $@ listgroup.o $(LDAPLIBS)

md5bind:       md5bind.o
	$(CC) -o $@ md5bind.o $(LDAPLIBS)

mkcontainer:       mkcontainer.o
	$(CC) -o $@ mkcontainer.o $(LDAPLIBS)

modattrs:       modattrs.o
	$(CC) -o $@ modattrs.o $(LDAPLIBS)

modifyACL:       modifyACL.o
	$(CC) -o $@ modifyACL.o $(LDAPLIBS)

modifyTimeStamp:       modifyTimeStamp.o
	$(CC) -o $@ modifyTimeStamp.o $(LDAPLIBS)

modpass:       modpass.o
	$(CC) -o $@ modpass.o $(LDAPLIBS)

multiSort:	multiSort.o
	$(CC) -o $@ multiSort.o $(LDAPLIBS)	

mutual:       mutual.o
	$(CC) -o $@ mutual.o $(LDAPLIBS) $(SSLLIBS)

nameandoid:       nameandoid.o
	$(CC) -o $@ nameandoid.o $(LDAPLIBS)

operationalAttrs:       operationalAttrs.o
	$(CC) -o $@ operationalAttrs.o $(LDAPLIBS)

rebind:       rebind.o
	$(CC) -o $@ rebind.o $(LDAPLIBS)

renamerdn:       renamerdn.o
	$(CC) -o $@ renamerdn.o $(LDAPLIBS)

saslExternal:       saslExternal.o
	$(CC) -o $@ saslExternal.o $(LDAPLIBS) $(SSLLIBS)

schema:       schema.o
	$(CC) -o $@ schema.o $(LDAPLIBS)

search:       search.o
	$(CC) -o $@ search.o $(LDAPLIBS)

searchAsynch:       searchAsynch.o
	$(CC) -o $@ searchAsynch.o $(LDAPLIBS)

searchBinary:       searchBinary.o
	$(CC) -o $@ searchBinary.o $(LDAPLIBS)

searchPersist:       searchPersist.o
	$(CC) -o $@ searchPersist.o $(LDAPLIBS)

searchURL:	searchURL.o
	$(CC) -o $@ searchURL.o $(LDAPLIBS) $(SSLLIBS)

searchmsg:       searchmsg.o
	$(CC) -o $@ searchmsg.o $(LDAPLIBS)

setpass:       setpass.o
	$(CC) -o $@ setpass.o $(LDAPLIBS)

simplepass:	simplepass.o
	$(CC) -o $@ simplepass.o $(LDAPLIBS)

sortcntl:       sortcntl.o
	$(CC) -o $@ sortcntl.o $(LDAPLIBS)

sslbind:       sslbind.o
	$(CC) -o $@ sslbind.o $(LDAPLIBS) $(SSLLIBS)

sslbindi:       sslbindi.o
	$(CC) -o $@ sslbindi.o $(LDAPLIBS) $(SSLLIBS)

starttls:       starttls.o
	$(CC) -o $@ starttls.o $(LDAPLIBS) $(SSLLIBS)

threadSafe:       threadSafe.o
	$(CC) -o $@ threadSafe.o $(LDAPLIBS) $(SSLLIBS) 

utf8towc:       utf8towc.o
	$(CC) -o $@ utf8towc.o $(LDAPLIBS) $(SSLLIBS)

verpass:       verpass.o
	$(CC) -o $@ verpass.o $(LDAPLIBS)

vlvcntl:       vlvcntl.o
	$(CC) -o $@ vlvcntl.o $(LDAPLIBS)

utf8bind:	utf8bind.o
	$(CC) -o $@ utf8bind.o $(LDAPLIBS) 

gssbind:	gssbind.o
	$(CC) -o $@ gssbind.o $(LDAPLIBS) $(GSSLIBS)

clean:
	/bin/rm -f $(PROGRAMS) $(OBJS)  a.out core
