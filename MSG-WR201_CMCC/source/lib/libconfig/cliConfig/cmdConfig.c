#include "uttMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "daughter.h"
#include "cliConvert.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef SRC
#define SRC 1
#define DEST 2
#endif

int cliCmdGetIp(CliConfOp *cliOp);
int cliCmdGetIf(CliConfOp *cliOp);
int cliCmdGetAdmin(CliConfOp *cliOp);
int cliCmdGetIfNum(CliConfOp *cliOp);
#if (DHCP_SERVER == FYES)
int cliCmdGetDhcpServer(CliConfOp *cliOp);
#endif
int cliCmdGetMtu(CliConfOp *cliOp);
int cliCmdGetAuthType(CliConfOp *cliOp);
int cliCmdGetNamePass(CliConfOp *cliOp);
static int DayToBinaryStr(unsigned int day, char *groupDay);
#if (FIREWALL == FYES)
int cliCmdGetAcl(CliConfOp *cliOp);
static int getIpAddress(FireWallProfile *prof, char *address, int flag, int protocol);
#endif
#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
int cliCmdGetRateLimit(CliConfOp *cliOp);
#endif
#endif

#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
static CliConfig cliRateLimitConfig [] =
{
    {"", CMD_LAST_BROTHER | CMD_USE_FATHER_PROF, cliCmdGetRateLimit, NULL, TNONE, 0, NULL, 0}
};
#endif
#endif

static CliConfig cliIfConfig[]=
{   
    {NULL, CMD_LINE_NORMAL, cliCmdGetIp, NULL},
#if 0
    {"mac", CMD_USE_FATHER_PROF,NULL,NULL, TMACADDR, OFFANDSIZE(InterfaceProfile, ether.mac), NULL,0},
#endif
    {NULL, CMD_LINE_NORMAL, cliCmdGetNamePass, NULL},
    {NULL, CMD_LINE_NORMAL, cliCmdGetMtu, NULL},
    {NULL, CMD_LINE_NORMAL, cliCmdGetAuthType, NULL},
#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
    {"rate-limit", CMD_DEPEND_SUB | CMD_LINE_NEND | CMD_LAST_BROTHER | CMD_SUB_DEP_NADD | CMD_CPROF_BY_MIBTYPE | CMD_USE_FATHER_PROF, NULL, cliRateLimitConfig, TNONE, 0, 0, NULL, MIB_PROF_GROUP_BASE},
#endif
#endif
};

#if (STATIC_ROUTE == FYES)
static CliConfig cliRouteConfig [] =
{
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, NULL, NULL, TIPADDR, OFFANDSIZE(StaticRouteProfile, dstAddr), NULL, 0},
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, NULL, NULL, TIPADDR, OFFANDSIZE(StaticRouteProfile, netmask), NULL, 0},
    {"", CMD_LAST_BROTHER | CMD_USE_FATHER_PROF, NULL, NULL, TIPADDR, OFFANDSIZE(StaticRouteProfile, gwAddr), NULL, 0}
};
#endif

#if (STATIC_NAT == FYES)
static CliConfig cliStaticNatConfig [] =
{
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, NULL, NULL, TENUM, OFFANDSIZE(StaticNatProfile, protocol), (Pointer)nat_protocol, 0},
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, NULL, NULL, TIPADDR, OFFANDSIZE(StaticNatProfile, IP), NULL, 0},
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, NULL, NULL, TINT, OFFANDSIZE(StaticNatProfile, OutPort), NULL, 0},
    {"", CMD_USE_FATHER_PROF | CMD_LINE_NEND, cliCmdGetIfNum, 0, 0, 0, NULL, 0},
    {"", CMD_LAST_BROTHER | CMD_USE_FATHER_PROF, NULL, NULL, TINT, OFFANDSIZE(StaticNatProfile, InnerPort), NULL, 0}
};
#endif

#if (FIREWALL == FYES)
static CliConfig cliAccessListConfig [] =
{
    {"", CMD_LAST_BROTHER | CMD_USE_FATHER_PROF, cliCmdGetAcl, NULL, TNONE, 0, NULL, 0},
};
#endif


