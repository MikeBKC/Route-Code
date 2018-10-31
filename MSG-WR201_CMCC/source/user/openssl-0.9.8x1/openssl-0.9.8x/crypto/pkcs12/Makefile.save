#
# OpenSSL/crypto/pkcs12/Makefile
#

DIR=	pkcs12
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
LIBSRC= p12_add.c p12_asn.c p12_attr.c p12_crpt.c p12_crt.c p12_decr.c \
	p12_init.c p12_key.c p12_kiss.c p12_mutl.c\
	p12_utl.c p12_npas.c pk12err.c p12_p8d.c p12_p8e.c
LIBOBJ= p12_add.o p12_asn.o p12_attr.o p12_crpt.o p12_crt.o p12_decr.o \
	p12_init.o p12_key.o p12_kiss.o p12_mutl.o\
	p12_utl.o p12_npas.o pk12err.o p12_p8d.o p12_p8e.o

SRC= $(LIBSRC)

EXHEADER=  pkcs12.h
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

test:

all:	lib

lib:	$(LIBOBJ)
	$(ARX) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB) || echo Never mind.
	@touch lib

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:
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
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

p12_add.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_add.o: ../cryptlib.h p12_add.c
p12_asn.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_asn.o: ../cryptlib.h p12_asn.c
p12_attr.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_attr.o: ../cryptlib.h p12_attr.c
p12_crpt.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_crpt.o: ../cryptlib.h p12_crpt.c
p12_crt.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_crt.o: ../cryptlib.h p12_crt.c
p12_decr.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_decr.o: ../cryptlib.h p12_decr.c
p12_init.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_init.o: ../cryptlib.h p12_init.c
p12_key.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_key.o: ../cryptlib.h p12_key.c
p12_kiss.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_kiss.o: ../cryptlib.h p12_kiss.c
p12_mutl.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_mutl.o: ../cryptlib.h p12_mutl.c
p12_npas.o: ../../include/openssl/fips.h p12_npas.c
p12_p8d.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_p8d.o: ../cryptlib.h p12_p8d.c
p12_p8e.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_p8e.o: ../cryptlib.h p12_p8e.c
p12_utl.o: ../../include/openssl/e_os.h ../../include/openssl/fips.h
p12_utl.o: ../cryptlib.h p12_utl.c
pk12err.o: ../../include/openssl/fips.h pk12err.c
