# Makefile for libiconv

#### Start of system configuration section. ####

# Directories used by "make":
srcdir = .
TOPDIR=../
include $(TOPDIR)Rules.mak

# Directories used by "make install":
prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

# Programs used by "make":
RM = rm -f


# Programs used by "make install":
#INSTALL = /usr/bin/install -c
#INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
mkinstalldirs = $(SHELL) ./build-aux/mkinstalldirs

#### End of system configuration section. ####

SHELL = /bin/sh


LIBICONV=libiconv.la
LIBICONV_SHARED=libiconv.so
LIBICONV_SHARED_FULLNAME=libiconv.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL)


all : lib/localcharset.h force
	cd lib && $(MAKE) all

lib/localcharset.h :
	builddir="`pwd`"; cd libcharset && $(MAKE) all && $(MAKE) install-lib libdir="$$builddir/lib" includedir="$$builddir/lib"

# Installs the library and include files only. Typically called with only
#install-lib : all force
#	cd lib && $(MAKE) install-lib libdir='$(libdir)' includedir='$(includedir)'
#	if [ ! -d $(includedir) ] ; then $(mkinstalldirs) $(includedir) ; fi
#	$(INSTALL_DATA) include/iconv.h.inst $(includedir)/iconv.h

shared: all
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 lib/.libs/$(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED)
	$(LN) -sf $(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 lib/$(LIBICONV) $(TOPDIR)lib


clean : force
	cd libcharset && $(MAKE) clean
	$(RM) lib/charset.alias lib/libcharset.* lib/localcharset.h
	cd lib && $(MAKE) clean


force :

# Work around an automake-1.8 bug.
am--refresh:
