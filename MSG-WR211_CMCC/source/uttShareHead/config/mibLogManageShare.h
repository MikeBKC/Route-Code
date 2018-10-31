#ifndef MIBLOGMANAGE_SHARE_H
#define MIBLOGMANAGE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"


/*
 *
 * 行为审计结构体
 * user   -   网页认证用户名
 * passwd -   网页认证密码
 * remark -   网页认证描述
 * ip	  -   使用此账号的IP，0.0.0.0认为此账号未使用
 */
typedef struct st_logManageProfile 
{ 
    mibHead head;
    LogEnableEnum webLogEnable;
    LogEnableEnum QQLogEnable;
    LogEnableEnum MSNLogEnable;
    LogEnableEnum mailLogEnable;
    LogEnableEnum eimLogEnable;
#if CONFIG_XPORT_LOG 
    LogEnableEnum luntanLogEnable;
    LogEnableEnum weiboLogEnable;
    LogEnableEnum NatLogEnable;
#endif
} LogManageProfile;
#endif
