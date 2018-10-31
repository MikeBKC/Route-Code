#!/usr/bin/make

######################################################################
# YOU SHOULD NOT NEED TO TOUCH ANYTHING BELOW THIS LINE
######################################################################
IPSET_VERSION:=4.5

ifndef ROOTDIR
ROOTDIR=../../
endif
KERNEL_DIR=$(ROOTDIR)/$(LINUXDIR)

ROOT_DIRECTORY=/bin
LIBDIR:=$(ROOTDIR)/romfs/lib
IPSET_LIB_DIR:=/lib/ipset

PREFIX:=/usr/local
MANDIR:=$(PREFIX)/man

COPT_FLAGS:=-O2
WARN_FLAGS:=-Wall
#EXTRA_WARN_FLAGS:=\
	-Wextra \
	-Waggregate-return \
	-Wbad-function-cast \
	-Wcast-align \
	-Wformat=2 \
	-Wfloat-equal \
	-Winit-self \
	-Winline \
	-Wmissing-declarations \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wold-style-definition \
	-Wpacked \
	-Wredundant-decls \
	-Wshadow \
	-Wsign-compare \
	-Wstrict-prototypes \
	-Wswitch-default \
	-Wswitch-enum \
	-Wundef \
	-Wwrite-strings \
	-Wno-missing-field-initializers \
	-Werror

ifndef NO_EXTRA_WARN_FLAGS
    WARN_FLAGS+=$(EXTRA_WARN_FLAGS)
endif

ABI_FLAGS:=
CFLAGS:=$(ABI_FLAGS) $(COPT_FLAGS) $(WARN_FLAGS) -I$(KERNEL_DIR)/include -I. # -g -DIPSET_DEBUG
SH_CFLAGS:=$(CFLAGS) -fPIC
LDFLAGS:=$(ABI_FLAGS)
#SETTYPES:=ipmap portmap macipmap
#SETTYPES+=iptree iptreemap
#SETTYPES+=iphash nethash ipporthash ipportiphash ipportnethash
#SETTYPES+=setlist

ifneq ("$(CONFIG_IP_NF_SET_IPMAP)", "")
    SETTYPES += ipmap
endif
ifneq ("$(CONFIG_IP_NF_SET_PORTMAP)", "")
    SETTYPES += portmap
endif
ifneq ("$(CONFIG_IP_NF_SET_MACIPMAP)", "")
    SETTYPES += macipmap
endif
ifneq ("$(CONFIG_IP_NF_SET_IPHASH)", "")
    SETTYPES += iphash
endif
ifneq ("$(CONFIG_IP_NF_SET_NETHASH)", "")
    SETTYPES += nethash
endif
ifneq ("$(CONFIG_IP_NF_SET_IPPORTHASH)", "")
    SETTYPES += ipporthash
endif
ifneq ("$(CONFIG_IP_NF_SET_IPPORTIPHASH)", "")
    SETTYPES += ipportiphash
endif
ifneq ("$(CONFIG_IP_NF_SET_IPPORTNETHASH)", "")
SETTYPES += ipportnethash
endif
ifneq ("$(CONFIG_IP_NF_SET_IPTREE)", "")
SETTYPES += iptree
endif
ifneq ("$(CONFIG_IP_NF_SET_IPTREEMAP)", "")
SETTYPES += iptreemap
endif
ifneq ("$(CONFIG_IP_NF_SET_SETLIST)", "")
SETTYPES += setlist
endif

PROGRAMS=ipset
SHARED_LIBS=$(foreach T, $(SETTYPES),libipset_$(T).so)
INSTALL+=$(foreach T, $(SETTYPES), $(DESTDIR)$(LIBDIR)/ipset/libipset_$(T).so)

all:binaries $(INSTALL)
 

.PHONY: tests

tests:
	cd tests; ./runtest.sh

binaries: $(PROGRAMS) $(SHARED_LIBS)

romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/$(PROGRAMS)
clean: $(EXTRA_CLEANS)
	rm -rf $(PROGRAMS) $(SHARED_LIBS) *.o *~ tests/*~ *.so
	rm -rf $(DESTDIR)$(LIBDIR)/ipset							

#The ipset(8) self
ipset.o: ipset.c ipset.h
	$(CC) $(CFLAGS) -DIPSET_VERSION=\"$(IPSET_VERSION)\" -DIPSET_LIB_DIR=\"$(IPSET_LIB_DIR)\" -c -o $@ $<

ipset: ipset.o
	$(CC) $(CFLAGS) $(LDFLAGS) -rdynamic -o $@ $^ -ldl

#Pooltypes
ipset_%.o: ipset_%.c ipset.h
	$(CC) $(SH_CFLAGS) -o $@ -c $<

libipset_%.so: ipset_%.o
	$(CC) -shared $(LDFLAGS) -o $@ $<

$(DESTDIR)$(LIBDIR)/ipset/libipset_%.so: libipset_%.so
	@[ -d $(DESTDIR)$(LIBDIR)/ipset ] || mkdir -p $(DESTDIR)$(LIBDIR)/ipset
	cp $< $@

$(DESTDIR)$(MANDIR)/man8/ipset.8: ipset.8
	@[ -d $(DESTDIR)$(MANDIR)/man8 ] || mkdir -p $(DESTDIR)$(MANDIR)/man8
	cp $< $@
