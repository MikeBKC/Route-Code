# Makefile for libiconv

srcdir = .
TOPDIR=../
include $(TOPDIR)Rules.mak

RM = rm -f
CP = cp

SHELL = /bin/sh

LIBICONV=libiconv.la
LIBICONV_SHARED=libiconv.so
LIBICONV_SHARED_FULLNAME=libiconv.so.$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL)


all : lib/localcharset.h force
	cd lib && $(MAKE) all

lib/localcharset.h :
	$(CP) $(srcdir)/include/localcharset.h.in $(srcdir)/lib/localcharset.h

shared: all
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 lib/.libs/$(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED)
	$(LN) -sf $(LIBICONV_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBICONV_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 lib/$(LIBICONV) $(TOPDIR)lib


clean : force
	$(RM) lib/charset.alias lib/libcharset.* lib/localcharset.h
	cd lib && $(MAKE) clean


force :

# Work around an automake-1.8 bug.
am--refresh:
