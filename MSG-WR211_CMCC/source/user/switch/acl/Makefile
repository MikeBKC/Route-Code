

EXEC = sw_acl

CUR = $(shell pwd)
ROOT_DIRECTORY = /bin

LINUXDIR = $(CUR)/../../../linux-2.6.21.x
CFLAGS += -Wall -I$(LINUXDIR)/include/
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/mib -I$(LINUXDIR)/../lib/libconfig/profacc
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/cli 
CFLAGS += -L$(LINUXDIR)/../lib/lib
LDLIBS += -lswitch -lconfig
CFLAGS += -DCONFIG_SWITCH_EXT -DCONFIG_IP1725
#CROSS = mipsel-linux-uclibc-

all: $(EXEC)

$(EXEC): $(EXEC).c
#	$(CROSS)gcc  $(CFLAGS) -o $@ $@.c 
	$(CC)  $(CFLAGS) $(LDLIBS) -o $@ $@.c 
	$(STRIP) --remove-section=.note --remove-section=.comment $@
romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/$(EXEC)
clean:
	@rm -vf  *.o *~ $(EXEC)
