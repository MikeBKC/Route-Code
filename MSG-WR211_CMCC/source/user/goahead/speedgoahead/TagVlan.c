
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

#if (TAG_VLAN == FYES)
extern int headbits(unsigned int nm);

static void TagVlanInit();
static void TagVlanAdd(TagVlanProfile *);
static void TagVlanEdit(TagVlanProfile *, TagVlanProfile *);
static void TagVlanDel(TagVlanProfile *);
#define TAG_VLAN_FORWARD_CHAIN "tag_vlan_isolation_chain"
extern void pvidSet(unsigned int port, unsigned int pvid);
extern void tagVlanConfig(int op, Uint32 vid, Uint32 mbrmsk, Uint32 untagmsk);
static void vlanCommunicateInit(void);
extern unsigned int uttSerioNo;

static void TagVlanInit()
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    TagVlanProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++) {
	prof = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (prof->head.active == 1)) {
	    TagVlanAdd(prof);
	}
    }
#if (TAG_VLAN == FYES)
    /* 新建vlan路由隔离规则 */
    doSystem("iptables -D FORWARD -j %s > /dev/null 2>&1", TAG_VLAN_FORWARD_CHAIN);
    doSystem("iptables -F %s > /dev/null 2>&1", TAG_VLAN_FORWARD_CHAIN);
    doSystem("iptables -X  %s > /dev/null 2>&1", TAG_VLAN_FORWARD_CHAIN);

    doSystem("iptables -N %s", TAG_VLAN_FORWARD_CHAIN);
    doSystem("iptables -A FORWARD -j %s", TAG_VLAN_FORWARD_CHAIN);
    vlanCommunicateInit();
#endif

    return;
}

static void TagVlanAdd(TagVlanProfile *prof)
{
    unsigned int tmpsn;
    char tmpName[16];
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
    unsigned int iIpStart0 = 0U;
    char ip0[16];

    memset(ip0, 0, sizeof(ip0));
#endif

    InterfaceProfile *profIf = NULL;
    profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/

    //printf("%s, %d, vid = %d, mbrmsk=0x%x, untagmsk=0x%x\n", __func__, __LINE__, prof->vid, prof->mbrmsk, prof->untagmsk);
    tagVlanConfig(1, prof->vid, (0x100 | prof->mbrmsk), prof->untagmsk);
    doSystem("vconfig add %s %d", "eth2", prof->vid);
    /* 设置mac */
    tmpsn = uttSerioNo + prof->vid;
    doSystem("ifconfig %s.%d hw ether 00:22:aa:%02x:%02x:%02x", "eth2", prof->vid, (0xFF & (tmpsn >> 16)), (0xFF & (tmpsn >> 8)), (0xFF & tmpsn));
    doSystem("ifconfig %s.%d up", "eth2", prof->vid);
    doSystem("ifconfig %s.%d %N netmask %N", "eth2", prof->vid, prof->gw, prof->netmask);


    if((profIf != NULL) && (ProfInstIsFree(profIf) == 0)) /*配置已用*/
    {
	if (profIf->arpAllowOtherEn == FUN_DISABLE){
	    sprintf(tmpName,"%s.%d",getLanIfName(),prof->vid);
	    doSystem("arptables -A INPUT -j DROP -i %s", tmpName);
	    doSystem("ip neigh flush dev %s 2>/dev/null",tmpName);/*清除arp，让他立即生效*/
	}
    }


#if (IP_GRP == FYES)
    iIp0 = htonl(prof->gw);
    iIpStart0 = (prof->gw & prof->netmask);
    if(iIp0 != 0U)
    {
	/*接口启用了*/
	int2ip(iIp0, ip0);
	ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_ADD);/*加入组*/
	ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_ADD);
    }
#endif

    return;
}

static void TagVlanEdit(TagVlanProfile *oldProf, TagVlanProfile *newProf)
{

    TagVlanDel(oldProf);
    TagVlanAdd(newProf);

    return;
}

