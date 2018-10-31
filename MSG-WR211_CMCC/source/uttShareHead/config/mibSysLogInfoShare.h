#ifndef MIBSYSLOGINFO_SHARE_H
#define MIBSYSLOGINFO_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

/*
 *
 * ϵͳ��־��Ϣ�ṹ��
 * NoticeLogEnable - ͨ����־����
 * ArpLogEnable - ARP��־����
 * DhcpLogEnable - DHCP��־����
 */
typedef struct st_sysLogInfoProfile
{ 
    mibHead head;
#ifdef CONFIG_UTT_DHCP_COLLISION
    LogEnableEnum DhcpLogEnable;
#endif
#ifdef CONFIG_UTT_NOTICE_LOG
    LogEnableEnum NoticeLogEnable;
#endif
#ifdef CONFIG_UTT_ARP_COLLISION
    LogEnableEnum ArpLogEnable;
#endif
    LogEnableEnum PppoeLogEnable;
} SysLogInfoProfile;
#endif