CliConfig cliRootConfig[]=
{/*这里的配置添加要类似思科的顺序,且要自己维护该顺序，程序不会自动维护。即优先级高的在前面。否则按照命令名称排序*/

    /*如果有子节点使用CMD_USE_FATHER_PROF，则该对应的父节点 mibType必须指定正确*/
    {"interface FastEthernet", CMD_LINE_SPLITER | CMD_DEPEND_SUB , cliCmdGetIf,cliIfConfig ,0,0,0,NULL,MIB_PROF_INTERFACE},
#if (STATIC_ROUTE == FYES)
    {"ip route", CMD_DEPEND_SUB| CMD_SUB_DEP_NADD | CMD_LINE_NEND | CMD_CPROF_BY_MIBTYPE |CMD_SIMILAR_SPLITER, NULL, cliRouteConfig, TNONE, 0 ,0 ,NULL, MIB_PROF_STATIC_ROUTE },
#endif
#if (ADMIN_CONF == FYES)
    {"username admin password", CMD_LINE_SPLITER, cliCmdGetAdmin, NULL, 0, 0, 0, NULL, MIB_PROF_ADMIN_CONF},
#endif
#if (STATIC_NAT == FYES)
    {"ip nat inside source static", CMD_DEPEND_SUB | CMD_SUB_DEP_NADD | CMD_LINE_NEND | CMD_CPROF_BY_MIBTYPE |CMD_SIMILAR_SPLITER, NULL, cliStaticNatConfig, TNONE, 0 ,0 ,NULL, MIB_PROF_STATIC_NAT},
#endif
#if (DHCP_SERVER == FYES)
    {"ip dhcp pool defaultpool", CMD_LINE_SPLITER, cliCmdGetDhcpServer, NULL, 0, 0 ,0 ,NULL, MIB_PROF_DHCP_SERVER},
#endif
#if (FIREWALL == FYES)
    {"access-list", CMD_DEPEND_SUB | CMD_LINE_NEND | CMD_SIMILAR_SPLITER | CMD_SUB_DEP_NADD | CMD_CPROF_BY_MIBTYPE | CMD_LAST_BROTHER, NULL, cliAccessListConfig, TNONE, 0, 0, NULL, MIB_PROF_FIRE_WALL},
#endif
    CLICONFEND
};

/**
 *   return: 0  失败
 *           1 成功   
 *   */
int cliCmdGetIf(CliConfOp *cliOp)
{
    int ret = 1;
    int wanCount = getWanSlotCount();
    InterfaceProfile* profIf;
    CliConfBuildAss* curNodeP = cliOp->curNodeP;
    if(curNodeP->curSimBrother == 0 )
    {/*lan 口*/  
	ret &= cliConfWrite(cliOp,"%d", 0);
    }
    else
    {/*wan口*/
	ret &= cliConfWrite(cliOp,"%d", curNodeP->curSimBrother);
    }

    profIf = ProfGetInstPointByIndex(MIB_PROF_INTERFACE, curNodeP->curSimBrother);
    if((curNodeP->curSimBrother == 0) ||
	    ((ProfInstIsFree(profIf) == 0) && (profIf->head.active == PROF_ENABLE)))
    {
	/*方便子节点使用本节点profile*/
	curNodeP->prof = profIf;
    }
    else
    {
	/*未启用接口。子节点不生成配置*/
	curNodeP->prof = NULL;
    }

    if(curNodeP->curSimBrother < wanCount) 
    {/*还有其他配置*/
	curNodeP->nextSimBrother = curNodeP->curSimBrother + 1;
    }
    else
    {/*已达最大配置*/
	curNodeP->nextSimBrother = -1;
    }
    return ret;
}

