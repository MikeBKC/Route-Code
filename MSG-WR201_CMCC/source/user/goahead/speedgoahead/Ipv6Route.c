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
#include "Ipv6Route.h"

#if (IPV6_ROUTE == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#define IPV6_TABLE_NO 120
static void Ipv6RouteProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Ipv6RoutConfProfile *oldProfileDataPtr, const Ipv6RoutConfProfile *newProfileDataPtr );
extern void Ipv6RouteInit(void);
static void Ipv6RouteDel(Ipv6RoutConfProfile *prof);
static void Ipv6RouteDelAll();
static void Ipv6RouteAdd(Ipv6RoutConfProfile *prof);
static void Ipv6RouteEdit(Ipv6RoutConfProfile *profold,Ipv6RoutConfProfile *profnew);

/********************************************************************
 * * 函数： Ipv6RouteAdd
 * * 功能： IPv6添加一条路由 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6RouteAdd(Ipv6RoutConfProfile *prof)
{
    char *port;
    
//    printf("--prof->port=%s---\n",prof->port);
    if(strcmp(prof->port,"LAN0") == 0){
	port = getLanIfName();
    }else if(strcmp(prof->port,"WAN1") == 0){
	port = getWanIfName(1);
    } else if(strcmp(prof->port,"WAN2") == 0){
	port = getWanIfName(2);
    } else if(strcmp(prof->port,"WAN3") == 0){
	port = getWanIfName(3);
    } else if(strcmp(prof->port,"WAN4") == 0){
        port = getWanIfName(4);
    }
//    printf("port=%s  ---------\n",port);
    if(prof->head.active == 1){
	doSystem("ip -6 route add table %d %s/%d dev %s via %s metric %d",IPV6_TABLE_NO,prof->desIP,prof->desNetPrefix,port,prof->GateWay,prof->priority);
//    printf("ip -6 route add %s/%d dev %s via %s metric %d",prof->desIP,prof->desNetPrefix,port,prof->GateWay,prof->priority);

    }
}
/********************************************************************
 * * 函数： Ipv6RouteDel
 * * 功能： IPv6删除一条路由 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

static void Ipv6RouteDel(Ipv6RoutConfProfile *prof)
{
    char *port;
    if(strcmp(prof->port,"LAN0") == 0){
	port = getLanIfName();
    }else if(strcmp(prof->port,"WAN1") == 0){
	port = getWanIfName(1);
    } else if(strcmp(prof->port,"WAN2") == 0){
	port = getWanIfName(2);
    } else if(strcmp(prof->port,"WAN3") == 0){
	port = getWanIfName(3);
    } else if(strcmp(prof->port,"WAN4") == 0){
        port = getWanIfName(4);
    }


  //    printf("*******%s-----%d\n",__func__,__LINE__);
    doSystem("ip -6 route del table %d %s/%d dev %s via %s metric %d",IPV6_TABLE_NO,prof->desIP,prof->desNetPrefix,port,prof->GateWay,prof->priority);
//    printf("ip -6 route del %s/%d dev %s via %s metric %d\n",prof->desIP,prof->desNetPrefix,port,prof->GateWay,prof->priority);

//	doSystem("ip -6 rule del from table %d prio %d",IPV6_TABLE_NO,prof->priority);
}


/********************************************************************
 * * 函数： Ipv6RouteDelAll
 * * 功能： IPv6静态路由表清空 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang                      还需修改
 * ********************************************************************/

static void Ipv6RouteDelAll()
{
    doSystem("ip -6 route flush table %d",IPV6_TABLE_NO);
    doSystem("ip -6 rule flush");
    doSystem("ip -6 rule add table main prio 32766");
}


/********************************************************************
 * * 函数： Ipv6RouteEdit
 * * 功能： IPv6静态路由修改 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6RouteEdit(Ipv6RoutConfProfile *profold,Ipv6RoutConfProfile *profnew)
{  
    Ipv6RouteDel(profold);    //删除旧的路由
    Ipv6RouteAdd(profnew);    //建立新的路由
}

/********************************************************************
 * * 函数： Ipv6RouteInit
 * * 功能： IPv6静态路由初始化函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/

extern void Ipv6RouteInit(void)
{
    int num=0;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_IPV6_ROUTECONF;
    Ipv6RoutConfProfile *prof = NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(index=min;index<max;index++){
	prof = (Ipv6RoutConfProfile *)ProfGetInstPointByIndex(mibType, index);   //从profile中读取配置信息
	if(prof!=NULL &&  (ProfInstIsFree(prof) == 0) &&(prof->head.active == 1)){
	    Ipv6RouteAdd(prof);     //逐条添加路由
	}
    }
   
    doSystem("ip -6 rule add table %d prio 10200",IPV6_TABLE_NO);
   doSystem("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
}
/********************************************************************
 * * 函数： Ipv6RouteProfileChange
 * * 功能： IPv6静态路由 函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Ipv6RouteProfileChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Ipv6RoutConfProfile *oldProfileDataPtr, const Ipv6RoutConfProfile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:    //添加一条新路由

	    Ipv6RouteAdd((Ipv6RoutConfProfile *)newProfileDataPtr);
	    break;
        case PROF_CHANGE_EDIT:   //修改原有的路由

//    printf("--%s %d\n",__func__,__LINE__);
	     Ipv6RouteEdit((Ipv6RoutConfProfile *)oldProfileDataPtr,(Ipv6RoutConfProfile *)newProfileDataPtr);
	    
	    break;
	case PROF_CHANGE_DEL:     //删除一条路由

//    printf("--%s %d\n",__func__,__LINE__);
	    Ipv6RouteDel((Ipv6RoutConfProfile *)oldProfileDataPtr);
            break;

	case PROF_CHANGE_DELALL:   //清空路由表
//    printf("--%s %d\n",__func__,__LINE__);
            Ipv6RouteDelAll();
            break;
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
void funInitIpv6Route(void)
{
    
    registerForProfileUpdates(MIB_PROF_IPV6_ROUTECONF, (Pointer)NULL, (ProfileUpdateFnc)Ipv6RouteProfileChange);/*触发函数*/

    Ipv6RouteInit();/*ipv6静态路由初始化*/
    return;
}

#endif
