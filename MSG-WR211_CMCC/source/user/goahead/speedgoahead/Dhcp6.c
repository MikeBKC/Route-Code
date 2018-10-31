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


#if (IPV6_DHCPV6 == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void Dhcp6ServerChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Dhcp6Profile *oldProfileDataPtr, const Dhcp6Profile *newProfileDataPtr );

static void Dhcp6init(void);
static void Dhcp6SerConf(Dhcp6Profile *prof);

/********************************************************************
 * * 函数： Dhcp6init
 * * 功能： DHCP初始化函数
 * * 创建： 2014-09-30
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Dhcp6init()
{
    MibProfileType mibType=MIB_PROF_DHCP6;
    Dhcp6Profile   *prof=NULL;
    prof=(Dhcp6Profile *)ProfGetInstPointByIndex(mibType, 0);
    Dhcp6SerConf(prof);
}

/********************************************************************
 * * 函数： Dhcp6SerConf
 * * 功能： DHCP服务器写配置文件函数
 * * 创建： 2014-09-30
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Dhcp6SerConf(Dhcp6Profile *prof)
{
    FILE *fp;
    char *port = getLanIfName();
    if((fp = fopen("etc/dhcp6s.conf","w+")) == NULL)
    {    
	printf("open dhcp6s.conf is error!\n");
        return;
    }
    fprintf(fp,"interface %s\n",port);
    fprintf(fp,"{\n");
    fprintf(fp,"server-preference 255;\n");
    fprintf(fp,"renew-time %lu;\n",prof->staleTime/4);
    fprintf(fp,"rebind-time %lu;\n",prof->staleTime*2/5);
    fprintf(fp,"prefer-life-time %lu;\n",prof->staleTime/2);
    fprintf(fp,"valid-life-time %lu;\n",prof->staleTime);
    fprintf(fp,"allow rapid-commit;\n");
    fprintf(fp,"link %s {\n",port);
    fprintf(fp,"allow unicast;\n");
    fprintf(fp,"send unicast;\n");
    fprintf(fp,"pool{\n");
    fprintf(fp,"range %s to %s/%d;\n",prof->sta_ipv6_addr,prof->end_ipv6_addr,prof->dhcpPrefixLength);
    fprintf(fp,"prefix %s/%d;\n",prof->sta_ipv6_addr,prof->dhcpPrefixLength);
    fprintf(fp,"};\n");
    fprintf(fp,"};\n");
    fprintf(fp,"option dns_servers %s %s;\n",prof->ipv6DnsZ,prof->ipv6DnsB);
    fprintf(fp,"};\n");
    fclose(fp);
    doSystem("killall dhcp6s");

//    doSystem("echo 'nameserver %s' > /tmp/ipv6resolv.conf",prof->ipv6DnsZ);
//    doSystem("echo 'nameserver %s' >> /tmp/ipv6resolv.conf",prof->ipv6DnsB);
//    doSystem("cat /tmp/ipv4resolv.conf > /etc/resolv.conf");
//    doSystem("cat /tmp/ipv6resolv.conf >> /etc/resolv.conf");

    if(prof->dhcpEnabled == 1){
	doSystem("dhcp6s -c /etc/dhcp6s.conf -d %s &",port);
    }

}

/********************************************************************
 * * 函数： Ipv6RadvdGlobalChange
 * * 功能： RADVD全局配置Change函数
 * * 创建： 2014-09-25
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang 
 * ********************************************************************/
static void Dhcp6ServerChange(Pointer tag, MibProfileType profileType,
	ProfChangeType changeType, Uint32 index,
	const Dhcp6Profile *oldProfileDataPtr, const Dhcp6Profile *newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
	    Dhcp6SerConf((Dhcp6Profile *)newProfileDataPtr);/*重新写配置文件*/
            break;

	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
        }
        return;
}



/********************************************************************
 * * 函数： funInitRadvd
 * * 功能： Radvd 初始化入口函数
 * * 创建： 2014-09-28
 * * 参数： 略
 * * 返回： 略
 * * 输出： 略
 * * 作者： xiao.yunyang
 * ********************************************************************/
void funInitDhcp6(void)
{
    
    registerForProfileUpdates(MIB_PROF_DHCP6, (Pointer)NULL, (ProfileUpdateFnc)Dhcp6ServerChange);/*DHCP服务器配置触发函数*/
    Dhcp6init();/*初始化*/
    return;
}

#endif