#if (ADMIN_CONF == FYES)
/********************************************************************
* 函数： cliCmdGetAdmin
* 功能： 输出admin的配置
* 创建： 2012-12-10
* 参数： 思科命令行默认参数
* 返回： 0  失败
*	 1  成功
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
int cliCmdGetAdmin(CliConfOp *cliOp)
{
    int ret = 0;
    AdminConfProfile* prof;
    CliConfBuildAss* curNodeP = cliOp->curNodeP;

    prof = ProfGetInstPointByIndex(MIB_PROF_ADMIN_CONF, 0);
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	cliConfWrite(cliOp, "%s", prof->password);
	ret = 1;
    } 
    return ret;
}
#endif

#if (STATIC_NAT == FYES)
/********************************************************************
* 函数： cliCmdGetIfNum
* 功能： 输出接口对应的接口号
* 创建： 2012-12-10
* 参数： 思科命令行默认参数
* 返回： 0  失败
*	 1  成功
* 输出： 略
* 作者： Jianqing.Sun 
********************************************************************/
int cliCmdGetIfNum(CliConfOp *cliOp)
{
    int ret = 0;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    StaticNatProfile* prof;
    CliConfBuildAss* curNodeP = cliOp->curNodeP;

    prof = ProfGetInstPointByIndex(MIB_PROF_STATIC_NAT, fNodeP->curSimBrother);
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	cliConfWrite(cliOp, "interface FastEthernet %d", (prof->NatBind[3] - '0'));
	ret = 1;
    } 
    return ret;
}
#endif

int cliCmdGetIp(CliConfOp *cliOp)
{
    int ret = 1;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    InterfaceProfile *profIf = fNodeP->prof;
    struct in_addr sddr;
    char ipstr[32];
    char nmstr[32];

    if(profIf != NULL)
    {/*线路未删除*/
	if((fNodeP->curSimBrother == 0) || (profIf->ether.connMode == ETH_CONN_STATIC))
	{/*lan口或静态接入*/
	    sddr.s_addr =  profIf->ether.sta.ip;
	    strncpy(ipstr, inet_ntoa(sddr), sizeof(ipstr));
	    sddr.s_addr = profIf->ether.sta.nm;
	    strncpy(nmstr, inet_ntoa(sddr), sizeof(nmstr));
	    ipstr [sizeof(ipstr) - 1] = '\0';
	    nmstr [sizeof(nmstr) - 1] = '\0';
	    cliConfWrite(cliOp, "ip address %s %s",ipstr, nmstr);
	}
	else
	{
#if 0
#if (WIRELESS == FYES)
	    if (fNodeP->curSimBrother == get3GSlotIndex()) {
		cliConfWrite(cliOp, "ipaddr 3G");
	    } 
	    else
#endif
#endif
	    {
		if(profIf->ether.connMode == ETH_CONN_DHCP)
		{
		    cliConfWrite(cliOp, "ip address dhcp");
		}
	    }
	}
    }
    return ret;
}


#if (FIREWALL == FYES)
/***********************************************************
 * 函数功能：把Profile中保存的ip地址范围转换为思科命令形式，即any，host，直接输入ip3种
 * 创建时间：2012-12-21
 * 参数：prof--读取数据的结构体，address--存放转换后的命令字符flag--判断源ip地址或目的ip地址
 * 返回值：1--成功 0--失败
 ***********************************************************/
