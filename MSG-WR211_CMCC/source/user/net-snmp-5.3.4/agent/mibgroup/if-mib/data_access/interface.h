/*
 * interface data access header
 *
 * $Id: interface.h,v 1.1 2013/08/09 17:11:23 hua.yuye Exp $
 */
#ifndef NETSNMP_ACCESS_INTERFACE_CONFIG_H
#define NETSNMP_ACCESS_INTERFACE_CONFIG_H

/**---------------------------------------------------------------------*/
/*
 * configure required files
 *
 * Notes:
 *
 * 1) prefer functionality over platform, where possible. If a method
 *    is available for multiple platforms, test that first. That way
 *    when a new platform is ported, it won't need a new test here.
 *
 * 2) don't do detail requirements here. If, for example,
 *    HPUX11 had different reuirements than other HPUX, that should
 *    be handled in the *_hpux.h header file.
 */
config_require(if-mib/data_access/interface)
#if defined( linux )
config_require(if-mib/data_access/interface_linux)
config_require(if-mib/data_access/interface_ioctl)
#else
#   define NETSNMP_ACCESS_INTERFACE_NOARCH 1
#endif

#endif /* NETSNMP_ACCESS_INTERFACE_CONFIG_H */
