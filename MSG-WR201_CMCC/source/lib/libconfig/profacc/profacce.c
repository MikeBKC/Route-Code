/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be  copied,    ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1990-1993 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.

 * SCCS ID: $Id: profacce.c,v 1.87.2.6 2016/08/02 02:32:35 cao.yongxiang Exp $
 * 
 * File:  profAccess.c
 * 
 * Overview:
 *  Implementation of interface for code throughout the system to access
 *  current profile information as well as be informed of profile updates.
 */



/*
* All registrants with this module are kept in a linked list based
* on the profile type.  They are called whenever the profile changes.
*
*  RBC: added the oldProfileType to map between the old ProfileTypes
*  used throughout code and the internal MibProfileType.
*  REMOVE when all code is converted to use MibProfileType.
*/

#include "uttMachine.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdarg.h>
#include "string.h"

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "daughter.h"
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif
#define SWORD_DEBUG 1
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
NvramProfiles *nvramProfile;
/*
 * For each profile type, we keep a linked list of registrants -- pretty
 * simple.
 * ( RBC:  The update notification mechanism uses "old" profile type,
 *		not the MIB-profile types. )
 */
static Registrant* _typeHeads[ MIB_PROF_MAX ];

/*
 * A static copy of slot number any used as a param to the
 * nvram access functions for the "per system" profiles.
 */
static const SlotNumber _slotAny = {
    SLOT_NUMBER_ANY
};

void registerForProfileUpdates(MibProfileType	mibType,
			   Pointer		updateTag,
			   ProfileUpdateFnc	updateFnc )
{
    Registrant*		newRegistrant;
    
    if( mibType >= MIB_PROF_MAX ) {
	return;
    }

    newRegistrant = (Registrant*)MALLOC(sizeof(Registrant));
    if( newRegistrant == NULL ) {
	return;
    }

#if 0
    newRegistrant->rCreTm = getMasterClock();
    newRegistrant->rLastTm = getMasterClock();
#endif
    newRegistrant->rUseCnt = 0;

    newRegistrant->updateFnc = updateFnc;
    newRegistrant->updateTag = updateTag;
    newRegistrant->oldProfileType = mibType;

    newRegistrant->next = _typeHeads[ mibType ];
    _typeHeads[ mibType ] = newRegistrant;
}

#if(CWMP == FYES)
void cwmp_value_change_notice();
#endif
void reportProfileUpdate( SlotNumber	slot,
                     ProfChangeType     changeType,
		     MibProfileType	profileType,
		     Uint32		index,
		     Pointer		oldDataPtr,
             int            flag)
{
    Registrant*			curRegistrant;
    int size = 0;

    Pointer newDataPtr = NULL;
   /* 
    * Traverse the linked list of registered entities calling each
    * one with the wonderful news.
    */

    curRegistrant = _typeHeads[ profileType ];
#if 0
    if(changeType == PROF_CHANGE_ADD) {
        size = profileBuildFactory(profileType, &newDataPtr);
    } else {
        newDataPtr = _getProfilePointer(slot, profileType, index, &size, NULL, NULL);
    }
#else
    
    newDataPtr = _getProfilePointer(slot, profileType, index, &size, NULL, NULL);
#endif
    if((newDataPtr != NULL) && (memcmp(oldDataPtr, newDataPtr, size) != 0)) {
        while(curRegistrant != NULL) {
            curRegistrant->rUseCnt++;

            (*(curRegistrant->updateFnc))( curRegistrant->updateTag,
            			       curRegistrant->oldProfileType,
                                       changeType,
            			       index,
            			       oldDataPtr,
            			       newDataPtr );
#if(CWMP == FYES)
#if 0
    if(flag == 0)
    {
        cwmp_value_change_notice();
    }
#endif
#endif

            curRegistrant = curRegistrant->next;
        }
    }
    return;
}

