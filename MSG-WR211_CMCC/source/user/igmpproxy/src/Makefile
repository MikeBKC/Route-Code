EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o udpsock.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

CFLAGS= -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth

ifneq ("$(CONFIG_PRODUCT)", "RT2880")
IGMP_PROXY_OBJ+= rt3052.o rt3052_reg.o
CFLAGS+= -DRT3052_SUPPORT
endif

all: $(EXEC)

igmpproxy: build.h $(IGMP_PROXY_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(IGMP_PROXY_OBJ)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/igmpproxy.sh

clean:
	rm -f *.o *.asm build.h $(EXEC)  

build.h	:  
	echo '#define BUILD "' `date +%y%m%d` '"' >build.h
