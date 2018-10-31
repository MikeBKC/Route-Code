OBJS = main.o timers.o sets.o pool.o table.o regexp.o dirtree.o support.o netaddr.o inet.o child.o parser.o log.o lastlog.o xferlog.o bindings.o netacl.o class.o scoreboard.o help.o feat.o netio.o response.o ident.o data.o modules.o display.o auth.o fsio.o mkhome.o ctrls.o event.o var.o trace.o utf8.o proctitle.o pidfile.o env.o 

CFLAGS += -DHAVE_CONFIG_H -DLINUX -I../include -I$(ROOTDIR)/lib/include
LDLIBS += -L../lib -L../lib/libcap

ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
LDLIBS	+= -L$(ROOTDIR)/lib/lib -liconv
endif

all: $(OBJS) 

romfs:

clean:
	rm -f *.o

