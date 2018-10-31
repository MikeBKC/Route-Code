#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/mutex.h>
#include <linux/netfilter_ipv4/utt_module_init.h>

#define UTT_CONFIG 1

#include "mibShare.h"
#if UTT_CONFIG
#include "profacceShare.h"
#endif

static DEFINE_MUTEX(performance_opt_mutex);

#define PROC_MEMSHARE_DIR		        "memshare"
#define PROC_MEMSHARE_PHYADDR		        "phymem_addr"
#define PROC_MEMSHARE_SIZE                      "phymem_size"


struct proc_dir_entry *proc_memshare_dir ;
unsigned long kernel_memaddr = 0;
EXPORT_SYMBOL_GPL(kernel_memaddr);

unsigned long kernel_memsize= 0;
unsigned int order;
unsigned int page;
unsigned long page_addr;
#if UTT_CONFIG
NvramProfiles *nvramProfile = NULL;

void ProfInstLowHigh(MibProfileType profileType, int *max, int *min)
{

    int instMax = 0, instMin = 0;
    switch ( profileType ) {
        case MIB_PROF_INTERFACE:
            instMax = MAX_INTERFACE_PROFILES;
            instMin = 0;
            break;
#if (IP_GRP == FYES)
	case MIB_PROF_IPGRP:
            instMax = MAX_IPGRP_PROFILES;
            instMin = 0;
            break;
#endif
#if (TIME_GRP == FYES)
	case MIB_PROF_TIMEGRP:
            instMax = MAX_TIMEGRP_PROFILES;
            instMin = 0;
            break;
#endif
#if (DHCP_SERVER == FYES)
        case MIB_PROF_DHCP_SERVER:
            instMax = 1;
            instMin = 0;
            break;
#endif
        case MIB_PROF_USER:
            instMax = MAX_USER_PROFILES;
            instMin = 0;
            break;

#if (DHCP_POOL == FYES)
        case MIB_PROF_DHCP_POOL:
            instMax = MAX_DHCP_POOL_PROFILES;
            instMin = 0;
            break;
#endif

#if (DNS_AGENCY == FYES)
	case MIB_PROF_DNS_AGENCY:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif

#if (AP_PERMISSION == FYES)
	case MIB_PROF_AP_PERMISSION:
	    instMin = 0;
	    instMax = MAX_AP_PERMISSION_PROFILES;
	    break;
#endif

#if (AP_CONF == FYES)
	case MIB_PROF_AP_CONF:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif

#if (SERVICE_ZONE == FYES)
	case MIB_PROF_SERVICE_ZONE:
	    instMin = 0;
	    instMax = 10;
	    break;
#endif

#if (TAG_VLAN == FYES)
	case MIB_PROF_TAG_VLAN:
	    instMin = 0;
	    instMax = MAX_TAG_VLAN_PROFILES;
	    break;
#if (WAN_TAG_VLAN == FYES)
	case MIB_PROF_WAN_TAG_VLAN:
	    instMin = 0;
	    instMax = MAX_WAN_TAG_VLAN_PROFILES;
	    break;
#endif
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
	    instMax = MAX_SW_PORT_CONF_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
	case MIB_PROF_PORT_VLAN:
            instMax = MAX_PORT_VLAN_PROFILES;
            instMin = 0;
	    break;
#endif
#if (PVLAN_REDUCED == FYES)
	case MIB_PROF_RDD_PORT_VLAN:
            instMax = MAX_LAN_PORT_COUNT;
            instMin = 0;
	    break;
#endif
#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    instMax = MAX_L2TP_SRV_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_L2TP_SRV_ACCOUNT:
	    instMax = MAX_L2TP_SRV_ACCOUNT_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    instMax = MAX_PPTP_SRV_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_PPTP_SRV_ACCOUNT:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_PPTP_SRV_ACCOUNT_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_PPTP_CLI_PROFILES;
#endif
	    instMin = 0;
	    break;
#endif

#if (STATIC_ROUTE == FYES)
	case MIB_PROF_STATIC_ROUTE:
	    instMax = MAX_STATIC_ROUTE_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
	    instMax = MAX_DDNS_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    instMax = MAX_PPPOE_SRV_GLO_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_PPPOE_SRV_ACC:
	    instMax = MAX_PPPOE_SRV_ACC_PROFILES;
	    instMin = 0;
	    break;
#endif

#if (GROUP_BASE == FYES)
	case MIB_PROF_GROUP_BASE:
	    instMax = MAX_GROUP_BASE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	case MIB_PROF_MIRROR:
		instMax = MAX_MIRROR_PROFILES;
		instMin = 0;
		break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	case MIB_PROF_TRUNK:
	    instMax = MAX_TRUNK_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_SW_SNMP == FYES)
	case MIB_PROF_SNMP:
	    instMax = MAX_SNMP_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_ACL == FYES)
        case MIB_PROF_ACL:
            instMax = MAX_ACL_PROFILES;
            instMin = 0;
            break;
        case MIB_PROF_ACL_STATUS:
            instMax = 1;
            instMin = 0;
            break;
