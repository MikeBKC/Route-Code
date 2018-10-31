/** @file

  Some small general interest definitions. The general standard is to
  prefix these defines with TS_.

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef _ink_config_h
#define	_ink_config_h

// Note: All "defines" should be prefixed with TS_ when appropriate, please
// don't use ATS_ any more.

/* GENERATED FILE WARNING!  DO NOT EDIT ink_config.h
 *
 * You must modify ink_config.h.in instead.
 *
 */

/* Include automake generated defines
 */
#include "ink_autoconf.h"

#define BUILD_MACHINE                  "localhost.localdomain"
#define BUILD_PERSON                   "brian"
#define BUILD_GROUP                    "brian"

/* Includes */
#define TS_HAVE_SYS_EPOLL_H            1
#define TS_HAVE_SYS_EVENT_H            0
#define TS_HAVE_MACHINE_ENDIAN_H       0
#define TS_HAVE_ENDIAN_H               1
#define TS_HAVE_NETINET_IN_H           1
#define TS_HAVE_NETINET_IN_SYSTM_H     1
#define TS_HAVE_NETINET_TCP_H          1
#define TS_HAVE_SYS_IOCTL_H            1
#define TS_HAVE_SYS_BYTEORDER_H        0
#define TS_HAVE_SYS_SOCKIO_H           0
#define TS_HAVE_SYS_SYSCTL_H           1
#define TS_HAVE_SYS_SYSINFO_H          1
#define TS_HAVE_SYS_SYSTEMINFO_H       0
#define TS_HAVE_ARPA_INET_H            1
#define TS_HAVE_ARPA_NAMESER_H         1
#define TS_HAVE_ARPA_NAMESER_COMPAT_H  1
#define TS_HAVE_EXECINFO_H             1
#define TS_HAVE_NETDB_H                1
#define TS_HAVE_CTYPE_H                1
#define TS_HAVE_SIGINFO_H              0
#define TS_HAVE_MALLOC_H               1
#define TS_HAVE_WAIT_H                 1
#define TS_HAVE_FLOAT_H                1
#define TS_HAVE_LIBGEN_H               1
#define TS_HAVE_VALUES_H               1
#define TS_HAVE_ALLOCA_H               1
#define TS_HAVE_CPIO_H                 1
#define TS_HAVE_STROPTS_H              1
#define TS_HAVE_SYS_MOUNT_H            1
#define TS_HAVE_SYS_PARAM_H            1
#define TS_HAVE_SYS_SYSMACROS_H        1
#define TS_HAVE_MATH_H                 1
#define TS_HAVE_NET_PPP_DEFS_H         1
#define TS_HAVE_NETINET_IP_H           1
#define TS_HAVE_NETINET_IP_ICMP_H      1
#define TS_HAVE_EXECINFO_H             1
#define TS_HAVE_IFADDRS_H              1
#define TS_HAVE_READLINE_READLINE_H    0

/* Libraries */
#define TS_HAS_LIBZ                    1
#define TS_HAS_LZMA                    0
#define TS_HAS_EXPAT                   1
#define TS_HAS_JEMALLOC                0
#define TS_HAS_TCMALLOC                0


/* Features */
#define TS_HAS_EVENTFD                 0
#define TS_HAS_CLOCK_GETTIME           0
#define TS_HAS_POSIX_MEMALIGN          1
#define TS_HAS_POSIX_FADVISE           1
#define TS_HAS_LRAND48_R               1
#define TS_HAS_SRAND48_R               1
#define TS_HAS_STRLCPY                 0
#define TS_HAS_STRLCAT                 0
#define TS_HAS_IN6_IS_ADDR_UNSPECIFIED 1

#define TS_HAS_BACKTRACE               1
#define TS_HAS_PROFILER                0
#define TS_USE_FAST_SDK                0
#define TS_USE_DIAGS                   1
#define TS_USE_EPOLL                   1
#define TS_USE_KQUEUE                  0
#define TS_USE_PORT                    0
#define TS_USE_POSIX_CAP               0
#define TS_USE_TPROXY                  0
#define TS_HAS_SO_MARK                 1
#define TS_HAS_IP_TOS                  1
#define TS_USE_HWLOC                   0
#define TS_USE_FREELIST                1
#define TS_USE_TLS_NPN                 0
#define TS_USE_TLS_SNI                 0

/* OS API definitions */
#define GETHOSTBYNAME_R_HOSTENT_DATA   0
#define GETHOSTBYNAME_R_GLIBC2         1
#define NEED_UNION_SEMUN               1
#define SIZEOF_VOID_POINTER            4
#define TS_IP_TRANSPARENT              0

/* API */
#define TS_IS_MICRO_BUILD              0
#define TS_HAS_STANDALONE_IOCORE       0
#define TS_HAS_INKAPI                  1
#define TS_HAS_DEMANGLE                0
#define TS_HAS_TESTS                   1
#define TS_HAS_WCCP                    0

/* XXX: Should make those individually selectable ? */
/* TODO: Use TS prefix instead no prefix or INK    */
#if TS_IS_MICRO_BUILD
# define TS_MICRO                       1
# define INK_NO_ACL                     1
# define INK_NO_CLUSTER                 1
# define TS_USE_DIAGS                   0
# define INK_NO_HOSTDB                  1
# define INK_NO_ICP                     1
# define INK_NO_LOG                     1
# define INK_NO_REVERSE                 1
# define INK_NO_SOCKS                   1
# define INK_NO_STAT_PAGES              1
# define INK_USE_MUTEX_FOR_ATOMICLISTS  1
# define USE_MD5_FOR_MMH                1
#endif

# define TS_MAX_THREADS_IN_EACH_THREAD_TYPE  3072
# define TS_MAX_NUMBER_EVENT_THREADS  4096

# define TS_ARG_MAX	                2621440
# define TS_ARG_MAX_STR                 "2621440"

# define TS_MAX_HOST_NAME_LEN           256


#if TS_HAS_INKAPI
#define TS_MAX_API_STATS               512
/* XXX: Should make those individually selectable ? */
#else
# define TS_NO_TRANSFORM                1
# define TS_NO_API                      1
#endif

#if TS_HAS_STANDALONE_IOCORE
# define STANDALONE_IOCORE              1
#else
# define FIXME_NONMODULAR               1
# define SPLIT_DNS                      1
# define NON_MODULAR                    1
# define HTTP_CACHE                     1
#endif

/* Defaults for user / group */
#define TS_PKGSYSUSER                   "nobody"
#define TS_PKGSYSGROUP                  "nobody"

/* Various "build" defines */
#define TS_BUILD_PREFIX "/usr"
#define TS_BUILD_EXEC_PREFIX ""
#define TS_BUILD_BINDIR "bin"
#define TS_BUILD_SBINDIR "sbin"
#define TS_BUILD_SYSCONFDIR "/etc"
#define TS_BUILD_SYSCONFIGDIR "/etc"
#define TS_BUILD_DATADIR "share"
#define TS_BUILD_INCLUDEDIR "include"
#define TS_BUILD_LIBDIR "lib"
#define TS_BUILD_LIBEXECDIR "lib"
#define TS_BUILD_LOCALSTATEDIR "/var"
#define TS_BUILD_RUNTIMEDIR "/var/trafficserver"
#define TS_BUILD_LOGDIR "/var/log/trafficserver"
#define TS_BUILD_MANDIR "man"
#define TS_BUILD_CACHEDIR "/var/trafficserver"
#define TS_BUILD_INFODIR "info"

#define TS_BUILD_DEFAULT_LOOPBACK_IFACE "lo"

#endif /* _ink_config_h */
