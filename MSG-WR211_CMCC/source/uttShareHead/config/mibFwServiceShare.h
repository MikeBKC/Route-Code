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
	20,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389, /*  18 ,��ʾTcp�˿� */
	67, 53,67,68,69,123,137,138,161,162,500,1701,/* 12	,��ʾUdp�˿�*/
	1,2,4,47,50,51,89,9,46/*	9 ,��ʾЭ���	*/
};


static const unsigned short int port_end[STATIC_SER_NUM]={
	21,22,23,25,66,79,80,110,135,139,443,445,1433,1434,1720,1723,1863,3389, /*  18  */
	68, 53,67,68,69,123,137,138,161,162,500,1701,/* 12	*/
	1,2,4,47,50,51,89,9,46/*	9 	*/
};

/*tcp����udp�˿���*/
typedef struct st_portRange{
    unsigned short ssport;
    unsigned short seport;
    unsigned short dsport;
    unsigned short deport;
}PortRange;

/*ϵͳԤ��������*/
typedef struct st_sysusedGrp{
    char  sysGrpName[UTT_PORTGRP_NAME_LEN + 1];  // ϵͳԤ������������
    PortRange  portRange;//������˿���
   unsigned short protocol; 
}sysUsedGrp;

/*��������Ŀֵ*/
typedef union{
    PortRange      portRange;  /* �˿ڶ� */
    unsigned short protocol;    /* ����Э��*/
    sysUsedGrp     sysGrp;     /*  ϵͳԤ�������� */
    char grpName[UTT_PORTGRP_NAME_LEN + 1];/* ��������飬�˴�Ϊ�������� */
    char content[UTT_PORTGRP_NAME_LEN + 1];/* �����URL��DNS��MAC����Ϊ������*/
}portGrpEntryVal;

typedef enum{
    PORTENT_NONE = 0,/*����Ŀû��*/

    PORTENT_NORMAL,

    PORTENT_TCP,
    PORTENT_UDP,

    PORTENT_OTHER,

    PORTENT_SYS,
    PORTENT_GRP/*����*/
}portGrpEntryType;

/*�����������һ����Ŀ*/
typedef struct st_portGrpEntry{
    //  ���£����Ϊ���ͣ��ұ�Ϊ��Ӧ���Ͷ��ڷ�������Ŀֵ
    //  ����     ��������Ŀֵ
    //  ����          ������������ƣ��磺test ��
    //  ϵͳԤ��      ϵͳԤ���������ƣ��磺sys_ftp������������
    //  TCP�ڵ�      �˿���
    //  UDP�ڵ�      �˿���
    //  ����Э��      Э���
    //  �ַ���        URL��DNS��MAC���ؼ��ֵȵ�����
    portGrpEntryType entryType;    /*����*/
    portGrpEntryVal  entryVal;     /*��������Ŀֵ������������*/
} portGrpEntry;

/*����������*/
typedef enum{
    PORTGRP_PORT,
    PORTGRP_URL,
    PORTGRP_KEY,
    PORTGRP_DNS,
    PORTGRP_MAC
}portGrpType;
/*ʵ������һ��������*/
typedef struct st_portgroup_profile {
    mibHead       head;
    //���������ͣ���ͨ����, URL��DNS��MAC���ؼ���
    portGrpType   grpType; 
    //��������Ŀ������������
    portGrpEntry  portEntry[PORTGRP_ENTRYS];
} PortGrpProfile; 
#endif
#endif
