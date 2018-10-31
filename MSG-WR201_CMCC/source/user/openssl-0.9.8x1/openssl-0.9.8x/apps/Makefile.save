#
#  apps/Makefile
#

DIR=		apps
TOP=		..
CC=		cc
INCLUDES=	-I$(TOP) -I../include $(KRB5_INCLUDES)
CFLAG=		-g -static
MAKEFILE=	Makefile
PERL=		perl
RM=		rm -f
# KRB5 stuff
KRB5_INCLUDES=
LIBKRB5=

PEX_LIBS=
EX_LIBS= 
EXE_EXT= 

SHLIB_TARGET=

CFLAGS= -DMONOLITH $(INCLUDES) $(CFLAG)

GENERAL=Makefile makeapps.com install.com

DLIBCRYPTO=../libcrypto.a
DLIBSSL=../libssl.a
LIBCRYPTO=-L.. -lcrypto
LIBSSL=-L.. -lssl

PROGRAM= openssl

SCRIPTS=CA.sh CA.pl

EXE= $(PROGRAM)$(EXE_EXT)

E_EXE=	verify asn1pars req dgst dh dhparam enc passwd gendh errstr \
	ca crl rsa rsautl dsa dsaparam ec ecparam \
	x509 genrsa gendsa s_server s_client speed \
	s_time version pkcs7 cms crl2pkcs7 sess_id ciphers nseq pkcs12 \
	pkcs8 spkac smime rand engine ocsp prime

PROGS= $(PROGRAM).c

A_OBJ=apps.o
A_SRC=apps.c
S_OBJ=	s_cb.o s_socket.o
S_SRC=	s_cb.c s_socket.c
RAND_OBJ=app_rand.o
RAND_SRC=app_rand.c

E_OBJ=	verify.o asn1pars.o req.o dgst.o dh.o dhparam.o enc.o passwd.o gendh.o errstr.o \
	ca.o pkcs7.o crl2p7.o crl.o \
	rsa.o rsautl.o dsa.o dsaparam.o ec.o ecparam.o \
	x509.o genrsa.o gendsa.o s_server.o s_client.o speed.o \
	s_time.o $(A_OBJ) $(S_OBJ) $(RAND_OBJ) version.o sess_id.o \
	ciphers.o nseq.o pkcs12.o pkcs8.o spkac.o smime.o rand.o engine.o \
	ocsp.o prime.o cms.o

E_SRC=	verify.c asn1pars.c req.c dgst.c dh.c enc.c passwd.c gendh.c errstr.c ca.c \
	pkcs7.c crl2p7.c crl.c \
	rsa.c rsautl.c dsa.c dsaparam.c ec.c ecparam.c \
	x509.c genrsa.c gendsa.c s_server.c s_client.c speed.c \
	s_time.c $(A_SRC) $(S_SRC) $(RAND_SRC) version.c sess_id.c \
	ciphers.c nseq.c pkcs12.c pkcs8.c spkac.c smime.c rand.c engine.c \
	ocsp.c prime.c cms.c

SRC=$(E_SRC)

EXHEADER=
HEADER=	apps.h progs.h s_apps.h \
	testdsa.h testrsa.h \
	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	@(cd ..; $(MAKE) DIRS=$(DIR) all)

all:	exe

exe:	$(EXE)

req: sreq.o $(A_OBJ) $(DLIBCRYPTO)
	shlib_target=; if [ -n "$(SHARED_LIBS)" ]; then \
		shlib_target="$(SHLIB_TARGET)"; \
	fi; \
	$(MAKE) -f $(TOP)/Makefile.shared -e \
		APPNAME=req OBJECTS="sreq.o $(A_OBJ) $(RAND_OBJ)" \
		LIBDEPS="$(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)" \
		link_app.$${shlib_target}

sreq.o: req.c 
	$(CC) -c $(INCLUDES) $(CFLAG) -o sreq.o req.c

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

install:
	@[ -n "$(INSTALLTOP)" ] # should be set by top Makefile...
	@set -e; for i in $(EXE); \
	do  \
	(echo installing $$i; \
	 cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i.new; \
	 chmod 755 $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i.new; \
	 mv -f $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i.new $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i ); \
	 done;
	@set -e; for i in $(SCRIPTS); \
	do  \
	(echo installing $$i; \
	 cp $$i $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i.new; \
	 chmod 755 $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i.new; \
	 mv -f $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i.new $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i ); \
	 done
	@cp openssl.cnf $(INSTALL_PREFIX)$(OPENSSLDIR)/openssl.cnf.new; \
	chmod 644 $(INSTALL_PREFIX)$(OPENSSLDIR)/openssl.cnf.new; \
	mv -f  $(INSTALL_PREFIX)$(OPENSSLDIR)/openssl.cnf.new $(INSTALL_PREFIX)$(OPENSSLDIR)/openssl.cnf

