/*speedweb通用函数集合
 *speedweb common function
 *2011-11-11
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "internet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "spdComFun.h"
#include "ifmisc.h"

/**
 * 根据当前配置数据类型产生iptables所需iprange字串
 * fromIp:起始ip地址
 * endIp:结束ip地址
 * isSrc: 1本次操作针对源ip ， 0 本次操作针对目的ip
 */
extern char* getIpRangeForIptables(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    strcpy(ipRangeStr, " -m iprange");/*iptables中ip范围相关*/
    if(isSrc!=0)    
    {
	/*本次操作针对源ip*/
	strcat(ipRangeStr, " --src-range");
    }
    else
    {/*本次操作针对目的ip*/
	strcat(ipRangeStr, " --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*起始与结束ip都为零则针对整个网段进行设置*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*起始ip地址*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*结束ip地址*/
    }

    return ipRangeStr;

}
 /*追加目的地址组  */
extern char* getDstIpRangeForIptables(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    if(isSrc!=0)    
    {
	/*本次操作针对源ip*/
	strcat(ipRangeStr, " --src-range");
    }
    else
    {/*本次操作针对目的ip*/
	strcat(ipRangeStr, " --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*起始与结束ip都为零则针对整个网段进行设置*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*起始ip地址*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*结束ip地址*/
    }

    return ipRangeStr;

}


/**
 * 根据当前配置数据类型产生iptables所需iprange字串且取非
 * fromIp:起始ip地址
 * endIp:结束ip地址
 * isSrc: 1本次操作针对源ip ， 0 本次操作针对目的ip
 */
extern char* getIpRangeForIptables_opposite(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    strcpy(ipRangeStr, " -m iprange");/*iptables中ip范围相关*/
    if(isSrc!=0)    
    {
	/*本次操作针对源ip*/
	strcat(ipRangeStr, " ! --src-range");
    }
    else
    {/*本次操作针对目的ip*/
	strcat(ipRangeStr, " ! --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*起始与结束ip都为零则针对整个网段进行设置*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*起始ip地址*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*结束ip地址*/
    }

    return ipRangeStr;

}
/**
 * 根据当前配置数据类型产生iptables所需time字串
 */
extern char* getTimeForIptables(char* timeStart, char* timeEnd)
{
    static char timeStr[96];

    memset(timeStr, 0, sizeof(timeStr));

    if(strcmp(timeStart, "All")==0){/*全天*/
	sprintf(timeStr," -m time --timestart 00:00 --timestop 23:59 ");
    }
    else
    {
	sprintf(timeStr," -m time --timestart %s --timestop %s ", timeStart, timeEnd);
    }

    return timeStr;

}
/*根据当前配置数据类型产生iptables所需天字串*/
extern char* getDayForIptables(Uint32 day)
{
    static char dayStr[96];

    memset(dayStr, 0, sizeof(dayStr));

#if ((UTT_MV_821 == FYES)|| (UTT_LV_5830G == FYES) || (UTT_LV_6530G == FYES) || (UTT_LV_4330G == FYES) || (UTT_LV_N8000 == FYES) || (UTT_MV_811 == FYES)|| (UTT_MV_841 == FYES) || (UTT_LV_6830G == FYES)|| (UTT_WX_2000 == FYES)) || (CONFIG_RALINK_MT7620 == FYES) || (CONFIG_RALINK_MT7621 == FYES) || (CONFIG_RALINK_MT7628 == FYES) || (CONFIG_PPC_85xx == FYES)
    strcpy(dayStr, " --weekdays ");;
#else 
    strcpy(dayStr, " --days ");;
#endif
    getDays(day ,&dayStr[strlen(dayStr)]);
    strcat(dayStr, " ");
    
    return dayStr;

}
/********************************************************************
 * 函数： getDateForIptables
 * 功能： 根据当前配置数据类型产生iptables所需日期字串
 * 创建： 2012-04-11
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern char* getDateForIptables(char* dateStart, char* dateEnd)
{
    static char dateStr[96];

    memset(dateStr, 0, sizeof(dateStr));
#if ((UTT_MV_821 == FYES)|| (UTT_LV_5830G == FYES) || (UTT_LV_6530G == FYES) || (UTT_LV_4330G == FYES) || (UTT_LV_N8000 == FYES) || (UTT_MV_811 == FYES)|| (UTT_MV_841 == FYES) || (UTT_LV_6830G == FYES) || (UTT_WX_2000 == FYES)) || (CONFIG_RALINK_MT7620 == FYES) || (CONFIG_RALINK_MT7621 == FYES) || (CONFIG_RALINK_MT7628 == FYES) || (CONFIG_PPC_85xx == FYES)
    StrTransform(dateStart, COLON_SPLIT_SIGN_STR, CROSS_SPLIT_SIGN_STR);
    StrTransform(dateEnd, COLON_SPLIT_SIGN_STR, CROSS_SPLIT_SIGN_STR);
    sprintf(dateStr," --datestart %sT00:00:00 --datestop %sT23:59:59 ", dateStart, dateEnd);
#else 
    StrTransform(dateStart, CROSS_SPLIT_SIGN_STR, COLON_SPLIT_SIGN_STR);
    StrTransform(dateEnd, CROSS_SPLIT_SIGN_STR, COLON_SPLIT_SIGN_STR);
    sprintf(dateStr," --datestart %s:00:00:00 --datestop %s:23:59:59 ", dateStart, dateEnd);
#endif


    return dateStr;

}
/**
 * 为crond获得时分字串
 * return：静态内部变量
 * */
char* getTimeForCrond(Uchar* time)
{
    static char timestr[16];
    Uint32 hour = 0u, minute = 0u;

    sscanf((char *)time, "%u:%u", &hour, &minute);/*读入时分*/

    sprintf(timestr, "%02u %02u", minute, hour);/*crond先分后时*/

    return timestr;
}
/**
 * 根据配置数据类型获取 计划任务crond所需的星期
 * return：静态内部变量字串
 */
char* getWeekForCrond(Uint32 week_bit)
{
    Uint32 i = 0u;
    static char weekstr[16];

    /*crond星期模板（0-6,0代表星期天）*/
    const char* weekTemplate[] = 
    {
	"1","2","3","4","5","6","0"
    };

    memset(weekstr, 0, sizeof(weekstr));

    strcpy(weekstr, " ");/*星期与前面的天分割符*/
    /*111 1111 各位代表星期几*/
    if(week_bit == 0x7fu)
    {/*每天都执行*/
	strcat(weekstr,"*");
    }
    else
    {
	/*每周七天*/
	for(i=0u;i<7u;i++)
	{
	    if( (week_bit& (0x40u >> i)) != 0u)
	    {
		strcat(weekstr, weekTemplate[i]);
		strcat(weekstr, ",");/*分隔符*/
	    }
	}
	weekstr[strlen(weekstr) - 1u] = '\0';/*去掉最后的分割符*/
    }
    return weekstr;
}
/**
 * 把时间转换为 秒
 */
Uint32 converTimeToSec(char *time)
{
    Uint32 hour = 0u, mintue = 0u, second = 0u;
    sscanf(time, "%u:%u", &hour, &mintue);/*读入时分*/
    second =  (hour*60 + mintue) *60;
    return second;

}
#if (PPPOE_SERVER == FYES)
 /********************************************************************
 * 函数： AccAddIpStr
 * 功能： 循环遍历账号对应的所有会话，把会话ip加入或移出组
 * 创建： 2012-08-24
 * 参数： 
 *	  prof     -    账号对应profile
 *	  grpName  -    组名
 *	  addOrDel -    添加还是删除
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
extern int AccAddIpStr(PPPoESrvAccProfile* prof, char* grpName, char addOrDel)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int i = 0, j = 0;

    profS = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(profS != NULL)
    {
	j = prof->sesIndex;
	if(SesIndexIsOk(j) == 1)
	{
	    for(i = 0; i < prof->actualSession; i++)
	    {
		if(nvramProfile->Session[j].ConTime == 0U)
		{/*在线*/
		    ipsetAOrDIpStr(grpName, nvramProfile->Session[j].ip, addOrDel);/*更新相应的ipset组*/
		}
		j = nvramProfile->Session[j].nextIndex;
		if(SesIndexIsOk(j) == 0)
		{
		    break;
		}
	    }
	}
    }
    return 0;
}
#endif

