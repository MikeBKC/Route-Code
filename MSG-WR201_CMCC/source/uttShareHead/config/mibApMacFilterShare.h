#ifndef _H_MIBAPMACFILTER_SHARE_H_ 
#define _H_MIBAPMACFILTER_SHARE_H_

#include "mibunionShare.h"
#include "mibtypeShare.h"

#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))

#define MAX_MAC_FILTER_SIGNAL_AP 32
#define MAX_AP_MAC_FILTER_NUM 10
#define MAX_MAC_BLACK_NUM MAX_MAC_FILTER_SIGNAL_AP
typedef struct st_ap_mac_filter_profile
{
	mibHead head;
	char  sz[MAX_AP_MAC_FILTER_NUM][UTT_INST_NAME_LEN+1];
    UINT32 mode;  // 0关闭 1允许 2禁止
    UINT32 num;   // MAC过滤条数
    MacAddr mac[MAX_MAC_BLACK_NUM];
} ApMacFilterProfile;

#endif

#endif
