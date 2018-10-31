#added by brwang

EXEC = iplink

OBJS = ddns_iplink.o hmacmd5.o base64.o rc4.o md5.o

CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
CFLAGS += -I$(ROOTDIR)/include -I$(ROOTDIR)/$(LINUXDIR)/include 
LDFLAGS += -L$(ROOTDIR)/lib/lib
LDLIBS  += -lconfig


all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	rm -f $(EXEC) *.o