static void TagVlanDel(TagVlanProfile *prof)
{
#if(IP_GRP == FYES)
    unsigned int iIp0 = 0U;
    unsigned int iIpStart0 = 0U;
    char ip0[16];

    memset(ip0, 0, sizeof(ip0));
#endif

#if (IP_GRP == FYES)
    iIp0 = htonl(prof->gw);
    iIpStart0 = (prof->gw & prof->netmask);
    if(iIp0 != 0U)
    {
	/*接口启用了*/
	int2ip(iIp0, ip0);
	ipsetAOrDIpStr(SYS_LAN_GROUP, ip0, IPSET_MEM_DEL);/*加入组*/
	ipsetAOrDNet(SYS_LAN_RANGE_GROUP, iIpStart0, headbits(htonl(prof->netmask))%100, IPSET_MEM_DEL);
    }
#endif
    doSystem("arptables -D INPUT -j DROP -i %s.%d 2>/dev/null",getLanIfName(),prof->vid);
    //printf("%s %d, eth=%s,vid=%d\n", __func__, __LINE__, "eth2", prof->vid);

    doSystem("vconfig rem %s.%d", "eth2", prof->vid);
    //printf("%s, %d, vid=%d,mbrmsk=0x%x,untagmsk=0x%x\n", __func__, __LINE__, prof->vid, prof->mbrmsk, prof->untagmsk);
    tagVlanConfig(0, prof->vid, (0x100 | prof->mbrmsk), prof->untagmsk);

    return;
}

void TagVlanChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const TagVlanProfile* oldProfileDataPtr, const TagVlanProfile* newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    TagVlanAdd((TagVlanProfile *)newProfileDataPtr);
	    vlanCommunicateInit();
	    break;
	case PROF_CHANGE_EDIT:
	    TagVlanEdit((TagVlanProfile *)oldProfileDataPtr, (TagVlanProfile *)newProfileDataPtr);
	    vlanCommunicateInit();
	    break;
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	    TagVlanDel((TagVlanProfile *)oldProfileDataPtr);
	    break;
	default:
	    break;
    };
}
static void vlanCommunicateInit(void) {
    MibProfileType mibType = MIB_PROF_TAG_VLAN;
    int min, max, i, j;
    TagVlanProfile *profi = NULL, *profj = NULL;
    char *lan_if;

    /*
     * 清空ipables规则表
     */
    doSystem("iptables -F %s", TAG_VLAN_FORWARD_CHAIN);

    lan_if = "eth2";

    ProfInstLowHigh(mibType, &max, &min);

    /*
     * 循环所有vlan实例，如果vlan1 vlan2不能通讯，通过
     * iptables -i eth_vlan1 -o eth_vlan2 -j DROP 和 iptables -i eth_vlan2 -o eth_vlan1 -j DROP隔离开
     */
    for (i = min; i < max; i++) { /* 跳过 default, 遍历其他实例 */
	profi = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, i);
	if (ProfInstIsFree(profi) == 1) {
	    continue;
	}
	if (profi->cc_vlan == 1) { /* 可通信vlan：禁止和不可通信vlan通信 */
	    for (j = i + 1; j < max; j++) {
		profj = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, j);
		if ((ProfInstIsFree(profj) == 0) && (profj->cc_vlan == 0)) {
		    doSystem("iptables -A %s -i %s.%d -o %s.%d -j DROP", TAG_VLAN_FORWARD_CHAIN, lan_if, profi->vid, lan_if, profj->vid);
		    doSystem("iptables -A %s -i %s.%d -o %s.%d -j DROP", TAG_VLAN_FORWARD_CHAIN, lan_if, profj->vid, lan_if, profi->vid);
		}
	    }
	} else { /* 不可通信vlan: 和所有vlan都不能通信 */
	    for (j = i + 1; j < max; j++) {
		profj = (TagVlanProfile *)ProfGetInstPointByIndex(mibType, j);
		if (ProfInstIsFree(profj) == 0) {
		    doSystem("iptables -A %s -i %s.%d -o %s.%d -j DROP", TAG_VLAN_FORWARD_CHAIN, lan_if, profi->vid, lan_if, profj->vid);
		    doSystem("iptables -A %s -i %s.%d -o %s.%d -j DROP", TAG_VLAN_FORWARD_CHAIN, lan_if, profj->vid, lan_if, profi->vid);
		}
	    }
	}
    }

    return;
}

