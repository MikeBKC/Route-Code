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
  GBT_FINE_LIMIT = 0,/*精细化限速*/
  GBT_ACT_CONTROL
} GroupBaseType;

#if (MACADDR_ADD_LIMIT== FYES)
typedef enum{
    EXCLUSIVE_RATE = 1,/*独享带宽*/
    SHARE_RATE = 2, /*共享带宽*/
    MAC_LIMIT_RATE = 3
}RATE_LIMIT_POLICY;
#else
typedef enum{
    EXCLUSIVE_RATE = 1,/*独享带宽*/
    SHARE_RATE = 2 /*共享带宽*/
}RATE_LIMIT_POLICY;


#endif
typedef struct st_groupBase_profile
{
    mibHead head;
    GroupBaseType groupType;
    addrObjTypeEn addrTypeEn;/*地址对象类型*/
    addrObjTypeEn dst_addrTypeEn;/*目的地址对象类型*/
    int ProfIndex;
    IPADDR ipStart;
    IPADDR ipEnd;
    IPADDR dst_ipStart;
    IPADDR dst_ipEnd;
#if (MACADDR_ADD_LIMIT== FYES)
    MacAddr  mac_addr;
#endif

#if (IP_GRP == FYES)
    char ipGrpName[UTT_INST_NAME_LEN + 1u];/*地址组名称*/
    char dst_ipGrpName[UTT_INST_NAME_LEN + 1u]; /*1.7.4目的地址组*/
#endif
    UINT32 day;
    char timeStart[UTT_TIME_LEN];
    unsigned long sTimeStart;/*转换为秒后的时间。该变量不存配置。只是为了加速内核生成策略*/
    char timeStop[UTT_TIME_LEN];
    unsigned long sTimeStop;/*转换为秒后的时间*/
    Uchar pdbPolicy[UTT_MAX_GROUP_PDBPOLICY];
    Uchar pdbPolicy1[UTT_MAX_PDB_POLICY]; //按bit位存pdb策略

    RATE_LIMIT_POLICY limitPolicy;/*限速策略*/

    Uint32 uRate;
    Uint32 dRate;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    int appVersion;/*应用版本，用于策略更新控制*/
#endif
    
}GroupBaseProfile;


#endif
#endif