static int getIpAddress(FireWallProfile *prof, char *address, int flag, int protocol)
{
    int ret = 1;
    struct in_addr FromIp, EndIp;
    char FromIpStr[30];
    char EndIpStr[30];
    char portRange[20];
    
    memset(portRange, 0, sizeof(portRange));

    /*根据flag判断src或dest*/
    if (flag == SRC)
    {
	FromIp.s_addr = prof->srcFromIp;
	EndIp.s_addr = prof->srcEndIp;
	if ((protocol == Filter_TCP) || (protocol == Filter_UDP))
	{
	    sprintf(portRange, " %u %u", prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort);
	} 
    }
    else
    {
	FromIp.s_addr = prof->IpFilter.dstFromIp;
        EndIp.s_addr = prof->IpFilter.dstEndIp;

	if ((protocol == Filter_TCP) || (protocol == Filter_UDP))
	{
	    if ((prof->IpFilter.dstFromPort == 1U) && (prof->IpFilter.dstEndPort == 65535U))
	    {
		strcpy(portRange, " any");
	    }
	    else if ((prof->IpFilter.dstFromPort == 135U) && (prof->IpFilter.dstEndPort == 135U))
            {
                strcpy(portRange, " epmap");
            }
	    else if ((prof->IpFilter.dstFromPort == 79U) && (prof->IpFilter.dstEndPort == 79U))
            {
                strcpy(portRange, " finger");
            }
	    else if ((prof->IpFilter.dstFromPort == 21U) && (prof->IpFilter.dstEndPort == 21U))
            {
                strcpy(portRange, " ftp");
            }
	    else if ((prof->IpFilter.dstFromPort == 1720U) && (prof->IpFilter.dstEndPort == 1720U))
            {
                strcpy(portRange, " h.323");
            }
	    else if ((prof->IpFilter.dstFromPort == 443U) && (prof->IpFilter.dstEndPort == 443U))
            {
                strcpy(portRange, " https");
            }
	    else if ((prof->IpFilter.dstFromPort == 445U) && (prof->IpFilter.dstEndPort == 445U))
            {
                strcpy(portRange, " ms-ds");
            }
	    else if ((prof->IpFilter.dstFromPort == 1434U) && (prof->IpFilter.dstEndPort == 1434U))
            {
                strcpy(portRange, " ms-sql-m");
            }
	    else if ((prof->IpFilter.dstFromPort == 1433U) && (prof->IpFilter.dstEndPort == 1433U))
            {
                strcpy(portRange, " ms-sql-s");
            }
	    else if ((prof->IpFilter.dstFromPort == 3389U) && (prof->IpFilter.dstEndPort == 3389U))
            {
                strcpy(portRange, " ms-ts");
            }
	    else if ((prof->IpFilter.dstFromPort == 1863U) && (prof->IpFilter.dstEndPort == 1863U))
            {
                strcpy(portRange, " msnp");
            }
	    else if ((prof->IpFilter.dstFromPort == 139U) && (prof->IpFilter.dstEndPort == 139U))
            {
                strcpy(portRange, " netbios-ssn");
            }
	    else if ((prof->IpFilter.dstFromPort == 110U) && (prof->IpFilter.dstEndPort == 110U))
            {
                strcpy(portRange, " pop3");
            }
	    else if ((prof->IpFilter.dstFromPort == 1723U) && (prof->IpFilter.dstEndPort == 1723U))
            {
                strcpy(portRange, " pptp");
            }
	    else if ((prof->IpFilter.dstFromPort == 25U) && (prof->IpFilter.dstEndPort == 25U))
            {
                strcpy(portRange, " smtp");
            }
	    else if ((prof->IpFilter.dstFromPort == 66U) && (prof->IpFilter.dstEndPort == 66U))
            {
                strcpy(portRange, " sql*net");
            }
	    else if ((prof->IpFilter.dstFromPort == 22U) && (prof->IpFilter.dstEndPort == 22U))
            {
                strcpy(portRange, " ssh");
            }
	    else if ((prof->IpFilter.dstFromPort == 23U) && (prof->IpFilter.dstEndPort == 23U))
            {
                strcpy(portRange, " telnet");
            }
	    else if ((prof->IpFilter.dstFromPort == 80U) && (prof->IpFilter.dstEndPort == 80U))
            {
                strcpy(portRange, " web");
            }
	    else if ((prof->IpFilter.dstFromPort == 68U) && (prof->IpFilter.dstEndPort == 68U))
            {
                strcpy(portRange, " bootpc");
            }
	    else if ((prof->IpFilter.dstFromPort == 67U) && (prof->IpFilter.dstEndPort == 67U))
            {
                strcpy(portRange, " bootps");
            }
	    else if ((prof->IpFilter.dstFromPort == 53U) && (prof->IpFilter.dstEndPort == 53U))
            {
                strcpy(portRange, " dns");
            }
	    else if ((prof->IpFilter.dstFromPort == 500U) && (prof->IpFilter.dstEndPort == 500U))
            {
                strcpy(portRange, " isakmp");
            }
	    else if ((prof->IpFilter.dstFromPort == 1701U) && (prof->IpFilter.dstEndPort == 1701U))
            {
                strcpy(portRange, " l2tp");
            }
	    else if ((prof->IpFilter.dstFromPort == 137U) && (prof->IpFilter.dstEndPort == 137U))
            {
                strcpy(portRange, " netbios-ns");
            }
	    else if ((prof->IpFilter.dstFromPort == 138U) && (prof->IpFilter.dstEndPort == 138U))
            {
                strcpy(portRange, " netbios-dgm");
            }
	    else if ((prof->IpFilter.dstFromPort == 123U) && (prof->IpFilter.dstEndPort == 123U))
            {
                strcpy(portRange, " ntp");
            }
	    else if ((prof->IpFilter.dstFromPort == 161U) && (prof->IpFilter.dstEndPort == 161U))
            {
                strcpy(portRange, " snmp");
            }
	    else if ((prof->IpFilter.dstFromPort == 162U) && (prof->IpFilter.dstEndPort == 162U))
            {
                strcpy(portRange, " snmptrap");
            }
	    else if ((prof->IpFilter.dstFromPort == 69U) && (prof->IpFilter.dstEndPort == 69U))
            {
                strcpy(portRange, " tftp");
            }
	    else
	    {
		sprintf(portRange, " %u %u", prof->IpFilter.dstFromPort, prof->IpFilter.dstEndPort);
	    }
	}
    }
    
    if ((FromIp.s_addr == 0x0) && (EndIp.s_addr == 0x0))
    {
	sprintf(address, "any%s", portRange);
    }
    else if (FromIp.s_addr == EndIp.s_addr)
    {
	memset(FromIpStr, 0, sizeof(FromIpStr));
	strcpy(FromIpStr, inet_ntoa(FromIp));
	if (FromIpStr != NULL)
	{
	    sprintf(address, "host %s%s", FromIpStr, portRange);
	}
	else
	{
	    ret = 0;
	}
    }
    else
    {
	memset(FromIpStr, 0, sizeof(FromIpStr));
	strcpy(FromIpStr, inet_ntoa(FromIp));
	memset(EndIpStr, 0, sizeof(EndIpStr));
	strcpy(EndIpStr, inet_ntoa(EndIp));
	if ((FromIpStr != NULL) && (EndIpStr != NULL))
	{
	    sprintf(address, "%s %s%s", FromIpStr, EndIpStr, portRange);
	}
	else
	{
	    ret = 0;
	}
    }

    return ret;
}
#endif

