cmd_usr/initramfs_data.lzma.o := /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc -Wp,-MD,usr/.initramfs_data.lzma.o.d  -nostdinc -isystem /opt/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -D"VMLINUX_LOAD_ADDRESS=0xffffffff80000000" -D"DATAOFFSET=0" -D__ASSEMBLY__ -ffunction-sections  -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -DCONFIG_MIPS_TC3262  -march=mips32r2 -Wa,-mips32r2 -Wa,--trap -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-ralink -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/linux-2.6.36NEW.x/arch/mips/include/asm/mach-generic        -c -o usr/initramfs_data.lzma.o usr/initramfs_data.lzma.S

deps_usr/initramfs_data.lzma.o := \
  usr/initramfs_data.lzma.S \

usr/initramfs_data.lzma.o: $(deps_usr/initramfs_data.lzma.o)

$(deps_usr/initramfs_data.lzma.o):
