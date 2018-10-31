#ifndef SNMPSHARE_H
#define SNMPSHARE_H
#if(SNMP == FYES)
#define MAX_SNMP_COMMUNITY_LEN 31
typedef struct st_snmp_info_profile {
    mibHead head;
	Uint32 snmp_enable;
    char community[MAX_SNMP_COMMUNITY_LEN];/*读社区名*/
    char dev_name[MAX_SNMP_COMMUNITY_LEN];/*设备名*/
    char contactor[MAX_SNMP_COMMUNITY_LEN];/*联系人*/
    char position[MAX_SNMP_COMMUNITY_LEN];/*位置*/
    Uint32 host_manage_enable;/*允许主机管理*/
    char host1[16];
    char host2[16];
    char host3[16];
} SnmpInfoProfile;
#endif
#endif