/********************************************
 * 函数功能：将MTU值转换成思科命令ip mtu 
 * 创建时间：2012-12-31
 ******************************************/
int cliCmdGetMtu(CliConfOp *cliOp)
{
    int ret = 1;
    int mtu = 0;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    InterfaceProfile *profIf = fNodeP->prof;
    
    if (profIf != NULL)
    {
	if((fNodeP->curSimBrother != 0) && (profIf->ether.connMode == ETH_CONN_PPPOE))
	{
	    mtu = profIf->ether.pppoe.mtu;
	    if (mtu != 0)
	    {
		cliConfWrite(cliOp, "ip mtu %d", mtu);
	    }
	}
    }
    return ret;    
}

/********************************************
 * 函数功能:将拨号验证方式为PAP的转换为思科命令ppp authentication pap callout 
 * 创建时间：2012-12-31
 ******************************************/
int cliCmdGetAuthType(CliConfOp *cliOp)
{
    int ret = 1;
    int authType = 0;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    InterfaceProfile *profIf = fNodeP->prof;

    if (profIf != NULL)
    {
        if((fNodeP->curSimBrother != 0) && (profIf->ether.connMode == ETH_CONN_PPPOE))
        {
            authType = profIf->ether.pppoe.authType;
            if (authType == PPP_AUTH_PAP)
            {
                cliConfWrite(cliOp, "ppp authentication pap callout");
            }
        }
    }
    return ret;
}

#if (DHCP_SERVER == FYES)
/*
 * 函数： cliCmdGetDhcpServer
 * 功能： 输出Dhcp Server的信息
 * 创建： 2012-12-20
 * 参数： 思科命令行默认参数
 * 返回： 0  失败
 *	  1  成功
 * 输出： 略
 * 作者 
 * */
