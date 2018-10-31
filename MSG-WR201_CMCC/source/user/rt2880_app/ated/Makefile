NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) 
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
CFLAGS 	+= -I$(ROOTDIR)/$(LINUXDIR)/include
LDFLAGS += -L$(ROOTDIR)/lib/lib
LDLIBS  += -lconfig

ifeq ($(CONFIG_LIB_LIBNVRAM_FORCE), y)
LDLIBS += -lnvram
endif

EXEC = ated
SRCS = ated.c

#EXTRA_CFLAGS += -DDBG

all: $(EXEC)

$(EXEC): $(SRCS) $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.o

