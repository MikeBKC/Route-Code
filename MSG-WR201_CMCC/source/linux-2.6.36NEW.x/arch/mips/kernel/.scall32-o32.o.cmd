cmd_arch/mips/kernel/scall32-o32.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/kernel/.scall32-o32.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -D__ASSEMBLY__ -ffunction-sections  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic         -c -o arch/mips/kernel/scall32-o32.o arch/mips/kernel/scall32-o32.S

deps_arch/mips/kernel/scall32-o32.o := \
  arch/mips/kernel/scall32-o32.S \
    $(wildcard include/config/mips/mt/fpaff.h) \
  include/linux/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/errno.h \
  include/asm-generic/errno-base.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sgidefs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asmmacro.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/cpu/mipsr2.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/rm9000.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asmmacro-32.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/regdef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/fpregdef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
    $(wildcard include/config/hugetlb/page.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/linkage.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink/war.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/irqflags.h \
    $(wildcard include/config/irq/cpu.h) \
    $(wildcard include/config/trace/irqflags.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/stackframe.h \
    $(wildcard include/config/cpu/r3000.h) \
    $(wildcard include/config/cpu/tx39xx.h) \
    $(wildcard include/config/cpu/has/smartmips.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/mips/pgd/c0/context.h) \
    $(wildcard include/config/cpu/jump/workarounds.h) \
    $(wildcard include/config/cpu/loongson2f.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/isadep.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sysmips.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/mips32/o32.h) \
    $(wildcard include/config/mips32/n32.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/unistd.h \

arch/mips/kernel/scall32-o32.o: $(deps_arch/mips/kernel/scall32-o32.o)

$(deps_arch/mips/kernel/scall32-o32.o):
