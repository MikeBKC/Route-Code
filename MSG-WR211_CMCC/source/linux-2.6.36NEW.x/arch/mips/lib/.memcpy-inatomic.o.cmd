cmd_arch/mips/lib/memcpy-inatomic.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,arch/mips/lib/.memcpy-inatomic.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -D__ASSEMBLY__ -ffunction-sections  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic        -c -o arch/mips/lib/memcpy-inatomic.o arch/mips/lib/memcpy-inatomic.S

deps_arch/mips/lib/memcpy-inatomic.o := \
  arch/mips/lib/memcpy-inatomic.S \
    $(wildcard include/config/dma/noncoherent.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
    $(wildcard include/config/mips/malta.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asm.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/sgi/ip28.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/sgidefs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/regdef.h \

arch/mips/lib/memcpy-inatomic.o: $(deps_arch/mips/lib/memcpy-inatomic.o)

$(deps_arch/mips/lib/memcpy-inatomic.o):
