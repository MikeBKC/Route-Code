#ifndef MIBSYSLOGINFO_SHARE_H
#define MIBSYSLOGINFO_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

/*
 *
 * 系统日志信息结构体
 * NoticeLogEnable - 通告日志开关
 * ArpLogEnable - ARP日志开关
 * DhcpLogEnable - DHCP日志开关
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
