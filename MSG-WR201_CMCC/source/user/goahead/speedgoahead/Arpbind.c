/*arp绑定相关
 * bhou
 * 2011-10-25*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "internet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#if (ARP_BINDS == FYES)
static void arpAllowNotBindEn(FunEnableEnum allowEn, char* ifName);
#if (HOTEL_PNP == FYES)
extern int isHotelPnpEn(void);
void  arpGlobalInit(void);
#else
static void  arpGlobalInit(void);
#endif

static void addArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName);
static void delArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName);
static void delAllArpBind(void);
static void arpBindInit(void);
static void editArpBindJumpRule(char action, char *ifName);

static void editArpBindJumpRule(char action, char *ifName)
{
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    static int arpBindCount = 0;
    static int buildFlag = 0;/*规则已添加标志 1--已添加 0--未添加*/
    int allowOther = 1;


    if (action == Iptables_ADD)/*添加*/
    {
	arpBindCount += 1;
    }
    else if (action == Iptables_DEL)
    {
	arpBindCount -= 1;
    }
    else if (action == Iptables_FLUSH)/*清空*/
    {
	arpBindCount = 0;
    }
    
    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/
    if (ProfInstIsFree(prof) == PROFOK) /*配置已用*/
    {
	if(prof->arpAllowOtherEn == FUN_DISABLE)
	{
	    allowOther = 0;
	}
#if (HOTEL_PNP == FYES)
	/*开启酒店功能则允许非绑定上网*/
	if(isHotelPnpEn() == 1 )   
	{
	    allowOther = 1;
	}
#endif
	if (buildFlag == 0)
	{
	    if ((arpBindCount > 0) || (allowOther != 1))/*条目数不为0或者不允许非绑定用户登陆则需要自定义链*/
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);/*先删除再添加*/
		doSystem("arptables -X %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		doSystem("arptables -N %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		doSystem("arptables -I INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);
		buildFlag = 1;
	    }
	}
	else/*规则已添加*/
	{
	    if ((arpBindCount <= 0) && (allowOther == 1))
	    {
		doSystem("arptables -D INPUT -j %s -i %s+ 1>/dev/null 2>&1", ARP_BIND_CHAIN, ifName);
		doSystem("arptables -X %s 1>/dev/null 2>&1", ARP_BIND_CHAIN);
		buildFlag = 0;
	    }
	}
    }
}

/*arp全局设置profilechange 触发函数*/
void arpGlobalProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const InterfaceProfile *oldProf, const InterfaceProfile *newProf )
{
    if(changeType == PROF_CHANGE_EDIT)
    {
	if(index == 0) {
	    /*lan口相关*/
	    /*只有当arpAllowOtherEn变化触发该函数时，才做以下设置*/
	    if(oldProf->arpAllowOtherEn != newProf->arpAllowOtherEn)
	    {
		arpAllowNotBindEn(newProf->arpAllowOtherEn, getLanIfName());/*允许非arp绑定用户连接到设备*/
	    }
	    /*end*/
	}

    }
    return;
}
#if (DHCP_POOL == FYES)
static char* getLanIfNameForAc(char* poolName){

    DhcpPoolProfile *prof= NULL;
    MibProfileType mibType= MIB_PROF_DHCP_POOL;
    int min = 0, max = 0, i;
    static char interface[16];

    memset(interface,'\0',sizeof(interface));
    if ('\0' == *poolName){
        return  getLanIfName();
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
            if ( !strcmp(prof->head.name,poolName) ){
                if (i == 0) {
                    return getLanIfName();
                 } else {
                    sprintf(interface,"%s.%d",getLanIfName(),prof->vid);
//					printf("interface=%s,prof->interface=%s,prof->vid=%d\n",interface,prof->interface,prof->vid);
                    return interface;
//                  return prof->interface;
                }
            }
        }
    }

    return  getLanIfName();
}
#endif


void arpBindProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const UserProfile* oldProfileDataPtr, const UserProfile* newProfileDataPtr )
{
#if 0
    printf("arp prof change type:%d\n", changeType);
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
#if (DHCP_POOL == FYES)
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfNameForAc(newProfileDataPtr->dhcpPoolName));/*添加*/
#else
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfName());/*添加*/
#endif
	    break;
	case PROF_CHANGE_ADD_ALL:
	    arpBindInit();/*重新初始化绑定信息*/
	    break;
	case PROF_CHANGE_EDIT:
