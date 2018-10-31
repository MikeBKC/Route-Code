EXEC = mii_mgr
OBJS = mii_mgr.o
ifneq ($(CONFIG_IP1725),)
EXEC1 = 1725_mgr
OBJS1 = 1725_mgr.o
endif
ifneq ($(CONFIG_AR8337N),)
EXEC2 = 8337_mgr
OBJS2 = 8337_mgr.o
endif
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth

all: $(EXEC) $(EXEC1) $(EXEC2)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

ifneq ($(CONFIG_IP1725),)
$(EXEC1): $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1)
endif
ifneq ($(CONFIG_AR8337N),)
$(EXEC2): $(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $(OBJS2)
endif
romfs:
	$(ROMFSINST) /bin/$(EXEC)
ifneq ($(CONFIG_IP1725),)
	$(ROMFSINST) /bin/$(EXEC1)
endif
ifneq ($(CONFIG_AR8337N),)
	$(ROMFSINST) /bin/$(EXEC2)
endif

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
ifneq ($(CONFIG_IP1725),)
	-rm -f $(EXEC1) *.elf *.gdb *.o
endif
ifneq ($(CONFIG_AR8337N),)
	-rm -f $(EXEC2) *.elf *.gdb *.o
endif
