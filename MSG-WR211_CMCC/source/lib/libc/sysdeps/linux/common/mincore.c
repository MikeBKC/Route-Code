/* 
 * Distributed under the terms of the GNU General Public License v2
 * $Header: /home/cvs/3520G/lib/libc/sysdeps/linux/common/mincore.c,v 1.1.1.1 2011/08/17 11:11:59 brwang Exp $
 *
 * This file provides the mincore() system call to uClibc.
 * 20041215 - <solar@gentoo.org>
 *
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR_mincore
_syscall3(int, mincore, void *, start, size_t, length, unsigned char *, vec);
#endif
