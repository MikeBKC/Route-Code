#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "Radvd.h"
#include "Ipv6Internet.h"
//#include "ifmisc.h"
#if (IPV6_INTERFACE == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Ipv6InterfaceProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Ipv6InterfaceProfile *oldProfileDataPtr, const Ipv6InterfaceProfile *newProfileDataPtr );
static void Ipv6LanInit(void);
extern void Ipv6WanInit(void);

static void Ipv6LanConfig(Ipv6InterfaceProfile *profold,Ipv6InterfaceProfile *profnew);
static void Ipv6WanConfigAdd(Ipv6InterfaceProfile *prof);
static void Ipv6WanConfigDel(Ipv6InterfaceProfile *prof);

/********************************************************************
 * * 函数： Ipv6LanConfigDel
 * * 功能： Ipv6Lan口删除配置函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanConfigDel(Ipv6InterfaceProfile *profold)
{
    char *port = getLanIfName();
    if(strcmp(profold->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,profold->ipv6LAN_addr,profold->LANprefixLength);
    }
//    getIpv6addr(name,addr);
//    printf("***%s----%d--addr=%s\n",__func__,__LINE__,addr);
//    if(strcmp(profnew->ipv6LAN_addr,addr) != 0){
	Radvdinit();
//    }
}

/********************************************************************
 * * 函数： Ipv6LanConfig
 * * 功能： Ipv6Lan口配置函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanConfig(Ipv6InterfaceProfile *profold,Ipv6InterfaceProfile *profnew)
{
    char *port = getLanIfName();
    if(strcmp(profold->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,profold->ipv6LAN_addr,profold->LANprefixLength);
    }
    if(strcmp(profnew->ipv6LAN_addr,"") != 0){
	doSystem("ifconfig %s add %s/%d",port,profnew->ipv6LAN_addr,profnew->LANprefixLength);
	Radvdinit();
    }
}
/********************************************************************
 * * 函数： Ipv6WanConfigAdd
 * * 功能： Ipv6Wan口配置添加函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6WanConfigAdd(Ipv6InterfaceProfile *prof)
{   
    int flag;
    char *port = getWanIfName(prof->port);
    if(strcmp(prof->ipv6WAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ifconfig %s add %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ip -6 route add %s dev %s via %s metric 2",prof->ipv6Dns1,port,prof->ipv6Gateway); 
	doSystem("ip -6 route add %s dev %s via %s metric 2",prof->ipv6Dns2,port,prof->ipv6Gateway); 

	printf("ifconfig %s add %s/%d\n",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("route -A inet6 add default gw %s dev %s metric 2",prof->ipv6Gateway,port);
	doSystem("echo 'nameserver %s' > /tmp/ipv6resolv.conf",prof->ipv6Dns1);
	doSystem("echo 'nameserver %s' >> /tmp/ipv6resolv.conf",prof->ipv6Dns2);
	doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
	doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");
    }

}
/********************************************************************
 * * 函数： Ipv6WanConfigDel
 * * 功能： Ipv6Wan口配置删除函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6WanConfigDel(Ipv6InterfaceProfile *prof)
{   
    char *port = getWanIfName(prof->port);
    if(strcmp(prof->ipv6WAN_addr,"") != 0){
	doSystem("ifconfig %s del %s/%d",port,prof->ipv6WAN_addr,prof->WANprefixLength);
	doSystem("ip -6 route del %s dev %s via %s metric 2",prof->ipv6Dns1,port,prof->ipv6Gateway); 
	doSystem("ip -6 route del %s dev %s via %s metric 2",prof->ipv6Dns2,port,prof->ipv6Gateway); 
	doSystem("route -A inet6 del default gw %s dev %s metric 2",prof->ipv6Gateway,port);
	doSystem("echo '' > /tmp/ipv6resolv.conf");
	doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
	doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");

    }
}

/********************************************************************
 * * 函数： Ipv6LanInit
 * * 功能： Ipv6Lan口初始化函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6LanInit(void)
{
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;
    prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , 0);
    Ipv6LanConfig((Ipv6InterfaceProfile *)prof,(Ipv6InterfaceProfile *)prof);
}
/********************************************************************
 * * 函数： Ipv6WanInit
 * * 功能： Ipv6Wan口初始化函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

extern void Ipv6WanInit(void)
{
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_IPV6_INTERFACE;
    Ipv6InterfaceProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
   
    for(index=min;index<max-1;index++){
	prof = (Ipv6InterfaceProfile *)ProfGetInstPointByIndex(mibType , index+1);

//        printf("*****************************\n");
	if((prof != NULL) &&  (ProfInstIsFree(prof) == 0) && (prof->head.active == 1)){
             
	    Ipv6WanConfigAdd((Ipv6InterfaceProfile *)prof);
        printf("addr=%s,-%s --%d\n",prof->ipv6WAN_addr,__func__,__LINE__);
	}
    }
}

/********************************************************************
 * * 函数： Ipv6Interface ProfileChange
 * * 功能： IPv6接口配置变化 函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6InterfaceProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType, Uint32 index,
        const Ipv6InterfaceProfile *oldProfileDataPtr, const Ipv6InterfaceProfile *newProfileDataPtr )
{
        switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    if((strcmp(oldProfileDataPtr->ipv6LAN_addr,newProfileDataPtr->ipv6LAN_addr) != 0)|| (oldProfileDataPtr->LANprefixLength != newProfileDataPtr->LANprefixLength)||(oldProfileDataPtr->LanAutoEnabled != newProfileDataPtr->LanAutoEnabled)){                   //LAN口配置发生改变
		Ipv6LanConfig((Ipv6InterfaceProfile *)oldProfileDataPtr,(Ipv6InterfaceProfile *)newProfileDataPtr);
	    }
            if((strcmp(oldProfileDataPtr->ipv6WAN_addr,newProfileDataPtr->ipv6WAN_addr) != 0) || (oldProfileDataPtr->WANprefixLength != newProfileDataPtr->WANprefixLength) || (strcmp(oldProfileDataPtr->ipv6Dns1, newProfileDataPtr->ipv6Dns1) != 0)|| (strcmp(oldProfileDataPtr->ipv6Dns2,newProfileDataPtr->ipv6Dns2) != 0) || (strcmp(oldProfileDataPtr->ipv6Gateway, newProfileDataPtr->ipv6Gateway) != 0)){
              //WAN口配置发生改变 
		Ipv6WanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);    //删除旧配置
                Ipv6WanConfigAdd((Ipv6InterfaceProfile *)newProfileDataPtr);    //添加新配置
	    }
	    break;
	case PROF_CHANGE_DEL:
                
	    if((strcmp(oldProfileDataPtr->ipv6LAN_addr,newProfileDataPtr->ipv6LAN_addr) != 0)|| (oldProfileDataPtr->LANprefixLength != newProfileDataPtr->LANprefixLength)||(oldProfileDataPtr->LanAutoEnabled != newProfileDataPtr->LanAutoEnabled)){                   //LAN口配置发生改变
		Ipv6LanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);
	    }

	    if((strcmp(oldProfileDataPtr->ipv6WAN_addr,newProfileDataPtr->ipv6WAN_addr) != 0) || (oldProfileDataPtr->WANprefixLength != newProfileDataPtr->WANprefixLength) || (strcmp(oldProfileDataPtr->ipv6Dns1, newProfileDataPtr->ipv6Dns1) != 0)|| (strcmp(oldProfileDataPtr->ipv6Dns2,newProfileDataPtr->ipv6Dns2) != 0) || (strcmp(oldProfileDataPtr->ipv6Gateway, newProfileDataPtr->ipv6Gateway) != 0)){
		Ipv6WanConfigDel((Ipv6InterfaceProfile *)oldProfileDataPtr);
	    }
                break;
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;



}


/********************************************************************
 * * 函数： funInitIpv6Interface
 * * 功能： IPv6接口 初始化入口函数
 * * 创建： 2014-09-19
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao。yunyang
 * ********************************************************************/
void funInitIpv6Interface(void)
{
    registerForProfileUpdates(MIB_PROF_IPV6_INTERFACE, (Pointer)NULL, (ProfileUpdateFnc)Ipv6InterfaceProfileChange);/*触发函数*/

//    Ipv6LanInit();/*LAN初始化*/
//    Ipv6WanInit();/*WAN初始化*/
    return;
}

#endif
