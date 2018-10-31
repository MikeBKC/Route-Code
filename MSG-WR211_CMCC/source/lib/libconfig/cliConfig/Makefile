TOPDIR=../../
include $(TOPDIR)Rules.mak
-include $(CONFIG_CONFIG)
    
CFLAGS =$(SSP_ALL_CFLAGS) -DUSE_HOSTCC -Dlinux -D__linux__ -Dunix -DEMBED -DLINUX

CFLAGS += -I$(ROOTDIR)/uttShareHead/config 
CFLAGS +=-I../mib -I../cli -I../profacc -I../cliConfig

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/include

MAJOR_VERSION=0
MINOR_VERSION=0
SUBLEVEL=1
LIBCLICONF=libcliconf.a
LIBCLICONF_SHARED=libcliconf.so
LIBCLICONF_SHARED_FULLNAME=libcliconf-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

OBJS	:= cliConvert.o cmdConfig.o

HEADERS := 

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@	
	$(STRIPTOOL) -x -R .note -R .comment $*.o

all: shared
#	$(CC) $(CFLAGS) -L./ -L/data3/wang.bingrong/brwang/x86/atomlinux/user/libxml2-2.7.8/.libs -o uttcli uttcli.c -lreos -lxml2
#	$(CC) $(CFLAGS) -L./ -L/data3/wang.bingrong/brwang/x86/atomlinux/user/libxml2-2.7.8/.libs -o xmlw conver.c -lreos -lxml2

#LDFLAGS = -shared -Wl,-soname,$(LIBNVRAM_SHARED).0 
#-L$(ROOTDIR)/lib/mxml-2.6
#-lmxml
shared:$(OBJS)
	$(LD) $(LDFLAGS) -soname=$(LIBCLICONF_SHARED).$(MAJOR_VERSION) \
		-o $(LIBCLICONF_SHARED_FULLNAME) --no-whole-archive $(OBJS) \
		-L$(TOPDIR)lib  -lc -lpthread -lconfig $(LDADD_LIBFLOAT) $(LIBGCC);
#	$(CC) -o $(LIBCLICONF_SHARED_FULLNAME) $(OBJS) -$(LDFLAGS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBCLICONF_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCLICONF_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBCLICONF_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBCLICONF_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCLICONF_SHARED)
	$(LN) -sf $(LIBCLICONF_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCLICONF_SHARED).$(MAJOR_VERSION)

$(OBJS): $(HEADERS) Makefile
romfs:

clean:
	$(RM) *.[oa] $(LIBCLICONF_SHARED)* $(LIBCLICONF_SHARED_FULLNAME)*