#if (EASY_TASK == FYES)
/*
 * 修改重定向到LAN口的规则
 */
extern int editLANRedirectRule(char action, char *lanIp)
{
    int retVal = 0;

#if 0
    uttSemP(SEM_CONNSTA_NO, 0);
#endif
    doSystem("iptables -%c FORWARD -m uttdev --is-lan-in -p udp --dport 53 -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1", action, lanIp);
    doSystem("iptables -%c INPUT -m uttdev --is-lan-in -p udp --dport 53 -j REJECT --reject-with dns-redirect:%s 1>/dev/null 2>&1", action, lanIp);
    if (action == 'A')
    {
	doSystem("iptables -t nat -N %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
	doSystem("iptables -t nat -A PREROUTING -m uttdev --is-lan-in -p tcp --dport 80 -j %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
	doSystem("iptables -t nat -A %s -m layer7 --l7proto GetPage -j DNAT --to %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN, lanIp);
	doSystem("iptables -t nat -A %s -m layer7 --l7proto nonZeroLen -j DROP 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
	doSystem("iptables -t nat -A %s -j DNAT --to %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN, lanIp);
    }
    else
    {
	doSystem("iptables -t nat -F %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
	doSystem("iptables -t nat -D PREROUTING -m uttdev --is-lan-in -p tcp --dport 80 -j %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
	doSystem("iptables -t nat -X %s 1>/dev/null 2>&1", EASY_TASK_HTTP_CHAIN);
    }
#if 0
    uttSemV(SEM_CONNSTA_NO, 0);
#endif

    return retVal;
}
#endif
#if (HOTEL_PNP ==FYES || EASY_TASK == FYES || UTT_DOMAINACCESS ==FYES)
/*即插即用开启或易管理断线后都要添加dns重定向规则*/
int editDNSRedirectRule(void)
{
    char *lanIf = getLanIfName();
#if (EASY_TASK == FYES)
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
#endif
    doSystem("iptables -t nat -%c PREROUTING -i %s -p udp --dport 53 -j REDIRECT 1>/dev/null 2>&1", Iptables_DEL, lanIf);/*防止重复添加*/
#if (EASY_TASK == FYES)
    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
#endif
    if (
#if (HOTEL_PNP ==FYES)
isHotelPnpEn()||
#endif
#if(UTT_DOMAINACCESS ==FYES)
isWanLinkStatus == 0 ||
#elif(EASY_TASK == FYES)
((prof->easyTaskNoticeEn == FUN_ENABLE) &&
(prof->wanLinkStates == 0U))||
#endif
(0==1))
    {
	/*dns包指向路由器。同时路由器还需开启dns代理（其他地方实现）*/
	doSystem("iptables -t nat -%c PREROUTING -i %s -p udp --dport 53 -j REDIRECT 1>/dev/null 2>&1", Iptables_ADD, lanIf);
    }
    return 0;
}
#endif
