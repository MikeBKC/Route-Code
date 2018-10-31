#added by jqSun
EXEC_WEB_CONFIG = uttWebconf

OBJS_WEB_CONFIG = uttWebconf.o

CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include -I$(ROOTDIR)/lib/libconfig/cliConfig
CFLAGS += -I$(ROOTDIR)/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc -I$(ROOTDIR)/lib/libconfig/mib -I$(ROOTDIR)/lib/libconfig/cli

CC=gcc
AR=ar
RANLIB=ranlib
STRIP=strip
EXE_SETS = $(EXEC_WEB_CONFIG)

all:$(EXE_SETS)

ifeq ("$(CONFIG_USER_GOAHEAD_HTTPD)", "y")
$(EXEC_WEB_CONFIG):$(OBJS_WEB_CONFIG)
	$(CC) -o $@ $(OBJS_WEB_CONFIG)
	$(STRIP) --remove-section=.note --remove-section=.comment $@ 
	../uttUserConf/uttWebconf
endif
romfs:

clean:
	rm -f $(EXEC_WEB_CONFIG) $(EXEC_WEB_CONFIG) *.o 
