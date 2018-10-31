#ifndef MIBNETSHAREMANAGE_SHARE_H
#define MIBNETSHAREMANAGE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"


#define NET_SHARE_FILE_PATH_LEN 128
#if (NET_SHARE_MANAGE == FYES)
typedef struct st_NetShareManage_Profile {
    mibHead head;
    FunEnableEnum GlbEn;
    FunEnableEnum need_passwd;
    char smbEnable[MAX_STORAGE_NUM];
#if (UTT_SMART_UI == FYES)
    FunEnableEnum netShareEn;
    FunEnableEnum sambaEn;
    FunEnableEnum ftpEn;
    FunEnableEnum allow_wan_visit;
    int change;/*”√¿¥¥•∑¢profchange*/
    char dirName[32];
    char net_share_file_path[NET_SHARE_FILE_PATH_LEN];
#endif
} NetShareManageProfile;
#endif

#endif
