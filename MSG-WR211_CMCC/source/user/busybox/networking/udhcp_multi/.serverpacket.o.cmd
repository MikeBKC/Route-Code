cmd_networking/udhcp_multi/serverpacket.o := /opt/buildroot-gcc342/bin/mipsel-linux-uclibc-gcc -Wp,-MD,networking/udhcp_multi/.serverpacket.o.d   -std=gnu99 -Iinclude -Ilibbb  -include include/autoconf.h -D_GNU_SOURCE -DNDEBUG  -D"BB_VER=KBUILD_STR(1.12.1)" -DBB_BT=AUTOCONF_TIMESTAMP -O2 -fomit-frame-pointer -pipe  -Dlinux -D__linux__ -Dunix -DEMBED -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uClibc++/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source  -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/include -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib -I/data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/cli -Wall -Wshadow -Wwrite-strings -Wundef -Wstrict-prototypes -Wunused -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement  -fno-builtin-strlen -finline-limit=0 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-guess-branch-probability -funsigned-char -static-libgcc -falign-functions=1 -falign-jumps=1 -falign-labels=1 -falign-loops=1 -Os -Dlinux    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(serverpacket)"  -D"KBUILD_MODNAME=KBUILD_STR(serverpacket)" -c -o networking/udhcp_multi/serverpacket.o networking/udhcp_multi/serverpacket.c

