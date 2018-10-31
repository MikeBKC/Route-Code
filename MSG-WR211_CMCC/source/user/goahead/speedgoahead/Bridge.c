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
static void bridgeConfigSet();
/*消息处理函数
 *chen.bin
 *2013-01-29
 *
 */
void bridgeProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const BridgeProfile* oldProfileDataPtr, const BridgeProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
	    bridgeConfigSet(oldProfileDataPtr,newProfileDataPtr);
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
 * 2013-01-29
 */
static void bridgeConfigInit(void)
{
	BridgeProfile *prof = NULL;
	char macStr[MAX_VALUE_LEN];
	doSystem("bridge.sh");
	prof = (BridgeProfile *)ProfGetInstPointByIndex(MIB_PROF_BRIDGE,0);

	doSystem("brctl delbr br0");
	doSystem("brctl addbr br0");
	if(prof != NULL)
	{
		int i,len;
		len = (sizeof(prof->interface)/sizeof(prof->interface[0]));
		for (i=0;i<len;i++)
		{
			if(prof->interface[i].enable == FUN_ENABLE)
			{
				mac2str(macStr,prof->interface[i].mac);
				doSystem("ifconfig %s 0.0.0.0 hw ether %s up",prof->interface[i].ifName,macStr);
				doSystem("brctl addif br0 %s",prof->interface[i].ifName);
			}
		}
		//doSystem("ifconfig br0 up");
		doSystem("ifconfig br0 %N netmask %N up",prof->ip,prof->nm);
		doSystem("route add default gw %N ",prof->gw);
#if(IPV6 == FYES)
		doSystem("echo 'nameserver %N' > /tmp/ipv4resolv.conf",prof->dns1);
                doSystem("echo 'nameserver %N' >> /tmp/ipv4resolv.conf",prof->dns2);
		doSystem(" cat /tmp/ipv4resolv.conf >/etc/resolv.conf");
                doSystem(" cat /tmp/ipv6resolv.conf >>/etc/resolv.conf");
#else
                doSystem("echo 'nameserver %N' > /etc/resolv.conf",prof->dns1);
                doSystem("echo 'nameserver %N' >> /etc/resolv.conf",prof->dns2);
#endif	
	

	}
	ntp_sh();
    return;
}

/*
 *配置修改生效
 *
 */
static void bridgeConfigSet(const BridgeProfile* oldProf, const BridgeProfile* newProf)
{
	CacheProfile * cacheProf = NULL;
	char macStr[MAX_VALUE_LEN];	
	doSystem("bridge.sh");
	int i,len;
	len = (sizeof(oldProf->interface)/sizeof(oldProf->interface[0]));
	//修改接口
	for (i=0;i<len;i++)
	{
		if(oldProf->interface[i].enable == FUN_ENABLE)
		{
			if(newProf->interface[i].enable == FUN_DISABLE)
			{
				doSystem("brctl delif br0 %s",newProf->interface[i].ifName);
			}
		}else{
			if(newProf->interface[i].enable == FUN_ENABLE){
				mac2str(macStr,newProf->interface[i].mac);
				doSystem("ifconfig %s 0.0.0.0 hw ether %s up",newProf->interface[i].ifName,macStr);
				doSystem("brctl addif br0 %s",newProf->interface[i].ifName);
			}
		}
	}	
	//修改桥IP
	if(oldProf->ip!=newProf->ip)
	{
		doSystem("ifconfig br0 %N netmask %N up",newProf->ip,newProf->nm);
	}
	//避免重复配置mask，只IP不变时才单独配置mask
	if((oldProf->ip==newProf->ip)&&(oldProf->nm!=newProf->nm)){
		doSystem("ifconfig br0 netmask %N ",newProf->nm);
	}

	if(oldProf->gw!=newProf->gw){
		doSystem("route add default gw %N ",newProf->gw);
	}
#if(IPV6 == FYES)
                doSystem("echo 'nameserver %N' > /tmp/ipv4resolv.conf",newProf->dns1);
                doSystem("echo 'nameserver %N' >> /tmp/ipv4resolv.conf",newProf->dns2);
		doSystem(" cat /tmp/ipv4resolv.conf >/etc/resolv.conf");
                doSystem(" cat /tmp/ipv6resolv.conf >>/etc/resolv.conf");
#else
                doSystem("echo 'nameserver %N' > /etc/resolv.conf",newProf->dns1);
                doSystem("echo 'nameserver %N' >> /etc/resolv.conf",newProf->dns2);
#endif	
	if(oldProf->ip!=newProf->ip)
	{
		cacheProf = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);
		if((cacheProf!=NULL)&&(cacheProf->cacheEnable==FUN_ENABLE))
		{

			doSystem("iptables -t nat -D PREROUTING ! -d %N  -p tcp --dport 80 -j DNAT --to-destination %N:3128",oldProf->ip,oldProf->ip);
			doSystem("iptables -t nat -A PREROUTING ! -d %N  -p tcp --dport 80 -j DNAT --to-destination %N:3128",newProf->ip,newProf->ip);
		}
	}
}

/*
 * 初始化函数
 *chen.bin
 *2012-02-01
 */
void funInitBridge(void)
{
    registerForProfileUpdates(MIB_PROF_BRIDGE, (Pointer)NULL, (ProfileUpdateFnc)bridgeProfileChange);
    bridgeConfigInit();//配置初始化生效

    return;
}
#endif
