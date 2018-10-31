
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010), y)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net
endif

LDFLAGS += -lconfig 

EXEC = nvram_daemon
EXEC1 = mii_6530G
EXEC2 = reset
#ralink_init

ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
all: $(EXEC) xml_nvram $(EXEC1) $(EXEC2) 
else
all: $(EXEC) xml_nvram 
endif

ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7621),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7628),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3052),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3883),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT2880),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT5350),y)
nvram_daemon: nvram_daemon-rt3050.o
endif
ifeq ($(CONFIG_DEFAULTS_ATHEROS_AR7100),y)
nvram_daemon: nvram_daemon-ar7100.o
endif

ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
nvram_daemon: nvram_daemon-atom-seavoN330.o
endif

ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010), y)
nvram_daemon: nvram_daemon-p1010.o
endif
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
reset: reset.o
endif
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
mii_6530G:mii_6530G.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
endif
#xml_nvram  start
CFLAGS += -I$(ROOTDIR)/lib/mxml-2.6
xml_nvram: xml_nvram.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
#xml_nvmra end

clean:
	rm -f *.o $(EXEC) xml_nvram $(EXEC1) $(EXEC2) 

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
	for j in $(EXEC1) ; do $(ROMFSINST) /bin/$$j ; done
	for i in $(EXEC2) ; do $(ROMFSINST) /bin/$$i ; done
endif
#	mv -f nvram_get $(ROMFSDIR)/bin
#	mv -f nvram_set $(ROMFSDIR)/bin
#	ln -sf ralink_init nvram_get
#	ln -sf ralink_init nvram_set