#if (DHCP_POOL == FYES)
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfNameForAc(oldProfileDataPtr->dhcpPoolName));/*先删除老的*/
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfNameForAc(newProfileDataPtr->dhcpPoolName));/*添加新的*/
#else
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfName());/*先删除老的*/
	    addArpBind(newProfileDataPtr->ip, newProfileDataPtr->mac, newProfileDataPtr->bindEn, getLanIfName());/*添加新的*/
#endif
	    break;
	case PROF_CHANGE_DEL:
#if (DHCP_POOL == FYES)
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfNameForAc(oldProfileDataPtr->dhcpPoolName));/*删除老的*/
#else
	    delArpBind(oldProfileDataPtr->ip, oldProfileDataPtr->mac, oldProfileDataPtr->bindEn, getLanIfName());/*删除老的*/
#endif
	    break;
	case PROF_CHANGE_DELALL:
	    delAllArpBind();/*删除所有*/
	    break;
	default:
	    break;
    };
}
/**
 * arp 允许非绑定用户连接到设备
 * bhou
 * 2011-10-25
 */
static void arpAllowNotBindEn(FunEnableEnum allowEn, char* ifName)
{
#if 0
#if (DHCP_POOL == FYES)
    int min = 0, max = 0, index;
    UserProfile *prof;
	ServiceZoneProfile *profSz = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    char tmpName[16];
    memset(tmpName,0,sizeof(tmpName));
#endif
#endif

    doSystem("arptables -D %s -j DROP 2>/dev/null", ARP_BIND_CHAIN);/*先删除之前添加的*/

    editArpBindJumpRule(Iptables_SPECIAL, ifName);
#if (HOTEL_PNP == FYES)
    /*开启酒店功能则允许非绑定上网*/
    if(isHotelPnpEn() == 0 )   
    {
#endif
#if 0
#if (DHCP_POOL == FYES)
	ProfInstLowHigh(MIB_PROF_SERVICE_ZONE, &max, &min);

	for (index = (min + 1); index < max; index++) {
		profSz = (ServiceZoneProfile *)ProfGetInstPointByIndex(MIB_PROF_SERVICE_ZONE, index);
		if ((profSz != NULL) && (profSz->head.active == 1)) {
            if ( profSz->serviceEn == PROF_ENABLE ){
                sprintf(tmpName,"%s.%d",getLanIfName(),profSz->vid);
                doSystem("arptables -D %s -j DROP 2>/dev/null", ARP_BIND_CHAIN);/*先删除之前添加的*/
                if(allowEn == FUN_DISABLE)
                {
                    doSystem("arptables -A %s -j DROP", ARP_BIND_CHAIN);
                    doSystem("ip neigh flush dev %s 2>/dev/null",tmpName);/*清除arp，让他立即生效*/
                }
            }
	    }
	}

#endif
#endif

	if(allowEn == FUN_DISABLE)
	{
	    /*默认为允许。不允许则需添加规则*/
	    doSystem("arptables -A %s -j DROP", ARP_BIND_CHAIN);
#if 0
	    doSystem("ip neigh flush dev %s 2>/dev/null",ifName);/*清除arp，让他立即生效*/
#else
	    doSystem("ip neigh flush all");/*清除arp，让他立即生效*/
#endif
	}

#if (HOTEL_PNP == FYES)
    }
#endif

    return;
}
/*arp全局设置初始化
 * bhou
 * 2011-10-25
 * */