deps_networking/udhcp_multi/serverpacket.o := \
  networking/udhcp_multi/serverpacket.c \
    $(wildcard include/config/board/p1010/c.h) \
    $(wildcard include/config/xport/log.h) \
    $(wildcard include/config/feature/udhcpd/write/leases/early.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/stdlib.h \
    $(wildcard include/config/en.h) \
    $(wildcard include/config/.h) \
    $(wildcard include/config/c99.h) \
    $(wildcard include/config/c.h) \
    $(wildcard include/config/d.h) \
    $(wildcard include/config/en/extended.h) \
    $(wildcard include/config/ix.h) \
    $(wildcard include/config/en2k.h) \
    $(wildcard include/config/e/offset64.h) \
    $(wildcard include/config/gefile64.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/features.h \
    $(wildcard include/config/ix2.h) \
    $(wildcard include/config/ix199309.h) \
    $(wildcard include/config/ix199506.h) \
    $(wildcard include/config/x98.h) \
    $(wildcard include/config/gefile.h) \
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
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stddef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/waitflags.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/waitstatus.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/endian.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/endian.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/kernel_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/wordsize.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/pthreadtypes.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sched.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/time.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/select.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/select.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sigset.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/time.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/sysmacros.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/alloca.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/stdio.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/uClibc_stdio.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/wchar.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/wchar.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stdarg.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/stdio_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/cli/typedef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/typedefShare.h \
  networking/udhcp_multi/common.h \
    $(wildcard include/config/dhcpc/default/script.h) \
    $(wildcard include/config/udhcpc/slack/for/buggy/servers.h) \
    $(wildcard include/config/feature/udhcp/debug.h) \
  include/libbb.h \
    $(wildcard include/config/selinux.h) \
    $(wildcard include/config/locale/support.h) \
    $(wildcard include/config/use/bb/pwd/grp.h) \
    $(wildcard include/config/feature/shadowpasswds.h) \
    $(wildcard include/config/use/bb/shadow.h) \
    $(wildcard include/config/lfs.h) \
    $(wildcard include/config/feature/buffers/go/on/stack.h) \
    $(wildcard include/config/buffer.h) \
    $(wildcard include/config/ubuffer.h) \
    $(wildcard include/config/feature/buffers/go/in/bss.h) \
    $(wildcard include/config/inux.h) \
    $(wildcard include/config/feature/ipv6.h) \
    $(wildcard include/config/feature/check/names.h) \
    $(wildcard include/config/feature/prefer/applets.h) \
    $(wildcard include/config/busybox/exec/path.h) \
    $(wildcard include/config/getopt/long.h) \
    $(wildcard include/config/feature/pidfile.h) \
    $(wildcard include/config/feature/syslog.h) \
    $(wildcard include/config/feature/individual.h) \
    $(wildcard include/config/o.h) \
    $(wildcard include/config/ntf.h) \
    $(wildcard include/config/t.h) \
    $(wildcard include/config/l.h) \
    $(wildcard include/config/wn.h) \
    $(wildcard include/config/ktop.h) \
    $(wildcard include/config/route.h) \
    $(wildcard include/config/feature/hwib.h) \
    $(wildcard include/config/debug/crond/option.h) \
    $(wildcard include/config/use/bb/crypt.h) \
    $(wildcard include/config/ioctl/hex2str/error.h) \
    $(wildcard include/config/feature/editing.h) \
    $(wildcard include/config/feature/editing/history.h) \
    $(wildcard include/config/ture/editing/savehistory.h) \
    $(wildcard include/config/feature/editing/savehistory.h) \
    $(wildcard include/config/feature/tab/completion.h) \
    $(wildcard include/config/feature/username/completion.h) \
    $(wildcard include/config/feature/editing/vi.h) \
    $(wildcard include/config/feature/topmem.h) \
    $(wildcard include/config/pgrep.h) \
    $(wildcard include/config/pkill.h) \
    $(wildcard include/config/pidof.h) \
    $(wildcard include/config/feature/devfs.h) \
  include/platform.h \
    $(wildcard include/config/werror.h) \
    $(wildcard include/config///.h) \
    $(wildcard include/config/nommu.h) \
    $(wildcard include/config//nommu.h) \
    $(wildcard include/config//mmu.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/byteswap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/byteswap.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/arpa/inet.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/netinet/in.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/stdint.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/limits.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/limits.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/posix1_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/local_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/limits.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/posix2_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/xopen_lim.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sockaddr.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/sockios.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm-generic/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/in.h \
  /opt/buildroot-gcc342/bin/../lib/gcc/mipsel-linux-uclibc/3.4.2/include/stdbool.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/mount.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/ioctl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/ioctls.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/ioctls.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/ioctl-types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/ttydefaults.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/ctype.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/uClibc_touplow.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/dirent.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/dirent.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/errno.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/errno_values.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/fcntl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/fcntl.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/stat.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/stat.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/inttypes.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/netdb.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/rpc/netdb.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/siginfo.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/netdb.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/setjmp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/setjmp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/signal.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/signum.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sigaction.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sigcontext.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/sigcontext.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/types.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/64bit/phys/addr.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm-generic/int-ll64.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/bitsperlong.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm-generic/bitsperlong.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/posix_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/stddef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/posix_types.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/sgidefs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sigstack.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/ucontext.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/ucontext.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sgidefs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/sigthread.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/string.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/poll.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/poll.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/mman.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/mman.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/socket.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/uio.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/uio.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/time.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/wait.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/resource.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/resource.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/termios.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/termios.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/uClibc_clk_tck.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/unistd.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/posix_opt.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/environments.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/confname.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/getopt.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/utime.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/param.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/param.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm/param.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/mntent.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/paths.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/statfs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/bits/statfs.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/pwd.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/grp.h \
  include/xatonum.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/netinet/udp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/netinet/ip.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/net/if.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/uttMachine.h \
    $(wildcard include/config/utt/netfilter.h) \
    $(wildcard include/config/profiles.h) \
    $(wildcard include/config/switch/ext.h) \
    $(wildcard include/config/ip1725.h) \
    $(wildcard include/config/vsc7390.h) \
    $(wildcard include/config/ralink/rt3052.h) \
    $(wildcard include/config/ralink/mt7628.h) \
    $(wildcard include/config/rtl8306/phy.h) \
    $(wildcard include/config/it8712/ar8316.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/linux/autoconf.h \
    $(wildcard include/config/nf/conntrack/h323.h) \
    $(wildcard include/config/scsi/dma.h) \
    $(wildcard include/config/mt/wsc/included.h) \
    $(wildcard include/config/ip/nf/target/redirect.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/ext3/fs/xattr.h) \
    $(wildcard include/config/second/card/external/pa.h) \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/dot11n/draft3.h) \
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
    $(wildcard include/config/second/if/eeprom/flash.h) \
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
    $(wildcard include/config/wsc/included.h) \
    $(wildcard include/config/ralink/ram/size.h) \
    $(wildcard include/config/blk/dev/bsg.h) \
    $(wildcard include/config/bridge/ebt/arpreply.h) \
    $(wildcard include/config/legacy/ptys.h) \
    $(wildcard include/config/crypto/rng2.h) \
    $(wildcard include/config/msdos/fs.h) \
    $(wildcard include/config/mt/wsc/v2/support.h) \
    $(wildcard include/config/coc/support.h) \
    $(wildcard include/config/utt/time/set/max.h) \
    $(wildcard include/config/serial/8250.h) \
    $(wildcard include/config/usb/ehci/root/hub/tt.h) \
    $(wildcard include/config/wireless/ext/sysfs.h) \
    $(wildcard include/config/rd/lzma.h) \
    $(wildcard include/config/usb.h) \
    $(wildcard include/config/crypto/hmac.h) \
    $(wildcard include/config/mt7610/apcli/conn/status.h) \
    $(wildcard include/config/sched/omit/frame/pointer.h) \
    $(wildcard include/config/mips/mt/disabled.h) \
    $(wildcard include/config/mtd/partitions.h) \
    $(wildcard include/config/second/if/mt7612e.h) \
    $(wildcard include/config/net/ipgre/demux.h) \
    $(wildcard include/config/rt/3052/esw.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/nf/conntrack/proc/compat.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
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
    $(wildcard include/config/rtmp/flash/support.h) \
    $(wildcard include/config/ip/nf/match/set.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
    $(wildcard include/config/max/swapfiles/shift.h) \
    $(wildcard include/config/zlib/inflate.h) \
    $(wildcard include/config/ralink/gpio/led.h) \
    $(wildcard include/config/ralink/rdm.h) \
    $(wildcard include/config/ip/nf/set/max.h) \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/stacktrace/support.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/cpu/has/sync.h) \
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
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/utt/skb/iiftype.h) \
    $(wildcard include/config/nls.h) \
    $(wildcard include/config/rtdev.h) \
    $(wildcard include/config/netfilter/xt/match/httpqq.h) \
    $(wildcard include/config/pppoe.h) \
    $(wildcard include/config/mtd/spi/ralink.h) \
    $(wildcard include/config/mt7628/asic.h) \
    $(wildcard include/config/ip/advanced/router.h) \
    $(wildcard include/config/rlt/ap/support/module.h) \
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
    $(wildcard include/config/scsi/wait/scan/module.h) \
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
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/esw/double/vlan/tag.h) \
    $(wildcard include/config/mt/dot11k/rrm/support.h) \
    $(wildcard include/config/pcmcia/load/cis.h) \
    $(wildcard include/config/ip/nf/set/iphash.h) \
    $(wildcard include/config/usb/ohci/hcd.h) \
    $(wildcard include/config/utt/flag.h) \
    $(wildcard include/config/module/force/load.h) \
    $(wildcard include/config/mbss/support.h) \
    $(wildcard include/config/nls/ascii.h) \
    $(wildcard include/config/max/user/rt/prio.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/mtd/physmap.h) \
    $(wildcard include/config/mempool.h) \
    $(wildcard include/config/hardware/watchpoints.h) \
    $(wildcard include/config/crypto/md5.h) \
    $(wildcard include/config/xattr.h) \
    $(wildcard include/config/pccard.h) \
    $(wildcard include/config/binfmt/elf.h) \
    $(wildcard include/config/scsi/proc/fs.h) \
    $(wildcard include/config/first/if/eeprom/prom.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/apcli/support.h) \
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
    $(wildcard include/config/utt/dhcp/server/detection.h) \
    $(wildcard include/config/wifi/mode/ap.h) \
    $(wildcard include/config/nls/codepage/936.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/multi/inf/support.h) \
    $(wildcard include/config/cpu/supports/32bit/kernel.h) \
    $(wildcard include/config/serial/core/console.h) \
    $(wildcard include/config/utt/mac/c850e9.h) \
    $(wildcard include/config/sys/has/cpu/mips32/r2.h) \
    $(wildcard include/config/ra/network/tasklet/bh.h) \
    $(wildcard include/config/raeth/skb/recycle/2k.h) \
    $(wildcard include/config/crypto/zlib.h) \
    $(wildcard include/config/wifi/driver.h) \
    $(wildcard include/config/mt/uapsd.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/crypto/pcomp2.h) \
    $(wildcard include/config/netfilter/xt/target/uttexceptali.h) \
    $(wildcard include/config/igmp.h) \
    $(wildcard include/config/nf/conntrack/ftp.h) \
    $(wildcard include/config/modules.h) \
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
    $(wildcard include/config/wsc/v2/support.h) \
    $(wildcard include/config/cmdline/bool.h) \
    $(wildcard include/config/have/function/tracer.h) \
    $(wildcard include/config/nf/nat/tftp.h) \
    $(wildcard include/config/netfilter/xt/match/multiport.h) \
    $(wildcard include/config/ralink/timer/dfs.h) \
    $(wildcard include/config/crypto/manager2.h) \
    $(wildcard include/config/bridge/ebt/dnat.h) \
    $(wildcard include/config/ip/nf/set/macipmap.h) \
    $(wildcard include/config/crypto/ecb.h) \
    $(wildcard include/config/rt/second/card/eeprom.h) \
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
    $(wildcard include/config/cpu/mips32/r2.h) \
    $(wildcard include/config/mtd/map/bank/width/1.h) \
    $(wildcard include/config/constructors.h) \
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
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/lockdep/support.h) \
    $(wildcard include/config/generic/hardirqs/no//do/irq.h) \
    $(wildcard include/config/usb/arch/has/ehci.h) \
    $(wildcard include/config/mtd/blkdevs.h) \
    $(wildcard include/config/sysctl/syscall.h) \
    $(wildcard include/config/need/dma/map/state.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
    $(wildcard include/config/preempt/none.h) \
    $(wildcard include/config/netfilter/xt/match/time.h) \
    $(wildcard include/config/netfilter/xt/match/mac.h) \
    $(wildcard include/config/ip/nf/set/ipportiphash.h) \
    $(wildcard include/config/have/ftrace/mcount/record.h) \
    $(wildcard include/config/netfilter/xt/target/utthoststa.h) \
    $(wildcard include/config/bridge/ebt/ip.h) \
    $(wildcard include/config/iosched/noop.h) \
    $(wildcard include/config/new/rate/adapt/support.h) \
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
    $(wildcard include/config/1.h) \
    $(wildcard include/config/embedded.h) \
    $(wildcard include/config/have/kretprobes.h) \
    $(wildcard include/config/ppp/filter.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/lan/wan/support.h) \
    $(wildcard include/config/scsi.h) \
    $(wildcard include/config/rlt/wifi/module.h) \
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
    $(wildcard include/config/led/control/support.h) \
    $(wildcard include/config/net/cls/fw.h) \
    $(wildcard include/config/wireless.h) \
    $(wildcard include/config/wext/proc.h) \
    $(wildcard include/config/netfilter/xt/match/webstr.h) \
    $(wildcard include/config/hz/250.h) \
    $(wildcard include/config/ip/nf/set/nethash.h) \
    $(wildcard include/config/ip/nf/set/iptreemap.h) \
    $(wildcard include/config/fat/default/iocharset.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/frame/warn.h) \
    $(wildcard include/config/board/sys/led/gpio.h) \
    $(wildcard include/config/board/mtk7628nn/a/v1/0.h) \
    $(wildcard include/config/generic/hweight.h) \
    $(wildcard include/config/initramfs/source.h) \
    $(wildcard include/config/mt/wifi/mode/ap.h) \
    $(wildcard include/config/rtmp/pci/support.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/usb/debug.h) \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/utt/nf/dns/filter.h) \
    $(wildcard include/config/utt/mt/apcli/conn/status.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/serial/8250/nr/uarts.h) \
    $(wildcard include/config/utt/lan2/ifname.h) \
    $(wildcard include/config/cpu/mips32.h) \
    $(wildcard include/config/default/iosched.h) \
    $(wildcard include/config/nlattr.h) \
    $(wildcard include/config/hotplug/pci.h) \
    $(wildcard include/config/tcp/cong/cubic.h) \
    $(wildcard include/config/full/panic.h) \
    $(wildcard include/config/netfilter/xt/connmark.h) \
    $(wildcard include/config/firmware/in/kernel.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/have/function/trace/mcount/test.h) \
    $(wildcard include/config/netfilter/xt/match/limit.h) \
    $(wildcard include/config/mtd/physmap/len.h) \
    $(wildcard include/config/cpu/mipsr2.h) \
    $(wildcard include/config/raeth/checksum/offload.h) \
    $(wildcard include/config/mt/rate/adaption.h) \
    $(wildcard include/config/msdos/partition.h) \
    $(wildcard include/config/have/oprofile.h) \
    $(wildcard include/config/have/generic/dma/coherent.h) \
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
    $(wildcard include/config/netfilter/xt/target/uttwebind.h) \
    $(wildcard include/config/ip/multicast.h) \
    $(wildcard include/config/generic/cmos/update.h) \
    $(wildcard include/config/default/security.h) \
    $(wildcard include/config/rt/first/if/rf/offset.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
    $(wildcard include/config/default/noop.h) \
    $(wildcard include/config/have/function/graph/tracer.h) \
    $(wildcard include/config/mips/l1/cache/shift.h) \
    $(wildcard include/config/base/small.h) \
    $(wildcard include/config/crypto/blkcipher2.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/mtd/block.h) \
    $(wildcard include/config/wext/priv.h) \
    $(wildcard include/config/scsi/lowlevel.h) \
    $(wildcard include/config/sys/supports/arbit/hz.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/ate/support.h) \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/mtd/physmap/compat.h) \
    $(wildcard include/config/utt/connlimit.h) \
    $(wildcard include/config/kernel/nvram.h) \
    $(wildcard include/config/utt/mac/limit.h) \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/rt/first/card.h) \
    $(wildcard include/config/ralink/mt7612e.h) \
    $(wildcard include/config/ralink/timer/wdg/reset/output.h) \
    $(wildcard include/config/ppp/async.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/chr/dev/st.h) \
    $(wildcard include/config/rt/second/if/rf/offset.h) \
    $(wildcard include/config/usb/ehci/tt/newsched.h) \
    $(wildcard include/config/fat/default/codepage.h) \
    $(wildcard include/config/mips/fpu/emu.h) \
    $(wildcard include/config/blk/dev.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/rt/first/card/eeprom.h) \
    $(wildcard include/config/tracing/support.h) \
    $(wildcard include/config/unix98/ptys.h) \
    $(wildcard include/config/netfilter/xt/target/connmark.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/jbd.h) \
    $(wildcard include/config/ppp.h) \
    $(wildcard include/config/mt/led/control/support.h) \
    $(wildcard include/config/nf/nat/ftp.h) \
    $(wildcard include/config/elf/core.h) \
    $(wildcard include/config/textsearch.h) \
    $(wildcard include/config/rt2880/rootfs/in/ram.h) \
    $(wildcard include/config/dma/noncoherent.h) \
    $(wildcard include/config/usb/support.h) \
    $(wildcard include/config/mtd/physmap/buswidth.h) \
    $(wildcard include/config/mtd/char.h) \
    $(wildcard include/config/inet/lro.h) \
    $(wildcard include/config/swap/io/space.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cpu/supports/hr/sched/clock.h) \
    $(wildcard include/config/blk/dev/ram.h) \
    $(wildcard include/config/netfilter/xt/match/state.h) \
    $(wildcard include/config/mt/wifi.h) \
    $(wildcard include/config/bridge/ebt/log.h) \
    $(wildcard include/config/irq/cpu.h) \
    $(wildcard include/config/bridge/ebt/stp.h) \
    $(wildcard include/config/bridge/ebt/arp.h) \
    $(wildcard include/config/performance/optimization.h) \
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
    $(wildcard include/config/mac/repeater/support.h) \
    $(wildcard include/config/crc/ccitt.h) \
    $(wildcard include/config/blk/dev/sd.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/bitreverse.h) \
    $(wildcard include/config/devport.h) \
    $(wildcard include/config/second/card/external/lna.h) \
    $(wildcard include/config/blk/dev/ram/size.h) \
    $(wildcard include/config/ralink/timer/wdg.h) \
    $(wildcard include/config/crypto/pcomp.h) \
    $(wildcard include/config/crypto/blkcipher.h) \
    $(wildcard include/config/rate/adaption.h) \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/blk/dev/ub.h) \
    $(wildcard include/config/first/if/mt7628.h) \
    $(wildcard include/config/arch/supports/oprofile.h) \
    $(wildcard include/config/ip/nf/target/reject.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/mt/dot11r/ft/support.h) \
    $(wildcard include/config/dot11/vht/ac.h) \
    $(wildcard include/config/mtd/map/bank/width/4.h) \
    $(wildcard include/config/cpu/little/endian.h) \
    $(wildcard include/config/ext4/fs/xattr.h) \
    $(wildcard include/config/nls/default.h) \
    $(wildcard include/config/crypto/aead2.h) \
    $(wildcard include/config/crypto/algapi2.h) \
    $(wildcard include/config/wext/spy.h) \
    $(wildcard include/config/mt/ap/support.h) \
    $(wildcard include/config/proc/sysctl.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/bridge/ebt/nflog.h) \
    $(wildcard include/config/utt/lan/ifname.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/rt3xxx/ohci.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/uttMachineAllMacro.h \
  networking/udhcp_multi/dhcpc.h \
    $(wildcard include/config/ture/udhcp/port.h) \
    $(wildcard include/config/feature/udhcp/port.h) \
    $(wildcard include/config/feature/udhcpc/arping.h) \
  networking/udhcp_multi/dhcpd.h \
    $(wildcard include/config/dhcpd/leases/file.h) \
  networking/udhcp_multi/options.h \
    $(wildcard include/config/feature/rfc3397.h) \
    $(wildcard include/config/ap.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/syslog.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/include/sys/syslog.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mib.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/cli/typedef.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/uttMachine.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibunion.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibunionShare.h \
    $(wildcard include/config/user/goahead/lang/zhcn.h) \
    $(wildcard include/config/user/goahead/lang/en.h) \
    $(wildcard include/config/user/goahead/lang/zhtw.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/autoconf.h \
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
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibtype.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibtypeShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/uttMachine.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibSystem.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibSystemShare.h \
    $(wildcard include/config/rtl8367m.h) \
    $(wildcard include/config/ralink/mt7620.h) \
    $(wildcard include/config/rtl8367rb.h) \
    $(wildcard include/config/ralink/mt7621.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibInterface.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibInterfaceShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibunionShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibtypeShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibUser.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibUserShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/uttSem.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/uttMachine.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/uttNfNetlink.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/ipsetApi.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibIpGroup.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibIpGroupShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibTimeGrp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibTimeGrpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibDhcpServer.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibDhcpServerShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibRoute.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibRouteShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibGroupBase.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibGroupBaseShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibpdb.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibpdbShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibnat.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibnatShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibAdminConf.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibAdminConfShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibDnsFilter.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibDnsFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibWireless.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibWirelessShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibNtp.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibNtpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibLang.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibLangShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibTaskSched.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibTaskSchedShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibDebug.h \
    $(wildcard include/config/utt/debug.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibEasyTask.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibEasyTaskShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibMacFilter.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibMacFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibAppMenu.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibAppMenuShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibShare.h \
    $(wildcard include/config/pppoe/srv/glo/ipcount.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/typedefShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibSystemShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibInterfaceShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibUserShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibIpGroupShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibTimeGrpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibDhcpServerShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibRouteShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibGroupBaseShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibpdbShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibnatShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibAdminConfShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibDnsFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibWirelessShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibNtpShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibLangShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibTaskSchedShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibEasyTaskShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibMacFilterShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibAppMenuShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/mibCloudConfigShare.h \
    $(wildcard include/config/share/h.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/mib/mibunion.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/profacce.h \
    $(wildcard include/config/flash/size.h) \
    $(wildcard include/config/buff/size.h) \
    $(wildcard include/config/header/len.h) \
    $(wildcard include/config/ap/flash/size.h) \
    $(wildcard include/config/ap/buff/size.h) \
    $(wildcard include/config/ze.h) \
    $(wildcard include/config/file/path.h) \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/conver.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/a2n.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/uttShareHead/config/profacceShare.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/ifmisc.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/comExeFun.h \
  /data3/wu.xiaoyu/BR_SOHO_189_GIT/MSG-WR211_CMCC/source/lib/libconfig/profacc/uttRtChipFun.h \
    $(wildcard include/config/utt/wanport/set.h) \
    $(wildcard include/config/utt/rdd/mirror.h) \
    $(wildcard include/config/utt/rdd/pvlan.h) \

networking/udhcp_multi/serverpacket.o: $(deps_networking/udhcp_multi/serverpacket.o)

$(deps_networking/udhcp_multi/serverpacket.o):
