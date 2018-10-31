#ifndef MIBCAPWAPCONFIGSHARE_H
#define MIBCAPWAPCONFIGSHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (CAPWAP_CONFIG_AP == FYES)
#define MAX_CAPWAP_AC_HOSTNAME 32

typedef struct st_ap_capwap_config_profile
{
    mibHead head;
    IPADDR pd;		/*��DNS*/
    IPADDR sd;		/*��DNS*/
    char acHostInfo[3][MAX_CAPWAP_AC_HOSTNAME];		/*��¼ac��ַ�����Ϣ,IP��ַ��������*/
}ApCapwapConfigProfile;
#endif

#endif
