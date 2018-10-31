# Makefile for ntpclient/adjtimex
#
# Makefile by Uwe Beutin, modified by John Williams
# 
# ntpclient by Larry Dolittle
#
# /home/uclinux/uClinux-dist/user/ntpclient/Makefile

#PB44DIR=/data3/wang.bingrong/brwang/wlan/orig_pb44
#CROSS_COMPILE    = mipsel-linux-uclibc-
#AS       = $(CROSS_COMPILE)as
#LD       = $(CROSS_COMPILE)ld
#CC       = $(CROSS_COMPILE)gcc
#CPP      = $(CC) -E
#AR       = $(CROSS_COMPILE)ar
#NM       = $(CROSS_COMPILE)nm
#STRIP        = $(CROSS_COMPILE)strip
#OBJCOPY      = $(CROSS_COMPILE)objcopy
#OBJDUMP      = $(CROSS_COMPILE)objdump
CP = cp
#CFLAGS =-O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -D__USE_BSD
CFLAGS =-O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -D__USE_BSD
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS  += -I$(ROOTDIR)/lib/libconfig/profacc
CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/include

#LDFLAGS =-Wl,-rpath,-L/opt/buildroot-gdb/lib 
#LDFLAGS = -L/opt/buildroot-gdb/lib 
	
EXEC1 = ntpclient
#EXEC2 = adjtimex

OBJS1 = ntpclient.o phaselock.o
#OBJS2 = adjtimex.o

#CFLAGS += -D__USE_BSD
all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJS1)
#	$(CC) $(LDFLAGS) -o $@ $(OBJS1) -lc -lgcc 
	$(CC) -o $@ $(OBJS1) -lc -lgcc 
	$(STRIP) --remove-section=.note --remove-section=.comment $(EXEC1)

#$(EXEC2): $(OBJS2)
#	$(CC) $(LDFLAGS) -o $@ $(OBJS2) $(LDLIBS$(LDLIBS_$@))
romfs:
	$(ROMFSINST) -e CONFIG_USER_NTPCLIENT_NTPCLIENT /bin/$(EXEC1)
#	cp $(EXEC1) $(EXEC2) $(PB44DIR)/rootfs.build/usr/bin
clean:
	rm -f $(EXEC1) $(EXEC2) *.gdb *.elf *.o

test: $(EXEC)
	./$(EXEC) -d -r < test.dat

