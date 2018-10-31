EXEC = gpio
EXEC_SRC = gpio
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char

ifeq ($(CONFIG_RALINK_MT7620),y)
EXEC_SRC = gpio_7620
endif
ifeq ($(CONFIG_RALINK_MT7621),y)
EXEC_SRC = gpio_7620
endif
ifeq ($(CONFIG_RALINK_MT7628),y)
EXEC_SRC = gpio_4300
endif


all: $(EXEC)

$(EXEC): $(EXEC_SRC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

