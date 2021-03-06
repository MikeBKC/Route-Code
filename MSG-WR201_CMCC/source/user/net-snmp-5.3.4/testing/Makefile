#
# Minimum environment and virtual path setup
#
SHELL		= /bin/sh
srcdir		= .
top_srcdir	= ..
VERSION		= 5.3.4


#
# Paths
#
prefix		= /usr/local
exec_prefix	= /usr/local
bindir		= ${exec_prefix}/bin
sbindir		= ${exec_prefix}/sbin
libdir		= ${exec_prefix}/lib
datadir		= ${prefix}/share
includedir	= ${prefix}/include/net-snmp
ucdincludedir	= ${prefix}/include/ucd-snmp
mandir		= ${prefix}/man
man1dir		= $(mandir)/man1
man3dir		= $(mandir)/man3
man5dir		= $(mandir)/man5
man8dir		= $(mandir)/man8
snmplibdir	= $(datadir)/snmp
mibdir		= $(snmplibdir)/mibs
persistentdir	= /var/net-snmp
DESTDIR         = 
INSTALL_PREFIX  = $(DESTDIR)

#
# Programs
#
INSTALL		= $(LIBTOOL) --mode=install /usr/bin/install -c
UNINSTALL	= $(LIBTOOL) --mode=uninstall rm -f
LIBTOOLCLEAN	= $(LIBTOOL) --mode=clean rm -f
INSTALL_DATA    = ${INSTALL} -m 644
SED		= /bin/sed
LN_S		= ln -s
AUTOCONF	= :
AUTOHEADER	= :
PERL            = /usr/bin/perl
FIND            = find

#
# Compiler arguments
#
CFLAGS		= -fno-strict-aliasing -I/data3/hua.yuye/yudehai/3520G/nv510W/source/lib/include -Dlinux 
EXTRACPPFLAGS	= -x c
LDFLAGS		= -L/data3/hua.yuye/yudehai/3520G/nv510W/source/lib/lib 
LIBTOOL		= $(SHELL) $(top_builddir)/libtool 
EXEEXT		= 

# Misc Compiling Stuff
#CC	        = /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc

# use libtool versioning the way they recommend.
# The (slightly clarified) rules:
#
# - If any interfaces/structures have been removed or changed since the
#   last update, increment current, and set age and revision to 0. Stop.
#
# - If any interfaces have been added since the last public release, then
#   increment current and age, and set revision to 0. Stop.
# 
# - If the source code has changed at all since the last update,
#   then increment revision (c:r:a becomes c:r+1:a). 
#
# Note: maintenance releases (eg 5.2.x) should never have changes
#       that would require a current to be incremented.
#
LIBCURRENT  = 10
LIBAGE      = 0
LIBREVISION = 6

LIB_LD_CMD      = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) -rpath $(libdir) -version-info $(LIBCURRENT):$(LIBREVISION):$(LIBAGE) -o
LIB_EXTENSION   = la
LIB_VERSION     =
LIB_LDCONFIG_CMD = $(LIBTOOL) --mode=finish $(libdir)
LINK		= $(LIBTOOL) --mode=link $(CC)
# RANLIB 	= ranlib
RANLIB		= :

# libtool definitions
.SUFFIXES: .c .o .lo .rc
.c.lo:
	$(LIBTOOL) --mode=compile $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
.rc.lo:
	$(LIBTOOL) --mode=compile windres -o $@ -i $<

# include paths
#
SRC_TOP_INCLUDES            = -I$(top_srcdir)/include
SRC_SNMPLIB_INCLUDES        = -I$(top_srcdir)/snmplib
SRC_AGENT_INCLUDES          = -I$(top_srcdir)/agent
SRC_HELPER_INCLUDES         = -I$(top_srcdir)/agent/helpers
SRC_MIBGROUP_INCLUDES       = -I$(top_srcdir)/agent/mibgroup

