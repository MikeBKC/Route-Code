cmd_libbb/ptr_to_globals.o := /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc -Wp,-MD,libbb/.ptr_to_globals.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.12.1)" -DBB_BT=AUTOCONF_TIMESTAMP -O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uClibc++/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source  -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/cli -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement  -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os -Dlinux    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ptr_to_globals)"  -D"KBUILD_MODNAME=KBUILD_STR(ptr_to_globals)" -c -o libbb/ptr_to_globals.o libbb/ptr_to_globals.c

deps_libbb/ptr_to_globals.o := \
  libbb/ptr_to_globals.c \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/errno.h \
    $(wildcard include/config/.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/features.h \
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
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/uClibc_config.h \
    $(wildcard include/config/mips/isa/1//.h) \
    $(wildcard include/config/mips/isa/2//.h) \
    $(wildcard include/config/mips/isa/3//.h) \
    $(wildcard include/config/mips/isa/4//.h) \
    $(wildcard include/config/mips/isa/mips32//.h) \
    $(wildcard include/config/mips/isa/mips64//.h) \
    $(wildcard include/config//.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/cdefs.h \
    $(wildcard include/config/espaces.h) \
    $(wildcard include/config/tify/level.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/errno_values.h \

libbb/ptr_to_globals.o: $(deps_libbb/ptr_to_globals.o)

$(deps_libbb/ptr_to_globals.o):
