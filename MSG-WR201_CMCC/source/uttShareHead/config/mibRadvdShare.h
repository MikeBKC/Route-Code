#ifndef MIBRADVD_SHARE_H
#define MIBRADVD_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "typedefShare.h"

#if (IPV6_RADVD == FYES)
typedef struct ipv6_st_radvd {
    mibHead head;
    char DesPrefix[40];   /*通告前缀*/
    int manageEnabled;
    int otherEnabled;
    int DesPrefixLength;  /*通告前缀长度*/
    int PrefixstaleTime;  /*前缀生命周期*/
    int noticeInterval;//间隔通告
    int routerPreferences;//路由器首选项
    int MTU_config;//MTU配置
    int routerLifetime;//路由器生命期
    }RadvdProfile;
#endif
#endif
