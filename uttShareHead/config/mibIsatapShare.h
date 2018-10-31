#ifndef MIBISATAP_SHARE_H
#define MIBISATAP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_ISATAP == FYES)
typedef struct ipv6_st_isatap {
    mibHead head;
    int ISATAPEnabled;   /*���ÿ���*/
    char ISATAP_IPv6Pre[40];  /*IPv6ǰ׺*/
    IPADDR ISATAP_IPv4Gat;  /*IPv4����*/
}IsatapProfile;
#endif
#endif
