ifndef ROOTDIR
ROOTDIR = ../..
export ROOTDIR
endif

LIB_PATH = $(ROOTDIR)/lib/lib
LIB_PATH += -L$(ROOTDIR)/lib/libupnp/ixml -L$(ROOTDIR)/lib/libupnp/upnp -L$(ROOTDIR)/lib/libupnp/threadutil
INCLUDES=  -I$(ROOTDIR)/lib/libupnp/ixml/inc -I$(ROOTDIR)/lib/libupnp/upnp/inc -I$(ROOTDIR)/lib/libupnp/threadutil/inc 
INCLUDES += -I$(ROOTDIR)/include
LIBS= -L$(LIB_PATH) -lpthread -lupnp -lixml -lthreadutil
#LIBS= -L./ -lpthread -lupnp -lixml -lthreadutil
FILES= main.o gatedevice.o pmlist.o util.o config.o

CFLAGS += -Wall -g -O2
#CFLAGS += -I$(ROOTDIR)/linux-2.6.21.x/drivers/net/raeth 
CFLAGS += -I$(ROOTDIR)/user/goahead/speedgoahead 
CFLAGS += -I$(ROOTDIR)/user/goahead/src 
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli

ifdef HAVE_LIBIPTC
ifdef LIBIPTC_PREFIX
LIBS += -L$(LIBIPTC_PREFIX)/lib
INCLUDES += -I$(LIBIPTC_PREFIX)/include
endif

LIBS += -liptc
INCLUDES += -DHAVE_LIBIPTC
FILES += iptc.o
endif

uttUpnpFile += uttUpnpC.o
all: upnpd uttUpnpDemo

upnpd: $(FILES)
	$(CC) $(CFLAGS) $(FILES) $(LIBS) -o $@
	$(STRIP) -s $@
	@echo "make $@ finished on `date`"

uttUpnpDemo: $(uttUpnpFile)
	$(CC)  $(uttUpnpFile) -o $@
	$(STRIP) -s $@

%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

romfs:
	$(ROMFSINST) /bin/upnpd
	$(ROMFSINST) /bin/uttUpnpDemo
	$(ROMFSINST) /bin/upnp_xml.sh
	$(ROMFSINST) -d etc/upnpd.conf /etc/linuxigd/upnpd.conf
	$(ROMFSINST) -d etc/ligd.gif /etc/linuxigd/ligd.gif

clean:
	rm -f *.o upnpd uttUpnpDemo
