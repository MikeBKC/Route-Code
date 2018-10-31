

EXEC = urcp 

CUR = $(shell pwd)
ROOT_DIRECTORY = /bin

LINUXDIR = $(CUR)/../../../$(CONFIG_LINUXDIR)
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM), y)
#LINUXDIR = $(CUR)/../../../linux-2.6.30.8
CFLAGS += -I$(LINUXDIR)/../lib/zlib
#else
#LINUXDIR = $(CUR)/../../../linux-2.6.21.x
endif

CFLAGS += -Wall -I$(LINUXDIR)/include/
CFLAGS += -I$(ROOTDIR)/config
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/mib
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/profacc
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/cli
CFLAGS += -L$(LINUXDIR)/../lib/lib
CFLAGS += -DUTT_SWVERSION=\"$(UTTVERSION)\" -DUTT_HWMODEL=\"$(UTTHWMODEL)\"
ifeq ($(CONFIG_SWITCH_EXT),y)
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
CFLAGS += -DCONFIG_SWITCH_EXT
LDLIBS += -lswitch -lconfig -lpthread
else
ifeq ($(CONFIG_UTT_AC), y)
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
LDLIBS += -lswitch -lconfig -lpthread
else
LDLIBS += -lconfig -lpthread
endif
endif

ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
CFLAGS += -I$(LINUXDIR)/../lib/libiconv
LDLIBS	+= -liconv
endif

ifneq ("$(CONFIG_LIB_LIBUSB_USER_FORCE)", "")
CFLAGS += -Wall -I$(ROOTDIR)/lib/libusb-user
LDLIBS += -lusb-user
endif

OBJS  = main.o  urcp.o trap_sta.o
#CROSS = mipsel-linux-uclibc-
ifneq ("$(CONFIG_SWITCH_EXT)$(CONFIG_UTT_AC)", "")
OBJS   += urcp_cmd.o app.o urcp_hello.o app_msg.o urcp_tftp.o urcp_config.o common.o \
	 web_msg.o urcp_ap.o crc32.o urcp_debug.o hash.o urcp_sta_log.o
else
OBJS   += app.o urcp_tftp.o urcp_config.o common.o web_msg.o urcp_ap.o urcp_cmd.o crc32.o \
			ap_msg.o ap_netlink.o urcp_power.o urcp_debug.o dhcp.o hash.o
endif


all: $(EXEC)

$(EXEC): $(OBJS)
#	$(CROSS)gcc  $(CFLAGS) -o $@ $@.c 
	$(CC)  -o $@ $(OBJS) $(CFLAGS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/$(EXEC)
clean:
	@rm -vf  *.o *~ $(EXEC)
