cmd_kernel/sys_ni.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,kernel/.sys_ni.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262 -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(sys_ni)"  -D"KBUILD_MODNAME=KBUILD_STR(sys_ni)"  -c -o kernel/sys_ni.o kernel/sys_ni.c

deps_kernel/sys_ni.o := \
  kernel/sys_ni.c \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/linkage.h \
  include/linux/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/errno.h \
  include/asm-generic/errno-base.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/unistd.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/mips32/o32.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sgidefs.h \

kernel/sys_ni.o: $(deps_kernel/sys_ni.o)

$(deps_kernel/sys_ni.o):
