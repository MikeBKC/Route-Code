/* 解决编译过程中lib/linux/switch/switch.h 和 lib/include/termios.h 中 speed_t 冲突问题 */
#ifndef _UDHCPD_COMPILE_CONFLICT_H_
#define _UDHCPD_COMPILE_CONFLICT_H_

#define FEATURE_PORT_VLAN FNO
#define FEATURE_SW_PORT_MIRROR FNO
#define FEATURE_SW_PORT_TRUNK FNO
#define FEATURE_ACL FNO
#endif
