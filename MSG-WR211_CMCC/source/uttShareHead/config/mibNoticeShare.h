#ifndef MIBNOTICE_SHARE_H
#define MIBNOTICE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define NOTICE_OK_GRP IPSET_SYS_GNAME_PRE"notice_ok"
#define NOTICE_WEB_GRP IPSET_SYS_GNAME_PRE"notice_web"
#define NOTICE_R_GRP IPSET_SYS_GNAME_PRE"notice_web_r"
#define NOTICE_CHAIN "notice_chain"
#define NOTICE_CHAIN_R "notice_chain_r"
#define PRE_NOTICE "pre_notice"
#define NOTICE_IP_LEN 15
#define NOTICE_DATE_LEN 10
#define UTT_URL_LEN 63
#define UTT_SKIP_TIME 3

typedef struct st_notice_profile {
    mibHead head;
    ProfWebSubmitEnum webSubmitType;
    IPADDR ipstart;
    IPADDR ipend;
    UINT32 day;
    char timestart[UTT_TIME_LEN];
    char timestop[UTT_TIME_LEN];
    Uint32 remainDays;
    char datestart[NOTICE_DATE_LEN + 1];
    char datestop[NOTICE_DATE_LEN + 1];
}NoticeProfile;

#endif
