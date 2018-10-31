#pragma once

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (DHCP_POOL == FYES)

typedef enum en_dhcpOption43Type{
    DHCP_OPTION_43_TYPE_DISABLE = 0,
    DHCP_OPTION_43_TYPE_HEX,
    DHCP_OPTION_43_TYPE_ASCII,
    DHCP_OPTION_43_TYPE_COSTUM
}DhcpOption43Type;
#define DHCP_OPTION_43_LEN 70

typedef struct st_dhcpPool_profile {
    mibHead head;
	ProfileEnableEnum dhcpEn;
	char interface[UTT_INTERFACE_LEN];
	Uint32 vid;
    IPADDR ipStart;
    IPADDR ipEnd;
    IPADDR netmask;
    IPADDR gateway;
    Uint32 lease;
    IPADDR pDns;
    IPADDR sDns;

#if (DHCP_SERVER_OPTION_43 == FYES)
    int    option43Type;
    char   option43Addr[DHCP_OPTION_43_LEN+1];
    char   option43Addr2[DHCP_OPTION_43_LEN+1];
#endif

}DhcpPoolProfile;

#endif
