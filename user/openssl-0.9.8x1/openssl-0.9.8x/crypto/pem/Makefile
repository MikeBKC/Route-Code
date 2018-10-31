#
# OpenSSL/crypto/pem/Makefile
#

DIR=	pem
TOP=	../..
CC=	cc
INCLUDES= -I.. -I$(TOP) -I../../include
CFLAG=-g
MAKEFILE=	Makefile
AR=		ar r

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile
TEST=
APPS=

LIB=$(TOP)/libcrypto.a
LIBSRC= pem_sign.c pem_seal.c pem_info.c pem_lib.c pem_all.c pem_err.c \
	pem_x509.c pem_xaux.c pem_oth.c pem_pk8.c pem_pkey.c

LIBOBJ=	pem_sign.o pem_seal.o pem_info.o pem_lib.o pem_all.o pem_err.o \
	pem_x509.o pem_xaux.o pem_oth.o pem_pk8.o pem_pkey.o

SRC= $(LIBSRC)

EXHEADER= pem.h pem2.h
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

all:	lib

lib:	$(LIBOBJ)
	$(ARX) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB) || echo Never mind.
	@touch lib

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links: $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@[ -n "$(INSTALLTOP)" ] # should be set by top Makefile...
	@headerlist="$(EXHEADER)"; for i in $$headerlist ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	@[ -n "$(MAKEDEPEND)" ] # should be set by upper Makefile...
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

pem_all.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_all.o: ../cryptlib.h pem_all.c
pem_err.o: ../../include/openssl/fips.h pem_err.c
pem_info.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_info.o: ../cryptlib.h pem_info.c
pem_lib.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_lib.o: ../cryptlib.h pem_lib.c
pem_oth.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_oth.o: ../cryptlib.h pem_oth.c
pem_pk8.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_pk8.o: ../cryptlib.h pem_pk8.c
pem_pkey.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_pkey.o: ../cryptlib.h pem_pkey.c
pem_seal.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_seal.o: ../cryptlib.h pem_seal.c
pem_sign.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_sign.o: ../cryptlib.h pem_sign.c
pem_x509.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_x509.o: ../cryptlib.h pem_x509.c
pem_xaux.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
pem_xaux.o: ../cryptlib.h pem_xaux.c
