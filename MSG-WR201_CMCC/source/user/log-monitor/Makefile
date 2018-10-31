CLIENT = uttlog
OBJS_CLI = client.o
OBJS = md5.o aes.o
LDFLAGS += -L$(ROOTDIR)/lib/lib -lconfig
CFLAGS  += -I$(ROOTDIR)/uttShareHead/config -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include

.PHONY: all clean romfs srv

all: $(CLIENT) srv

$(CLIENT): $(OBJS_CLI) $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS_CLI) $(OBJS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

srv:
	make -C server

romfs:
	$(ROMFSINST) /bin/$(CLIENT)

clean:
	rm -rf $(CLIENT) $(OBJS_CLI) $(OBJS)
	make -C server clean