int profileBuildFactory(MibProfileType profType, Pointer *_prof)
{
    int size = 0;
    static ProfileUnion unionProf;

    switch(profType)
    {
        case MIB_PROF_SYSTEM:
            *_prof = (Pointer*)buildFactorySystemProfile(&unionProf);
            size = sizeof(SystemProfile);
	    break;
        case MIB_PROF_INTERFACE:
            *_prof = (Pointer*)buildFactoryInterfaceProfile(&unionProf);
            size = sizeof(InterfaceProfile);
	    break;
#if (IPV6_INTERFACE == FYES)
        case MIB_PROF_IPV6_INTERFACE:
            *_prof = (Pointer*)buildFactoryIpv6InterfaceProfile(&unionProf);
            size = sizeof(Ipv6InterfaceProfile);
            break;
#endif
#if (IPV6_RADVD == FYES)
        case MIB_PROF_RADVD:
            *_prof = (Pointer*)buildFactoryRadvdProfile(&unionProf);
            size = sizeof(RadvdProfile);
            break;
#endif
#if(IPV6_ROUTE ==FYES)
        case MIB_PROF_IPV6_ROUTECONF:
            *_prof = (Pointer*)buildFactoryIpv6RoutConfProfile(&unionProf);
            size = sizeof(Ipv6RoutConfProfile);
            break;
#endif
#if (IPV6_DHCPV6 == FYES)
        case MIB_PROF_DHCP6:
            *_prof = (Pointer*)buildFactoryDhcp6Profile(&unionProf);
            size = sizeof(Dhcp6Profile);
            break;
#endif

#if (IPV6_TUNNEL == FYES)
        case MIB_PROF_TUNNEL:
            *_prof = (Pointer*)buildFactoryTunnelProfile(&unionProf);
            size = sizeof(TunnelProfile);
            break;
#endif
#if (IPV6_ISATAP == FYES)
        case MIB_PROF_ISATAP:
            *_prof = (Pointer*)buildFactoryIsatapProfile(&unionProf);
            size = sizeof(IsatapProfile);
            break;
#endif

#if(IPV6_NOTLISTPREFIXS == FYES)
        case MIB_PROF_IPV6_NOTLISTPREFIXS:
            *_prof = (Pointer*)buildFactoryIpGrpProfile(&unionProf);
            size = sizeof(Ipv6PrefixListProfile);
            break;
#endif
#if (IP_GRP == FYES)
	case MIB_PROF_IPGRP:
            *_prof = (Pointer*)buildFactoryIpGrpProfile(&unionProf);
            size = sizeof(IpGrpProfile);
	    break;
#endif
#if (PORT_GRP == FYES)
	case MIB_PROF_PORTGRP:
            *_prof = (Pointer*)buildFactoryPortGrpProfile(&unionProf);
            size = sizeof(PortGrpProfile);
	    break;
#endif
#if (TIME_GRP == FYES)
	case MIB_PROF_TIMEGRP:
            *_prof = (Pointer*)buildFactoryTimeGrpProfile(&unionProf);
            size = sizeof(TimeGrpProfile);
	    break;
#endif
#if (DHCP_SERVER == FYES)
	case MIB_PROF_DHCP_SERVER:
            *_prof = (Pointer*)buildFactoryDhcpServerProfile(&unionProf);
            size = sizeof(DhcpServerProfile);
	    break;
#endif
#if (DHCP_POOL == FYES)
	case MIB_PROF_DHCP_POOL:
            *_prof = (Pointer*)buildFactoryDhcpPoolProfile(&unionProf);
            size = sizeof(DhcpPoolProfile);
	    break;
#endif
#if (VLAN_CONFIG == FYES)
	case MIB_PROF_VLAN_CONFIG:
            *_prof = (Pointer*)buildFactoryVlanConfigProfile(&unionProf);
            size = sizeof(VlanConfigProfile);
	    break;
#endif
	case MIB_PROF_USER:
            *_prof = (Pointer*)buildFactoryUserProfile(&unionProf);
            size = sizeof(UserProfile);
		break;

#if (DNS_AGENCY == FYES)
	case MIB_PROF_DNS_AGENCY:
			*_prof = (Pointer *)buildFactoryDnsAgencyProfile(&unionProf);
			size = sizeof(DnsAgencyProfile);
			break;
#endif

#if (L2TP_SERVER == FYES)
	case MIB_PROF_L2TP_SRV_GLOBAL:
	    *_prof = (Pointer*)buildFactoryL2TPSrvGloProfile(&unionProf);
            size = sizeof(L2TPSrvGlobalProfile);
	    break;
	case MIB_PROF_L2TP_SRV_ACCOUNT:
	    *_prof = (Pointer*)buildFactoryL2TPSrvAccProfile(&unionProf);
            size = sizeof(L2TPSrvAccProfile);
	    break;
#endif

#if (L2TP_CLIENT == FYES)
	case MIB_PROF_L2TP_CLIENT:
	    *_prof = (Pointer*)buildFactoryL2TPCliProfile(&unionProf);
	    size = sizeof(L2TPCliProfile);
	    break;
#endif

#if (PPTP_SERVER == FYES)
	case MIB_PROF_PPTP_SRV_GLOBAL:
	    *_prof = (Pointer*)buildFactoryPPTPSrvGloProfile(&unionProf);
            size = sizeof(PPTPSrvGlobalProfile);
	    break;
	case MIB_PROF_PPTP_SRV_ACCOUNT:
	    *_prof = (Pointer*)buildFactoryPPTPSrvAccProfile(&unionProf);
            size = sizeof(PPTPSrvAccProfile);
	    break;
#endif

#if (PPTP_CLIENT == FYES)
	case MIB_PROF_PPTP_CLI:
	    *_prof = (Pointer*)buildFactoryPPTPCliProfile(&unionProf);
            size = sizeof(PPTPCliProfile);
	    break;
#endif

#if (STATIC_ROUTE == FYES)
	case MIB_PROF_STATIC_ROUTE:
	    *_prof = (Pointer*)buildFactoryStaticRouteProfile(&unionProf);
            size = sizeof(StaticRouteProfile);
	    break;
#endif

#if (DDNS == FYES)
	case MIB_PROF_DDNS:
	    *_prof = (Pointer*)buildFactoryDdnsProfile(&unionProf);
            size = sizeof(DDNSProfile);
	    break;
#endif

#if (PPPOE_SERVER == FYES)
	case MIB_PROF_PPPOE_SRV_GLO:
	    *_prof = (Pointer*)buildFactoryPppoeSrvGloProfile(&unionProf);
            size = sizeof(PPPoESrvGloProfile);
	    break;

	case MIB_PROF_PPPOE_SRV_ACC:
	    *_prof = (Pointer*)buildFactoryPppoeSrvAccProfile(&unionProf);
            size = sizeof(PPPoESrvAccProfile);
	    break;
#endif

#if (FEATURE_SW_PORT_CONF == FYES)
	case MIB_PROF_SW_PORT_CONF:
            *_prof = (Pointer*)buildFactorySwPortConfProfile(&unionProf);
            size = sizeof(SwPortConfProfile);
	    break;
#endif
#if (FEATURE_PORT_VLAN == FYES)
        case MIB_PROF_PORT_VLAN:
            *_prof = (Pointer*)buildFactoryPortVlanProfile(&unionProf);
            size = sizeof(PortVlanProfile);
            break;
#endif
#if (PVLAN_REDUCED == FYES)
        case MIB_PROF_RDD_PORT_VLAN:
            *_prof = (Pointer*)buildFactoryRddPortVlanProfile(&unionProf);
            size = sizeof(RddPortVlanProfile);
            break;
#endif
#if (FEATURE_SW_PORT_MIRROR == FYES)
	case MIB_PROF_MIRROR:
		*_prof = (Pointer*)buildFactoryMirrorProfile(&unionProf);
		size = sizeof(MirrorProfile);
		break;
#endif
#if (GROUP_BASE == FYES)
	case MIB_PROF_GROUP_BASE:
		*_prof = (Pointer*)buildFactoryGroupBaseProfile(&unionProf);
		size = sizeof(GroupBaseProfile);
		break;
#endif
#if (FEATURE_SW_PORT_TRUNK == FYES)
	 case MIB_PROF_TRUNK:
		*_prof = (Pointer*)buildFactoryTrunkProfile(&unionProf);
		size = sizeof(TrunkProfile);
		break;
#endif
#if (FEATURE_SW_SNMP == FYES)
	 case MIB_PROF_SNMP:
		*_prof = (Pointer*)buildFactorySnmpProfile(&unionProf);
		size = sizeof(SnmpProfile);
		break;
#endif
#if (SNMP == FYES)
       case MIB_PROF_SNMP_INFO:
              *_prof = (Pointer*)buildFactorySnmpInfoProfile(&unionProf);
              size = sizeof(SnmpInfoProfile);
              break;
#endif
#if (FEATURE_ACL == FYES)
	case MIB_PROF_ACL:
            *_prof = (Pointer*)buildFactoryAclProfile(&unionProf);
            size = sizeof(AclProfile);
            break;
	case MIB_PROF_ACL_STATUS:
            *_prof = (Pointer*)buildFactoryAclStatusProfile(&unionProf);
            size = sizeof(AclStatusProfile);
            break;
#endif
#if (UTTPDB  == FYES)
	 case MIB_PROF_PDB_GLOBAL:
		*_prof = (Pointer*)buildFactoryPdbProfile(&unionProf);
		size = sizeof(PdbProfile);
		break;
#endif
#if (STATIC_NAT == FYES)
	 case MIB_PROF_STATIC_NAT:
		*_prof = (Pointer*)buildFactoryStaticNatProfile(&unionProf);
		size = sizeof(StaticNatProfile);
		break;
#endif
#if (NAT_RULE == FYES)
	 case MIB_PROF_NAT_RULE:
		*_prof = (Pointer*)buildFactoryNatRuleProfile(&unionProf);
		size = sizeof(NatRuleProfile);
		break;
#endif
#if (DMZ == FYES)
	 case MIB_PROF_DMZ_NAT:
		*_prof = (Pointer*)buildFactoryDMZProfile(&unionProf);
		size = sizeof(DMZProfile);
		break;
#endif
#if (ADMIN_CONF == FYES)
	case MIB_PROF_ADMIN_CONF:
		*_prof = (Pointer*)buildFactoryAdminConfProfile(&unionProf);
		size = sizeof(AdminConfProfile);
		break;
#endif
#if (DNS_FILTER == FYES)
	case MIB_PROF_DNS_FILTER:
		*_prof = (Pointer*)buildFactoryDnsFilterProfile(&unionProf);
		size = sizeof(DnsFilterProfile);
		break;
#endif

#if (CLI_FOR_ENGINEER_ONLY == FYES)
    case MIB_PROF_CLI_FOR_ENGINEER_ONLY:
        *_prof = (Pointer *)buildFactoryCliForEngineerOnlyProfile(&unionProf);
        size = sizeof(CliForEngineerOnlyProfile);
        break;
#endif
#if (WIRELESS == FYES)
	case MIB_PROF_WIRELESS:
	    *_prof = (Pointer*)buildFactoryWirelessProfile(&unionProf);
            size = sizeof(WirelessProfile);
	    break;
	case MIB_PROF_WIRELESS_MAC_FILTER:
	    *_prof = (Pointer*)buildFactoryWirelessMacFilterProfile(&unionProf);
            size = sizeof(WirelessMacFilterProfile);
	    break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_WIRELESS_5G_MAC_FILTER:
	    *_prof = (Pointer*)buildFactoryWireless5gMacFilterProfile(&unionProf);
            size = sizeof(Wireless5gMacFilterProfile);
	    break;
#endif
#endif
#if (FEATURE_NTP == FYES)
	case MIB_PROF_NTP:
		*_prof = (Pointer*)buildFactoryNtpProfile(&unionProf);
		size = sizeof(NtpProfile);
		break;
#endif
#if (FEATURE_LANG == FYES)
	case MIB_PROF_LANG:
		*_prof = (Pointer*)buildFactoryLangProfile(&unionProf);
		size = sizeof(LangProfile);
		break;
#endif
#if (TASK_SCHEDULER == FYES)
	case MIB_PROF_TASK_SCHED:
	    *_prof = (Pointer*)buildFactoryTaskSchedProfile(&unionProf);
	    size = sizeof(TaskSchedProfile);
	    break;
#endif
#if (FIREWALL == FYES)
	case MIB_PROF_FIRE_WALL:
	    *_prof = (Pointer*)buildFactoryFireWallProfile(&unionProf);
	    size = sizeof(FireWallProfile);
	    break;
#endif
#if (NOTICE == FYES)
	case MIB_PROF_NOTICE:
	    *_prof = (Pointer*)buildFactoryNoticeProfile(&unionProf);
	    size = sizeof(NoticeProfile);
	    break;
#endif
#if (CWMP == FYES)
	case MIB_PROF_CWMP:
	    *_prof = (Pointer*)buildFactoryCwmpProfile(&unionProf);
	    size = sizeof(CwmpProfile);
	    break;
#endif
#if (WEB_AUTH == FYES)
	case MIB_PROF_WEB_AUTH_GLOBAL:
	    *_prof = (Pointer*)buildFactoryWebAuthGlobalProfile(&unionProf);
	    size = sizeof(WebAuthGlobalProfile);
	    break;
	case MIB_PROF_WEB_AUTH:
	    *_prof = (Pointer*)buildFactoryWebAuthProfile(&unionProf);
	    size = sizeof(WebAuthProfile);
	    break;
#if (WEBAUTH_AUTO == FYES)
	case MIB_PROF_WEB_AUTH_CPT:
	    *_prof = (Pointer*)buildFactoryWebAuthProfile(&unionProf);
	    size = sizeof(WebAuthProfile);
	    break;
#endif
#endif
#if (WIFIDOG_AUTH == FYES)
#if (WHITELIST == 1)
	case MIB_PROF_WHITE_LIST:
		*_prof = (Pointer*)buildFactoryWhiteListProfile(&unionProf);
		size = sizeof(WhiteListProfile);
		break;
#endif
#endif
#if (EXCEPT_QQ == FYES)
	case MIB_PROF_EXCEPT_QQ:
		*_prof = (Pointer*)buildFactoryExceptQQProfile(&unionProf);
		size = sizeof(ExceptQQProfile);
		break;
#endif
#if (EXCEPT_MSN == FYES)
	case MIB_PROF_EXCEPT_MSN:
		*_prof = (Pointer*)buildFactoryExceptMSNProfile(&unionProf);
		size = sizeof(ExceptMSNProfile);
		break;
#endif
#if (EXCEPT_ALI == FYES)
	case MIB_PROF_EXCEPT_ALI:
		*_prof = (Pointer*)buildFactoryExceptAliProfile(&unionProf);
		size = sizeof(ExceptAliProfile);
		break;
#endif
#if (AP_LOG_MANAGE == FYES)
	case MIB_PROF_AP_LOG_MANAGE:
		*_prof = (Pointer*)buildFactoryApLogManageProfile(&unionProf);
		size = sizeof(ApLogManageProfile);
		break;
#endif
#if (LOG_MANAGE == FYES)
	case MIB_PROF_LOG_MANAGE:
		*_prof = (Pointer*)buildFactoryLogManageProfile(&unionProf);
		size = sizeof(LogManageProfile);
		break;
#endif
#if (IP_SEC == FYES)
	case MIB_PROF_IPSEC:
		*_prof = (Pointer*)buildFactoryipsecConfProfile(&unionProf);
		size = sizeof(ipsecConfProfile);
		break;
	case MIB_PROF_TRANSFORMSET:
		*_prof = (Pointer*)buildFactoryipsectransformSetProfile(&unionProf);
		size = sizeof(ipsecTransformSetProfile);
		break;
	case MIB_PROF_ISAKMP:
		*_prof = (Pointer*)buildFactoryisakmpConfProfile(&unionProf);
		size = sizeof(isakmpConfProfile);
		break;
	case MIB_PROF_ISAKMP_POLICY:
		*_prof = (Pointer*)buildFactoryisakmpPolicyProfile(&unionProf);
		size = sizeof(isakmpPolicyProfile);
		break;
#endif
#if (SYSLOG == FYES)
	case MIB_PROF_SYSLOG:
		*_prof = (Pointer*)buildFactorySyslogProfile(&unionProf);
		size = sizeof(SyslogProfile);
		break;
#endif
#if (UTT_LICENSE == FYES)
	case MIB_PROF_LICENSE:
		*_prof = (Pointer*)buildFactoryLicenseProfile(&unionProf);
		size = sizeof(LicenseProfile);
		break;
#endif
#if (POLICY_ROUTE == FYES)
	case MIB_PROF_POLICY_ROUTE:
		*_prof = (Pointer*)buildFactoryPolicyRouteProfile(&unionProf);
		size = sizeof(PolicyRouteProfile);
		break;
#endif
#if (PARENTS_CONTROL == FYES)
	case MIB_PROF_PARENTS_CONTROL:
		*_prof = (Pointer*)buildFactoryParentsControlProfile(&unionProf);
		size = sizeof(ParentsControlProfile);
		break;
#endif
#if (HTTP_CACHE == FYES)
	case MIB_PROF_CACHE:
		*_prof = (Pointer*)buildFactoryCacheProfile(&unionProf);
		size = sizeof(CacheProfile);
		break;
#endif
#if (NET_MODE_SWITCH== FYES)
	case MIB_PROF_NET_MODE_SWITCH:
		*_prof = (Pointer*)buildFactoryNetModeSwitchProfile(&unionProf);
		size = sizeof(NetModeSwitchProfile);
		break;
#endif
#if (BRIDGE == FYES)
	case MIB_PROF_BRIDGE:
		*_prof = (Pointer*)buildFactoryBridgeProfile(&unionProf);
		size = sizeof(BridgeProfile);
		break;
#endif
#if (CACHE_SERVER == FYES)
	case MIB_PROF_CACHE_SERVER:
		*_prof = (Pointer*)buildFactoryCacheServerProfile(&unionProf);
		size = sizeof(CacheServerProfile);
		break;
#endif
 
#if (FAT_FIT_SWITCH == FYES)
	case MIB_PROF_FAT_FIT_SWITCH:
		*_prof = (Pointer*)buildFactoryFatFitSwitchProfile(&unionProf);
		size = sizeof(FatFitSwitchProfile);
		break;
#endif
#if (AC_WORK_MODE== FYES)
	case MIB_PROF_AC_WORK_MODE:
		*_prof = (Pointer*)buildFactoryAcWorkModeProfile(&unionProf);
		size = sizeof(AcWorkModeProfile);
		break;
#endif

#if (AP_LOAD_BALANCE == FYES)
	case MIB_PROF_AP_LOAD_BALANCE:
		*_prof = (Pointer*)buildFactoryApLoadBalanceProfile(&unionProf);
		size = sizeof(ApLoadBalanceProfile);
		break;
#endif
#if (AP_PERMISSION == FYES)
	case MIB_PROF_AP_PERMISSION:
		*_prof = (Pointer *)buildFactoryApPermissionProfile(&unionProf);
		size = sizeof(ApPermissionProfile);
		break;
#endif
#if (AP_CONF == FYES)
	case MIB_PROF_AP_CONF:
		*_prof = (Pointer *)buildFactoryApConfProfile(&unionProf);
		size = sizeof(ApConfProfile);
		break;
#endif
#if (SERVICE_ZONE == FYES)
	case MIB_PROF_SERVICE_ZONE:
		*_prof = (Pointer *)buildFactoryServiceZoneProfile(&unionProf);
		size = sizeof(ServiceZoneProfile);
		break;
#endif
#if ((FEATURE_AP_MAC_FILTER == FYES)||(FIT_AP_MAC_FILTER == FYES))
	case MIB_PROF_AP_MAC_FILTER:
		*_prof = (Pointer *)buildFactoryApMacFilterProfile(&unionProf);
		size = sizeof(ApMacFilterProfile);
		break;
#endif
#if (TAG_VLAN == FYES)
	case MIB_PROF_TAG_VLAN:
		*_prof = (Pointer *)buildFactoryTagVlanProfile(&unionProf);
		size = sizeof(TagVlanProfile);
		break;
#if (WAN_TAG_VLAN == FYES)
	case MIB_PROF_WAN_TAG_VLAN:
		*_prof = (Pointer *)buildFactoryWanTagVlanProfile(&unionProf);
		size = sizeof(WanTagVlanProfile);
		break;
#endif
#endif

#if (FEATURE_AP_CONF_TEMP == FYES)
	case MIB_PROF_AP_CONF_TEMP:
		*_prof = (Pointer*)buildFactoryApConfTempProfile(&unionProf);
		size = sizeof(ApConfTempProfile);
		break;
#endif
#if (FEATURE_AP_SCAN == FYES)
	case MIB_PROF_AP_SCAN:
		*_prof = (Pointer *)buildFactoryApScanProfile(&unionProf);
		size = sizeof(ApScanProfile);
		break;
#endif
#if (FEATURE_AP_SW_MNG == FYES)
	case MIB_PROF_AP_SW_MNG:
		*_prof = (Pointer *)buildFactoryApSwMngProfile(&unionProf);
		size = sizeof(ApSwMngProfile);
		break;
#endif
#if (FEATURE_AP_BASIC_INFO == FYES)
	case MIB_PROF_AP_BASIC_CONF:
		*_prof = (Pointer *)buildFactoryApBasicConfProfile(&unionProf);
		size = sizeof(ApBasicConfProfile);
		break;
#endif
#if (CAPWAP_CONFIG_AP == FYES)
	case MIB_PROF_AP_CAPWAP_CONFIG:
		*_prof = (Pointer *)buildFactoryApCapwapCongfigProfile(&unionProf);
		size = sizeof(ApCapwapConfigProfile);
		break;
#endif
#if (FEATURE_AP_EFFECT_CONF == FYES)
	case MIB_PROF_AP_EFFECT_CONF:
		*_prof = (Pointer*)buildFactoryApEffectConfProfile(&unionProf);
		size = sizeof(ApEffectConfProfile);
		break;
#endif
#if (FIT_AP_SERVICE_ZONE == FYES)
	case MIB_PROF_FIT_AP_SZ:
		*_prof = (Pointer*)buildFactoryFitApSzProfile(&unionProf);
		size = sizeof(FitApSzProfile);
		break;
#if (WIRELESS_5G == FYES)
	case MIB_PROF_FIT_AP_SZ_5G:
		*_prof = (Pointer*)buildFactoryFitAp5gSzProfile(&unionProf);
		size = sizeof(FitAp5gSzProfile);
		break;
#endif 
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
        case MIB_PROF_AP_BLACK_WHITE_LIST:
		*_prof = (Pointer*)buildFactoryApBWListProfile(&unionProf);
		size = sizeof(ApBlackWhiteListProfile);
		break;
#endif
#if (SYS_LOG_INFO == FYES)
		case MIB_PROF_SYS_LOG_INFO:
		*_prof = (Pointer*)buildFactorySysLogInfoProfile(&unionProf);
		size = sizeof(SysLogInfoProfile);
		break;
#endif
#if (MAC_FILTER == FYES)
	case MIB_PROF_MAC_FILTER:
		*_prof = (Pointer*)buildFactoryMacFilterProfile(&unionProf);
		size = sizeof(MacFilterProfile);
		break;
#endif
#if (UTT_SMART_UI == FYES)
#if (APP_MENU == FYES)
	case MIB_PROF_SHORTCUT_MENU:
		*_prof = (Pointer*)buildFactoryShortCutMenuProfile(&unionProf);
		size = sizeof(ShortCutMenuProfile);
		break;
#endif
#endif
#if (NET_SHARE_MANAGE == FYES)
    case MIB_PROF_NET_SHARE_MANAGE:
		*_prof = (Pointer*)buildFactoryNetShareManageProfile(&unionProf);
		size = sizeof(NetShareManageProfile);
		break;
#endif
#if (FTP_SERVER == FYES)
    case MIB_PROF_FTP_SERVER:
		*_prof = (Pointer*)buildFactoryFtpServerProfile(&unionProf);
		size = sizeof(FtpServerProfile);
		break;
    case MIB_PROF_FTP_SHARE_DIR:
		*_prof = (Pointer*)buildFactoryFtpShareDirProfile(&unionProf);
		size = sizeof(FtpShareDirProfile);
		break;
#endif
#if (NET_SHARE_USER == FYES)
    case MIB_PROF_NET_SHARE_USER:
		*_prof = (Pointer*)buildFactoryNetShareUserProfile(&unionProf);
		size = sizeof(NetShareUserProfile);
		break;
#endif
#if (ADFILTER == FYES)
    case MIB_PROF_ADFILTER:
		*_prof = (Pointer*)buildFactoryAdfilterProfile(&unionProf);
		size = sizeof(AdfilterProfile);
		break;
#endif
#if (MAKE_MONEY == FYES)
    case MIB_PROF_MAKE_MONEY:
                *_prof = (Pointer*)buildFactoryAdfilterProfile(&unionProf);
                size = sizeof(MakeMoneyProfile);
                break;
#endif
#if (CLOUD_MANAGE == FYES)
    case MIB_PROF_CLOUD_MANAGE:
                *_prof = (Pointer*)buildFactoryCloudManageProfile(&unionProf);
                size = sizeof(CloudManageProfile);
                break;
		
#endif
#if (CLOUD_CONFIG == FYES)
    case MIB_PROF_CLOUD_CONFIG:
                *_prof = (Pointer*)buildFactoryCloudConfigProfile(&unionProf);
                size = sizeof(CloudConfigProfile);
                break;
		
#endif
#if (UTT_KNOWIFI == FYES)
    case MIB_PROF_KNOWIFI_DOMAIN:
		*_prof = (Pointer*)buildFactoryKnowifiDomainProfile(&unionProf);
		size = sizeof(KnowifiDomainProfile);
		break;
#endif
        default:
            memset(&unionProf, 0, sizeof(unionProf));
            *_prof = &unionProf;
	    break;
    }
    return size;
} 

