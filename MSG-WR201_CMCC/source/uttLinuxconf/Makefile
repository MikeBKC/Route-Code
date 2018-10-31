EXEC_LINUX_CONFIG = uttLinuxconf 

OBJS_LINUX_CONFIG = uttLinuxconf.o

ROOT_DIR = $(shell pwd)/..
include $(ROOT_DIR)/.config
CFLAGS += -I$(ROOT_DIR)/uttShareHead/config
CFLAGS += -I$(ROOT_DIR)/$(CONFIG_LINUXDIR)/include -I$(UTT_ROOTDIR)/lib/libconfig/cliConfig
CFLAGS += -I$(ROOT_DIR)/config

CC=gcc
AR=ar
RANLIB=ranlib
STRIP=strip
EXE_SETS = $(EXEC_LINUX_CONFIG)

all:$(EXE_SETS) clean

$(EXEC_LINUX_CONFIG):$(OBJS_LINUX_CONFIG)
	$(CC) -o $@ $(OBJS_LINUX_CONFIG)
	@$(STRIP) --remove-section=.note --remove-section=.comment $@ 
	@./uttLinuxconf
	@cat linux-config >> $(ROOT_DIR)/$(CONFIG_LINUXDIR)/.config

clean:
	@rm -f $(EXEC_LINUX_CONFIG) $(EXEC_LINUX_CONFIG) *.o linux-config