int cliCmdGetDhcpServer(CliConfOp *cliOp) 
{
    int ret = 0;
    DhcpServerProfile* prof;
    CliConfBuildAss* curNodeP = cliOp->curNodeP;
    struct in_addr addrFrom;
    struct in_addr addrTo;
    struct in_addr iptmp;

    prof = ProfGetInstPointByIndex(MIB_PROF_DHCP_SERVER, 0);
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	iptmp.s_addr = prof->ipStart;
	cliConfWrite(cliOp, "\n included-address %s", inet_ntoa(iptmp));
	iptmp.s_addr = prof->ipEnd;
	cliConfWrite(cliOp, " %s", inet_ntoa(iptmp));
	iptmp.s_addr = prof->netmask;
	cliConfWrite(cliOp, " %s\n", inet_ntoa(iptmp));
	if(prof->gateway != 0U) {
	    memset(&iptmp, 0, sizeof(iptmp));
	    iptmp.s_addr = prof->gateway;
	    cliConfWrite(cliOp, " default-router %s\n", inet_ntoa(iptmp));
	}
	if(prof->pDns != 0U) {
	    memset(&iptmp, 0, sizeof(iptmp));
	    iptmp.s_addr = prof->pDns;
	    cliConfWrite(cliOp, " dns-server %s", inet_ntoa(iptmp));
	    if(prof->sDns != 0U) {
	    memset(&iptmp, 0, sizeof(iptmp));
		iptmp.s_addr = prof->sDns;
		cliConfWrite(cliOp, " %s", inet_ntoa(iptmp));
	    }
	}
	if(prof->pPDns != 0U){
	    memset(&iptmp, 0, sizeof(iptmp));
	    iptmp.s_addr = prof->pPDns;
	    cliConfWrite(cliOp, "\n ip name-server %s", inet_ntoa(iptmp));
	    if(prof->sPDns != 0U) {
	    memset(&iptmp, 0, sizeof(iptmp));
		iptmp.s_addr = prof->sPDns;
		cliConfWrite(cliOp, " %s", inet_ntoa(iptmp));
	    }
	}
	ret = 1;
    } 
    return ret;

}
#endif


/********************************************
 * 函数功能:将pppoe设置的用户名和密码转换为思科命令 
 * 创建时间：2012-12-31
 ******************************************/
int cliCmdGetNamePass(CliConfOp *cliOp)
{
    int ret = 1;
    char *username = NULL;
    char *password = NULL;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    InterfaceProfile *profIf = fNodeP->prof;

    if (profIf != NULL)
    {
        if ((fNodeP->curSimBrother != 0) && (profIf->ether.connMode == ETH_CONN_PPPOE))
        {
	    username = profIf->ether.pppoe.user;
	    password = profIf->ether.pppoe.passwd;
            if ((strcmp(username, "") != 0) && (strcmp(password, "") != 0))
            {
                cliConfWrite(cliOp, "ppp pap sent-username %s password %s", username, password);
            }
        }
    }
    return ret;
}

/*********************************************************
 * 函数功能：将无符号10进制转换成形式像二进制的整数
 * 创建时间：2012-12-28
 * 备注：并不是真的转换成二进制，只是输出时看起来是，如127U转换成1111111
 *********************************************************/
static int UintToBinary(unsigned int day)
{
    int ret = 0;
    int multiple = 1;
    for (;day != 0U; multiple *= 10, day /= 2U)
    {
	if ((day % 2U) == 1U)
	{
	    ret += multiple;
	}
    }
    return ret;
}

#if (FIREWALL == FYES)
/************************************************************
 * 函数功能：把访问控制策略实例转换为思科命令
 * 创建时间：2012-12-21
 * 返回值：0--成功 1--失败
 **********************************************************/
