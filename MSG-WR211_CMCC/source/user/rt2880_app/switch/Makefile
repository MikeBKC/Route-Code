EXEC = switch
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
EXEC_7530 = switch_7530
endif
EXEC_SRCF = switch
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth

ifeq ($(CONFIG_DEFAULTS_RALINK_RT3883),y)
EXEC_SRCF = switch_3883
endif
ifeq ($(CONFIG_RALINK_MT7620),y)
EXEC_SRCF = switch_gsw
endif
ifeq ($(CONFIG_RALINK_MT7621),y)
EXEC_SRCF = switch_gsw_7621
endif
ifeq ($(CONFIG_RALINK_MT7628),y)
EXEC_SRCF = switch_7628
endif

ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
all: $(EXEC) $(EXEC_7530)
else
all: $(EXEC)
endif

$(EXEC): $(EXEC_SRCF).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
switch_7530: $(EXEC_SRCF).c 
	$(CC) $(CFLAGS) -DSWITCH_7530 $(LDFLAGS) -o $@ $^ $(LDLIBS)
endif

romfs:
	$(ROMFSINST) /bin/$(EXEC)
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
	$(ROMFSINST) /bin/$(EXEC_7530)
endif

clean:
	-rm -f $(EXEC) $(EXEC_7530) *.elf *.gdb *.o

