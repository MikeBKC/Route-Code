#ifndef __SNMP_SHARE_H__
#define __SNMP_SHARE_H__

#define MAX_SNMP_COMMUNITY_LEN 16
typedef struct st_snmp_profile {
    mibHead head;
	Uint32 snmp_enable;
	char trap_ip[MAX_SNMP_COMMUNITY_LEN + 4];
    char r_community[MAX_SNMP_COMMUNITY_LEN];
    char w_community[MAX_SNMP_COMMUNITY_LEN];
    char trap_community[MAX_SNMP_COMMUNITY_LEN];
} SnmpProfile;

#endif
