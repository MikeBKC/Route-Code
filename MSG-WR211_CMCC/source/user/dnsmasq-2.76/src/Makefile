NAME = dnsmasq

OBJS = cache.o rfc1035.o util.o option.o forward.o network.o \
       dnsmasq.o dhcp.o lease.o rfc2131.o netlink.o dbus.o bpf.o \
       helper.o tftp.o log.o conntrack.o dhcp6.o rfc3315.o \
       dhcp-common.o outpacket.o radv.o slaac.o auth.o ipset.o \
       domain.o dnssec.o blockdata.o tables.o loop.o inotify.o \
       poll.o rrfilter.o edns0.o arp.o

hdrs = dnsmasq.h config.h dhcp-protocol.h dhcp6-protocol.h \
       dns-protocol.h radv-protocol.h ip6addr.h

CFLAGS += -O2 -D__uClinux__ -DNO_GETTEXT
ifneq ("$(CONFIG_DEFAULTS_FREESCALE_P1010)", "")
CFLAGS += -DP1010
endif

include ../../uttUserConf/web-config

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)
ifeq ("$(CONFIG_UTT_VPN_PROXY)", "y")
	$(ROMFSINST) /sbin/dnsmasqConf.sh
	$(ROMFSINST) /etc/chinaip.list
endif

clean:
	rm -f *~ *.mo *~ *.pot 
	rm -f *.o dnsmasq.a dnsmasq core */core
