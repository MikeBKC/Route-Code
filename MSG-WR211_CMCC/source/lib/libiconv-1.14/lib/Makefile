# Makefile for libiconv/lib

#### Start of system configuration section. ####

# Directories used by "make":
srcdir = .

prefix = /usr/local
local_prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

#CC = /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc
CFLAGS = -g -O2 
CPPFLAGS = 
LDFLAGS =  $(LDFLAGS_no)
LDFLAGS_yes = -Wl,--export-all-symbols
LDFLAGS_no =
INCLUDES = -I. -I$(srcdir) -I../include -I$(srcdir)/../include -I.. -I$(srcdir)/..
# -DBUILDING_LIBICONV: Change expansion of LIBICONV_DLL_EXPORTED macro.
# -DBUILDING_DLL: Change expansion of RELOCATABLE_DLL_EXPORTED macro.
DEFS = -DLIBDIR=\"$(libdir)\" -DBUILDING_LIBICONV -DBUILDING_DLL \
-DENABLE_RELOCATABLE=1 -DIN_LIBRARY -DINSTALLDIR=\"$(libdir)\" -DNO_XMALLOC \
-Dset_relocation_prefix=libiconv_set_relocation_prefix \
-Drelocate=libiconv_relocate -DHAVE_CONFIG_H
LIBTOOL = /bin/sh $(top_builddir)/libtool
LIBTOOL_COMPILE = $(LIBTOOL) --mode=compile
LIBTOOL_LINK = $(LIBTOOL) --mode=link
LIBTOOL_INSTALL = $(LIBTOOL) --mode=install
LIBTOOL_UNINSTALL = $(LIBTOOL) --mode=uninstall
# Windows resource compiler (windres). Used via libtool.
RC = 
CP = cp
MV = mv
LN = ln
LN_S = ln -s
RM = rm -f

SHELL = /bin/sh

LIBICONV_VERSION_INFO = `expr $(MAJOR_VERSION) + $(MINOR_VERSION)`:$(SUBLEVEL):$(MINOR_VERSION)


top_builddir = ..

SOURCES = $(srcdir)/iconv.c $(srcdir)/localcharset.c $(srcdir)$/iconv-user.c (srcdir)/relocatable.c

OBJECTS = iconv.lo localcharset.lo relocatable.lo iconv-user.o $(OBJECTS_EXP_no) $(OBJECTS_RES_no)
#OBJECTS_EXP_yes = iconv-exports.lo
OBJECTS_EXP_no =
#OBJECTS_RES_yes = libiconv.res.lo
OBJECTS_RES_no =

all : libiconv.la

libiconv.la : $(OBJECTS)
	$(LIBTOOL_LINK) --cc=$(CC) $(CC) $(LDFLAGS) $(CFLAGS) -o libiconv.la -rpath $(libdir) -version-info $(LIBICONV_VERSION_INFO) -no-undefined $(OBJECTS)

iconv.lo : $(srcdir)/iconv.c $(srcdir)/converters.h \
           $(srcdir)/encodings.def $(srcdir)/encodings_aix.def $(srcdir)/encodings_osf1.def $(srcdir)/encodings_dos.def $(srcdir)/encodings_local.def \
           $(srcdir)/aliases.h $(srcdir)/aliases_sysaix.h $(srcdir)/aliases_syshpux.h $(srcdir)/aliases_sysosf1.h $(srcdir)/aliases_syssolaris.h \
           $(srcdir)/aliases_aix.h $(srcdir)/aliases_aix_sysaix.h \
           $(srcdir)/aliases_osf1.h $(srcdir)/aliases_osf1_sysosf1.h \
           $(srcdir)/aliases_dos.h \
           $(srcdir)/aliases_extra.h \
           $(srcdir)/flags.h $(srcdir)/translit.h
	$(LIBTOOL_COMPILE) --cc=$(CC) $(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(DEFS) -c $(srcdir)/iconv.c

localcharset.lo : $(srcdir)/localcharset.c $(srcdir)/relocatable.h
	$(LIBTOOL_COMPILE) --cc=$(CC) $(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(DEFS) -c $(srcdir)/localcharset.c

relocatable.lo : $(srcdir)/relocatable.c $(srcdir)/relocatable.h
	$(LIBTOOL_COMPILE) --cc=$(CC) $(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(DEFS) -c $(srcdir)/relocatable.c

iconv-user.o : $(srcdir)/iconv-user.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean : force
	$(RM) *.o *.lo *.a *.la core *.stackdump so_locations
	$(RM) -r .libs libs _libs

force :