#endif
#if (UTTPDB == FYES)
	case MIB_PROF_PDB_GLOBAL:
	    instMax = MAX_PDB_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (STATIC_NAT == FYES)
	case MIB_PROF_STATIC_NAT:
	    instMax = MAX_STATIC_NAT_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (NAT_RULE == FYES)
	case MIB_PROF_NAT_RULE:
	    instMax = MAX_NAT_RULE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (DMZ == FYES)
	case MIB_PROF_DMZ_NAT:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif
#if (ADMIN_CONF == FYES)  
	case MIB_PROF_ADMIN_CONF:
	    instMax = MAX_ADMIN_CONF_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (DNS_FILTER == FYES)  
	case MIB_PROF_DNS_FILTER:
	    instMax = MAX_DNS_FILTER_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (WIRELESS == FYES)  
	case MIB_PROF_WIRELESS:
	    instMax = MAX_WIRELESS_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_WIRELESS_MAC_FILTER:
	    instMax = MAX_WIRELESS_MAC_FILTER_PROFILES;
	    instMin = 0;
	    break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_WIRELESS_5G_MAC_FILTER:
	    instMax = MAX_WIRELESS_5G_MAC_FILTER_PROFILES;
	    instMin = 0;
	    break;
#endif
#endif
#if (FEATURE_NTP == FYES)  
	case MIB_PROF_NTP:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif
#if (TASK_SCHEDULER == FYES)
	case MIB_PROF_TASK_SCHED:
	    instMax = MAX_TASK_SCHED_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FIREWALL == FYES)
	case MIB_PROF_FIRE_WALL:
	    instMax = MAX_FIREWALL_FILTER_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (NOTICE == FYES)
	case MIB_PROF_NOTICE:
	    instMax = MAX_NOTICE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (WEB_AUTH == FYES)
	case MIB_PROF_WEB_AUTH_GLOBAL:
	    instMax = MAX_WEB_AUTH_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_WEB_AUTH:
	    instMax = MAX_WEB_AUTH_PROFILES;
	    instMin = 0;
	    break;
#if (WEBAUTH_AUTO == FYES)
	case MIB_PROF_WEB_AUTH_CPT:
	    instMax = MAX_WEB_AUTH_PROFILES;
	    instMin = 0;
	    break;
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
	case MIB_PROF_WHITE_LIST:
	    instMax = MAX_WHITE_LIST;
	    instMin = 0;
	    break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
	case MIB_PROF_EXCEPT_QQ:
	    instMax = MAX_EXCEPT_QQ_PROFILES;
	    instMin=0;
	    break;
#endif
#if (EXCEPT_MSN == FYES)
	case MIB_PROF_EXCEPT_MSN:
	    instMax = MAX_EXCEPT_MSN_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (EXCEPT_ALI == FYES)
	case MIB_PROF_EXCEPT_ALI:
	    instMax = MAX_EXCEPT_ALI_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (LOG_MANAGE == FYES)
	case MIB_PROF_LOG_MANAGE:
	    instMax = MAX_LOG_MANAGE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (AP_LOG_MANAGE == FYES)
	case MIB_PROF_AP_LOG_MANAGE:
	    instMax = MAX_AP_LOG_MANAGE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (IP_SEC == FYES)
	case MIB_PROF_IPSEC:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_IPSEC_PROFILES;
#endif
	    instMin = 0;
	    break;
	case MIB_PROF_TRANSFORMSET:
	    instMax = MAX_TRANSFORMSET_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_ISAKMP:
#if (VPN_ITEM_CONTROL == FYES)
	    instMax = MAX_VPN_ITEMS_NUM;
#else
	    instMax = MAX_ISAKMP_PROFILES;
#endif
	    instMin = 0;
	    break;
	case MIB_PROF_ISAKMP_POLICY:
	    instMax = MAX_ISAKMP_POLICY_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FAT_FIT_SWITCH == FYES)  
	case MIB_PROF_FAT_FIT_SWITCH:
	    instMax = MAX_FAT_FIT_SWITCH_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (AC_WORK_MODE == FYES)  
	case MIB_PROF_AC_WORK_MODE:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif

#if (AP_LOAD_BALANCE == FYES)
        case MIB_PROF_AP_LOAD_BALANCE:
            instMax = MAX_AP_LOAD_BL_PROFILES;
            instMin = 0;
            break;
