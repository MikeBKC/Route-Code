cmd_utt/common/utt_time_set.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,utt/common/.utt_time_set.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -ffunction-sections -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262 -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(utt_time_set)"  -D"KBUILD_MODNAME=KBUILD_STR(utt_time_set)"  -c -o utt/common/utt_time_set.o utt/common/utt_time_set.c

deps_utt/common/utt_time_set.o := \
  utt/common/utt_time_set.c \
    $(wildcard include/config/utt/time/set/max.h) \
  include/linux/ip.h \
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
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/posix_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sgidefs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/swab.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  include/linux/byteorder/generic.h \
  include/linux/skbuff.h \
    $(wildcard include/config/imq.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/netfilter/xt/target/uttindentify.h) \
    $(wildcard include/config/netfilter/xt/target/uttwebind.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/raeth/skb/recycle/2k.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/net/dma.h) \
    $(wildcard include/config/network/secmark.h) \
    $(wildcard include/config/performance/optimization.h) \
    $(wildcard include/config/pppopptp.h) \
    $(wildcard include/config/pppol2tp.h) \
    $(wildcard include/config/network/phy/timestamping.h) \
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
  /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include/stdarg.h \
  include/linux/linkage.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/linkage.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/bitops.h \
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
    $(wildcard include/config/32bit.h) \
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
  include/linux/kmemcheck.h \
    $(wildcard include/config/kmemcheck.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/auxvec.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/prefetch.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/processor.h \
    $(wildcard include/config/cavium/octeon/cvmseg/size.h) \
    $(wildcard include/config/mips/mt/fpaff.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cachectl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
    $(wildcard include/config/hugetlb/page.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/prefetch.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/system.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/dma/noncoherent.h) \
  include/linux/const.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/cmpxchg.h \
  include/asm-generic/cmpxchg-local.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/dsp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/watch.h \
    $(wildcard include/config/hardware/watchpoints.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/mips32/o32.h) \
    $(wildcard include/config/mips32/n32.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  include/linux/spinlock_up.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_up.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/atomic.h \
  include/asm-generic/atomic64.h \
  include/asm-generic/atomic-long.h \
  include/linux/prio_tree.h \
  include/linux/rbtree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/completion.h \
  include/linux/wait.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/current.h \
  include/asm-generic/current.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/page.h \
    $(wildcard include/config/cpu/mips32.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
  include/linux/pfn.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/io.h \
  include/asm-generic/iomap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/pgtable-bits.h \
    $(wildcard include/config/cpu/tx39xx.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/ioremap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/mangle-port.h \
    $(wildcard include/config/swap/io/space.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/getorder.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mmu.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/seqlock.h \
  include/linux/math64.h \
  include/linux/net.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sockios.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/sockios.h \
  include/linux/uio.h \
  include/linux/random.h \
  include/linux/ioctl.h \
  include/linux/irqnr.h \
  include/linux/fcntl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutiny.h \
    $(wildcard include/config/no/hz.h) \
  include/linux/sysctl.h \
    $(wildcard include/config/utt/connlimit.h) \
    $(wildcard include/config/utt/sys/log/info.h) \
    $(wildcard include/config/utt/dhcp/server/detection.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/textsearch.h \
  include/linux/module.h \
    $(wildcard include/config/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/stat.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/stat.h \
  include/linux/kmod.h \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/debug/vm.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/topology.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/jiffies.h \
  include/linux/timex.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/timex.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/elf.h \
    $(wildcard include/config/mips32/compat.h) \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/tracepoint.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/module.h \
    $(wildcard include/config/cpu/mips32/r1.h) \
    $(wildcard include/config/cpu/mips32/r2.h) \
    $(wildcard include/config/cpu/mips64/r1.h) \
    $(wildcard include/config/cpu/mips64/r2.h) \
    $(wildcard include/config/cpu/r4300.h) \
    $(wildcard include/config/cpu/r4x00.h) \
    $(wildcard include/config/cpu/tx49xx.h) \
    $(wildcard include/config/cpu/r5000.h) \
    $(wildcard include/config/cpu/r5432.h) \
    $(wildcard include/config/cpu/r6000.h) \
    $(wildcard include/config/cpu/nevada.h) \
    $(wildcard include/config/cpu/rm7000.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/uaccess.h \
  include/trace/events/module.h \
  include/trace/define_trace.h \
  include/linux/err.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/slab.h) \
  include/linux/slab_def.h \
  include/trace/events/kmem.h \
  include/trace/events/gfpflags.h \
  include/linux/kmalloc_sizes.h \
  include/net/checksum.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/checksum.h \
  include/linux/in6.h \
  include/linux/dmaengine.h \
    $(wildcard include/config/async/tx/disable/channel/switch.h) \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/async/tx/dma.h) \
  include/linux/device.h \
    $(wildcard include/config/of.h) \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/pm/ops.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/device.h \
  include/asm-generic/device.h \
  include/linux/pm_wakeup.h \
    $(wildcard include/config/pm.h) \
  include/linux/dma-mapping.h \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/have/dma/attrs.h) \
    $(wildcard include/config/need/dma/map/state.h) \
  include/linux/dma-attrs.h \
  include/linux/bug.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/scatterlist.h \
  include/asm-generic/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
  include/linux/mm.h \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/memory/failure.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/range.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/pgtable.h \
    $(wildcard include/config/cpu/supports/uncached/accelerated.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/pgtable-32.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/fixmap.h \
    $(wildcard include/config/bcm63xx.h) \
  include/asm-generic/pgtable-nopmd.h \
  include/asm-generic/pgtable-nopud.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/s390.h) \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/dma-mapping.h \
  include/asm-generic/dma-coherent.h \
    $(wildcard include/config/have/generic/dma/coherent.h) \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
  include/net/sch_utt.h \
  include/linux/netfilter_ipv4/time_set.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibShare.h \
    $(wildcard include/config/ap.h) \
    $(wildcard include/config/profiles.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/typedefShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/uttMachine.h \
    $(wildcard include/config/utt/netfilter.h) \
    $(wildcard include/config/switch/ext.h) \
    $(wildcard include/config/ip1725.h) \
    $(wildcard include/config/vsc7390.h) \
    $(wildcard include/config/ralink/rt3052.h) \
    $(wildcard include/config/ralink/mt7628.h) \
    $(wildcard include/config/rtl8306/phy.h) \
    $(wildcard include/config/it8712/ar8316.h) \
  include/linux/autoconf.h \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/mt/wsc/included.h) \
    $(wildcard include/config/ip/nf/target/redirect.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/ext3/fs/xattr.h) \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/flatmem/manual.h) \
    $(wildcard include/config/ext3/defaults/to/ordered.h) \
    $(wildcard include/config/nf/conntrack/netbios/ns.h) \
    $(wildcard include/config/scsi/dh.h) \
    $(wildcard include/config/bridge/ebt/t/filter.h) \
    $(wildcard include/config/net/ethernet.h) \
    $(wildcard include/config/experimental.h) \
    $(wildcard include/config/ppp/sync/tty.h) \
    $(wildcard include/config/arch/suspend/possible.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/netfilter/xt/match/helper.h) \
    $(wildcard include/config/ssb/possible.h) \
    $(wildcard include/config/bridge/ebt/t/nat.h) \
    $(wildcard include/config/nf/nat/sip.h) \
    $(wildcard include/config/bridge/nf/ebtables.h) \
    $(wildcard include/config/usb/ohci/little/endian.h) \
    $(wildcard include/config/net/sch/fifo.h) \
    $(wildcard include/config/stp.h) \
    $(wildcard include/config/arch/flatmem/enable.h) \
    $(wildcard include/config/nf/conntrack/sip.h) \
    $(wildcard include/config/bridge/ebt/redirect.h) \
    $(wildcard include/config/default/security/dac.h) \
    $(wildcard include/config/fib/rules.h) \
    $(wildcard include/config/bridge/ebt/ulog.h) \
    $(wildcard include/config/crypto/aead.h) \
    $(wildcard include/config/rt2880/dram/64m.h) \
    $(wildcard include/config/default/tcp/cong.h) \
    $(wildcard include/config/uevent/helper/path.h) \
    $(wildcard include/config/usb/devicefs.h) \
    $(wildcard include/config/mt/apcli/support.h) \
    $(wildcard include/config/utt/wifidog/xt/wifi/match.h) \
    $(wildcard include/config/wan/at/p4.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/ralink/ram/size.h) \
    $(wildcard include/config/blk/dev/bsg.h) \
    $(wildcard include/config/bridge/ebt/arpreply.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/crypto/rng2.h) \
    $(wildcard include/config/msdos/fs.h) \
    $(wildcard include/config/mt/wsc/v2/support.h) \
    $(wildcard include/config/serial/8250.h) \
    $(wildcard include/config/usb/ehci/root/hub/tt.h) \
    $(wildcard include/config/wireless/ext/sysfs.h) \
    $(wildcard include/config/rd/lzma.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/crypto/hmac.h) \
    $(wildcard include/config/sched/omit/frame/pointer.h) \
    $(wildcard include/config/mips/mt/disabled.h) \
    $(wildcard include/config/mtd/partitions.h) \
    $(wildcard include/config/second/if/mt7612e.h) \
    $(wildcard include/config/net/ipgre/demux.h) \
    $(wildcard include/config/rt/3052/esw.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/mtd/cfi/i2.h) \
    $(wildcard include/config/crypto/authenc.h) \
    $(wildcard include/config/cevt/r4k/lib.h) \
    $(wildcard include/config/inetpeer.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/mtd.h) \
    $(wildcard include/config/decompress/lzma.h) \
    $(wildcard include/config/ask/ip/fib/hash.h) \
    $(wildcard include/config/ralink/gpio.h) \
    $(wildcard include/config/crypto/des.h) \
    $(wildcard include/config/nls/codepage/437.h) \
    $(wildcard include/config/ip/nf/match/set.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
    $(wildcard include/config/max/swapfiles/shift.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/ralink/gpio/led.h) \
    $(wildcard include/config/ralink/rdm.h) \
    $(wildcard include/config/ip/nf/set/max.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/netfilter/xt/target/uttnetsniper.h) \
    $(wildcard include/config/mtd/cfi/util.h) \
    $(wildcard include/config/ralink/wdg/refresh/interval.h) \
    $(wildcard include/config/netfilter/xt/match/string.h) \
    $(wildcard include/config/hw/has/pci.h) \
    $(wildcard include/config/usb/storage.h) \
    $(wildcard include/config/standalone.h) \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/webauth/mac/validate.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/have/ide.h) \
    $(wildcard include/config/init/env/arg/limit.h) \
    $(wildcard include/config/ppp/deflate.h) \
    $(wildcard include/config/textsearch/kmp.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/utt/skb/iiftype.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/rtdev.h) \
    $(wildcard include/config/netfilter/xt/match/httpqq.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/mtd/spi/ralink.h) \
    $(wildcard include/config/mt7628/asic.h) \
    $(wildcard include/config/ip/advanced/router.h) \
    $(wildcard include/config/utt/lan/arp/collision/detect.h) \
    $(wildcard include/config/pcmcia.h) \
    $(wildcard include/config/bridge/ebt/limit.h) \
    $(wildcard include/config/sys/supports/little/endian.h) \
    $(wildcard include/config/ip/nf/target/set.h) \
    $(wildcard include/config/netfilter/xt/match/uttdev.h) \
    $(wildcard include/config/nls/iso8859/1.h) \
    $(wildcard include/config/crypto/workqueue.h) \
    $(wildcard include/config/mt/ate/support.h) \
    $(wildcard include/config/usb/ehci/hcd.h) \
    $(wildcard include/config/textsearch/bm.h) \
    $(wildcard include/config/ppp/mppe.h) \
    $(wildcard include/config/netdevices.h) \
    $(wildcard include/config/initramfs/root/gid.h) \
    $(wildcard include/config/defconfig/list.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/serial/8250/console.h) \
    $(wildcard include/config/netfilter/xt/target/uttpolicyroute.h) \
    $(wildcard include/config/ip/nf/target/masquerade.h) \
    $(wildcard include/config/netfilter/xt/target/uttdnsredirect.h) \
    $(wildcard include/config/scsi/wait/scan.h) \
    $(wildcard include/config/utt/nv520w.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/wan/port3.h) \
    $(wildcard include/config/select/memory/model.h) \
    $(wildcard include/config/netfilter/advanced.h) \
    $(wildcard include/config/crypto/deflate.h) \
    $(wildcard include/config/netfilter/xt/match/tcpmss.h) \
    $(wildcard include/config/mtd/cfi.h) \
    $(wildcard include/config/have/dynamic/ftrace.h) \
    $(wildcard include/config/cevt/r4k.h) \
    $(wildcard include/config/utt/mt7628/flash/soften.h) \
    $(wildcard include/config/ip/nf/mangle.h) \
    $(wildcard include/config/bridge/ebt/vlan.h) \
    $(wildcard include/config/ip/nf/filter.h) \
    $(wildcard include/config/ext3/fs.h) \
    $(wildcard include/config/utt/wifidog/redirect.h) \
    $(wildcard include/config/netfilter/xt/match/length.h) \
    $(wildcard include/config/bridge/ebt/pkttype.h) \
    $(wildcard include/config/fat/fs.h) \
    $(wildcard include/config/textsearch/fsm.h) \
    $(wildcard include/config/sys/has/cpu/mips32/r1.h) \
    $(wildcard include/config/generic/clockevents.h) \
    $(wildcard include/config/esw/double/vlan/tag.h) \
    $(wildcard include/config/mt/dot11k/rrm/support.h) \
    $(wildcard include/config/pcmcia/load/cis.h) \
    $(wildcard include/config/ip/nf/set/iphash.h) \
    $(wildcard include/config/usb/ohci/hcd.h) \
    $(wildcard include/config/utt/flag.h) \
    $(wildcard include/config/module/force/load.h) \
    $(wildcard include/config/nls/ascii.h) \
    $(wildcard include/config/max/user/rt/prio.h) \
    $(wildcard include/config/mtd/physmap.h) \
    $(wildcard include/config/mempool.h) \
    $(wildcard include/config/crypto/md5.h) \
    $(wildcard include/config/xattr.h) \
    $(wildcard include/config/pccard.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/netfilter/xt/mark.h) \
    $(wildcard include/config/netfilter/xtables.h) \
    $(wildcard include/config/mt/wifi/path.h) \
    $(wildcard include/config/slabinfo.h) \
    $(wildcard include/config/crypto/hw.h) \
    $(wildcard include/config/cardbus.h) \
    $(wildcard include/config/mt/dot11w/pmf/support.h) \
    $(wildcard include/config/rlt/mac.h) \
    $(wildcard include/config/crc16.h) \
    $(wildcard include/config/generic/calibrate/delay.h) \
    $(wildcard include/config/net/cls.h) \
    $(wildcard include/config/broken/on/smp.h) \
    $(wildcard include/config/nls/codepage/936.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/cpu/supports/32bit/kernel.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/utt/mac/c850e9.h) \
    $(wildcard include/config/sys/has/cpu/mips32/r2.h) \
    $(wildcard include/config/ra/network/tasklet/bh.h) \
    $(wildcard include/config/crypto/zlib.h) \
    $(wildcard include/config/wifi/driver.h) \
    $(wildcard include/config/mt/uapsd.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/crypto/pcomp2.h) \
    $(wildcard include/config/netfilter/xt/target/uttexceptali.h) \
    $(wildcard include/config/igmp.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/raeth.h) \
    $(wildcard include/config/arch/hibernation/possible.h) \
    $(wildcard include/config/unix.h) \
    $(wildcard include/config/crypto/hash2.h) \
    $(wildcard include/config/crypto/algapi.h) \
    $(wildcard include/config/netfilter/xt/target/uttpolicy.h) \
    $(wildcard include/config/bridge.h) \
    $(wildcard include/config/mtd/cfi/i1.h) \
    $(wildcard include/config/nf/nat.h) \
    $(wildcard include/config/crypto/hash.h) \
    $(wildcard include/config/log/buf/shift.h) \
    $(wildcard include/config/extra/firmware.h) \
    $(wildcard include/config/virt/to/bus.h) \
    $(wildcard include/config/vfat/fs.h) \
    $(wildcard include/config/utt/lan2/en.h) \
    $(wildcard include/config/trad/signals.h) \
    $(wildcard include/config/crypto/cbc.h) \
    $(wildcard include/config/serial/8250/runtime/uarts.h) \
    $(wildcard include/config/mt/mbss/support.h) \
    $(wildcard include/config/utt/bssid/before/reosse1point5.h) \
    $(wildcard include/config/fs/mbcache.h) \
    $(wildcard include/config/initramfs/root/uid.h) \
    $(wildcard include/config/cmdline/bool.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/ralink/timer/dfs.h) \
    $(wildcard include/config/crypto/manager2.h) \
    $(wildcard include/config/bridge/ebt/dnat.h) \
    $(wildcard include/config/ip/nf/set/macipmap.h) \
    $(wildcard include/config/crypto/ecb.h) \
    $(wildcard include/config/usb/libusual.h) \
    $(wildcard include/config/base/full.h) \
    $(wildcard include/config/zlib/deflate.h) \
    $(wildcard include/config/netfilter/xt/match/layer7.h) \
    $(wildcard include/config/fw/loader.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/generic/atomic64.h) \
    $(wildcard include/config/ra/nat/none.h) \
    $(wildcard include/config/pci/quirks.h) \
    $(wildcard include/config/ext4/fs.h) \
    $(wildcard include/config/bridge/ebt/802/3.h) \
    $(wildcard include/config/crypto/sha1.h) \
    $(wildcard include/config/net/sk/filter.h) \
    $(wildcard include/config/csrc/r4k.h) \
    $(wildcard include/config/csrc/r4k/lib.h) \
    $(wildcard include/config/bridge/ebt/mark/t.h) \
    $(wildcard include/config/has/iomem.h) \
    $(wildcard include/config/generic/irq/probe.h) \
    $(wildcard include/config/proc/kcore.h) \
    $(wildcard include/config/mtd/map/bank/width/1.h) \
    $(wildcard include/config/bridge/ebt/snat.h) \
    $(wildcard include/config/cpu/supports/highmem.h) \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/netfilter/xt/target/tcpmss.h) \
    $(wildcard include/config/ext2/fs.h) \
    $(wildcard include/config/netfilter/xt/match/dscp.h) \
    $(wildcard include/config/rt3xxx/ehci.h) \
    $(wildcard include/config/netfilter/xt/match/conntrack.h) \
    $(wildcard include/config/mtd/gen/probe.h) \
    $(wildcard include/config/packet.h) \
    $(wildcard include/config/netfilter/xt/match/iprange.h) \
    $(wildcard include/config/net/cls/basic.h) \
    $(wildcard include/config/nf/conntrack/tftp.h) \
    $(wildcard include/config/ip/nf/set/hashsize.h) \
    $(wildcard include/config/inet.h) \
    $(wildcard include/config/mt/mac.h) \
    $(wildcard include/config/ip/route/verbose.h) \
    $(wildcard include/config/prevent/firmware/build.h) \
    $(wildcard include/config/pci/domains.h) \
    $(wildcard include/config/bridge/ebt/broute.h) \
    $(wildcard include/config/rtc/lib.h) \
    $(wildcard include/config/have/kprobes.h) \
    $(wildcard include/config/raeth/lro.h) \
    $(wildcard include/config/autofs/fs.h) \
    $(wildcard include/config/utt/netfilter/hash/trigger.h) \
    $(wildcard include/config/nf/conntrack/mark.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/netfilter/xt/match/hashlimit.h) \
    $(wildcard include/config/bridge/ebt/among.h) \
    $(wildcard include/config/nr/ldiscs.h) \
    $(wildcard include/config/blk/dev/ram/count.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/nf/nat/needed.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/generic/hardirqs/no//do/irq.h) \
    $(wildcard include/config/usb/arch/has/ehci.h) \
    $(wildcard include/config/mtd/blkdevs.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/preempt/none.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/ip/nf/set/ipportiphash.h) \
    $(wildcard include/config/have/ftrace/mcount/record.h) \
    $(wildcard include/config/netfilter/xt/target/utthoststa.h) \
    $(wildcard include/config/bridge/ebt/ip.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/localversion.h) \
    $(wildcard include/config/utt/time/grp.h) \
    $(wildcard include/config/crypto.h) \
    $(wildcard include/config/default/mmap/min/addr.h) \
    $(wildcard include/config/mt/new/rate/adapt/support.h) \
    $(wildcard include/config/ip/nf/iptables.h) \
    $(wildcard include/config/cmdline.h) \
    $(wildcard include/config/initramfs/compression/lzma.h) \
    $(wildcard include/config/usb/arch/has/hcd.h) \
    $(wildcard include/config/mtd/physmap/bankwidth.h) \
    $(wildcard include/config/scsi/mod.h) \
    $(wildcard include/config/wifi/basic/func.h) \
    $(wildcard include/config/serial/core.h) \
    $(wildcard include/config/fuse/fs.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/ppp/filter.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/lan/wan/support.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/jbd2.h) \
    $(wildcard include/config/ralink/wdg/timer.h) \
    $(wildcard include/config/netfilter/xt/match/connlimit.h) \
    $(wildcard include/config/utt/performance.h) \
    $(wildcard include/config/nf/nat/h323.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/ip/route/multipath.h) \
    $(wildcard include/config/chr/dev/sg.h) \
    $(wildcard include/config/utt/cloud/config.h) \
    $(wildcard include/config/bridge/ebt/mark.h) \
    $(wildcard include/config/rt/second/card.h) \
    $(wildcard include/config/crypto/arc4.h) \
    $(wildcard include/config/boot/elf32.h) \
    $(wildcard include/config/slhc.h) \
    $(wildcard include/config/crypto/manager.h) \
    $(wildcard include/config/ppp/bsdcomp.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/netfilter/xt/target/mark.h) \
    $(wildcard include/config/net/cls/fw.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/wext/proc.h) \
    $(wildcard include/config/netfilter/xt/match/webstr.h) \
    $(wildcard include/config/hz/250.h) \
    $(wildcard include/config/ip/nf/set/nethash.h) \
    $(wildcard include/config/ip/nf/set/iptreemap.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/board/sys/led/gpio.h) \
    $(wildcard include/config/board/mtk7628nn/a/v1/0.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/initramfs/source.h) \
    $(wildcard include/config/mt/wifi/mode/ap.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/usb/debug.h) \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/utt/nf/dns/filter.h) \
    $(wildcard include/config/utt/mt/apcli/conn/status.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/serial/8250/nr/uarts.h) \
    $(wildcard include/config/utt/lan2/ifname.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/hotplug/pci.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/netfilter/xt/connmark.h) \
    $(wildcard include/config/firmware/in/kernel.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
    $(wildcard include/config/have/function/trace/mcount/test.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/mtd/physmap/len.h) \
    $(wildcard include/config/raeth/checksum/offload.h) \
    $(wildcard include/config/mt/rate/adaption.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/ip/nf/set.h) \
    $(wildcard include/config/netfilter/xt/match/layer7/mem.h) \
    $(wildcard include/config/have/arch/kgdb.h) \
    $(wildcard include/config/ip/fib/hash.h) \
    $(wildcard include/config/usb/arch/has/ohci.h) \
    $(wildcard include/config/nf/conntrack/ipv4.h) \
    $(wildcard include/config/zone/dma/flag.h) \
    $(wildcard include/config/max/wlan/client/num.h) \
    $(wildcard include/config/legacy/pty/count.h) \
    $(wildcard include/config/mtd/map/bank/width/2.h) \
    $(wildcard include/config/ralink/timer.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/generic/cmos/update.h) \
    $(wildcard include/config/default/security.h) \
    $(wildcard include/config/rt/first/if/rf/offset.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/default/noop.h) \
    $(wildcard include/config/have/function/graph/tracer.h) \
    $(wildcard include/config/crypto/blkcipher2.h) \
    $(wildcard include/config/mtd/block.h) \
    $(wildcard include/config/wext/priv.h) \
    $(wildcard include/config/scsi/lowlevel.h) \
    $(wildcard include/config/sys/supports/arbit/hz.h) \
    $(wildcard include/config/mtd/physmap/compat.h) \
    $(wildcard include/config/kernel/nvram.h) \
    $(wildcard include/config/utt/mac/limit.h) \
    $(wildcard include/config/rt/first/card.h) \
    $(wildcard include/config/ralink/mt7612e.h) \
    $(wildcard include/config/ralink/timer/wdg/reset/output.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/chr/dev/st.h) \
    $(wildcard include/config/rt/second/if/rf/offset.h) \
    $(wildcard include/config/usb/ehci/tt/newsched.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/mips/fpu/emu.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/unix98/ptys.h) \
    $(wildcard include/config/netfilter/xt/target/connmark.h) \
    $(wildcard include/config/jbd.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/mt/led/control/support.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/elf/core.h) \
    $(wildcard include/config/textsearch.h) \
    $(wildcard include/config/rt2880/rootfs/in/ram.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/mtd/physmap/buswidth.h) \
    $(wildcard include/config/mtd/char.h) \
    $(wildcard include/config/inet/lro.h) \
    $(wildcard include/config/cpu/supports/hr/sched/clock.h) \
    $(wildcard include/config/blk/dev/ram.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/mt/wifi.h) \
    $(wildcard include/config/bridge/ebt/log.h) \
    $(wildcard include/config/bridge/ebt/stp.h) \
    $(wildcard include/config/bridge/ebt/arp.h) \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/mtd/cfi/amdstd.h) \
    $(wildcard include/config/sys/supports/32bit/kernel.h) \
    $(wildcard include/config/sysvipc/sysctl.h) \
    $(wildcard include/config/nat/rcone.h) \
    $(wildcard include/config/mtd/physmap/start.h) \
    $(wildcard include/config/llc.h) \
    $(wildcard include/config/cross/compile.h) \
    $(wildcard include/config/serial/8250/pci.h) \
    $(wildcard include/config/nls/utf8.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/devport.h) \
    $(wildcard include/config/blk/dev/ram/size.h) \
    $(wildcard include/config/ralink/timer/wdg.h) \
    $(wildcard include/config/crypto/pcomp.h) \
    $(wildcard include/config/crypto/blkcipher.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/blk/dev/ub.h) \
    $(wildcard include/config/first/if/mt7628.h) \
    $(wildcard include/config/arch/supports/oprofile.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/mt/dot11r/ft/support.h) \
    $(wildcard include/config/mtd/map/bank/width/4.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/ext4/fs/xattr.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/crypto/aead2.h) \
    $(wildcard include/config/crypto/algapi2.h) \
    $(wildcard include/config/wext/spy.h) \
    $(wildcard include/config/mt/ap/support.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/bridge/ebt/nflog.h) \
    $(wildcard include/config/utt/lan/ifname.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/rt3xxx/ohci.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/uttMachineAllMacro.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibunionShare.h \
    $(wildcard include/config/user/goahead/lang/zhcn.h) \
    $(wildcard include/config/user/goahead/lang/en.h) \
    $(wildcard include/config/user/goahead/lang/zhtw.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/autoconf.h \
    $(wildcard include/config/lib/libgmp/force.h) \
    $(wildcard include/config/lib/libpthread/force.h) \
    $(wildcard include/config/lib/libntfs3g/force.h) \
    $(wildcard include/config/lib/libbt/force.h) \
    $(wildcard include/config/lib/readline/force.h) \
    $(wildcard include/config/lib/flex/force.h) \
    $(wildcard include/config/lib/libupnp/force.h) \
    $(wildcard include/config/lib/zlib/force.h) \
    $(wildcard include/config/lib/libstdc/plus/force.h) \
    $(wildcard include/config/lib/libswitch/force.h) \
    $(wildcard include/config/user/hello/world.h) \
    $(wildcard include/config/lib/libusb/user/force.h) \
    $(wildcard include/config/lib/libiconv/force.h) \
    $(wildcard include/config/lib/libcjson/force.h) \
    $(wildcard include/config/lib/crypt/force.h) \
    $(wildcard include/config/lib/libm/force.h) \
    $(wildcard include/config/lib/nsl/force.h) \
    $(wildcard include/config/lib/util/force.h) \
    $(wildcard include/config/lib/resolv/force.h) \
    $(wildcard include/config/lib/rt/force.h) \
    $(wildcard include/config/lib/usb0112/force.h) \
    $(wildcard include/config/lib/usb100/force.h) \
    $(wildcard include/config/user/mtdutils.h) \
    $(wildcard include/config/user/mtdutils/erase.h) \
    $(wildcard include/config/user/mtdutils/eraseall.h) \
    $(wildcard include/config/user/mtdutils/ftl/check.h) \
    $(wildcard include/config/user/mtdutils/ftl/format.h) \
    $(wildcard include/config/user/mtdutils/mkfsjffs.h) \
    $(wildcard include/config/user/mtdutils/mkfsjffs2.h) \
    $(wildcard include/config/user/mtdutils/nftldump.h) \
    $(wildcard include/config/user/mtdutils/nftl/format.h) \
    $(wildcard include/config/user/mtdutils/nanddump.h) \
    $(wildcard include/config/user/mtdutils/nandtest.h) \
    $(wildcard include/config/user/mtdutils/nandwrite.h) \
    $(wildcard include/config/user/mtdutils/doc/loadbios.h) \
    $(wildcard include/config/user/mtdutils/doc/loadipl.h) \
    $(wildcard include/config/user/3g.h) \
    $(wildcard include/config/rt2880/scripts.h) \
    $(wildcard include/config/user/brctl/brctl.h) \
    $(wildcard include/config/user/bigpond.h) \
    $(wildcard include/config/user/bt/utils.h) \
    $(wildcard include/config/user/bt/avinfo.h) \
    $(wildcard include/config/user/bt/bccmd.h) \
    $(wildcard include/config/user/bt/ciptool.h) \
    $(wildcard include/config/user/bt/hciconfig.h) \
    $(wildcard include/config/user/bt/hciattach.h) \
    $(wildcard include/config/user/bt/hcisecfilter.h) \
    $(wildcard include/config/user/bt/hcitool.h) \
    $(wildcard include/config/user/bt/l2ping.h) \
    $(wildcard include/config/user/bt/ppporc.h) \
    $(wildcard include/config/user/bt/sdptool.h) \
    $(wildcard include/config/user/ctorrent.h) \
    $(wildcard include/config/user/dhcp6.h) \
    $(wildcard include/config/user/dhcp6/server.h) \
    $(wildcard include/config/user/dhcp6/client.h) \
    $(wildcard include/config/user/dnsmasq.h) \
    $(wildcard include/config/user/disktype.h) \
    $(wildcard include/config/user/echo/server.h) \
    $(wildcard include/config/user/storage.h) \
    $(wildcard include/config/user/mkdosfs.h) \
    $(wildcard include/config/user/stupid/ftpd.h) \
    $(wildcard include/config/user/samba.h) \
    $(wildcard include/config/user/goahead/hostname.h) \
    $(wildcard include/config/user/iplink/ddns.h) \
    $(wildcard include/config/user/goahead/httpd.h) \
    $(wildcard include/config/user/goahead/ipv6.h) \
    $(wildcard include/config/user/goahead/ssl.h) \
    $(wildcard include/config/user/goahead/greenap.h) \
    $(wildcard include/config/user/matrixssl.h) \
    $(wildcard include/config/user/igmp/proxy.h) \
    $(wildcard include/config/user/inadyn.h) \
    $(wildcard include/config/user/iptables/iptables.h) \
    $(wildcard include/config/user/iptables/ip6tables.h) \
    $(wildcard include/config/user/iproute2.h) \
    $(wildcard include/config/user/iproute2/ss.h) \
    $(wildcard include/config/user/iproute2/nstat.h) \
    $(wildcard include/config/user/iproute2/ifstat.h) \
    $(wildcard include/config/user/iproute2/rtacct.h) \
    $(wildcard include/config/user/iproute2/lnstat.h) \
    $(wildcard include/config/user/iproute2/ip.h) \
    $(wildcard include/config/user/iproute2/rtmon.h) \
    $(wildcard include/config/user/iproute2/tc.h) \
    $(wildcard include/config/user/miniupnpd.h) \
    $(wildcard include/config/user/ntpclient/ntpclient.h) \
    $(wildcard include/config/user/ntpclient/adjtimex.h) \
    $(wildcard include/config/user/openssl.h) \
    $(wildcard include/config/user/openswan.h) \
    $(wildcard include/config/user/p910nd.h) \
    $(wildcard include/config/user/portmap.h) \
    $(wildcard include/config/user/pppd.h) \
    $(wildcard include/config/user/pppd/with/pppoe.h) \
    $(wildcard include/config/user/pppd/pppstats.h) \
    $(wildcard include/config/user/pppd/pppdump.h) \
    $(wildcard include/config/user/pppd/pppchat.h) \
    $(wildcard include/config/user/rppppoe/relay.h) \
    $(wildcard include/config/user/rppppoe/sniff.h) \
    $(wildcard include/config/user/pppoecd.h) \
    $(wildcard include/config/user/pptp/client.h) \
    $(wildcard include/config/user/l2tp/server.h) \
    $(wildcard include/config/user/zebra.h) \
    $(wildcard include/config/lib/libzebra/force.h) \
    $(wildcard include/config/user/rp/l2tp.h) \
    $(wildcard include/config/user/radvd.h) \
    $(wildcard include/config/user/radvd/dump.h) \
    $(wildcard include/config/user/802/1x.h) \
    $(wildcard include/config/user/rt2561/802/1x.h) \
    $(wildcard include/config/user/totd.h) \
    $(wildcard include/config/user/uvc/stream.h) \
    $(wildcard include/config/user/upnp/igd.h) \
    $(wildcard include/config/user/wpa/supplicant.h) \
    $(wildcard include/config/user/wsc.h) \
    $(wildcard include/config/user/wireless/tools.h) \
    $(wildcard include/config/user/wireless/tools/iwconfig.h) \
    $(wildcard include/config/user/wireless/tools/iwgetid.h) \
    $(wildcard include/config/user/wireless/tools/iwlist.h) \
    $(wildcard include/config/user/wireless/tools/iwpriv.h) \
    $(wildcard include/config/user/wireless/tools/iwspy.h) \
    $(wildcard include/config/user/wireless/tools/iwevent.h) \
    $(wildcard include/config/user/busybox/busybox.h) \
    $(wildcard include/config/user/bonnie.h) \
    $(wildcard include/config/user/ethtool.h) \
    $(wildcard include/config/dhrystone.h) \
    $(wildcard include/config/user/gdb/gdbreplay.h) \
    $(wildcard include/config/user/gdb/gdbserver.h) \
    $(wildcard include/config/user/ixia/endpoint.h) \
    $(wildcard include/config/user/lmbench3.h) \
    $(wildcard include/config/user/mtd/write.h) \
    $(wildcard include/config/user/netcat.h) \
    $(wildcard include/config/user/netstat/nat.h) \
    $(wildcard include/config/user/ntfs3g.h) \
    $(wildcard include/config/user/ntfsprogs.h) \
    $(wildcard include/config/user/strace/strace.h) \
    $(wildcard include/config/user/tcpdump.h) \
    $(wildcard include/config/user/pciutil/lspci.h) \
    $(wildcard include/config/user/pciutil/setpci.h) \
    $(wildcard include/config/user/lsusb.h) \
    $(wildcard include/config/user/usbmodeswitch/095.h) \
    $(wildcard include/config/user/usbmodeswitch/107.h) \
    $(wildcard include/config/user/usbmodeswitch/097.h) \
    $(wildcard include/config/user/comgt.h) \
    $(wildcard include/config/user/sdparm.h) \
    $(wildcard include/config/ralinkapp.h) \
    $(wildcard include/config/ralinkapp/ated.h) \
    $(wildcard include/config/ralinkapp/reg/rw.h) \
    $(wildcard include/config/ralinkapp/csr.h) \
    $(wildcard include/config/ralinkapp/flash.h) \
    $(wildcard include/config/ralinkapp/mii/mgr.h) \
    $(wildcard include/config/ralinkapp/nvram.h) \
    $(wildcard include/config/ralinkapp/switch.h) \
    $(wildcard include/config/ralinkapp/l2/manage.h) \
    $(wildcard include/config/ralinkapp/gpio.h) \
    $(wildcard include/config/ralinkapp/spi.h) \
    $(wildcard include/config/ralinkapp/i2c.h) \
    $(wildcard include/config/ralinkapp/memory/usage.h) \
    $(wildcard include/config/ralinkapp/swqos.h) \
    $(wildcard include/config/ralinkapp/wapi.h) \
    $(wildcard include/config/ralinkapp/scripts.h) \
    $(wildcard include/config/user/usb/disk.h) \
    $(wildcard include/config/lib/libnvram/force.h) \
    $(wildcard include/config/user/lltd.h) \
    $(wildcard include/config/user/netcwmp.h) \
    $(wildcard include/config/user/uttcare/ddns.h) \
    $(wildcard include/config/user/cfgser/lan.h) \
    $(wildcard include/config/user/wifidog.h) \
    $(wildcard include/config/user/alink.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibtypeShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibSystemShare.h \
    $(wildcard include/config/rtl8367m.h) \
    $(wildcard include/config/ralink/mt7620.h) \
    $(wildcard include/config/rtl8367rb.h) \
    $(wildcard include/config/ralink/mt7621.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibInterfaceShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibUserShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibIpGroupShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibTimeGrpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibDhcpServerShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibRouteShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibGroupBaseShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibpdbShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibnatShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibAdminConfShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibDnsFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibWirelessShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibNtpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibLangShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibTaskSchedShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibEasyTaskShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibMacFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibAppMenuShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/uttShareHead/config/mibCloudConfigShare.h \
    $(wildcard include/config/share/h.h) \
  include/linux/hardirq.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq.h \
    $(wildcard include/config/irq/per/cpu.h) \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/intr/remap.h) \
    $(wildcard include/config/generic/pending/irq.h) \
    $(wildcard include/config/sparse/irq.h) \
    $(wildcard include/config/numa/irq/desc.h) \
    $(wildcard include/config/cpumasks/offstack.h) \
  include/linux/irqreturn.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/irq.h \
    $(wildcard include/config/i8259.h) \
    $(wildcard include/config/mips/mt/smtc/irqaff.h) \
    $(wildcard include/config/mips/mt/smtc/im/backstop.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mipsmtregs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic/irq.h \
    $(wildcard include/config/irq/cpu/rm7k.h) \
    $(wildcard include/config/irq/cpu/rm9k.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/ptrace.h \
    $(wildcard include/config/cpu/has/smartmips.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/isadep.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/irq_regs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR201_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/hw_irq.h \
  include/linux/irq_cpustat.h \

utt/common/utt_time_set.o: $(deps_utt/common/utt_time_set.o)

$(deps_utt/common/utt_time_set.o):
