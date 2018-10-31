#if (NET_SNIPER == FYES)
#ifndef MIBNETSNIPER_SHARE_H
#define MIBNETSNIPER_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define SNIPER_NOTICE "sniper_notice"
#define SNIPER_NOTICE_CHAIN_R "sniper_notice_chain_r"
#define SNIPER_NOTICE_R_GRP IPSET_SYS_GNAME_PRE"notice_sniper_r"


typedef struct st_netSniper_profile
{
    mibHead head;
#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
#endif
    Uint32 ctlTime;/*管制时间*/
    Uint32 linearIntvl;/*id线性间隔*/
	Uint32 chkIntvl;/*ip_id检查间隔*/
	Uint32 chkCnt;/*ip_id检查次数*/
    
}NetSniperProfile;


#endif
#endif
