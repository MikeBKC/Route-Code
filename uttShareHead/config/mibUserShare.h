#ifndef MIBUSER_SHARE_H
#define MIBUSER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*arp����ƥ����*/
typedef enum
{
    MATCH_NONE=0,/*ip mac����ƥ��*/
    MATCH_IP_ONLY,/*����ƥ��ip*/
    MATCH_MAC_ONLY,/*����ƥ��mac*/
    MATCH_ALL/*ip macȫƥ��*/
} ARP_MATCH;

typedef struct st_user_ctl {
    mibHead head;
	char dhcpPoolName[UTT_DHCP_POOL_LEN];
    FunEnableEnum bindEn;
#if (DHCP_POOL == FYES)
	Uint32 vid;
#endif
    IPADDR ip;
    MacAddr mac;
#if (DHCP_AUTO_BIND == FYES)
    DhcpBindType bindType;
#endif
}UserProfile;

#endif
