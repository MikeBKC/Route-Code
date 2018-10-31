#ifndef MIBPOLICYROUTE_SHARE_H
#define MIBPOLICYROUTE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (POLICY_ROUTE == FYES)
#define PR_IPGRP_NAME_LEN 31 /*ip���Ա�����ȡ�������Ӧ���� ���� pppoe�û��� �Լ� web��֤�û��������еĽϳ���*/


/* ip type */
typedef enum{
    PR_IP_RANGE,
    PR_IP_GROUP
}prIpGrpType;
/* ip ��ַ�� */
typedef struct st_prIpRange {
    u_long ipFrom;	/*��ʼip*/
    u_long ipTo;	/*����ip*/
} prIpRange;
/*ip����Ŀ����*/
typedef struct st_pripGrpVal{
    prIpRange range;/*ip��*/
    char name[PR_IPGRP_NAME_LEN + 1];/*���� ����Ϊ������ �û���*/
} prIpGrpVal;

/*ip����Ŀ*/
typedef struct st_ipgrpEntry{
    prIpGrpType entryType;/*����*/
    prIpGrpVal  entryVal;/*ֵ*/
} prIpGrpEntry;

/*����·�ɻ���IP��ַ*/
typedef struct st_prIpGrp {
	prIpGrpEntry srcIP;
	prIpGrpEntry dstIP;
} prIpGrp;

/*����·�ɻ���Ӧ��*/
typedef struct st_prApply {
    unsigned int protocol;//Э��
    unsigned short dstFromPort; //Ŀ����ʼ�˿�
    unsigned short dstEndPort;	//Ŀ�Ľ����˿�
} prApply;

/*������Чʱ��*/
typedef struct st_prTime{
    u_long wday;/*���ڼ�*/
    char timestart_str[UTT_TIME_LEN];
    char timestop_str[UTT_TIME_LEN];
    u_long timestart;/*24*60*60.һ�����86400��*/
    u_long timestop;
}prTime;
/* policy route value */
typedef struct st_policyRouteProfile {
    mibHead head;
    char interfaceName[UTT_INTERFACE_LEN + 1];
    int bindMark;			/* �󶨽ӿ�mark */
    prIpGrp ip;				/* ����IP */
    prApply apply;			/* ����Ӧ�� */
    prTime prtime;			/* ��Чʱ�� */
} PolicyRouteProfile;
#endif
#endif