#endif
#if (FEATURE_AP_MAIL == FYES)  
	case MIB_PROF_AP_MAIL:
	    instMax = MAX_AP_MAIL_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)  
	case MIB_PROF_AP_CONF_TEMP:
	    instMax = MAX_AP_CONF_TEMP_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
	case MIB_PROF_AP_SW_MNG:
	    instMax = MAX_AP_SW_MNG_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)  
	case MIB_PROF_AP_BASIC_CONF:
	    instMax = MAX_AP_BASIC_CONF_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)  
	case MIB_PROF_AP_EFFECT_CONF:
	    instMax = MAX_AP_EFFECT_CONF_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)  
	case MIB_PROF_FIT_AP_SZ:
	    instMax = MAX_FIT_AP_SERVICE_ZONE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (UTT_LICENSE == FYES)
	case MIB_PROF_LICENSE:
	    instMax = MAX_LICENSE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (POLICY_ROUTE == FYES)
	case MIB_PROF_POLICY_ROUTE:
	    instMax = MAX_POLICY_ROUTE_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (PARENTS_CONTROL == FYES)
	case MIB_PROF_PARENTS_CONTROL:
	    instMax = MAX_PARENTS_CONTROL_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (HTTP_CACHE == FYES)
	case MIB_PROF_CACHE:
	    instMax = MAX_CACHE_PROFILES;
	    instMin = 0; 
	    break;
#endif
#if (NET_MODE_SWITCH == FYES)
	case MIB_PROF_NET_MODE_SWITCH:
	    instMax = MAX_NET_MODE_PROFILES;
	    instMin = 0; 
	    break;
#endif
#if (BRIDGE == FYES)
	case MIB_PROF_BRIDGE:
	    instMax = MAX_BRIDGE_PROFILES;
	    instMin = 0; 
	    break;
#endif
#if (MAC_FILTER == FYES)
	case MIB_PROF_MAC_FILTER_GLOBAL:
	    instMax = MAX_MAC_FILTER_GLOBAL_PROFILES;
	    instMin = 0;
	    break;
	case MIB_PROF_MAC_FILTER:
	    instMax = MAX_MAC_FILTER_PROFILES;
	    instMin = 0;
	    break;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
	case MIB_PROF_SHORTCUT_MENU:
	    instMax = MAX_SHORTCUT_MENU_PROFILES;
	    instMin = 0;
	    break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
	case MIB_PROF_NET_SHARE_MANAGE:
	    instMax = 1;
	    instMin = 0;
	    break;
#endif
#if (FTP_SERVER == FYES)
        case MIB_PROF_FTP_SERVER:
	    instMax = 1;
	    instMin = 0;
	    break;
        case MIB_PROF_FTP_SHARE_DIR:
	    instMax = MAX_FTP_SHARE_DIR;
	    instMin = 0;
	    break;
#endif
#if (NET_SHARE_USER == FYES)
        case MIB_PROF_NET_SHARE_USER:
	    instMax = MAX_NET_SHARE_USER;
	    instMin = 0;
	    break;
#endif
#if (ADFILTER == FYES)
        case MIB_PROF_ADFILTER:
	    instMax = MAX_ADFILTER_PROFILES;
	    instMin = 0;
	    break;
#endif
        default:
            instMax = 1;
            instMin = 0;
            break;
    }
    if(max != NULL) {
        *max = instMax;
    }

    if(min != NULL) {
        *min = instMin;
    }
    return;
}
EXPORT_SYMBOL_GPL(ProfInstLowHigh);

