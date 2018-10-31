#ifndef MIBLOGMANAGE_SHARE_H
#define MIBLOGMANAGE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"


/*
 *
 * ��Ϊ��ƽṹ��
 * user   -   ��ҳ��֤�û���
 * passwd -   ��ҳ��֤����
 * remark -   ��ҳ��֤����
 * ip	  -   ʹ�ô��˺ŵ�IP��0.0.0.0��Ϊ���˺�δʹ��
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
