cmd_libbb/perror_nomsg_and_die.o := /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc -Wp,-MD,libbb/.perror_nomsg_and_die.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.12.1)" -DBB_BT=AUTOCONF_TIMESTAMP -O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uClibc++/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source  -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/libconfig/profacc -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/libconfig/mib -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/libconfig/cli -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement  -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os -Dlinux    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(perror_nomsg_and_die)"  -D"KBUILD_MODNAME=KBUILD_STR(perror_nomsg_and_die)" -c -o libbb/perror_nomsg_and_die.o libbb/perror_nomsg_and_die.c

deps_libbb/perror_nomsg_and_die.o := \
  libbb/perror_nomsg_and_die.c \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config/nommu.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/byteswap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/byteswap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/endian.h \
    $(wildcard include/config/.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/features.h \
    $(wildcard include/config/c99.h) \
    $(wildcard include/config/ix.h) \
    $(wildcard include/config/ix2.h) \
    $(wildcard include/config/ix199309.h) \
    $(wildcard include/config/ix199506.h) \
    $(wildcard include/config/en.h) \
    $(wildcard include/config/en/extended.h) \
    $(wildcard include/config/x98.h) \
    $(wildcard include/config/en2k.h) \
    $(wildcard include/config/gefile.h) \
    $(wildcard include/config/gefile64.h) \
    $(wildcard include/config/e/offset64.h) \
    $(wildcard include/config/d.h) \
    $(wildcard include/config/c.h) \
    $(wildcard include/config/ntrant.h) \
    $(wildcard include/config/i.h) \
    $(wildcard include/config/ern/inlines.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/uClibc_config.h \
    $(wildcard include/config/mips/isa/1//.h) \
    $(wildcard include/config/mips/isa/2//.h) \
    $(wildcard include/config/mips/isa/3//.h) \
    $(wildcard include/config/mips/isa/4//.h) \
    $(wildcard include/config/mips/isa/mips32//.h) \
    $(wildcard include/config/mips/isa/mips64//.h) \
    $(wildcard include/config//.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/cdefs.h \
    $(wildcard include/config/espaces.h) \
    $(wildcard include/config/tify/level.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/endian.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/arpa/inet.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/netinet/in.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/stdint.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/wchar.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/wordsize.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/types.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stddef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/kernel_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/pthreadtypes.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/sched.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/limits.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/limits.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/posix1_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/local_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/linux/limits.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/posix2_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/xopen_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/stdio_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/time.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/select.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/select.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/sigset.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/time.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/sysmacros.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/sockaddr.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/asm/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/asm/sockios.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/asm/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/asm-generic/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/in.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stdbool.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/mount.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/ioctls.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/asm/ioctls.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/bits/ioctl-types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/lib/include/sys/ttydefaults.h \

libbb/perror_nomsg_and_die.o: $(deps_libbb/perror_nomsg_and_die.o)

$(deps_libbb/perror_nomsg_and_die.o):