static Pointer _getProfilePointer(SlotNumber		slot,
		   MibProfileType	profileType,
		   Word			index,
                   Uint			*length,
		   Uint			*maxInst,
		   Uint			*minInst)
{
    Pointer dest = NULL;
    Uint size = 0;

    switch ( profileType ) {
        case MIB_PROF_SYSTEM:
            dest = &(nvramProfile->systemProfile);
            size = sizeof(SystemProfile);
            break;
	case MIB_PROF_INTERFACE:
	    dest = &(nvramProfile->interfaceProfile[index]);
	    size = sizeof(InterfaceProfile);
	    break;
#if (IP_GRP == FYES)
	case MIB_PROF_IPGRP:
	    dest = &(nvramProfile->ipGrpProfile[index]);
	    size = sizeof(IpGrpProfile);
	    break;
#endif 
#if (TIME_GRP == FYES)
	case MIB_PROF_TIMEGRP:
	    dest = &(nvramProfile->timeGrpProfile[index]);
	    size = sizeof(TimeGrpProfile);
	    break;
#endif
#if (DHCP_SERVER == FYES)  
	case MIB_PROF_DHCP_SERVER:
	    dest = &(nvramProfile->dhcpServerProfile);
	    size = sizeof(DhcpServerProfile);
	    break;
#endif
#if (DHCP_POOL == FYES)
	case MIB_PROF_DHCP_POOL:
	    dest = &(nvramProfile->dhcpPoolProfile[index]);
	    size = sizeof(DhcpPoolProfile);
	    break;
#endif
	case MIB_PROF_USER:
	    dest = &(nvramProfile->userProfile[index]);
	    size = sizeof(UserProfile);
	    break;

#if (DNS_AGENCY == FYES)
	case MIB_PROF_DNS_AGENCY:
	    dest = &(nvramProfile->dnsAgencyProfile);
	    size = sizeof(DnsAgencyProfile);
	    break;
#endif
#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
	    dest = &(nvramProfile->swPortConfProfile[index]);
	    size = sizeof(SwPortConfProfile);
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
	case MIB_PROF_PORT_VLAN:
	    dest = &(nvramProfile->portVlanProfile[index]);
	    size = sizeof(PortVlanProfile);
	    break;
#endif

#if (PVLAN_REDUCED == FYES)
	case MIB_PROF_RDD_PORT_VLAN:
	    dest = &(nvramProfile->rddPortVlanProfile[index]);
	    size = sizeof(RddPortVlanProfile);
	    break;
#endif

#if (AP_PERMISSION == FYES)
	case MIB_PROF_AP_PERMISSION:
	    dest = &(nvramProfile->apPermissionProfile[index]);
	    size = sizeof(ApPermissionProfile);
	    break;
#endif

#if (AP_CONF == FYES)
	case MIB_PROF_AP_CONF:
	    dest = &(nvramProfile->apConfProfile);
	    size = sizeof(ApConfProfile);
	    break;
#endif

#if (SERVICE_ZONE == FYES)
	case MIB_PROF_SERVICE_ZONE:
	    dest = &(nvramProfile->serviceZoneProfile[index]);
	    size = sizeof(ServiceZoneProfile);
	    break;
#endif

#if (TAG_VLAN == FYES)
	case MIB_PROF_TAG_VLAN:
	    dest = &(nvramProfile->tagVlanProfile[index]);
	    size = sizeof(TagVlanProfile);
	    break;
#if (WAN_TAG_VLAN == FYES)
	case MIB_PROF_WAN_TAG_VLAN:
	    dest = &(nvramProfile->wanTagVlanProfile);
	    size = sizeof(WanTagVlanProfile);
	    break;
#endif
#endif

#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    dest = &(nvramProfile->l2tpSrvGlobalProfile);
	    size = sizeof(L2TPSrvGlobalProfile);
	    break;
	case MIB_PROF_L2TP_SRV_ACCOUNT:
	    dest = &(nvramProfile->l2tpSrvAccProfile[index]);
	    size = sizeof(L2TPSrvAccProfile);
	    break;
#endif

#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    dest = &(nvramProfile->pptpSrvGlobalProfile);
	    size = sizeof(PPTPSrvGlobalProfile);
	    break;
	case MIB_PROF_PPTP_SRV_ACCOUNT:
	    dest = &(nvramProfile->pptpSrvAccProfile[index]);
	    size = sizeof(PPTPSrvAccProfile);
	    break;
#endif

#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
	    dest = &(nvramProfile->pptpCliProfile[index]);
	    size = sizeof(PPTPCliProfile);
	    break;
#endif

#if (STATIC_ROUTE == FYES)
	case MIB_PROF_STATIC_ROUTE:
	    dest = &(nvramProfile->staticRouteProfile[index]);
	    size = sizeof(StaticRouteProfile);
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
	    dest = &(nvramProfile->ddnsProfile);
	    size = sizeof(DDNSProfile);
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    dest = &(nvramProfile->pppoeSrvGloProfile);
	    size = sizeof(PPPoESrvGloProfile);
	    break;
	case MIB_PROF_PPPOE_SRV_ACC:
	    dest = &(nvramProfile->pppoeSrvAccProfile[index]);
	    size = sizeof(PPPoESrvAccProfile);
	    break;
#endif

#if (GROUP_BASE == FYES)
	case MIB_PROF_GROUP_BASE:
	    dest = &(nvramProfile->groupBaseProfile[index]);
	    size = sizeof(GroupBaseProfile);
	    break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	case MIB_PROF_MIRROR:
	    dest = &(nvramProfile->mirrorProfile);
	    size = sizeof(MirrorProfile);
	    break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	case MIB_PROF_TRUNK:
	    dest = &(nvramProfile->trunkProfile[index]);
	    size = sizeof(TrunkProfile);
	    break;
#endif
#if (FEATURE_SW_SNMP == FYES)
	case MIB_PROF_SNMP:
	    dest = &(nvramProfile->snmpProfile);
	    size = sizeof(SnmpProfile);
	    break;
#endif
#if (FEATURE_ACL == FYES)
	case MIB_PROF_ACL:
	    dest = &(nvramProfile->aclProfile[index]);
	    size = sizeof(AclProfile);
	    break;
	case MIB_PROF_ACL_STATUS:
	    dest = &(nvramProfile->aclStatusProfile);
	    size = sizeof(AclStatusProfile);
	    break;
#endif

#if (UTTPDB == FYES)
	case MIB_PROF_PDB_GLOBAL:
	    dest = &(nvramProfile->pdbProfile[index]);
	    size = sizeof(PdbProfile);
	    break;
#endif
#if (STATIC_NAT == FYES)
	case MIB_PROF_STATIC_NAT:
	    dest = &(nvramProfile->staticNatProfile[index]);
	    size = sizeof(StaticNatProfile);
	    break;
#endif
#if (NAT_RULE == FYES)
	case MIB_PROF_NAT_RULE:
	    dest = &(nvramProfile->natRuleProfile[index]);
	    size = sizeof(NatRuleProfile);
	    break;
#endif
#if (DMZ  == FYES)
	case MIB_PROF_DMZ_NAT:
	    dest = &(nvramProfile->dmzProfile);
	    size = sizeof(DMZProfile);
	    break;
#endif
#if (ADMIN_CONF == FYES)  
	case MIB_PROF_ADMIN_CONF:
	    dest = &(nvramProfile->adminConfProfile[index]);
	    size = sizeof(AdminConfProfile);
	    break;
#endif
#if (DNS_FILTER == FYES)  
	case MIB_PROF_DNS_FILTER:
	    dest = &(nvramProfile->dnsFilterProfile[index]);
	    size = sizeof(DnsFilterProfile);
	    break;
#endif
#if (WIRELESS == FYES)
	case MIB_PROF_WIRELESS:
	    dest = &(nvramProfile->wirelessProfile[index]);
	    size = sizeof(WirelessProfile);
	    break;

	case MIB_PROF_WIRELESS_MAC_FILTER:
	    dest = &(nvramProfile->wirelessMacFilterProfile[index]);
	    size = sizeof(WirelessMacFilterProfile);
	    break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_WIRELESS_5G_MAC_FILTER:
	    dest = &(nvramProfile->wireless5gMacFilterProfile[index]);
	    size = sizeof(Wireless5gMacFilterProfile);
	    break;
#endif
	case MIB_PROF_WIRELESS_MAC_FILTER_GLOBAL:
	    dest = &(nvramProfile->wirelessProfile[index].mFltGlb);
	    size = sizeof(WirelessMacFilterGlobalProfile);
	    break;
#endif
#if (FEATURE_NTP == FYES)  
	case MIB_PROF_NTP:
	    dest = &(nvramProfile->ntpProfile);
	    size = sizeof(NtpProfile);
	    break;
#endif
#if (FEATURE_LANG == FYES)  
	case MIB_PROF_LANG:
	    dest = &(nvramProfile->langProfile);
	    size = sizeof(LangProfile);
	    break;
#endif
#if (TASK_SCHEDULER == FYES)
	case MIB_PROF_TASK_SCHED:
	    dest = &(nvramProfile->taskSchedProfile[index]);
	    size = sizeof(TaskSchedProfile);
	    break;
#endif
#if (FIREWALL == FYES)
	case MIB_PROF_FIRE_WALL:
	    dest = &(nvramProfile->fireWallProfile[index]);
	    size = sizeof(FireWallProfile);
	    break;
#endif
#if (NOTICE == FYES)
	case MIB_PROF_NOTICE:
	    dest = &(nvramProfile->noticeProfile[index]);
	    size = sizeof(NoticeProfile);
	    break;
#endif
#if (WEB_AUTH == FYES)
	case MIB_PROF_WEB_AUTH_GLOBAL:
	    dest = &(nvramProfile->webAuthGlobalProfile[index]);
	    size = sizeof(WebAuthGlobalProfile);
	    break;
	case MIB_PROF_WEB_AUTH:
	    dest = &(nvramProfile->webAuthProfile[index]);
	    size = sizeof(WebAuthProfile);
	    break;
#if (WEBAUTH_AUTO == FYES)
	case MIB_PROF_WEB_AUTH_CPT:
	    dest = &(nvramProfile->webAuthProfile_auto[index]);
	    size = sizeof(WebAuthProfile);
	    break;
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
	case MIB_PROF_WHITE_LIST:
	    dest = &(nvramProfile->whiteListProfile[index]);
	    size = sizeof(WhiteListProfile);
	    break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
	case MIB_PROF_EXCEPT_QQ:
	    dest = &(nvramProfile->exceptQQProfile[index]);
	    size = sizeof(ExceptQQProfile);
	    break;
#endif
#if (EXCEPT_MSN == FYES)
	case MIB_PROF_EXCEPT_MSN:
	    dest = &(nvramProfile->exceptMSNProfile[index]);
	    size = sizeof(ExceptMSNProfile);
	    break;
#endif
#if (EXCEPT_ALI == FYES)
	case MIB_PROF_EXCEPT_ALI:
	    dest = &(nvramProfile->exceptAliProfile[index]);
	    size = sizeof(ExceptAliProfile);
	    break;
#endif
#if (LOG_MANAGE == FYES)
	case MIB_PROF_LOG_MANAGE:
	    dest = &(nvramProfile->logManageProfile[index]);
	    size = sizeof(LogManageProfile);
	    break;
#endif
#if (AP_LOG_MANAGE == FYES)
	case MIB_PROF_AP_LOG_MANAGE:
	    dest = &(nvramProfile->apLogManageProfile[index]);
	    size = sizeof(ApLogManageProfile);
	    break;
#endif
#if (IP_SEC == FYES)
	case MIB_PROF_IPSEC:
	    dest = &(nvramProfile->ipsecconfProfile[index]);
	    size = sizeof(ipsecConfProfile);
	    break;
	case MIB_PROF_TRANSFORMSET:
	    dest = &(nvramProfile->ipsectransformsetProfile[index]);
	    size = sizeof(ipsecTransformSetProfile);
	    break;
	case MIB_PROF_ISAKMP:
	    dest = &(nvramProfile->isakmpconfProfile[index]);
	    size = sizeof(isakmpConfProfile);
	    break;
	case MIB_PROF_ISAKMP_POLICY:
	    dest = &(nvramProfile->isakmppolicyProfile[index]);
	    size = sizeof(isakmpPolicyProfile);
	    break;
#endif
#if (SYSLOG == FYES)
	case MIB_PROF_SYSLOG:
	    dest = &(nvramProfile->syslogProfile);
	    size = sizeof(SyslogProfile);
	    break;
#endif

#if (FAT_FIT_SWITCH == FYES)
	case MIB_PROF_FAT_FIT_SWITCH:
	    dest = &(nvramProfile->fatFitSwitchProfile);
	    size = sizeof(FatFitSwitchProfile);
	    break;
#endif
#if (AC_WORK_MODE == FYES)
	case MIB_PROF_AC_WORK_MODE:
	    dest = &(nvramProfile->acWorkModeProfile);
	    size = sizeof(AcWorkModeProfile);
	    break;
#endif

#if (AP_LOAD_BALANCE == FYES)
	case MIB_PROF_AP_LOAD_BALANCE:
	    dest = &(nvramProfile->apLoadBalanceProfile[index]);
	    size = sizeof(ApLoadBalanceProfile);
	    break;
#endif
#if (FEATURE_AP_MAIL == FYES)
	case MIB_PROF_AP_MAIL:
	    dest = &(nvramProfile->apMailProfile[index]);
	    size = sizeof(ApMailProfile);
	    break;
#endif
#if (FEATURE_AP_CONF_TEMP == FYES)
	case MIB_PROF_AP_CONF_TEMP:
	    dest = &(nvramProfile->apConfTempProfile[index]);
	    size = sizeof(ApConfTempProfile);
	    break;
#endif
#if (FEATURE_AP_SCAN == FYES)
	case MIB_PROF_AP_SCAN:
	    dest = &(nvramProfile->apScanProfile);
	    size = sizeof(ApScanProfile);
	    break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
	case MIB_PROF_AP_SW_MNG:
	    dest = &(nvramProfile->apSwMngProfile[index]);
	    size = sizeof(ApSwMngProfile);
	    break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
	case MIB_PROF_AP_BASIC_CONF:
	    dest = &(nvramProfile->apBasicConfProfile[index]);
	    size = sizeof(ApBasicConfProfile);
	    break;
#endif

#if (FEATURE_AP_EFFECT_CONF == FYES)
	case MIB_PROF_AP_EFFECT_CONF:
	    dest = &(nvramProfile->apEffectConfProfile);
	    size = sizeof(ApEffectConfProfile);
	    break;
#endif

#if (FIT_AP_SERVICE_ZONE == FYES)
	case MIB_PROF_FIT_AP_SZ:
	    dest = &(nvramProfile->fitApSzProfile[index]);
	    size = sizeof(FitApSzProfile);
	    break;
#endif

#if (UTT_LICENSE == FYES)
	case MIB_PROF_LICENSE:
	    dest = &(nvramProfile->licenseProfile[index]);
	    size = sizeof(LicenseProfile);
	    break;
#endif
#if (POLICY_ROUTE == FYES)
	case MIB_PROF_POLICY_ROUTE:
	    dest = &(nvramProfile->policyrouteProfile[index]);
	    size = sizeof(PolicyRouteProfile);
	    break;
#endif
#if (PARENTS_CONTROL == FYES)
	case MIB_PROF_PARENTS_CONTROL:
	    dest = &(nvramProfile->parentscontrolProfile[index]);
	    size = sizeof(ParentsControlProfile);
	    break;
#endif
#if (P2P_LIMIT == FYES)
	case MIB_PROF_P2P_LIMIT:
	    dest = &(nvramProfile->p2pLimitProfile);
	    size = sizeof(P2PLimitProfile);
	    break;
#endif
#if (MAC_FILTER == FYES)
	case MIB_PROF_MAC_FILTER_GLOBAL:
	    dest = &(nvramProfile->macFilterGlobalProfile);
	    size = sizeof(MacFilterGlobalProfile);
	    break;
	case MIB_PROF_MAC_FILTER:
	    dest = &(nvramProfile->macFilterProfile[index]);
	    size = sizeof(MacFilterProfile);
	    break;
#endif
#if (SYS_LOG_INFO == FYES)
	case MIB_PROF_SYS_LOG_INFO:
	    dest = &(nvramProfile->sysLogInfoProfile);
	    size = sizeof(SysLogInfoProfile);
	    break;
#endif
#if (EASY_TASK == FYES)
	case MIB_PROF_EASY_TASK:
	    dest = &(nvramProfile->easyTaskProfile);
	    size = sizeof(EasyTaskProfile);
	    break;
#endif
#if (HTTP_CACHE == FYES)
	case MIB_PROF_CACHE:
	    dest = &(nvramProfile->cacheProfile[index]);
	    size = sizeof(CacheProfile);
	    break;
#endif
#if (NET_MODE_SWITCH == FYES)
	case MIB_PROF_NET_MODE_SWITCH:
	    dest = &(nvramProfile->netModeSwitchProfile[index]);
	    size = sizeof(NetModeSwitchProfile);
	    break;
#endif
#if (BRIDGE == FYES)
	case MIB_PROF_BRIDGE:
	    dest = &(nvramProfile->bridgeProfile[index]);
	    size = sizeof(BridgeProfile);
	    break;
#endif
#if (CACHE_SERVER == FYES)
	case MIB_PROF_CACHE_SERVER:
	    dest = &(nvramProfile->cacheServerProfile[index]);
	    size = sizeof(CacheServerProfile);
	    break;
#endif
#if (NET_SNIPER == FYES)
	case MIB_PROF_NET_SNIPER:
	    dest = &(nvramProfile->netSniperProfile);
	    size = sizeof(NetSniperProfile);
	    break;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
	case MIB_PROF_SHORTCUT_MENU:
	    dest = &(nvramProfile->shortCutMenuProfile[index]);
	    size = sizeof(ShortCutMenuProfile);
	    break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
	case MIB_PROF_NET_SHARE_MANAGE:
	    dest = &(nvramProfile->netShareManageProfile);
	    size = sizeof(NetShareManageProfile);
	    break;
#endif
#if (FTP_SERVER == FYES)
	case MIB_PROF_FTP_SERVER:
	    dest = &(nvramProfile->ftpServerProfile);
	    size = sizeof(FtpServerProfile);
	    break;
	case MIB_PROF_FTP_SHARE_DIR:
	    dest = &(nvramProfile->ftpShareDirProfile[index]);
	    size = sizeof(FtpShareDirProfile);
	    break;
#endif
#if (NET_SHARE_USER == FYES)
	case MIB_PROF_NET_SHARE_USER:
	    dest = &(nvramProfile->netShareUserProfile[index]);
	    size = sizeof(NetShareUserProfile);
	    break;
#endif
#if (ADFILTER == FYES)
	case MIB_PROF_ADFILTER:
	    dest = &(nvramProfile->adfilterProfile);
	    size = sizeof(AdfilterProfile);
	    break;
#endif
	default:
	    break;
    }

    if(length != NULL) {
        *length = size;
    }
    ProfInstLowHigh(profileType, maxInst, minInst);
    return dest;
}

