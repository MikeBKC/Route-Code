EXEC = reg
OBJS = reg.o
EXEC_MMAP = mmap
OBJS_MMAP = mmap.o

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/rt_rdm -I$(ROOTDIR)/$(LINUXDIR)/include

all: $(EXEC) $(EXEC_MMAP)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(EXEC_MMAP): $(OBJS_MMAP)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_MMAP)
romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(EXEC_MMAP)

clean:
	-rm -f $(EXEC) $(EXEC_MMAP)*.elf *.gdb *.o

