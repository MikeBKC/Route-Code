

EXEC = iplink

CFLAGS += -I$(UTT_ROOTDIR)/uttShareHead/config 
CFLAGS += -I./include/mib -I./include
OBJS += a2n.o argcmd.o cliaaa.o clibbb.o cmd.o cmdlist.o instmgr.o shell.o term.o
all:$(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
