#added by brwang

EXEC = netkeeper

OBJS = netkeeper.o jiangxi.o hunan.o henan.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
LDFLAGS +=

all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	rm -f $(EXEC) *.o