void makeFactoryProfiles(void)
{
    Pointer prof, p;
    Uint len, i, num, proftype;
    SlotNumber slot;
    bool bFlag;
    
    for(proftype = 0;proftype < MIB_PROF_MAX;proftype++) {
        if(profileBuildFactory((MibProfileType)proftype, &prof)) {
    	    i = 0;
    	    slot.slotNumber = SLOT_NUMBER_ANY;
    	    do {
    	    	p = _getProfilePointer(slot, (MibProfileType)proftype, i, &len, &num, NULL);
    	    	if(p != NULL) {
                    memcpy(p, prof, len);
                }
    	    } while(++i < num);
    	} 
    }

    /* 
     * Build some "factory" profiles 
     */     
    for(proftype = 0; proftype < MIB_PROF_MAX; proftype++) {     
        i = 0;
        slot.slotNumber = SLOT_NUMBER_ANY;
        do {  
           bFlag = FALSE;
           p = NULL;
             
           bFlag = BuildDefaultFactoryValue((MibProfileType)proftype, i, slot, &prof);
           p = _getProfilePointer(slot, (MibProfileType)proftype, i, &len, &num, NULL);         
           if((bFlag == TRUE) && (p!= NULL)) {
	       memcpy(p, prof, len);
	   }
        } while(++i < num);    
    }
}

