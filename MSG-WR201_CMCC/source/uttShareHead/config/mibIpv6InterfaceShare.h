#ifndef MIBIPV6INTERFACE_SHARE_H
#define MIBIPV6INTERFACE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (IPV6_INTERFACE == FYES)

typedef struct ipv6_st_interface {
    mibHead head;
    char ipv6LAN_addr[40];
    int LANprefixLength;
    int LanAutoEnabled;
    int port;
    char ipv6WAN_addr[40];
    char *ipv6Gateway[40];
    int WANprefixLength;
    char ipv6Dns1[40];
    char ipv6Dns2[40];
    
}Ipv6InterfaceProfile;

#endif
#endif
