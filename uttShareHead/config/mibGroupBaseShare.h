#include "uttMachine.h"
#if (GROUP_BASE == FYES)
#ifndef MIBGROUPBASE_SHARE_H
#define MIBGROUPBASE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define L7PROTO_PDB_CHAIN "l7proto_pdb"
#define PORT_DNS_PDB_CHAIN "port_dns_pdb_forward" 
#define DNS_PDB_CHAIN_LOOP "dns_pdb_input_loop"
#define GROUP_LIMIT_CHAIN       "group_limit"
#define GROUP_LIMIT_PRE_CHAIN "group_limit_pre"
#define GROUP_LIMIT_POST_CHAIN "group_limit_post"

typedef enum{
  GBT_FINE_LIMIT = 0,/*��ϸ������*/
  GBT_ACT_CONTROL
} GroupBaseType;

#if (MACADDR_ADD_LIMIT== FYES)
typedef enum{
    EXCLUSIVE_RATE = 1,/*�������*/
    SHARE_RATE = 2, /*�������*/
    MAC_LIMIT_RATE = 3
}RATE_LIMIT_POLICY;
#else
typedef enum{
    EXCLUSIVE_RATE = 1,/*�������*/
    SHARE_RATE = 2 /*�������*/
}RATE_LIMIT_POLICY;


#endif
typedef struct st_groupBase_profile
{
    mibHead head;
    GroupBaseType groupType;
    addrObjTypeEn addrTypeEn;/*��ַ��������*/
    addrObjTypeEn dst_addrTypeEn;/*Ŀ�ĵ�ַ��������*/
    int ProfIndex;
    IPADDR ipStart;
    IPADDR ipEnd;
    IPADDR dst_ipStart;
    IPADDR dst_ipEnd;
#if (MACADDR_ADD_LIMIT== FYES)
    MacAddr  mac_addr;
#endif

#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*��ַ������*/
    char dst_ipGrpName[UTT_INST_NAME_LEN + 1u]; /*1.7.4Ŀ�ĵ�ַ��*/
#endif
    UINT32 day;
    char timeStart[UTT_TIME_LEN];
    unsigned long sTimeStart;/*ת��Ϊ����ʱ�䡣�ñ����������á�ֻ��Ϊ�˼����ں����ɲ���*/
    char timeStop[UTT_TIME_LEN];
    unsigned long sTimeStop;/*ת��Ϊ����ʱ��*/
    Uchar pdbPolicy[UTT_MAX_GROUP_PDBPOLICY];
    Uchar pdbPolicy1[UTT_MAX_PDB_POLICY]; //��bitλ��pdb����

    RATE_LIMIT_POLICY limitPolicy;/*���ٲ���*/

    Uint32 uRate;
    Uint32 dRate;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    int appVersion;/*Ӧ�ð汾�����ڲ��Ը��¿���*/
#endif
    
}GroupBaseProfile;


#endif
#endif
