cmd_lib/zlib_deflate/deftree.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,lib/zlib_deflate/.deftree.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262 -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(deftree)"  -D"KBUILD_MODNAME=KBUILD_STR(zlib_deflate)"  -c -o lib/zlib_deflate/deftree.o lib/zlib_deflate/deftree.c

deps_lib/zlib_deflate/deftree.o := \
  lib/zlib_deflate/deftree.c \
  include/linux/zutil.h \
  include/linux/zlib.h \
  include/linux/zconf.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/types.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/posix_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sgidefs.h \
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/cpu/r3000.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/full/panic.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/utt/debug.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/linkage.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/linkage.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/bitops.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/irqflags.h \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/irq/cpu.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/rm9000.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cpu-features.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cpu.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/cpu-feature-overrides.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink/war.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
  include/asm-generic/bitops/sched.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/arch_hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/ext2-non-atomic.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/asm-generic/bitops/minix.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/dynamic_debug.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/div64.h \
  include/asm-generic/div64.h \
  lib/zlib_deflate/defutil.h \

lib/zlib_deflate/deftree.o: $(deps_lib/zlib_deflate/deftree.o)

$(deps_lib/zlib_deflate/deftree.o):