tags:
	ctags $(SRC)

tests:

links:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	@if [ -z "$(THIS)" ]; then \
	    $(MAKE) -f $(TOP)/Makefile reflect THIS=$@; \
	else \
	    $(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(SRC); \
	fi

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff $(EXE)
	rm -f req

$(DLIBSSL):
	(cd ..; $(MAKE) DIRS=ssl all)

$(DLIBCRYPTO):
	(cd ..; $(MAKE) DIRS=crypto all)

$(EXE): progs.h $(E_OBJ) $(PROGRAM).o $(DLIBCRYPTO) $(DLIBSSL)
	$(RM) $(EXE)
	shlib_target=; if [ -n "$(SHARED_LIBS)" ]; then \
		shlib_target="$(SHLIB_TARGET)"; \
	elif [ -n "$(FIPSCANLIB)" ]; then \
	  FIPSLD_CC="$(CC)"; CC=$(TOP)/fips/fipsld; export CC FIPSLD_CC; \
	fi; \
	LIBRARIES="$(LIBSSL) $(LIBKRB5) $(LIBCRYPTO)" ; \
	[ "x$(FIPSCANLIB)" = "xlibfips" ] && LIBRARIES="$$LIBRARIES -lfips"; \
	$(MAKE) -f $(TOP)/Makefile.shared -e \
		CC="$${CC}" APPNAME=$(EXE) OBJECTS="$(PROGRAM).o $(E_OBJ)" \
		LIBDEPS="$(PEX_LIBS) $$LIBRARIES $(EX_LIBS)" \
		link_app.$${shlib_target}
	@if [ -z "$(CROSS_COMPILE)" ]; then \
		(cd ..; \
	  	OPENSSL="`pwd`/util/opensslwrap.sh"; export OPENSSL; \
	  	$(PERL) tools/c_rehash certs) \
	fi

progs.h: progs.pl
	$(PERL) progs.pl $(E_EXE) >progs.h
	$(RM) $(PROGRAM).o

# DO NOT DELETE THIS LINE -- make depend depends on it.

app_rand.o: ../include/openssl/e_os.h ../include/openssl/fips.h app_rand.c
app_rand.o: apps.h
apps.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.c apps.h
asn1pars.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h
asn1pars.o: asn1pars.c
ca.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h ca.c
ciphers.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h ciphers.c
cms.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h cms.c
crl.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h crl.c
crl2p7.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h crl2p7.c
dgst.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h dgst.c
dh.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h dh.c
dsa.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h dsa.c
dsaparam.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h
dsaparam.o: dsaparam.c
ec.o: ec.c
ecparam.o: ecparam.c
enc.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h enc.c
engine.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h engine.c
errstr.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h errstr.c
gendh.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h gendh.c
gendsa.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h gendsa.c
genrsa.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h genrsa.c
nseq.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h nseq.c
ocsp.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h ocsp.c
openssl.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h openssl.c
openssl.o: progs.h s_apps.h
passwd.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h passwd.c
pkcs12.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h pkcs12.c
pkcs7.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h pkcs7.c
pkcs8.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h pkcs8.c
prime.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h prime.c
rand.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h rand.c
req.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h req.c
rsa.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h rsa.c
rsautl.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h rsautl.c
s_cb.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h s_apps.h
s_cb.o: s_cb.c
s_client.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h s_apps.h
s_client.o: s_client.c timeouts.h
s_server.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h s_apps.h
s_server.o: s_server.c timeouts.h
s_socket.o: ../e_os.h ../e_os2.h ../include/openssl/e_os.h
s_socket.o: ../include/openssl/fips.h apps.h s_apps.h s_socket.c
s_time.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h s_apps.h
s_time.o: s_time.c
sess_id.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h sess_id.c
smime.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h smime.c
speed.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h speed.c
speed.o: testdsa.h testrsa.h
spkac.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h spkac.c
verify.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h verify.c
version.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h version.c
x509.o: ../include/openssl/e_os.h ../include/openssl/fips.h apps.h x509.c
