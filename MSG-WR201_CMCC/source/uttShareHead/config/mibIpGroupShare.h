#ifndef MIB_IP_GROUP_SHARE_H
#define MIB_IP_GROUP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (IP_GRP == FYES)
/**
 * ip地址组是基于ip的组管理
 * 内容可以是ip 或 用户名
 * 为ip时直接展开到ipset组
 * 为用户名时。当该用户 上线 下线时更新相应的 ipset组
 */
#define UTT_IPGRP_NAME_LEN 31 /*ip组成员名长度。此名称应该是 组名 pppoe用户名 以及 web认证用户名长度中的较长者*/
#define MAX_IPGRP_DEP 2  /*ip组的最大嵌套 深度*/
#define IPGRP_IP_ENTRYS 10 /*IPGRP_IP类型组条目数*/
#define IPGRP_USER_ENTRYS 100 /*IPGRP_USER类型组条目数*/
#define IPGRP_ENTRYS 100 /*一个ip组包含的条目个数 为 IPGRP_IP 和 IPGRP_USER组条目中的较大者。*/
#define MAX_IPGRP_IP_NUM 65535 /*ip地址最大个数*/

/*ip地址组类型*/
typedef enum{
    IPGRP_IP,/*组里的内容为固定ip*/
    IPGRP_USER/*组的ip不为固定ip，会随用户名对应ip的变化而变化*/
}ipGrpType;

/*ip组条目类型*/
typedef enum{
    IPENT_NONE = 0,/*该条目没用*/

    /*start以下类型仅对应IPGRP_IP组类型*/
    IPENT_RANGE,/*ip地址段 */
    /*end*/

#if (PPPOE_SERVER == FYES)
    /*start以下类型仅对应IPGRP_USER组类型*/
    IPENT_POE_ACC,/*pppoe账号 */
#endif

#if (WEB_AUTH == FYES)
    IPENT_WEB_AUTH,/*web认证用户*/
#endif
    /*end*/

    IPENT_GRP/*子ip组*/
}ipGrpEntryType;

/*ip组范围*/
typedef struct st_ipRange{
    IPADDR ipFrom;
    IPADDR ipTo;
}ipRange;

/*ip组条目内容*/
typedef union{
    ipRange range;/*ip段*/
    char name[UTT_IPGRP_NAME_LEN + 1];/*名称 可能为组名或 用户名*/
}ipGrpEntryVal;

/*ip组条目*/
typedef struct st_ipGrpEntry{
    ipGrpEntryType entryType;/*类型*/
    ipGrpEntryVal  entryVal;/*值*/
} ipGrpEntry;

/*实例。即一个ip地址组*/
typedef struct st_ipGrpProfile{
    mibHead head;
    ipGrpType grpType;/*组类型*/
    ipGrpEntry ipEntry[IPGRP_ENTRYS];/*此处也可以动态分配*/
}IpGrpProfile;

#endif
#endif