#if (HOTEL_PNP == FYES)
void  arpGlobalInit(void)
#else
static void  arpGlobalInit(void)
#endif
{
    InterfaceProfile *prof = NULL;

    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/

    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*配置已用*/
    {
	arpAllowNotBindEn(prof->arpAllowOtherEn, getLanIfName());/*允许非arp绑定用户连接到设备*/
    }
    return;

}
/***********************************************************************
 * 函 数 名：   addArpBind
 * 功能描述：   添加ARP绑定配置时系统添加一条arp -s配置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	modify by bhou at 2011-10-31
***********************************************************************/
static void addArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName)
{
    struct in_addr addrS;
    char* ipStr, *macStr;
    char arpAction[32]="RETURN";/*允许该用户上网*/

    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = addrIp;
    ipStr = inet_ntoa(addrS);/*转化为字符串形式*/
    macStr = converMac6To17Byte(macAddr);/*转化为冒号分隔形式*/

    editArpBindJumpRule(Iptables_ADD, ifName);

    if(allow == FUN_DISABLE)
    {
	strcpy(arpAction,"DROP");/*不允许该用户上网*/
    }

    doSystem("arptables -I %s -s %s --source-mac %s -j %s", ARP_BIND_CHAIN, ipStr, macStr, arpAction);
    doSystem("arptables -A %s -s %s  -j DROP", ARP_BIND_CHAIN, ipStr);
    doSystem("arptables -A %s --source-mac %s  -j DROP", ARP_BIND_CHAIN, macStr);

    doSystem("ip neigh flush dev %s 2>/dev/null",ifName);/*清除arp，让他立即生效*/
}

/***********************************************************************
 * 函 数 名：   delArpBind
 * 功能描述：   删除或修改ARP绑定配置时系统所做的配置
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	modify by bhou at 2011-10-31
 ***********************************************************************/
static void delArpBind(IPADDR addrIp, MacAddr macAddr, FunEnableEnum allow, char*ifName)
{
    struct in_addr addrS;
    char* ipStr, *macStr;
    char arpAction[32]="RETURN";/*允许该用户上网*/

    memset(&addrS, 0, sizeof(addrS));
    addrS.s_addr = addrIp;
    ipStr = inet_ntoa(addrS);/*转化为字符串形式*/
    macStr = converMac6To17Byte(macAddr);/*转化为冒号分隔形式*/

    if(allow == FUN_DISABLE)
    {
	strcpy(arpAction,"DROP");/*不允许该用户上网*/
    }

    doSystem("arptables -D %s -s %s --source-mac %s -j %s", ARP_BIND_CHAIN, ipStr, macStr, arpAction);
    doSystem("arptables -D %s -s %s -j DROP", ARP_BIND_CHAIN, ipStr);
    doSystem("arptables -D %s --source-mac %s -j DROP", ARP_BIND_CHAIN, macStr);
    editArpBindJumpRule(Iptables_DEL, ifName);
}

/***********************************************************************
 * 函 数 名：   delAllArpBind
 * 功能描述：   列表删除所有配置进行操作时生效函数
 * 创建日期：	2011-11-04
 * 修改日期：
 * 作者：       bhou
 * 附加说明：	无
***********************************************************************/
static void delAllArpBind(void)
{
    doSystem("arptables -F %s", ARP_BIND_CHAIN);/*先删除所有*/
    editArpBindJumpRule(Iptables_FLUSH, getLanIfName());
    arpGlobalInit();/*全局设置初始化*/
    return;
}

/***********************************************************************
 * 功能描述：   arp bind 初始化函数
 * 创建日期：	2011-10-31
 * 作者：       bhou
 * 附加说明：	无
 ***********************************************************************/
static void arpBindInit(void)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int i = 0,max = 0,min = 0;

    doSystem("arptables -F %s", ARP_BIND_CHAIN);/*删除所有*/
    editArpBindJumpRule(Iptables_FLUSH, getLanIfName());

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) /*遍历配置添加所有*/
    {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) /*配置已用*/
	{
#if (DHCP_POOL == FYES)
	    addArpBind(prof->ip, prof->mac, prof->bindEn, getLanIfNameForAc(prof->dhcpPoolName));/*添加新的*/
#else
	    addArpBind(prof->ip, prof->mac, prof->bindEn, getLanIfName());/*添加新的*/
#endif
	}
    }
    arpGlobalInit();/*全局设置初始化*/
    return;
}


/*arp绑定功能入口
 * bhou
 * 2011-10-25
 * */
void funInitArpBind(void)
{
    registerForProfileUpdates(MIB_PROF_INTERFACE, NULL, (ProfileUpdateFnc)arpGlobalProfileChange);/*arp全局设置*/
    registerForProfileUpdates(MIB_PROF_USER, NULL, (ProfileUpdateFnc)arpBindProfileChange);/*arp绑定*/

    arpBindInit();/*绑定初始化*/
    return;
}
#endif
