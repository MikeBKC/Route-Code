
#CC = /data3/wang.bingrong/brwang/x86/backfire/staging_dir/toolchain-i386_gcc-4.1.2_uClibc-0.9.30.1/usr/bin/i486-openwrt-linux-uclibc-gcc
#STRIP = strip
TOPDIR=../../
include $(TOPDIR)Rules.mak
-include $(CONFIG_CONFIG)
    
CFLAGS =$(SSP_ALL_CFLAGS) -DUSE_HOSTCC -Dlinux -D__linux__ -Dunix -DEMBED -DLINUX
# 使用vendors/config中的config.arch文件中ENDIAN变量来判断用户空间大小端
ifeq ($(ENDIAN), little)
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010), y)
CFLAGS += -DLIBCONFIG_BIG_ENDIAN
else
CFLAGS += -DLIBCONFIG_LITTLE_ENDIAN
endif
else
CFLAGS += -DLIBCONFIG_BIG_ENDIAN
endif
# end
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS +=-I../mib -I../cli -I../profacc
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3052),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT3883),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT2880),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
#CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_RT5350),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7621),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7628),y)
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/uClibc++/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
endif
CFLAGS +=-I$(ROOTDIR)/lib/mxml-2.6
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/include
CFLAGS += -I$(ROOTDIR)
#CFLAGS += -I$(ROOTDIR)/config
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010), y)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/
endif
#CFLAGS += -I$(ROOTDIR)/lib/zlib
MAJOR_VERSION=0
MINOR_VERSION=0
SUBLEVEL=1
LIBCONFIG=libconfig.a
LIBCONFIG_SHARED=libconfig.so
LIBCONFIG_SHARED_FULLNAME=libconfig-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

OBJS	:= daughter.o nvram.o profacce.o profmisc.o flash_api.o conver.o internet.o cliaaa.o a2n.o argcmd.o uttSem.o\
	   base64.o comExeFun.o uttNfNetlink.o uttRtChipFun.o ikemd5.o ikesha1.o licenseFunction.o licenseAuth.o license_decipher.o
	   


ifeq ($(CONFIG_USER_IPTABLES_IPSET),y)
OBJS += ipsetApi.o 
endif

ifeq ($(CONFIG_USER_DES),y)
OBJS += des.o
endif
OBJS += hmac_md5.o

HEADERS := 

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@	
	$(STRIPTOOL) -x -R .note -R .comment $*.o

LIBMXML :=$(ROOTDIR)/lib/mxml-2.6/libmxml.a
LIBZIP  :=$(ROOTDIR)/lib/zlib/libz.a
all: shared
#	$(CC) $(CFLAGS) -L./ -L/data3/wang.bingrong/brwang/x86/atomlinux/user/libxml2-2.7.8/.libs -o uttcli uttcli.c -lreos -lxml2
#	$(CC) $(CFLAGS) -L./ -L/data3/wang.bingrong/brwang/x86/atomlinux/user/libxml2-2.7.8/.libs -o xmlw conver.c -lreos -lxml2

#LDFLAGS = -shared -Wl,-soname,$(LIBNVRAM_SHARED).0 
#-L$(ROOTDIR)/lib/mxml-2.6
#-lmxml
shared:$(OBJS)
	$(LD) $(LDFLAGS) -soname=$(LIBCONFIG_SHARED).$(MAJOR_VERSION) \
		-o $(LIBCONFIG_SHARED_FULLNAME) --whole-archive $(LIBMXML) \
		--no-whole-archive --whole-archive $(LIBZIP) \
		--no-whole-archive $(OBJS) \
		-L$(TOPDIR)lib  -lc -lpthread $(LDADD_LIBFLOAT) $(LIBGCC);
#	$(CC) -o $(LIBCONFIG_SHARED_FULLNAME) $(OBJS) -$(LDFLAGS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBCONFIG_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCONFIG_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBCONFIG_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBCONFIG_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCONFIG_SHARED)
	$(LN) -sf $(LIBCONFIG_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCONFIG_SHARED).$(MAJOR_VERSION)

$(OBJS): $(HEADERS) Makefile
romfs:

clean:
	$(RM) *.[oa] $(LIBCONFIG_SHARED)* $(LIBCONFIG_SHARED_FULLNAME)*

