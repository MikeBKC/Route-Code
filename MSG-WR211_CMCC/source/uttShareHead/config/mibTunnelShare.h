#ifndef MIBTUNNEL_SHARE_H
#define MIBTUNNEL_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_TUNNEL == FYES)
typedef struct ipv6_st_tunnel {
    mibHead head;
    int tunnelEnabled;   /*启用开关*/
    IPADDR local_ip4_addr;  /*本地IPv4地址*/
    IPADDR remote_ip4_addr;  /*远程IPv4地址*/
    char remote_ip6_addr[40]; /*远程IPv6地址*/
}TunnelProfile;
#endif
#endif