BLD_TOP_INCLUDES            = -I$(top_builddir)/include $(SRC_TOP_INCLUDES)
BLD_SNMPLIB_INCLUDES        = -I$(top_builddir)/snmplib $(SRC_SNMPLIB_INCLUDES)
BLD_AGENT_INCLUDES          = -I$(top_builddir)/agent $(SRC_AGENT_INCLUDES)
BLD_HELPER_INCLUDES         = -I$(top_builddir)/agent/helpers $(SRC_HELPER_INCLUDES)
BLD_MIBGROUP_INCLUDES       = -I$(top_builddir)/agent/mibgroup $(SRC_MIBGROUP_INCLUDES)

TOP_INCLUDES            = $(SRC_TOP_INCLUDES)
SNMPLIB_INCLUDES        = $(SRC_SNMPLIB_INCLUDES)
AGENT_INCLUDES          = $(SRC_AGENT_INCLUDES)
HELPER_INCLUDES         = $(SRC_HELPER_INCLUDES)
MIBGROUP_INCLUDES       = $(SRC_MIBGROUP_INCLUDES)

#
# Makefile for testing
#

# use GNU vpath, if available, to only set a path for source and headers
# VPATH will pick up objects too, which is bad if you are sharing a
# source dir...
#vpath %.h $(srcdir)
#vpath %.c $(srcdir)
# fallback to regular VPATH for non-gnu...
VPATH = $(srcdir)

#TARG		= etest ktest misctest stest

USELIBS		= ../snmplib/libsnmp.$(LIB_EXTENSION)$(LIB_VERSION)
LIBS		= -L../snmplib $(CC_RUNTIME_ARG) -lsnmp -lm 

PARSEOBJS	=

CPPFLAGS	= $(SNMPLIB_INCLUDES) 
#CC		= /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc $(CPPFLAGS)

all: $(TARG)

test:
	$(srcdir)/RUNTESTS -a -D `pwd`/.. $(TESTOPTS)

test-mibs:
	cd $(srcdir)/rfc1213 ; ./run

etest:    etimetest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ etimetest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

ktest:    keymanagetest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ keymanagetest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

misctest:    misctest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ misctest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

stest:    scapitest.o $(PARSEOBJS) $(USELIBS)
	${CC} -o $@ scapitest.o $(PARSEOBJS) ${LDFLAGS} ${LIBS} 

clean: testclean
	rm -f *.o core *.core $(TARG)

testclean:
	-rm -fr /tmp/snmp-test*

install: installdirs $(TARG)
	@for i in $(TARG) ; \
		do $(INSTALL) $$i $(INSTALL_PREFIX)$(bindir) ; \
		echo "install:  installed $$i in $(INSTALL_PREFIX)$(bindir)" ; \
	done

installdirs:
	@$(SHELL) $(srcdir)/../mkinstalldirs $(INSTALL_PREFIX)$(snmplibdir) $(INSTALL_PREFIX)$(bindir)

dependlocal: 
	makedepend $(CPPFLAGS) -o.lo $(srcdir)/*.c

depend: dependlocal

nosysdepend: 
	makedepend $(CPPFLAGS) -o.lo *.c
	$(PERL) -n -i.bak $(top_srcdir)/makenosysdepend.pl Makefile

makefileindepend: nosysdepend
	$(PERL) $(top_srcdir)/makefileindepend.pl


# DO NOT DELETE THIS LINE -- make depend depends on it.


etimetest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
etimetest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/lcd_time.h
etimetest.lo: ../snmplib/snmp_api.h ../snmplib/snmp_debug.h ../snmplib/tools.h
etimetest.lo: ../s/sysv.h  
keymanagetest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
keymanagetest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/keytools.h
keymanagetest.lo: ../snmplib/scapi.h ../snmplib/snmp_api.h ../snmplib/tools.h
keymanagetest.lo: ../snmplib/transform_oids.h ../s/sysv.h 
misctest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
misctest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/snmp_api.h
misctest.lo: ../snmplib/tools.h ../snmplib/transform_oids.h ../s/sysv.h
scapitest.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h
scapitest.lo: ../snmplib/asn1.h ../snmplib/callback.h ../snmplib/keytools.h
scapitest.lo: ../snmplib/scapi.h ../snmplib/snmp_api.h ../snmplib/tools.h
scapitest.lo: ../snmplib/transform_oids.h ../s/sysv.h 
T.lo: ../config.h ../m/generic.h ../s/generic.h ../s/linux.h ../s/sysv.h
