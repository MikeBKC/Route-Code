
#ifndef MIBDEBUG_H
#define MIBDEBUG_H
#include "mibunion.h"
#include "mibtype.h"
#include "uttMachine.h"
#define MAX_TTY_LEN 20

#ifdef CONFIG_UTT_DEBUG
#if ((NOTICE == FYES) && (DNS_FILTER == FYES))
#define DEBUG_DNS_NOTICE 0
#endif

#define DEBUG_GOAHEAD 0

#define DEBUG_SYSLOG 0

#if (DHCP_SERVER == FYES)
#define DEBUG_DHCP_COLLISION 1
#endif

typedef struct st_debugPara
{
    unsigned int printEn;
} DebugPara;
typedef struct st_debugProfile
{
#if ((NOTICE == FYES) && (DNS_FILTER == FYES))
    DebugPara dnsDebug;
#endif
#if (WEBAUTH_ACCOUNT == FYES)
    DebugPara webAuthAccDebug;
#endif
#if (POLICY_ROUTE == FYES)
    DebugPara policyrouteDebug;
#endif
#if (DEBUG_GOAHEAD == FYES)
    DebugPara goaheadDebug;
#endif
#if (DEBUG_DHCP_COLLISION == FYES)
    DebugPara dhcpCollisionDebug;
#endif
    char tty_str[MAX_TTY_LEN];
} DebugProfile;

#if DEBUG_GOAHEAD
#define goahead_debug_print(fmt, args...)    debug_print(&(nvramProfile->uttDebug.goaheadDebug), fmt, ## args)
#else
#define goahead_debug_print(fmt, args...)
#endif

#if (WEBAUTH_ACCOUNT == FYES)
#define DEBUG_WEBAUTH_ACC 0
#endif
#if (POLICY_ROUTE == FYES)
#define DEBUG_POLICY_ROUTE 0
#endif



#endif



#if DEBUG_WEBAUTH_ACC
#define webauth_acc_debug_print(fmt, args...)    debug_print(&(nvramProfile->uttDebug.webAuthAccDebug), fmt, ## args)
#else
#define webauth_acc_debug_print(fmt, args...)
#endif
#if DEBUG_POLICY_ROUTE
#define policy_route_debug_print(fmt, args...)    debug_print(&(nvramProfile->uttDebug.policyrouteDebug), fmt, ## args)
#else
#define policy_route_debug_print(fmt, args...)
#endif

#if DEBUG_DHCP_COLLISION
#define dhcp_collision_debug_print(fmt, args...)    debug_print(&(nvramProfile->uttDebug.dhcpCollisionDebug), fmt, ## args)
#else
#define dhcp_collision_debug_print(fmt, args...)
#endif
#endif
