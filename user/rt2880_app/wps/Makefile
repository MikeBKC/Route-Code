EXEC = wps_daemon
OBJS = wps_daemon.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli

LDFLAGS += -lconfig 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

