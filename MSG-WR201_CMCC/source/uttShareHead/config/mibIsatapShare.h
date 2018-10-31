#ifndef MIBISATAP_SHARE_H
#define MIBISATAP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_ISATAP == FYES)
typedef struct ipv6_st_isatap {
    mibHead head;
    int ISATAPEnabled;   /*启用开关*/
    char ISATAP_IPv6Pre[40];  /*IPv6前缀*/
    IPADDR ISATAP_IPv4Gat;  /*IPv4网关*/
}IsatapProfile;
#endif
#endif
