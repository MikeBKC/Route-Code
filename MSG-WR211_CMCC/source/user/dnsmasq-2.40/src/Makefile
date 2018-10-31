NAME = dnsmasq
OBJS = cache.o rfc1035.o util.o option.o forward.o isc.o network.o \
       dnsmasq.o dhcp.o lease.o rfc2131.o netlink.o dbus.o bpf.o \
       helper.o tftp.o log.o

CFLAGS = -O2 -D__uClinux__ -DNO_GETTEXT
ifneq ("$(CONFIG_DEFAULTS_FREESCALE_P1010)", "")
CFLAGS += -DP1010
endif

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)
	$(ROMFSINST) /sbin/dnsmasqConf.sh
	$(ROMFSINST) /etc/chinaip.list

clean:
	rm -f *~ *.mo *~ *.pot 
	rm -f *.o dnsmasq.a dnsmasq core */core
