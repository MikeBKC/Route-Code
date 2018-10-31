#ifndef MIBUSER_SHARE_H
#define MIBUSER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*arp查找匹配结果*/
typedef enum
{
    MATCH_NONE=0,/*ip mac都不匹配*/
    MATCH_IP_ONLY,/*仅仅匹配ip*/
    MATCH_MAC_ONLY,/*仅仅匹配mac*/
    MATCH_ALL/*ip mac全匹配*/
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