static Registrant* _typeHeads[MIB_PROF_MAX];

void registerForProfileUpdates(MibProfileType	mibType,
			   Pointer		updateTag,
			   ProfileUpdateFnc	updateFnc )
{
    Registrant*		newRegistrant;
    
    if( mibType >= MIB_PROF_MAX ) {
	return;
    }

    newRegistrant = (Registrant*)kmalloc(sizeof(Registrant), GFP_ATOMIC);
    if( newRegistrant == NULL ) {
	return;
    }

    newRegistrant->rUseCnt = 0;
    newRegistrant->updateFnc = updateFnc;
    newRegistrant->updateTag = updateTag;
    newRegistrant->oldProfileType = mibType;
    newRegistrant->next = _typeHeads[ mibType ];
    _typeHeads[ mibType ] = newRegistrant;
}
EXPORT_SYMBOL_GPL(registerForProfileUpdates);

void reportProfileUpdate( SlotNumber	slot,
                     ProfChangeType     changeType,
		     MibProfileType	profileType,
		     Uint32		index,
		     Pointer		oldDataPtr)
{
    Registrant*			curRegistrant;
    int size = 0;

    Pointer newDataPtr = NULL;
   /* 
    * Traverse the linked list of registered entities calling each
    * one with the wonderful news.
    */

    curRegistrant = _typeHeads[ profileType ];
    
    newDataPtr = _getProfilePointer(slot, profileType, index, &size, NULL, NULL);
    if((newDataPtr != NULL) && (memcmp(oldDataPtr, newDataPtr, size) != 0)) {
        while(curRegistrant != NULL) {
            curRegistrant->rUseCnt++;

            (*(curRegistrant->updateFnc))( curRegistrant->updateTag,
            			       curRegistrant->oldProfileType,
                                       changeType,
            			       index,
            			       oldDataPtr,
            			       newDataPtr );

            curRegistrant = curRegistrant->next;
        }
    }
    return;
}
EXPORT_SYMBOL_GPL(reportProfileUpdate);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
static inline int utt_rcv_conf_skb(struct sk_buff *skb, struct nlmsghdr *nlh)
#else
static inline int utt_rcv_conf_skb(struct sk_buff *skb, struct nlmsghdr *nlh, int *err)
#endif
{
	int nlmsglen, skblen;
        struct st_uttMsg *msg;
        SlotNumber		slot;

        printk("ff2\n");
	skblen = skb->len;
	if (skblen < sizeof(*nlh))
		return 1;
	nlmsglen = nlh->nlmsg_len;
        msg = (struct st_uttMsg*)NLMSG_DATA(nlh);
	switch((msg->msgPara).mainType) {
            case MSG_PROF:
	    printk("ff3,msg->msgPara.data.profChangeData.profType=%d\n",msg->msgPara.data.profChangeData.profType);
                reportProfileUpdate(slot, msg->msgPara.secType, msg->msgPara.data.profChangeData.profType,
                        msg->msgPara.data.profChangeData.instIndex,
                        &((msg->msgPara).data.profChangeData.oldDataPtr));
                break;
            default:
                break;
        }

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
static void utt_nlForConf(struct sk_buff *skb)
{
	mutex_lock(&performance_opt_mutex);

        printk("file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	netlink_rcv_skb(skb, utt_rcv_conf_skb);
	mutex_unlock(&performance_opt_mutex);
}
#else
static void utt_nlForConf(struct sock *sk, int qlen)
{
	mutex_lock(&performance_opt_mutex);

        printk("file:%s,func:%s,line:%d\n",__FILE__,__func__,__LINE__);
	netlink_run_queue(sk, &qlen,utt_rcv_conf_skb);
	mutex_unlock(&performance_opt_mutex);
}
#endif
static struct sock *nl_conf = NULL;
/*
 * register an netlink for rcv profile change
 */
static int utt_creatNlForConf(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
    nl_conf = netlink_kernel_create(&init_net, NETLINK_UTT_PROF_CHANGE, 0,
            utt_nlForConf, NULL, THIS_MODULE);
#else
    nl_conf = netlink_kernel_create(NETLINK_UTT_PROF_CHANGE, 0,
            utt_nlForConf, THIS_MODULE);
#endif
    if(nl_conf == NULL) {
        printk(KERN_ERR"creat netlink for profile change faild\n");
        return -1;
    }
    
    return 1;
}
#endif

static int proc_read_phymem_addr(char *page, char **start, off_t off,int count, int *eof, void *data)
{
	return sprintf(page, "%08lx\n", __pa(kernel_memaddr));
}

static int proc_read_phymem_size(char *page, char **start, off_t off,int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", kernel_memsize);
}

/**
 * 根据传入的bytes转换为__get_free_pages需要的参数order
 * size : bytes
 * */
int uttGetPageOrder(unsigned long size)
{
    int  i = 0;

    /*不足一页按照一页计算*/
    size = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;
    
    while( (1 << i) < size)
    {   
        i++;
    }   
    return i;
} 
static int __init init(void)
{
        int i;

	proc_memshare_dir = proc_mkdir(PROC_MEMSHARE_DIR, NULL);
	create_proc_read_entry(PROC_MEMSHARE_PHYADDR, 0, proc_memshare_dir, proc_read_phymem_addr,NULL);
	create_proc_read_entry(PROC_MEMSHARE_SIZE, 0, proc_memshare_dir, proc_read_phymem_size,NULL);

	/*alloc some pages*/
        order = uttGetPageOrder(sizeof(NvramProfiles));
	printk("%s %d  nvram size %d alloc order %d\n", __func__, __LINE__,sizeof(NvramProfiles), order);
	kernel_memaddr =__get_free_pages(GFP_KERNEL|__GFP_ZERO, order);
	if(!kernel_memaddr)
	{
		printk("Allocate memory failure!\n");
	}
	else
	{

                page = 1 << order;
                page_addr = kernel_memaddr;

                for(i = 0; i <  page; i++) {
                        SetPageReserved(virt_to_page(page_addr)); /* reserved for used */
                        page_addr += PAGE_SIZE; 
                }

#if UTT_CONFIG
                nvramProfile = (NvramProfiles*)kernel_memaddr;
		printk("Allocate memory success!. The phy mem addr=%08lx, size=%lu\n", __pa(kernel_memaddr), 1024*1024*4);
                utt_creatNlForConf();

#endif
	}

	return 0;
}

static void __exit fini(void)
{
        int i;

        page_addr = kernel_memaddr;
        for(i = 0; i <  page; i++) {
                SetPageReserved(virt_to_page(page_addr)); /* reserved for used */
                page_addr += PAGE_SIZE; 
        }

	ClearPageReserved(virt_to_page(kernel_memaddr));
	free_pages(kernel_memaddr, order);
	remove_proc_entry(PROC_MEMSHARE_PHYADDR, proc_memshare_dir);
	remove_proc_entry(PROC_MEMSHARE_SIZE, proc_memshare_dir);
	remove_proc_entry(PROC_MEMSHARE_DIR, NULL);
        /*
         * del netlink for utt config profile change
         */
#if UTT_CONFIG
        if(nl_conf != NULL){
            sock_release(nl_conf->sk_socket);
        }
#endif

	return;
}
module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("brwang (wang.bingrong@utt.com.cn)");
MODULE_DESCRIPTION("Kernel memory share module.");
