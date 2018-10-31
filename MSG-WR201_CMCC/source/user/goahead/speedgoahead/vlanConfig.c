
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"


#if (VLAN_CONFIG == FYES)
static void vlanConfigAdd(VlanConfigProfile *prof);
#if (WIFIDOG_AUTH == FYES) && defined(WIFI_PRE_REDIRECT)
extern int wifiUpdateRule_VlanConfig(char action,int index,unsigned int ip, int cidr);
#endif
static void vlanConfigInit()
{
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
	unsigned int iIpStart0 = 0U;
	char ip0[16];

	memset(ip0, 0, sizeof(ip0));
#endif
	int min, max, index = 0;
	MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
	VlanConfigProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			vlanConfigAdd(prof);
#if 0
			if (prof->vlanEn == PROF_ENABLE) {
				doSystem("vconfig add %s %d", getLanIfName(), prof->vid);
				doSystem("ifconfig %s.%d up", getLanIfName(), prof->vid);
				doSystem("ifconfig %s.%d %N netmask %N", getLanIfName(), prof->vid, prof->ip, prof->netmask);
			}
#if (IP_GRP == FYES)
		    iIp0 = htonl(prof->ip);
			iIpStart0 = (prof->ip & prof->netmask);
			if(iIp0 != 0U)
			{
				/*接口启用了*/
			    int2ip(iIp0, ip0);
			    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
			    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
			}
#endif
#endif
		}
	}

	/*解决大包传输问题*/
	doSystem("iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 1>/dev/null 2>&1");
	return;
}

static void vlanConfigAdd(VlanConfigProfile *prof)
{
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
	unsigned int iIpStart0 = 0U;
	char ip0[16];

	memset(ip0, 0, sizeof(ip0));
#endif

    InterfaceProfile *profIf = NULL;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/
    char tmpName[16];

		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {
			if (prof->vlanEn == PROF_ENABLE) {
				doSystem("vconfig add %s %d", getLanIfName(), prof->vid);
				doSystem("ifconfig %s.%d up", getLanIfName(), prof->vid);
				doSystem("ifconfig %s.%d %N netmask %N", getLanIfName(), prof->vid, prof->ip, prof->netmask);

				if((profIf != NULL) && (ProfInstIsFree(profIf) == 0)) /*配置已用*/
				{
				    if (profIf->arpAllowOtherEn == FUN_DISABLE){
				        sprintf(tmpName,"%s.%d",getLanIfName(),prof->vid);
				        doSystem("arptables -A INPUT -j DROP -i %s", tmpName);
				        doSystem("ip neigh flush dev %s 2>/dev/null",tmpName);/*清除arp，让他立即生效*/
				    }
				}

#if (IP_GRP == FYES)
				iIp0 = htonl(prof->ip);
				iIpStart0 = (prof->ip & prof->netmask);
				if(iIp0 != 0U)
				{
					/*接口启用了*/
				    int2ip(iIp0, ip0);
				    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
				    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
				}
#endif
			}

		}

	return;
}

static void vlanConfigDel(VlanConfigProfile *prof)
{
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
	unsigned int iIpStart0 = 0U;
	char ip0[16];

	memset(ip0, 0, sizeof(ip0));
#endif
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof))) {
			if (prof->vlanEn == PROF_ENABLE) {
#if (IP_GRP == FYES)
				iIp0 = htonl(prof->ip);
				iIpStart0 = (prof->ip & prof->netmask);
				if(iIp0 != 0U)
				{
					/*接口启用了*/
				    int2ip(iIp0, ip0);
				    ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_DEL);/*加入组*/
				    ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_DEL);
				}
#endif
				doSystem("arptables -D INPUT -j DROP -i %s.%d 2>/dev/null",getLanIfName(),prof->vid);
				doSystem("vconfig rem %s.%d &", getLanIfName(), prof->vid);
			}
		}

	return;
}

static void	vlanConfigEdit(VlanConfigProfile *oldProf, VlanConfigProfile *newProf)
{
	vlanConfigDel(oldProf);
	vlanConfigAdd(newProf);

	return;
}

void vlanConfigProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const VlanConfigProfile* oldProfileDataPtr, const VlanConfigProfile* newProfileDataPtr )
{
    switch(changeType) {
		case PROF_CHANGE_ADD:
		    vlanConfigAdd(newProfileDataPtr);
#if (WIFIDOG_AUTH == FYES) && defined(WIFI_PRE_REDIRECT)
		     wifiUpdateRule_VlanConfig(Iptables_INS,newProfileDataPtr->vid+4,(newProfileDataPtr->ip&newProfileDataPtr->netmask),headbits(htonl(newProfileDataPtr->netmask))%100);
#endif
			break;
        case PROF_CHANGE_EDIT:
#if (WIFIDOG_AUTH == FYES) && defined(WIFI_PRE_REDIRECT)
		     wifiUpdateRule_VlanConfig(Iptables_DEL,oldProfileDataPtr->vid+4,(oldProfileDataPtr->ip&oldProfileDataPtr->netmask),headbits(htonl(oldProfileDataPtr->netmask))%100);
#endif
			vlanConfigEdit(oldProfileDataPtr, newProfileDataPtr);
#if (WIFIDOG_AUTH == FYES) && defined(WIFI_PRE_REDIRECT)
		     wifiUpdateRule_VlanConfig(Iptables_INS,newProfileDataPtr->vid+4,(newProfileDataPtr->ip&newProfileDataPtr->netmask),headbits(htonl(newProfileDataPtr->netmask))%100);
#endif
			break;
		case PROF_CHANGE_DEL:
		case PROF_CHANGE_DELALL:
#if (WIFIDOG_AUTH == FYES) && defined(WIFI_PRE_REDIRECT)
		     wifiUpdateRule_VlanConfig(Iptables_DEL,oldProfileDataPtr->vid+4,(oldProfileDataPtr->ip&oldProfileDataPtr->netmask),headbits(htonl(oldProfileDataPtr->netmask))%100);
#endif
			vlanConfigDel(oldProfileDataPtr);
			break;
        default:
            break;
    };
}
#if(IP_GRP == FYES)
void add_vlan_sys_lan_group()
{
    unsigned int iIp0 = 0U;
    unsigned int iIpStart0 = 0U;
    int min, max, index = 0;
    char ip0[16];

    MibProfileType mibType = MIB_PROF_VLAN_CONFIG;
    VlanConfigProfile *prof = NULL;
    memset(ip0, 0, sizeof(ip0));
    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++) {
	prof = (VlanConfigProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active)) {				iIp0 = htonl(prof->ip);
	    iIp0 = htonl(prof->ip);
	    iIpStart0 = (prof->ip & prof->netmask);
	    if(iIp0 != 0U)
	    {
		/*接口启用了*/
		int2ip(iIp0, ip0);
		ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
		ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
	    }
	}
    }
}
#endif
void funInitVlanConfig()
{
	registerForProfileUpdates(MIB_PROF_VLAN_CONFIG, NULL, vlanConfigProfileChange);
	vlanConfigInit();
}

#endif
