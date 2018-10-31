

CC = /data3/wang.bingrong/brwang/x86/backfire/staging_dir/toolchain-i386_gcc-4.1.2_uClibc-0.9.30.1/usr/bin/i486-openwrt-linux-uclibc-gcc

TOPDIR=../
EXEC1 = client
OBJS1 = c.o

EXEC2 = server
OBJS2 = s.o
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS+=-I$(TOPDIR)mib -I$(TOPDIR)cli -I$(TOPDIR)profacc
LDFLAGS = -L./
LDLIBS = -lreos

all:$(EXEC1) $(EXEC2)

$(EXEC1):$(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1) $(LDLIBS)

$(EXEC2):$(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $(OBJS2) $(LDLIBS)