#define UTT_MSG_NVRAM_FILE "/tmp/uttMsgforRoute"
static int _msg_inited = 0;
int UttMsgInit(void)
{
    static int mqid;
    key_t mqkey;
    if(_msg_inited == 0) {
        mqkey = ftok(UTT_MSG_NVRAM_FILE, 0);
        if(mqkey == -1){
            return  -1;
        }
        mqid = msgget(mqkey, IPC_CREAT | 0666);
        if(mqid == -1) {
            return  -1;
        }
        _msg_inited = 1;
    }

    return mqid; 
}

#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#define UTT_SHM_NVRAM_FILE "/tmp/uttshmfornvram"
static int _shm_inited = 0;
char* ProfShmInit(void)
{
    static char _shm_inited = 0;
    int shm_id;
    static char *tmp;
    key_t key;
	if(_shm_inited == 0) {
		key = ftok(UTT_SHM_NVRAM_FILE, 0);
    	if(key == -1){
    	    return NULL;
    	}
    	shm_id = shmget(key, sizeof(NvramProfiles), IPC_CREAT|0666);
    	if(shm_id == -1) {
    	    perror("msgget error: ");
    	}
    	tmp = (char*)shmat(shm_id, NULL, 0);
		_shm_inited = 1;
	}
	return tmp; 
}
#else
char *map_addr = NULL;
int map_fd;
unsigned long phymem_addr = 0;
#define PHYMEM_SIZE sizeof(NvramProfiles) 
char* ProfShmInit(void)
{
    int fd = 0;
    char s[256];
    fd = open("/proc/memshare/phymem_addr", O_RDONLY);
    if(fd < 0) {
        printf("cannot open file /proc/memshare/phymem_addr\n"); 
        return 0; 
    }
    read(fd, s, sizeof(s));
    sscanf(s, "%lx", &phymem_addr);
    close(fd);

//    printf("utt addr:%x\n",phymem_addr);
    map_fd = open("/dev/mem", O_RDWR);
    if(map_fd < 0) {
        printf("cannot open file /dev/mem\n");
        return 0;
    }
    map_addr = mmap(NULL, PHYMEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, map_fd, phymem_addr);
    if (map_addr == MAP_FAILED) {
	perror("mmap failed");
	close(map_fd);
	return NULL;
    }
    return map_addr;
}
#endif

