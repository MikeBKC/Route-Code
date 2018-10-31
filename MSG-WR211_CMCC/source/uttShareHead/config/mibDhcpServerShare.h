#ifndef MIBDHCPSERVER_SHARE_H
#define MIBDHCPSERVER_SHARE_H

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if (DHCP_SERVER == FYES)

typedef enum en_dhcpOption43Type{
    DHCP_OPTION_43_TYPE_DISABLE = 0,
    DHCP_OPTION_43_TYPE_HEX,
    DHCP_OPTION_43_TYPE_ASCII,
    DHCP_OPTION_43_TYPE_COSTUM
}DhcpOption43Type;
#define DHCP_OPTION_43_LEN 70

typedef struct st_dhcpServer_profile {
    mibHead head;
    ProfileEnableEnum enable;
    IPADDR ipStart;
    IPADDR ipEnd;
    IPADDR netmask;
    IPADDR gateway;
    Uint32 lease;
    IPADDR pDns;
    IPADDR sDns;
    ProfileEnableEnum DnsProxyEn;
    IPADDR pPDns;
    IPADDR sPDns;
#if (DHCP_SERVER_OPTION_43 == FYES)
    int    option43Type;
    char   option43Addr[DHCP_OPTION_43_LEN+1];
    char   option43Addr2[DHCP_OPTION_43_LEN+1];
#endif
#if (DHCP_AUTO_BIND == FYES)
    FunEnableEnum DhcpNewAutobind;
    FunEnableEnum DhcpDelAutobind;
#endif
#if (MULTI_SSID_DHCP_POOL == FYES)
    ProfileEnableEnum ssid1_dhcp_enable;
    IPADDR ssid1_ipStart;
    IPADDR ssid1_ipEnd;

    ProfileEnableEnum ssid2_dhcp_enable;
    IPADDR ssid2_ipStart;
    IPADDR ssid2_ipEnd;
#if (WIRELESS_5G == FYES)
    ProfileEnableEnum ssid1_dhcp_enable_5g;
    IPADDR ssid1_ipStart_5g;
    IPADDR ssid1_ipEnd_5g;

    ProfileEnableEnum ssid2_dhcp_enable_5g;
    IPADDR ssid2_ipStart_5g;
    IPADDR ssid2_ipEnd_5g;
#endif
#endif
}DhcpServerProfile;

#if (DHCP_AUTO_BIND == FYES)
#define UTT_BIND_INTERVAL_INIT 60
struct DhcpAutoBindProf {
    unsigned char hostname[16];
    unsigned char mac[16];
    unsigned int ip;
    unsigned int expires;
};

#endif
#endif

#endif
