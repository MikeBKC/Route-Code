

EXEC = easyNetbios

CUR = $(shell pwd)
ROOT_DIRECTORY = /bin

LINUXDIR = $(CUR)/../../../linux-2.6.21.x
CFLAGS += -Wall -I$(LINUXDIR)/include/
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
CFLAGS += -L$(LINUXDIR)/../lib/lib
LDLIBS += -lswitch
CFLAGS += -DCONFIG_SWITCH_EXT
#CROSS = mipsel-linux-uclibc-

OBJS   = main.o netbios.o host_name.o

all: $(EXEC)

$(EXEC): $(OBJS)
#	$(CROSS)gcc  $(CFLAGS) -o $@ $@.c 
	$(CC)  -o $@ $(OBJS) $(CFLAGS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/$(EXEC)
clean:
	@rm -vf  *.o *~ $(EXEC)
