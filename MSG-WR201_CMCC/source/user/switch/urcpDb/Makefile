EXEC = urcpDb 

CUR = $(shell pwd)
ROOT_DIRECTORY = /bin
LINUXDIR = $(CUR)/../../../$(CONFIG_LINUXDIR)
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM), y)
CFLAGS += -I$(LINUXDIR)/../lib/zlib
endif

CFLAGS += -Wall -I$(LINUXDIR)/include/
CFLAGS += -I$(ROOTDIR)/include
CFLAGS += -I$(ROOTDIR)/config
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/mib
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/profacc
CFLAGS += -I$(LINUXDIR)/../lib/libconfig/cli
CFLAGS += -L$(LINUXDIR)/../lib/lib

ifeq ($(CONFIG_SWITCH_EXT),y)
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
CFLAGS += -DCONFIG_SWITCH_EXT
LDLIBS += -lswitch -lconfig -lpthread
else
ifeq ($(CONFIG_UTT_AC), y)
CFLAGS += -I$(LINUXDIR)/../lib/libswitch
LDLIBS += -lswitch -lconfig -lpthread
else
LDLIBS += -lconfig -lpthread
endif
endif

OBJS   = main.o

all: version $(EXEC)

$(EXEC): $(OBJS)
	$(CC)  -o $@ $(OBJS) $(CFLAGS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@
romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)/$(EXEC)
clean:
	@rm -vf  *.o *~ $(EXEC)

version:
	echo "#ifndef _H_URCPDB_VERSION_H"  > version.h
	echo "#define _H_URCPDB_VERSION_H" >> version.h
	echo "#define TIME \"$(shell date)\"" >> version.h
	echo "#define USER \"$(USER)\"" >> version.h
	echo "#define PATH \"$(shell pwd)\"" >> version.h
	echo "#define TAG  \"$(shell (cvs status Makefile | sed  -n '/Sticky Tag:/p' | sed -e 's/.*Tag:\t*\(.*\)/\1/')) \"" >> version.h
	echo "#endif" >>version.h
#
#	Dependencies
#
main.o: main.c ../urcp/debug_module.c  version.h