unsigned int uttSerioNo;
unsigned int getSystemSn(void)
{
    return uttSerioNo;
}
int ProfInit(void)
{
    int ret = PROFFAIL;
    char *data = NULL;
    int offset = 0;

#if ((CONFIG_X86_CPU) == 1)
    flash_read_mtdname(LICENSE_FILE, &uttSerioNo, 0, 4);
#else

#if (CONFIG_MTD_SPI_RALINK == 1) && (CONFIG_RALINK_MT7620 != 1) && (CONFIG_RALINK_MT7621 != 1) && (CONFIG_RALINK_MT7628 != 1)

    offset = 0x40+0x2c;
#elif (CONFIG_PPC_85xx == 1)
    offset = 0x400+0x2c;
#else
    offset = 0x2c;
#endif

    flash_read_mtdname("Bootloader", &uttSerioNo, offset, sizeof(uttSerioNo));
#endif

    data = ProfShmInit();

    if(data != NULL) {
	nvramProfile = (NvramProfiles *)data;
	ret = PROFOK;
#if 0
	memset(nvramProfile, 0, sizeof(nvramProfile));
#endif
    } 

    if(uttSemInit()>0)
    {
	ret = PROFOK;
    }
    return ret;
}
/*
 *detach the shared memory
 *
 *KeHuatao
 *
 */
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
int  ProfDetach(void)
{
    if(_shm_inited==1)
    {
	 if(shmdt((char*)nvramProfile) == -1)
	 {
	     printf("shared memory detach error\n");
	 }
    }

    return;
}
#else
int  ProfDetach(void)
{
    munmap(map_addr, PHYMEM_SIZE);
    close(map_fd);
}
#endif

