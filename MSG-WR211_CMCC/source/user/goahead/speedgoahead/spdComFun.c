/*speedwebͨ�ú�������
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
 * ���ݵ�ǰ�����������Ͳ���iptables����iprange�ִ�
 * fromIp:��ʼip��ַ
 * endIp:����ip��ַ
 * isSrc: 1���β������Դip �� 0 ���β������Ŀ��ip
 */
extern char* getIpRangeForIptables(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    strcpy(ipRangeStr, " -m iprange");/*iptables��ip��Χ���*/
    if(isSrc!=0)    
    {
	/*���β������Դip*/
	strcat(ipRangeStr, " --src-range");
    }
    else
    {/*���β������Ŀ��ip*/
	strcat(ipRangeStr, " --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*��ʼ�����ip��Ϊ��������������ν�������*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*��ʼip��ַ*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*����ip��ַ*/
    }

    return ipRangeStr;

}
 /*׷��Ŀ�ĵ�ַ��  */
extern char* getDstIpRangeForIptables(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    if(isSrc!=0)    
    {
	/*���β������Դip*/
	strcat(ipRangeStr, " --src-range");
    }
    else
    {/*���β������Ŀ��ip*/
	strcat(ipRangeStr, " --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*��ʼ�����ip��Ϊ��������������ν�������*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*��ʼip��ַ*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*����ip��ַ*/
    }

    return ipRangeStr;

}


/**
 * ���ݵ�ǰ�����������Ͳ���iptables����iprange�ִ���ȡ��
 * fromIp:��ʼip��ַ
 * endIp:����ip��ַ
 * isSrc: 1���β������Դip �� 0 ���β������Ŀ��ip
 */
extern char* getIpRangeForIptables_opposite(IPADDR fromIp, IPADDR endIp, bool isSrc)
{
    static char ipRangeStr[96];
    struct in_addr sAddr;

    memset(ipRangeStr, 0, sizeof(ipRangeStr));
    memset(&sAddr, 0, sizeof(sAddr));

    strcpy(ipRangeStr, " -m iprange");/*iptables��ip��Χ���*/
    if(isSrc!=0)    
    {
	/*���β������Դip*/
	strcat(ipRangeStr, " ! --src-range");
    }
    else
    {/*���β������Ŀ��ip*/
	strcat(ipRangeStr, " ! --dst-range");
    }


    if ((fromIp == 0u) && (endIp == 0u)) 
    {/*��ʼ�����ip��Ϊ��������������ν�������*/
	strcat(ipRangeStr," 0.0.0.0-255.255.255.255 ");
    }
    else
    {
	sAddr.s_addr = fromIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)]," %s", inet_ntoa(sAddr));/*��ʼip��ַ*/
	sAddr.s_addr = endIp;
	sprintf(&ipRangeStr[strlen(ipRangeStr)],"-%s ", inet_ntoa(sAddr));/*����ip��ַ*/
    }

    return ipRangeStr;

}
/**
 * ���ݵ�ǰ�����������Ͳ���iptables����time�ִ�
 */
extern char* getTimeForIptables(char* timeStart, char* timeEnd)
{
    static char timeStr[96];

    memset(timeStr, 0, sizeof(timeStr));

    if(strcmp(timeStart, "All")==0){/*ȫ��*/
	sprintf(timeStr," -m time --timestart 00:00 --timestop 23:59 ");
    }
    else
    {
	sprintf(timeStr," -m time --timestart %s --timestop %s ", timeStart, timeEnd);
    }

    return timeStr;

}
/*���ݵ�ǰ�����������Ͳ���iptables�������ִ�*/
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
 * ������ getDateForIptables
 * ���ܣ� ���ݵ�ǰ�����������Ͳ���iptables���������ִ�
 * ������ 2012-04-11
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
 * Ϊcrond���ʱ���ִ�
 * return����̬�ڲ�����
 * */
char* getTimeForCrond(Uchar* time)
{
    static char timestr[16];
    Uint32 hour = 0u, minute = 0u;

    sscanf((char *)time, "%u:%u", &hour, &minute);/*����ʱ��*/

    sprintf(timestr, "%02u %02u", minute, hour);/*crond�ȷֺ�ʱ*/

    return timestr;
}
/**
 * ���������������ͻ�ȡ �ƻ�����crond���������
 * return����̬�ڲ������ִ�
 */
char* getWeekForCrond(Uint32 week_bit)
{
    Uint32 i = 0u;
    static char weekstr[16];

    /*crond����ģ�壨0-6,0���������죩*/
    const char* weekTemplate[] = 
    {
	"1","2","3","4","5","6","0"
    };

    memset(weekstr, 0, sizeof(weekstr));

    strcpy(weekstr, " ");/*������ǰ�����ָ��*/
    /*111 1111 ��λ�������ڼ�*/
    if(week_bit == 0x7fu)
    {/*ÿ�춼ִ��*/
	strcat(weekstr,"*");
    }
    else
    {
	/*ÿ������*/
	for(i=0u;i<7u;i++)
	{
	    if( (week_bit& (0x40u >> i)) != 0u)
	    {
		strcat(weekstr, weekTemplate[i]);
		strcat(weekstr, ",");/*�ָ���*/
	    }
	}
	weekstr[strlen(weekstr) - 1u] = '\0';/*ȥ�����ķָ��*/
    }
    return weekstr;
}
/**
 * ��ʱ��ת��Ϊ ��
 */
Uint32 converTimeToSec(char *time)
{
    Uint32 hour = 0u, mintue = 0u, second = 0u;
    sscanf(time, "%u:%u", &hour, &mintue);/*����ʱ��*/
    second =  (hour*60 + mintue) *60;
    return second;

}
#if (PPPOE_SERVER == FYES)
 /********************************************************************
 * ������ AccAddIpStr
 * ���ܣ� ѭ�������˺Ŷ�Ӧ�����лỰ���ѻỰip������Ƴ���
 * ������ 2012-08-24
 * ������ 
 *	  prof     -    �˺Ŷ�Ӧprofile
 *	  grpName  -    ����
 *	  addOrDel -    ��ӻ���ɾ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
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
		{/*����*/
		    ipsetAOrDIpStr(grpName, nvramProfile->Session[j].ip, addOrDel);/*������Ӧ��ipset��*/
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
 * �޸��ض���LAN�ڵĹ���
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
/*���弴�ÿ������׹�����ߺ�Ҫ���dns�ض������*/
int editDNSRedirectRule(void)
{
    char *lanIf = getLanIfName();
#if (EASY_TASK == FYES)
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;
#endif
    doSystem("iptables -t nat -%c PREROUTING -i %s -p udp --dport 53 -j REDIRECT 1>/dev/null 2>&1", Iptables_DEL, lanIf);/*��ֹ�ظ����*/
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
	/*dns��ָ��·������ͬʱ·�������迪��dns���������ط�ʵ�֣�*/
	doSystem("iptables -t nat -%c PREROUTING -i %s -p udp --dport 53 -j REDIRECT 1>/dev/null 2>&1", Iptables_ADD, lanIf);
    }
    return 0;
}
#endif
