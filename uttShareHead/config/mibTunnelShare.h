#ifndef MIBTUNNEL_SHARE_H
#define MIBTUNNEL_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_TUNNEL == FYES)
typedef struct ipv6_st_tunnel {
    mibHead head;
    int tunnelEnabled;   /*���ÿ���*/
    IPADDR local_ip4_addr;  /*����IPv4��ַ*/
    IPADDR remote_ip4_addr;  /*Զ��IPv4��ַ*/
    char remote_ip6_addr[40]; /*Զ��IPv6��ַ*/
}TunnelProfile;
#endif
#endif