void UttSendMsgForSpeed(speedMainType mainType, 
		speedSecType secType, void* data, int size)
{
	static struct st_uttMsg msg;
	int msgID = 0;

	msgID = UttMsgInit();

	if(msgID != -1)
	{
	    msg.mtype = UTT_MSG_TYPE_1;
	    msg.msgPara.mainType = mainType;
	    msg.msgPara.secType = secType;
	    if(data != NULL) {
		memcpy(&(msg.msgPara.data), data, size);
	    }
	    msgsnd(msgID, &msg, sizeof(msg.msgPara), 0);
	}
        else
	{
	    printf("msg init error\n");
	}
	return;
}

void nvramWriteCommit(void)
{
    UttSendMsgForSpeed(MSG_NVRAM, 0, NULL, 0);
}

#if (NOTICE == FYES)
/********************************************************************
 * 函数： saveConfigToFile_skip
 * 功能： 保存title, content参数，蹈袷化通告页面文件中
 * 创建： 2012-03-21
 * 参数： 
 *	  title: 页面提交的通告标题参数
 *	  content: 页面提交的通告内容参数
 *	  skiptime: 自动跳转时间
 *	  skipurl: 自动跳转url
 *	  path: 通告页面路径
 * 返回： 成功，失败
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int saveConfigToFile_skip(char *title, char *content, char *skiptime, char *skipurl, const char *source, const char *path)
{
    /* 
     * 将输入参数保存到页面配置文件 
     * 1 读取模板文件 /etc_ro/web/Notify.htm，宏NOTICE_HTML_FORMAT
     * 2 将传入参数和模板文件按格式化合并，输出到通告文件path
     */
    FILE	*fp = NULL;
    Uchar	autoskip[NOTIFY_AUTO_SKIP_SIZE];
    Uchar	bufback[ NOTIFY_HTM1_MOUDLE_SIZE ] = {0u};
    Uchar	buf[ NOTIFY_HTM1_SIZE ] = {0u};
    char	*ptitle = NULL;
    char	*pcontent = NULL;
    char	*pskiptime = NULL;
    char	*pskipurl = NULL;
    char	*pskipurl2 = NULL;
    uint	rc = FAIL;

    if((title != NULL) && (content != NULL) && (path != NULL) 
	    && (NOTICE_CONTENT_LEN >= strlen(content)) && (NOTICE_TILTLE_LEN >= strlen(title))) {

	fp = fopen(source, "r");
	if((fp != NULL)) {
	    if(strcmp(skiptime,"") == 0){
		snprintf(autoskip, NOTIFY_AUTO_SKIP_SIZE, "<meta http-equiv=\"content-type\" content=\"%s;url=http://%s\">", skiptime, skipurl);
	    }else{
		snprintf(autoskip, NOTIFY_AUTO_SKIP_SIZE, "<meta http-equiv=\"refresh\" content=\"%s;url=http://%s\">", skiptime, skipurl);
	    }    
	    fread((void *)bufback, sizeof(char), NOTIFY_HTM1_MOUDLE_SIZE, fp);
	    fclose( fp );
	    SWORD_PRINTF("bufback:%s\n", (char*)bufback);

	    ptitle = strstr( bufback,"%s");
	    if(NULL != ptitle) {
		*ptitle = '\0';
		SWORD_PRINTF("bufback1:%s\n", (char*)bufback);

		pcontent = strstr((ptitle+2), "%s");
		if(pcontent != NULL)
		{
		    *pcontent = '\0';
		    SWORD_PRINTF("ptitle + 2:%s\n", ptitle + 2);
		    pskiptime = strstr((pcontent + 2), "%s");
		    if (pskiptime != NULL) {
			*pskiptime = '\0';
			SWORD_PRINTF("pcontent + 2:%s\n", pcontent + 2);
			pskipurl = strstr((pskiptime + 2), "%s");
			if (pskipurl != NULL) {
			    *pskipurl = '\0';
			    pskipurl2 = strstr((pskipurl + 2), "%s");
			    if (pskipurl2 != NULL) {
				*pskipurl2 = '\0';
				SWORD_PRINTF("pskiptime + 2:%s\n", pskiptime + 2);
				SWORD_PRINTF("pskipurl + 2:%s\n", pskipurl + 2);
				strcat(buf, autoskip);
				strcat(buf, bufback);
				strcat(buf, title);
				strcat(buf, ptitle + 2);
				strcat(buf, content);
				strcat(buf, pcontent + 2);
				strcat(buf, skiptime);
				strcat(buf, pskiptime + 2);
				strcat(buf, skipurl);
				strcat(buf, pskipurl + 2);
				strcat(buf, skipurl);
				strcat(buf, pskipurl2 + 2);

				fp = fopen( path, "w" );
				if( NULL != fp )
				{
				    SWORD_PRINTF("buf:%s\n", (char*)buf);
				    fwrite( (void*)buf, sizeof(char),  strlen((char*)buf), fp );
				    fclose( fp );
				    rc = SUCCESS;
				}
			    }
			}
		    }
		}
	    }
	}
    }
    return (int)rc;
}

/********************************************************************
 * 函数： saveConfigToFile 
 * 功能： 保存title, content参数，蹈袷化通告页面文件中
 * 创建： 2012-03-21
 * 参数： 
 *	  title: 页面提交的通告标题参数
 *	  content: 页面提交的通告内容参数
 *	  path: 通告页面路径
 * 返回： 成功，失败
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int saveConfigToFile(char *title, char *content, const char *source, const char *path)
{
#if 0
	/* 
	 * 将输入参数保存到页面配置文件 
	 * 1 读取模板文件 /etc_ro/web/Notify.htm，宏NOTICE_HTML_FORMAT
	 * 2 将传入参数和模板文件按格式化合并，输出到通告文件path
	 */
