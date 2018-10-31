#ifndef MIBSYSLOG_SHARE_H
#define MIBSYSLOG_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

/*
 * SYSLOG结构体
 * SyslogIp   -   服务器ip或者域名
 * SyslogPort -   服务器端口
 * SyslogType -   消息类别
 * SyslogInterval -   消息发送间隔
 */

#define MAX_DOMAIN_NAME_LEN 71
#define SYSLOG_TYPE_LEN 6
typedef struct st_SyslogProfile
{
    mibHead head;
    char    SyslogIp[MAX_DOMAIN_NAME_LEN + 1];
    Uint32  SyslogPort;
    char    SyslogType[SYSLOG_TYPE_LEN + 1];
#if (SYSLOG_HEARTBEAT == FYES)
    Uint32  SyslogInterval;
#endif
} SyslogProfile;
#endif