int cliCmdGetAcl(CliConfOp *cliOp)
{
    int ret = 1;/*返回值*/
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    FireWallProfile *prof= fNodeP->prof;

    char *enable = NULL;/*启用禁用*/
    char *action = NULL; /*动作*/
    char *protocol = NULL;/*协议字符串*/
    char source[80];/*源ip*/
    char dest[80];/*目的ip*/
    int groupDay = 0;

    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->addrTypeEn == ADDR_IPRANGE) && (prof->FilterType == TYPE_FILTER_IP))
    {
	/*启用禁用*/
	if (prof->head.active == TRUE)
	{
	    enable = "open";
	}
	else
	{
	    enable = "close";
	}
    
	/*1为permit，0为deny*/
	if (prof->Status == 1)
	{
	    action = "permit";
	}
	else
	{
	    action = "deny";
	}

	/*协议类型*/
	if (prof->IpFilter.Protocol == Filter_TCP)
	{
	    protocol = "tcp";
	}
	else if (prof->IpFilter.Protocol == Filter_UDP)
	{
	    protocol = "udp";
	}
	else if (prof->IpFilter.Protocol == Filter_ICMP)
	{
	    protocol = "icmp";
	}
	else if (prof->IpFilter.Protocol == Filter_AH)
        {
            protocol = "ahp";
        }
	else
	{
            protocol = "ip";
	}

	memset(source, 0, sizeof(source));
	/*得到源ip地址范围对应的输入方式，放入source中*/
	if (getIpAddress(prof, source, SRC, prof->IpFilter.Protocol) != 1)/*SRC表示源ip地址*/
	{
	    ret = 0;
	}
	else
	{
	    memset(dest, 0, sizeof(dest));
	    if (getIpAddress(prof, dest, DEST, prof->IpFilter.Protocol) != 1)/*DEST表示目的ip地址*/
	    {
		ret = 0;
	    }
	    else
	    {
		groupDay = UintToBinary(prof->fw_day);
		if ((strcmp(prof->fw_timestart, "All") == 0) && (strcmp(prof->fw_timestop, "0") == 0))
		{
		    cliConfWrite(cliOp, "%s %s %s %s %s %s %d any", prof->head.name, enable, action, protocol, source, dest, groupDay);
		}
		else
		{
		    cliConfWrite(cliOp, "%s %s %s %s %s %s %d %s %s", prof->head.name, enable, action, protocol, source, dest, groupDay, prof->fw_timestart, prof->fw_timestop);
		}
	    }
	}

    }	

    return ret; 
}
#endif

#if (GROUP_BASE == FYES)
#ifdef CONFIG_UTT_NETFILTER
int cliCmdGetRateLimit(CliConfOp *cliOp)
{
    int ret = 1;
    CliConfBuildAss* fNodeP = cliOp->curNodeP->father;
    if (fNodeP->father->curSimBrother != 0)
    {
	return 1;
    }
    GroupBaseProfile *prof = fNodeP->prof;
    
    char fromIPStr[30];
    char endIPStr[30];
    char limitPolicy[8];
    int groupDay = 0;
    struct in_addr fromIP, endIP;
 
    if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->groupType == GBT_FINE_LIMIT) && (prof->addrTypeEn == ADDR_IPRANGE))
    {
	fromIP.s_addr = prof->ipStart;
        memset(fromIPStr, 0, sizeof(fromIPStr));
        strcpy(fromIPStr, inet_ntoa(fromIP));

        endIP.s_addr = prof->ipEnd;
	memset(endIPStr, 0, sizeof(endIPStr));
	strcpy(endIPStr, inet_ntoa(endIP));

	memset(limitPolicy, 0, sizeof(limitPolicy));
	if (prof->limitPolicy == EXCLUSIVE_RATE)
	{
	    strcpy(limitPolicy, "Each");
	}
	else
	{
	    strcpy(limitPolicy, "Share");
	}

	groupDay = UintToBinary(prof->day);

	if ((strcmp(prof->timeStart, "All") == 0) && (strcmp(prof->timeStop, "0") == 0))
	{
	    cliConfWrite(cliOp, "%s %s %s %s input %u output %u %d any", prof->head.name, fromIPStr, endIPStr, limitPolicy, prof->dRate, prof->uRate, groupDay);
	}
	else
	{
	    cliConfWrite(cliOp, "%s %s %s %s input %u output %u %d %s %s", prof->head.name, fromIPStr, endIPStr, limitPolicy, prof->dRate, prof->uRate, groupDay, prof->timeStart, prof->timeStop);
	}
    }
    return ret;
}
#endif
#endif