#endif	
	FILE	*fp = NULL;
	Uchar	bufback[ NOTIFY_HTM1_MOUDLE_SIZE ] = {0u};
	Uchar	buf[ NOTIFY_HTM1_SIZE ] = {0u};
	char	*ptitle = NULL;
	char	*pcontent = NULL;
	uint	rc = FAIL;
	uint	notify_htm1_size = 0u;

#if 0
	notify_htm1_size = NOTICE_TILTLE_LEN  + NOTICE_MANAGER_LEN + NOTICE_CONTENT_LEN + NOTIFY_HTM1_MOUDLE_SIZE
#endif
	if( (NULL != title) && (NULL != content) && (NULL != path) 
	    && ( NOTICE_CONTENT_LEN >= strlen(content) ) && ( NOTICE_TILTLE_LEN >= strlen(title) ) )
	{
				
	    fp = fopen( source, "r" );
	    if( (fp != NULL) ) 
	    {
		fread( (void *)bufback, sizeof(char), NOTIFY_HTM1_MOUDLE_SIZE, fp );
		fclose( fp );
		SWORD_PRINTF( "bufback:%s\n", (char*)bufback );
		
		ptitle = strstr( bufback, "%s" );
		if( NULL != ptitle )
		{
			*ptitle = '\0';
			SWORD_PRINTF( "bufback1:%s\n", (char*)bufback );
	
			pcontent = strstr( (ptitle+2), "%s" );
			if( NULL != pcontent )
			{
				*pcontent = '\0';
				SWORD_PRINTF( "ptitle+2:%s\n", ptitle+2 );
				SWORD_PRINTF( "pcontent+2:%s\n", pcontent+2 );
		
				strcat( buf, bufback );
				strcat( buf, title );
				strcat( buf, ptitle+2 );
				strcat( buf, content );
				strcat( buf, pcontent+2 );
		
				fp = fopen( path, "w" );
				if( NULL != fp )
				{
					SWORD_PRINTF("buf:%s\n", (char*)buf);
					fwrite( (void*)buf, sizeof(char),  strlen((char*)buf), fp );
					fclose( fp );
					rc = SUCCESS;
				}
			}
		}
	    }
	}
	return (int)rc;
}
#endif

#if (WEB_AUTH == FYES)
/***********************************************************\
 * 函数功能：将字符串保存到文件中
 * 创建时间：2013-03-24
 * 输入参数：content--保存的字符串 path--文件路径
 **********************************************************/
extern int saveContactToFile(char *content, const char *path)
{
    FILE *fp = NULL;
    if ((content != NULL) && (path != NULL))
    {
	fp = fopen( path, "w");
	if (fp != NULL)
	{
	    fwrite(content, sizeof(char), strlen(content)+1, fp);
	    fclose(fp);
	    return 0;
	}		
    }
    return 1;
}

/***********************************************************\
 * 函数功能：从文件中读字符串
 * 创建时间：2013-03-24
 * 输入参数：content--保存字符串 path--文件路径
 **********************************************************/
extern int readContactFromFile(char *content, const char *path, int size)
{    
    FILE *fp = NULL;
    if ((content != NULL) && (path != NULL))
    {
	fp = fopen( path, "r");
	if (fp != NULL)
	{
	    fread((void*)content, sizeof(char), size, fp);
	    fclose(fp);
	    return 0;
	}		
    }
    return 1;
}
#endif

/**
 * 移除字符串中的指定字符（该函数会改变源字符串）
 * sourceStr:源字符串 removeChar:要移除的字符
 * return ：移除的字符个数
 * bhou
 * 2011-10-26
 */
extern unsigned int strRemoveChar(char *sourceStr, char removeChar)
{
    unsigned int i = 0u, j = 0u, delN = 0u;
    int len = strlen(sourceStr);

    for(i = 0u;i < len; i++)
    {
	if(sourceStr[i] != removeChar)/*不等于要删除的字符*/
	{
	    sourceStr[j++] = sourceStr[i];/*不用删除的字符前移*/
	}
	
    }
    /*以上循环结束后，j为新字符串的长度*/
    delN = len - j;/*删除的字符个数*/

    memset(&sourceStr[j], 0, delN);/*本次移除操作不用的空间，必须清零*/
    return delN;

}

/***********************************************************************
 * 函 数 名：   my_strlwr
 * 功能描述：	把一个字符串中的大写字母转换成小写字母
 * 输入参数:    str - 待转换的字符串
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
char * my_strlwr(char *str)
{
   char *p = str;
   while (*p != '\0')
   {
      if(*p >= 'A' && *p <= 'Z')
        *p = (*p) + 0x20;
      p++;
    }
  return str;
}
#ifdef CONFIG_UTT_DEBUG
void debug_print(Pointer para, const char *fmt, ...)
{
    va_list   args;
    FILE *fp = NULL;

    if(((DebugPara*)para)->printEn == 1) {
	va_start(args, fmt);
	if ((fp = fopen(nvramProfile->uttDebug.tty_str, "w")) != NULL) {
	    vfprintf(fp, fmt, args);
	    fclose(fp);
	}
	va_end(args);
    }
    return;
}
#endif

#if 0
#define PROF_LIST_HD_FMT	"\n%-3s %-12s %-12s %-7s %-12s %-8s %-5s %-11s\n"
#define PROF_LIST_FMT		"\n%-3d %-12d %-12d %-7d %-12p %-8d %-5d %-11s"

static int
_profilePointerListDump()
{
    ProfPointer*        pp;
    Uint32 i;

    printf( PROF_LIST_HD_FMT, 
		"ID",
		"MibProfType",
		"ProfPtrType",
		"usecnt",
		"Pointer",
		"slot",
		"index",
		"Age");
    for( i = 0, pp = _profPointerHead; pp; pp = pp->next ) {
      i++;
      printf( PROF_LIST_FMT, 
		i,
		pp->pType,
		pp->pPtrType,
		pp->usecnt,
		pp->profile,
		pp->slot,
		pp->index,
		timeSeconds2Str(sysUpTime() - pp->creTm));
#ifdef _DEEP_PROF_DEBUG
        printf("\n\tTrace: ");
        for ( int i =0; i < TRACE_SIZE; i ++) 
          printf( "%p ", pp->prof_stack[i]);
#endif
    }
    printf( "\n\n %s: old profHiWa : %lu, current prof cnt is %lu\n\n",
		__FUNCTION__,
		_profilePointerHiWater,
		i 
            );
    printf( "\n\n %s: _profileReqCnt: %lu, _profileRelCount: %lu\n\n",
		__FUNCTION__,
		_profileReqCount,
		_profileRelCount
            );
    if ( i > _profilePointerHiWater ) {
      _profilePointerHiWater = i;
    }
    return i;
}

#define PROF_REG_HD_FMT		"%3s %7s %3s %8s %7s %8s %10s %10s %12s\n"
#define PROF_REG_FMT		"%3d %7d %3d %8p %7d %8p %10d %10d %12d\n"

static void 
_profileUpdateListDump()
{

    Registrant*			curRegistrant;
    int index;
    int seq;

    index = 0;
    seq = 0;

    /* Traverse the linked list of registered entities calling each
       one with the wonderful news. */

    printf( "\n");
    printf( PROF_REG_HD_FMT, "idx", "mibtype", "seq", "tag", "oldType", "regFnc", "age(10ms)", "use", "lastTm(10ms)");
    
    for ( int mibType =0; mibType < MIB_PROF_MAX; mibType++) {
      curRegistrant = _typeHeads[ mibType ];
      seq = 0;
      while( curRegistrant ) {
        printf( PROF_REG_FMT, 
			index, mibType, seq, 
			curRegistrant->updateTag, curRegistrant->oldProfileType, curRegistrant->updateFnc,
			getMasterClock()-curRegistrant->rCreTm,
			curRegistrant->rUseCnt,
			getMasterClock()-curRegistrant->rLastTm
		);
	curRegistrant = curRegistrant->next;
	seq++;
	index++;
      }
    }
}

    /*
     *  Release a profile buffer.
     *
     *  Two varieties of request are handled here.  One is to
     *  release a particular buffer, the other to release one
     *  or more of a particular type of profile where the buffer
     *  address is not known (e.g., release the SystemProfile buffer).
     *  This is a bit of a hack.
     *
     *  Note: we tolerate invalid pointers here,
     *  because there are places in the user code where
     *  the source of the pointer is lost.  We have to
     *  attempt a free, to catch the cases where it is here.
     */

