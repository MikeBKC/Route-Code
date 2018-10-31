#ifndef MIB_PORT_GROUP_SHARE_H
#define MIB_PORT_GROUP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (PORT_GRP == FYES)

#define PORTGRP_ENTRYS 10
#define UTT_PORTGRP_NAME_LEN 31
#define MAX_PORTGRP_DEP 2


#define STATIC_TCPSERV_NUM 18
#define STATIC_UDPSERV_NUM 12
#define STATIC_OTHERSERV_NUM 9
#define MAX_PORTGROUP_NAME_LENGTH 11

#define STATIC_SER_NUM (STATIC_TCPSERV_NUM+STATIC_UDPSERV_NUM+STATIC_OTHERSERV_NUM) 

static const char tcpserver[STATIC_SER_NUM][MAX_PORTGROUP_NAME_LENGTH+1]={
	"ftp","ssh","telnet","smtp","sql*net","finger","web","pop3","epmap","netbios_ssn","https","ms-ds","ms-sql-s","ms-sql-m","h.323","pptp","msnlogin","ms-ts",
	"dhcp", "dns","bootps","bootpc","tftp","ntp","netbios-ns","netbios-dgm","snmp","snmptrap","isakmp","l2tp",
	"icmp","igmp","ipinip","gre","esp","ah","ospf","igrp","rsvp"
};
static const unsigned short int port_start[STATIC_SER_NUM]={
	20,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389, /*  18 ,表示Tcp端口 */
	67, 53,67,68,69,123,137,138,161,162,500,1701,/* 12	,表示Udp端口*/
	1,2,4,47,50,51,89,9,46/*	9 ,表示协议号	*/
};


static const unsigned short int port_end[STATIC_SER_NUM]={
	21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389, /*  18  */
	68, 53,67,68,69,123,137,138,161,162,500,1701,/* 12	*/
	1,2,4,47,50,51,89,9,46/*	9 	*/
};

/*tcp或者udp端口组*/
typedef struct st_portRange{
    unsigned short ssport;
    unsigned short seport;
    unsigned short dsport;
    unsigned short deport;
}PortRange;

/*系统预留服务组*/
typedef struct st_sysusedGrp{
    char  sysGrpName[UTT_PORTGRP_NAME_LEN + 1];  // 系统预留服务组名称
    PortRange  portRange;//服务组端口组
   unsigned short protocol; 
}sysUsedGrp;

/*服务组条目值*/
typedef union{
    PortRange      portRange;  /* 端口段 */
    unsigned short protocol;    /* 其它协议*/
    sysUsedGrp     sysGrp;     /*  系统预留服务组 */
    char grpName[UTT_PORTGRP_NAME_LEN + 1];/* 如果是子组，此处为其组名称 */
    char content[UTT_PORTGRP_NAME_LEN + 1];/* 如果是URL，DNS，MAC等则为其内容*/
}portGrpEntryVal;

typedef enum{
    PORTENT_NONE = 0,/*该条目没用*/

    PORTENT_NORMAL,

    PORTENT_TCP,
    PORTENT_UDP,

    PORTENT_OTHER,

    PORTENT_SYS,
    PORTENT_GRP/*子组*/
}portGrpEntryType;

/*服务组里面的一个条目*/
typedef struct st_portGrpEntry{
    //  如下，左边为类型，右边为相应类型对于服务组条目值
    //  类型     服务组条目值
    //  子组          子组服务组名称（如：test ）
    //  系统预留      系统预留服务名称（如：sys_ftp）及其它属性
    //  TCP节点      端口组
    //  UDP节点      端口组
    //  其它协议      协议号
    //  字符串        URL、DNS、MAC、关键字等的内容
    portGrpEntryType entryType;    /*类型*/
    portGrpEntryVal  entryVal;     /*服务组条目值，定义如下面*/
} portGrpEntry;

/*服务组类型*/
typedef enum{
    PORTGRP_PORT,
    PORTGRP_URL,
    PORTGRP_KEY,
    PORTGRP_DNS,
    PORTGRP_MAC
}portGrpType;
/*实例。即一个服务组*/
typedef struct st_portgroup_profile {
    mibHead       head;
    //服务组类型：普通服务, URL，DNS，MAC，关键字
    portGrpType   grpType; 
    //服务组条目，定义如下面
    portGrpEntry  portEntry[PORTGRP_ENTRYS];
} PortGrpProfile; 
#endif
#endif
