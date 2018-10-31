#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "uttfunction.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
extern void ntp_sh(void);
#if (NET_MODE_SWITCH == FYES)
void mac2str(char* buf,MacAddr mac){
	Byte* tmp = NULL;
	tmp = mac.addr;
    snprintf(buf, MAX_VALUE_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", 
            (Uint8)(*tmp), (Uint8)*(tmp+1), (Uint8)*(tmp+2), (Uint8)*(tmp+3), (Uint8)*(tmp+4), (Uint8)*(tmp+5));
}
static void netConfigSet();
/*消息处理函数
 *chen.bin
 *2013-04-07
 *
 */
void netProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const BridgeProfile* oldProfileDataPtr, const BridgeProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
	    netConfigSet(oldProfileDataPtr,newProfileDataPtr);
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}


/***
 * 配置生效函数
 * chen.bin
 * 2013-04-07
 */
static void netConfigInit(void)
{
	BridgeProfile *prof = NULL;
	char macStr[MAX_VALUE_LEN];
	doSystem("bridge.sh");
	prof = (BridgeProfile *)ProfGetInstPointByIndex(MIB_PROF_BRIDGE,0);

	if(prof != NULL)
	{
		mac2str(macStr,prof->interface[0].mac);
		doSystem("ifconfig %s 0.0.0.0 hw ether %s up",prof->interface[0].ifName,macStr);
		//doSystem("ifconfig br0 up");
		doSystem("ifconfig %s %N netmask %N up",prof->interface[0].ifName,prof->ip,prof->nm);
		doSystem("route add default gw %N ",prof->gw);
#if(IPV6 == FYES)
        doSystem("echo 'nameserver %N' > /tmp/ipv4resolv.conf",prof->dns1);
        doSystem("echo 'nameserver %N' >> /tmp/ipv4resolv.conf",prof->dns2);
#else
        doSystem("echo 'nameserver %N' > /etc/resolv.conf",prof->dns1);
        doSystem("echo 'nameserver %N' >> /etc/resolv.conf",prof->dns2);
#endif
#if(IPV6 == FYES)
        doSystem(" cat /tmp/ipv4resolv.conf >/etc/resolv.conf");
        doSystem(" cat /tmp/ipv6resolv.conf >>/etc/resolv.conf");
#endif	

	}
	ntp_sh();
    return;
}

/*
 *配置修改生效
 *
 */
static void netConfigSet(const BridgeProfile* oldProf, const BridgeProfile* newProf)
{
	CacheProfile * cacheProf = NULL;
//	doSystem("bridge.sh");
		
	//修改桥IP
	if(oldProf->ip!=newProf->ip)
	{
		doSystem("ifconfig %s %N netmask %N up",newProf->interface[0].ifName,newProf->ip,newProf->nm);
		doSystem("route add default gw %N ",newProf->gw);
	}
	//避免重复配置mask，只IP不变时才单独配置mask
	if((oldProf->ip==newProf->ip)&&(oldProf->nm!=newProf->nm)){
		doSystem("ifconfig %s netmask %N ",newProf->interface[0].ifName,newProf->nm);
		doSystem("route add default gw %N ",newProf->gw);
	}

	if(oldProf->gw!=newProf->gw){
		doSystem("route del default gw %N ",oldProf->gw);
		doSystem("route add default gw %N ",newProf->gw);
	}
#if(IPV6 == FYES)
        doSystem("echo 'nameserver %N' > /tmp/ipv4resolv.conf",newProf->dns1);
        doSystem("echo 'nameserver %N' >> /tmp/ipv4resolv.conf",newProf->dns2);
#else
        doSystem("echo 'nameserver %N' > /etc/resolv.conf",newProf->dns1);
        doSystem("echo 'nameserver %N' >> /etc/resolv.conf",newProf->dns2);
#endif
#if(IPV6 == FYES)
        doSystem(" cat /tmp/ipv4resolv.conf >/etc/resolv.conf");
        doSystem(" cat /tmp/ipv6resolv.conf >>/etc/resolv.conf");
#endif	
}

/*
 * 初始化函数
 *chen.bin
 *2012-04-07
 */
void funInitNetConfig(void)
{
    registerForProfileUpdates(MIB_PROF_BRIDGE, (Pointer)NULL, (ProfileUpdateFnc)netProfileChange);
    netConfigInit();//配置初始化生效

    return;
}
#endif