static void
_releaseProfilePointer( Uint pType,
                        ConstPtr  profile )
{
    _profileRelCount++;
    profile = NULL;
}

#if 0
#if (OBJECT_GROUP == FYES)	/*add by brwang*/
extern const IpGroupProfile*
requestIpGroupProfilePointer( Word index )
{
    IpGroupProfile     *profileData;
    if( _returnProfilePointer( _slotAny, MIB_PROF_IP_GROUP, index, (Pointer *) &profileData ) )
    {
        return profileData;
    }
   return NULL;
}

extern const PortGroupProfile*
requestPortGroupProfilePointer( Word index )
{
   PortGroupProfile     *profileData;
   if( _returnProfilePointer( _slotAny, MIB_PROF_PORT_GROUP, index, (Pointer *) &profileData ) ) 
   {
        return profileData;
   }
   return NULL;
}

extern void
releaseIpGroupProfilePointer( const IpGroupProfile* profileData )
{
    _releaseProfilePointer( MIB_PROF_IP_GROUP, profileData );

}

extern void
releasePortGroupProfilePointer( const PortGroupProfile* profileData )
{
    _releaseProfilePointer( MIB_PROF_PORT_GROUP, profileData );

}

#endif
#endif
#endif

#if 0
    /*
     * A system task whose job it is to free profile buffers
     * that have been allocated.  We free all 'dynamic' profile 
     * bufs on the list, i.e., those not marked STATIC.
     * We ignore the use count, because none can hold a pointer
     * across a task switch.
     */
static void
profFreeTask( Pointer argPtr )
{
    MibProfileType	profType;
    ProfPointer*	pp;
    ProfPointer**	ppPrev;
    ProfPointer*	ppNext;

    while ( 1 ) {
	profType = (MibProfileType) (Uint) recvMsg( profFreeMailbox );

	for( ppPrev = &_profPointerHead, pp = _profPointerHead;
	     pp; pp = ppNext ) {
	    ppNext = pp->next;

	    /* Don't free STATIC pointers - those are explicitly freed by
	       the requestor, or when the profile is written.  */

	    if( pp->pPtrType != PROF_STATIC ) {
		*ppPrev = ppNext;
		_freeProfile( pp->profile );
		FREEBUFF( _profPointerPool, pp );
	 	_profilePointerTaskFreeCount ++;
		_profilePointerFreeCount ++ ;
	    } else {
		ppPrev = &pp->next;
	    }
	}
    }
    (void) argPtr;
}



    /*
     * Initialization function called from initram.c.  Just initialize global
     * data -- the pool of registrants and the heads of the linked lists.
     */
#if ( SCAVENGE_RAM == FYES )
extern void
initProfileAccess( void )__attribute__ ((section (".init")));
#endif /* SCAVENGE_RAM */
extern void
initProfileAccess( void )
{
    extern void  mibgenInit( void );

    int		i;    

    bootPrint( "init profile...\n");
    
    for( i = 0 ; i < MIB_PROF_MAX ; i++ ) {
        _typeHeads[ i ] = NULL;
        _profPointerHead = NULL;
    }
    _registrantPool = makePool( sizeof( Registrant ),
				   MIB_PROF_MAX,
    				   TRUE, MIB_PROF_MAX,
				   "ProfUpdate",
				   POOL_FLAGS_NONE );

    _profPointerPool = makePool( sizeof( ProfPointer ),
				    1, TRUE, 1,
				   "ProfPointer",
				   POOL_FLAGS_MUST_FULFILL );

    _profBuffPool = makePool( sizeof( ProfileUnionList ),
				   1, TRUE, 1,
				   PROF_UNION_BUF_NAME,
				   POOL_FLAGS_NONE );
#if defined(LOCAL_FREE_LIST)
    _profFreeHead = NULL;
# if (PLATFORM & MPORT)
    for (i=0; i < 7; i++ ) {
	ProfileUnionList* prof = GETBUFF(ProfileUnionList, _profBuffPool );
	if ( !prof ) {
	    break;
	}
	_freeProfile(prof);
    }
# endif /* (PLATFORM & MPORT) */
#endif /* defined(LOCAL_FREE_LIST) */
    
    mibgenInit();
}
#endif

#if(CWMP == FYES)
#define MAX_TEXT 512  
#define UTT_MSG_CWMP_FILE "/tmp/uttMsgforCwmp"
static int _cwmp_msg_inited = 0;

struct msg_st  
{
    long int msg_type;  
    char text[MAX_TEXT];  
};  

int cwmp_msg_init(void)
{
    static int cwmp_mqid;
    key_t mqkey;
    if(_cwmp_msg_inited == 0) {
        mqkey = ftok(UTT_MSG_CWMP_FILE, 0);
        if(mqkey == -1){
            return  -1;
        }
        cwmp_mqid = msgget(mqkey, IPC_CREAT | 0666);
        if(cwmp_mqid == -1) {
            return  -1;
        }
        _cwmp_msg_inited = 1;
    }

    return cwmp_mqid;
}

void cwmp_value_change_notice()
{
	int msgID = 0;
    struct msg_st data;  

	msgID = cwmp_msg_init();
    if(msgID != -1)
    {
        data.msg_type = 1;
        strcpy(data.text, "value change");  
        if(msgsnd(msgID, (void*)&data, MAX_TEXT, 0) == -1)  
        {  
            printf("cwmp msgsnd failed\n");  
            return 0;
        }  
    }else{
	    printf("cwmp msg init error\n");
	}
}
#endif