static void pvidProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    SystemProfile *newProf = (SystemProfile *)newProfileDataPtr;
    SystemProfile *oldProf = (SystemProfile *)oldProfileDataPtr;

    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	    //printf("%s, %d, pvid1 = %d, pvid2 = %d, pvid3 = %d, pvid4 = %d\n", __func__, __LINE__, newProf->pvid.pvid1, newProf->pvid.pvid2, newProf->pvid.pvid3, newProf->pvid.pvid4);
	    if (newProf->pvid.pvid1 != oldProf->pvid.pvid1) {
		pvidSet(0, newProf->pvid.pvid1);
	    }
	    if (newProf->pvid.pvid2 != oldProf->pvid.pvid2) {
		pvidSet(1, newProf->pvid.pvid2);
	    }
	    if (newProf->pvid.pvid3 != oldProf->pvid.pvid3) {
		pvidSet(2, newProf->pvid.pvid3);
	    }
	    if (newProf->pvid.pvid4 != oldProf->pvid.pvid4) {
		pvidSet(3, newProf->pvid.pvid4);
	    }
	    break;
	default:
	    break;
    }
    return;
}

static void pvidInit(void) {
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    profSys = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);

    switch(profSys->wanIfCount) {
	case 1:
	    pvidSet(0, profSys->pvid.pvid1);
	    pvidSet(1, profSys->pvid.pvid2);
	    pvidSet(2, profSys->pvid.pvid3);
	    pvidSet(3, profSys->pvid.pvid4);

	    break;
	case 2:
	    pvidSet(0, profSys->pvid.pvid1);
	    pvidSet(1, profSys->pvid.pvid2);
	    pvidSet(2, profSys->pvid.pvid3);

	    break;
	case 3:
	    pvidSet(0, profSys->pvid.pvid1);
	    pvidSet(1, profSys->pvid.pvid2);

	    break;
	case 4:
	    pvidSet(0, profSys->pvid.pvid3);
	    break;
	default:
	    break;
    }
}

extern void tagVlanRouteChg(void) {
    vlanCommunicateInit();
}

#if (WAN_TAG_VLAN == FYES)
/* 
 * 配置tag vlan之后需要重启设备；
 * 此函数只要在设备初始化调用即可
 */
extern void wanTagVlanInit(void) {
    MibProfileType mibType = MIB_PROF_WAN_TAG_VLAN;
    WanTagVlanProfile *prof = NULL;
    prof = (WanTagVlanProfile *)ProfGetInstPointByIndex(mibType, 0);
    Uint32 mbrmsk = 0x200 | 0x10;
    Uint32 untagmsk;

    if (prof->head.active == FUN_ENABLE) {
	pvidSet(4, prof->vid);
	if (prof->tag == 1) {/* wan tagged */
	    untagmsk = 0x0;
	} else {
	    untagmsk = 0x10;
	}
	tagVlanConfig(1, prof->vid, mbrmsk, untagmsk);
	doSystem("vconfig add %s %d", "eth3", prof->vid);
    }
    /*
       wan tag vlan需要功能重启生效，所以不需要处理disable的情况  
    else {
	pvidSet(4, 1);
        tagVlanConfig(0, prof->vid, 0, 0);
    }
    */

    return;
}

#if 0
void WanTagVlanProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr ) {
    WanTagVlanProfile *newProf = (WanTagVlanProfile *)newProfileDataPtr;
    WanTagVlanProfile *oldProf = (WanTagVlanProfile *)oldProfileDataPtr;

    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	    if (newProf->head.active == FUN_ENABLE) {
		wanTagVlanInit();
	    }
	    break;
	default:
	    break;
    }

    return;
}
#endif
#endif

void funInitTagVlan()
{
    registerForProfileUpdates(MIB_PROF_TAG_VLAN, NULL, (ProfileUpdateFnc)TagVlanChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)pvidProfileChange);
#if 0
#if (WAN_TAG_VLAN == FYES)
    registerForProfileUpdates(MIB_PROF_WAN_TAG_VLAN, NULL, (ProfileUpdateFnc)WanTagVlanProfileChange);
#endif
#endif
    pvidInit();
    TagVlanInit();
}

#endif
