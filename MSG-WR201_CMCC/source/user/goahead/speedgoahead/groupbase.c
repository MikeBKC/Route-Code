#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "ldra.h"
#include    <string.h>
#include    <dirent.h>
#include "pdb.h"
#include "groupbase.h"

#if (GROUP_BASE == FYES)
#ifdef  GROUP_SE_DBG 
#define GROUP_SE_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define GROUP_SE_PRINTF(fmt, args...)
#endif
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
extern void actPolicyDelAll(void);
extern int uttNfNlPolicyDelAll(void);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif
#endif

extern void groupbaseInit(void);
extern void groupiptablesInit(void);
static void iptablesClearAllPdbChain(const GroupBaseProfile *prof);
extern void EditOneInstGroup(GroupBaseProfile *prof, char action);
static void EditOneActGroup(GroupBaseProfile *prof, char action);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
static void EditOneFineLimitGroup(const GroupBaseProfile *prof, char action);
#endif
extern void initPdbPolicyRule(void) ;
static char cmdstr[512];
#ifdef CONFIG_UTT_NETFILTER
static int changeCountByAllPdbPolicy(void);
static int changeCountByOnePdbPolicy(Uint8 *policy);
static int pdb_policy_match(Uint8 *policy, int index);
extern void createPdbL7IndRule(PdbProfile *prof);
extern void createPdbPortIndRule(PdbProfile *prof);
extern void createDnsIndRule(PdbProfile *prof);
extern int createIndRuleByCount(void);
extern void reNewPdbChainJumpRule(void);
static int count[MAX_PDB_PROFILES];/*记录每个行为管理类型开启的次数,比如QQ一共开启了两次，对应的数字为2*/
#endif

extern void utt_init_GROUP_LIMIT_CHAIN(LimitType type, char action)
{
    static char groupLimitCount = 0;/*精细化限速规则条数*/
    char groupLimitCountBak = groupLimitCount;
#if (PPPOE_LIMIT == FYES)
    static char pppAccLimitCount = 0;/*PPPoE限速规则条数*/
    char pppAccLimitCountBak = pppAccLimitCount;
#endif

    if (type == FIXED_RATE_LIMIT)
    {
	if (action == Iptables_ADD)
	{
	    groupLimitCount += 1;
	}
	else if (action == Iptables_DEL)
	{
	    groupLimitCount -= 1;
	}
	else if (action == Iptables_FLUSH)
	{
	    groupLimitCount = 0;
	}
    }
#if (PPPOE_LIMIT == FYES)
    else if (type == PPPOE_ACC_LIMIT)
    {
	if (action == Iptables_ADD)
	{
	    pppAccLimitCount += 1;
	}
	else if (action == Iptables_DEL)
	{
	    pppAccLimitCount -= 1;
	}
	else if (action == Iptables_FLUSH)
	{
	    pppAccLimitCount = 0;
	}
    }
#endif
    if (groupLimitCountBak == 0)
    {
	if (groupLimitCount != 0)
	{
	    doSystem("iptables -t mangle -I FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#if (WIFI_RATE_LIMIT == FYES)
	    doSystem("iptables -t mangle -D FORWARD -j "WIFI_RATE_LIMIT_CHAIN" 1>/dev/null 2>&1");
	    doSystem("iptables -t mangle -I FORWARD -j "WIFI_RATE_LIMIT_CHAIN" 1>/dev/null 2>&1");
#endif
	}
    }
    else
    {
	if (groupLimitCount == 0)
	{
	    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
	}
    }
#if (PPPOE_LIMIT == FYES)
    if ((groupLimitCountBak == 0) || (pppAccLimitCountBak == 0))
    {
	if ((groupLimitCount != 0) && (pppAccLimitCount != 0))
	{
	    doSystem("iptables -t mangle -I %s -m uttdev --lan-to-wan  -m set --set %s src -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	    doSystem("iptables -t mangle -I %s -m uttdev --wan-to-lan  -m set --set %s dst -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	}
    }
    else
    {
	if ((groupLimitCount == 0) || (pppAccLimitCount == 0))
	{
	    doSystem("iptables -t mangle -D %s -m uttdev --lan-to-wan  -m set --set %s src -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	    doSystem("iptables -t mangle -D %s -m uttdev --wan-to-lan  -m set --set %s dst -j RETURN 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, POE_SKIP_FIXED_LIMIT);
	}
    }
#endif
    return;
}

void GroupBaseProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const GroupBaseProfile* oldProfileDataPtr, const GroupBaseProfile* newProfileDataPtr )
{
#ifdef CONFIG_UTT_NETFILTER
    GroupBaseProfile* prof = NULL;
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(newProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)newProfileDataPtr,Iptables_ADD);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)newProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_EDIT:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)oldProfileDataPtr,Iptables_DEL);
	    EditOneInstGroup((GroupBaseProfile *)newProfileDataPtr,Iptables_ADD);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_DEL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    EditOneInstGroup((GroupBaseProfile *)oldProfileDataPtr,Iptables_DEL);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_DELALL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType == GBT_FINE_LIMIT) {
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
	    } else {
#endif
	    iptablesClearAllPdbChain((GroupBaseProfile *)oldProfileDataPtr);
#ifdef CONFIG_UTT_NETFILTER
	    prof = (GroupBaseProfile *)oldProfileDataPtr;
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif
	    break;
	case PROF_CHANGE_SPECIAL:
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(oldProfileDataPtr->groupType != GBT_FINE_LIMIT) {
#endif
#ifndef CONFIG_UTT_NETFILTER
	    /* 清除上网行为管理PDB所在的链 */
	    doSystem("iptables -t mangle -F %s" ,L7PROTO_PDB_CHAIN);
	    doSystem("iptables -t filter -F %s" ,PORT_DNS_PDB_CHAIN);
	    doSystem("iptables -t filter -F %s" ,DNS_PDB_CHAIN_LOOP);
#endif
	    doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_CHAIN);
	    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
		doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_PRE_CHAIN);
		doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_POST_CHAIN);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    } else {/*修改规则，则先全部删除，然后再逐个添加*/
		sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),PROF_CHANGE_DELALL);
	    }
#endif
	    groupbaseInit();
	    break;
	default:
	    break;
    }
#ifdef CONFIG_UTT_NETFILTER
    if((prof != NULL) && 
	    (prof->groupType ==  GBT_ACT_CONTROL))
    {
	/*删除内核所有策略
	 * 让内核再次生成*/
	actPolicyDelAll();
    }
    
    /*GroupBaseProfile有改变时改变数组*/
    changeCountByAllPdbPolicy();
    /*重新建立识别规则，如果没开启上网行为管理开关，会中途退出，不生成规则*/
    createIndRuleByCount();

#endif
    return;
}

#ifndef CONFIG_UTT_NETFILTER
#if (UTTPDB == FYES)
/*
 *
 *对iptales domian 类型的pdbname进行处理
 *
 */
static void _pdbProfAddDomainInit(char *str[],int lineNum,char grouptype,char *ipstr,char *timestr, char *daystr,FILE *fp)
{

    char temp[MAX_PDB_LINE_LENGTH],*temp1;
    char *ptr;
    char slipchar[]=",";
    int i;
    char domianstr[256];
    char cmdstr[256];
    for(i=0;i<lineNum;i++){
	if (strlen(str[i]) == 0) {
	    continue;
	}
	strcpy(temp,str[i]);
	temp1 = strtok_r(temp,slipchar,&ptr);
	while(temp1){
	    if(strlen(temp1) > 4 ){
		/*
		 * 对每一个域名进行iptables domain处理
		 */
		memset(domianstr, 0x00, sizeof(domianstr));
		memset(cmdstr, 0x00, sizeof(cmdstr));
		sprintf(domianstr," -m domain --name \"%s\" ",temp1);
		fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP\n",grouptype,PORT_DNS_PDB_CHAIN,ipstr,domianstr,timestr,daystr);
		fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP\n",grouptype,DNS_PDB_CHAIN_LOOP,ipstr,domianstr,timestr,daystr);

	    }
	    temp1=strtok_r(NULL,slipchar,&ptr);
	}
    }
}
#endif
#endif

/*上网行为管理设置*/
static void EditOneActGroup(GroupBaseProfile *prof, char action)
{
#ifdef CONFIG_UTT_NETFILTER
    if(action ==  Iptables_ADD)
    {/*加规则的时候进行转换*/
	uttSemP(SEM_UTTNF_NO,0);/*加锁 防止冲突*/
	if(strcmp(prof->timeStart, "All") == 0)
	{
	    prof->sTimeStart = 0u;/*从零秒开始*/
	    prof->sTimeStop = converTimeToSec("24:00");/*最后一秒结束*/
	}
	else
	{
	    /*转换为秒*/
	    prof->sTimeStart = converTimeToSec(prof->timeStart);
	    prof->sTimeStop = converTimeToSec(prof->timeStop);
	}
	uttSemV(SEM_UTTNF_NO,0);/*释放锁防止冲突*/
    }
#else
    /* FomeIp,EndIp used for lan Users, while FromIp2,EndIp2 used for PPPoE clients */
    unsigned int Allow;

    FILE *fp;
    /*pdb name strtok by PdbPolicy */
    char pdbname[20], pdbPolicys[256]; 
    char porttmp[512],portstr[512];

    /* initiate  local vairables */
    memset(pdbname, 0x00, sizeof(pdbname));
    memset(pdbPolicys, 0x00, sizeof(pdbPolicys));
    memset(cmdstr, 0x00, sizeof(cmdstr));
    memset(porttmp, 0x00, sizeof(porttmp));

    //doSystem("cat /dev/null>/sbin/GroupScript.sh");
    if((fp = fopen("/sbin/GroupScript.sh","w+")) <= 0)
    {
	DBGPRINT("open GroupScript.sh is error!\n");
    }
    Allow = prof->head.active;
    strcpy(pdbPolicys, prof->pdbPolicy);

    char* ipstr, *timestr, *daystr;
    char typestr[100];
    memset(typestr, 0x00, sizeof(typestr));


    ipstr = getIpRangeForIptables(prof->ipStart, prof->ipEnd, 1);
    timestr = getTimeForIptables(prof->timeStart, prof->timeStop);
    daystr = getDayForIptables(prof->day);
    /*
     * get PDB profile
     */
#if (UTTPDB == FYES)
    MibProfileType mibTypePdb = MIB_PROF_PDB_GLOBAL;
    PdbProfile *profPdb = NULL;
    char *tmp;
    /*
     *	每一个上网行为管理的实例 pdbPolicys 会分割出不同的pdbname,  每一个pdbname又是一个实例，它存储了pdbname对应的相关的信息
     */
    tmp = strtok(pdbPolicys, UTT_SPLIT_SIGN_RECORDS);
    while(tmp != NULL){
	strcpy(pdbname, tmp);
	profPdb = (PdbProfile*)ProfGetInstPointByName(mibTypePdb, pdbname);
	if(profPdb != NULL && ProfInstIsFree(profPdb) == 0 && profPdb->head.active == TRUE) {
	    /*
	     *  不同的PDB类型进行不同的实现方式  L7PROTO_PDB_CHAIN
	     */
	    if(profPdb->type == L7Pdb) {
		/* L7 通过iptables l7 实现 */
		sprintf(typestr," -m layer7 --l7proto %s ",pdbname);
		fprintf(fp,"iptables -t mangle -%c %s %s%s%s%s -j DROP 1>/dev/null 2>&1\n",action,L7PROTO_PDB_CHAIN,ipstr,typestr,timestr,daystr);
	    }
	    else if(profPdb->type == PortPdb) {
		/* PORT 通过控制iptables port 控制 将所有端口保存在porttmp中*/
		if(profPdb->line[0] != NULL)
		{
		    strcat(porttmp,profPdb->line[0]);
		    strcat(porttmp,",");
		}
		else
		{;}
	    }
	    else if(profPdb->type == DnsPdb) {


		/* DNS 通过域名iptables domain实现*/
		if(strcmp(pdbname,defaultPdbName4) == 0) {
		    /*webQQ */
		    /*  以下两条策略是解决 禁止w.qq.com 而匹配了www.qq.com 所以就需要先放掉 */
		    fprintf(fp,"iptables -t filter -%c %s %s -m domain --name www.qq.com %s%s -j ACCEPT\n",action,PORT_DNS_PDB_CHAIN,ipstr,timestr,daystr);
		    fprintf(fp,"iptables -t filter -%c %s %s -m domain --name www.qq.com %s%s -j ACCEPT\n",action,DNS_PDB_CHAIN_LOOP,ipstr,timestr,daystr);
		    _pdbProfAddDomainInit(pdb_webQQ, MAX_PDB_WEBQQ_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName23) == 0) {
		    /* 游戏网址*/
		    _pdbProfAddDomainInit(pdb_GAMEURL, MAX_PDB_GAMEURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName24) == 0) {
		    /* 证券网站  */
		    _pdbProfAddDomainInit(pdb_STOCKURL, MAX_PDB_STOCKURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName25) == 0) {
		    /* 购物网站  */
		    _pdbProfAddDomainInit(pdb_SHOPURL, MAX_PDB_SHOPURL_LINES,action,ipstr,timestr,daystr,fp);
		}else if(strcmp(pdbname,defaultPdbName26) == 0) {
		    /* 社交网站  */
		    _pdbProfAddDomainInit(pdb_SOCIALURL, MAX_PDB_SOCIALURL_LINES,action,ipstr,timestr,daystr,fp);
		}else
		{;}
	    }
	}
	else
	{;}
	tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
    /*
     *将所有PORT类型的端口收集起来，组合成一条规则
     *   在linux中有对 -m multiport --dport 的端口限制 以前是15个端口，已经修改为50个
     */
    /*去掉最后面的，号*/
    porttmp[strlen(porttmp)-1] = '\0';
    sprintf(portstr,"  -p tcp -m multiport --dport %s ",porttmp);
    fprintf(fp,"iptables -t filter -%c %s %s%s%s%s -j DROP 1>/dev/null 2>&1\n",action,DNS_PDB_CHAIN_LOOP,ipstr,portstr,timestr,daystr);
    fclose(fp);
    doSystem("GroupScript.sh");
    GROUP_SE_PRINTF("Run GroupScript.sh,please wait.....\n");
#endif
#endif
}

/**
 * 为精细化限速获取ip范围
 * ipstrRet:存储结果信息
 */
static void getIpRangeForFineLimit(char* ipstrRet, const GroupBaseProfile *prof, bool isSrc)
{
    char* ipstr = NULL;
    char *dst_ipstr = NULL;
    if(prof->addrTypeEn == ADDR_IPRANGE)
    {/*按网段进行限制*/
        ipstr = getIpRangeForIptables(prof->ipStart, prof->ipEnd, isSrc);/*ip范围*/
        strcpy(ipstrRet, ipstr);
    }
#if (IP_GRP == FYES)
    else
    {/*按组进行限速*/
        if(prof->ipGrpName[0] == '\0')/*所有用户*/ {
            ipstr = getIpRangeForIptables(0u, 0u, isSrc);/*ip范围*/
            strcpy(ipstrRet, ipstr);
        } else {/*ipset组*/
            sprintf(ipstrRet, " -m set --set %s ", prof->ipGrpName);
            if(isSrc) {
                strcat(ipstrRet, "src ");
            } else {
                strcat(ipstrRet,"dst ");
            }
        }
    }
#endif
    if(prof->dst_addrTypeEn == ADDR_IPRANGE )
    {
        dst_ipstr = getDstIpRangeForIptables(prof->dst_ipStart, prof->dst_ipEnd, !isSrc);
        if(prof->addrTypeEn == ADDR_IPGRP)
        {
            strcat(ipstrRet, "-m iprange ");
        }
        strcat(ipstrRet, dst_ipstr);
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstrRet);
    }
#if(IP_GRP == FYES)
    else
    {
        if(prof->dst_ipGrpName[0] == '\0'){
            dst_ipstr = getDstIpRangeForIptables(0u, 0u, !isSrc);
            strcat(ipstrRet, dst_ipstr);
        }else{
            //sprintf(ipstrRet, " -m set --set %s ", prof->dst_ipGrpName);
            strcat(ipstrRet, " -m set --set ");
            strcat(ipstrRet, prof->dst_ipGrpName);
            if(!isSrc){
                strcat(ipstrRet, " src");
            }else{
                strcat(ipstrRet, " dst");
            }
        }
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstrRet);
     }
#endif


    return;
}

#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
#define IF_DIR_LEN  32
/**
 * 精细化带宽设置
 * TODO:
 * 增加lan-to-wan和wan-to-lan之后对VPN线路不限速了，需要在proc/sys/net/ipv4/conf中对应的if_index输入ppp虚接口对应的实接口
 */
static void EditOneFineLimitGroup(const GroupBaseProfile *prof, char action)
{
    char ipstr[106], *timestr, *daystr;
    char ratestr[32];
    char limittype[32];
    int i = 0;

    Uint32 rateTmp = 0u;
    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, action);
	/* mantis_0011328 */

#if 1
    char if_dir[IF_DIR_LEN + 1] = {0};
#endif

    /*添加上传以及下载限速*/
    for(i = 0 ;i < 2; i++)
    {
        memset(ipstr, 0, sizeof(ipstr));
#if 1
        memset(if_dir, 0, IF_DIR_LEN + 1);
#endif
        if( i == 0)
        {/*上传限速*/
            getIpRangeForFineLimit(ipstr, prof, 1);/*源ip范围*/
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstr);
            strcpy(limittype, "src");/*针对源ip进行限速*/
            rateTmp = prof->uRate;
            //strcpy(dst_limittype, "src"); /*针对目的地址组源ip进行限速*/
#if 1
            strncpy(if_dir, " -m uttdev --lan-to-wan ", IF_DIR_LEN);
#endif
        }
        else
        {/*下载限速*/
            getIpRangeForFineLimit(ipstr, prof, 0);/* 目的ip范围*/
GROUP_SE_PRINTF("%s------%d------------%s", __func__, __LINE__, ipstr);
            strcpy(limittype, "dst");/*针对目的ip进行限速*/
            rateTmp = prof->dRate;
            //strcpy(dst_limittype, "dst");/*针对目的地址组目的ip进行限速*/

#if 1
            strncpy(if_dir, " -m uttdev --wan-to-lan ", IF_DIR_LEN);
#endif
        }
        timestr = getTimeForIptables((char*)prof->timeStart, (char*)prof->timeStop);/*iptables时间参数*/
        daystr = getDayForIptables(prof->day);/*iptables天参数*/

        if(rateTmp == 0u)
        {
            /* * 上传、下载为零时不限速，直接返回 -j RETURN **/
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -j RETURN", action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr);
            GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -j RETURN\n", action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr);
            doSystem(cmdstr);
        }
        else
        {
            if(prof->limitPolicy == EXCLUSIVE_RATE)
            {/*独立带宽*/
                sprintf(ratestr , "%.0u", rateTmp*1024 /8);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
                if(i==0){
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in  %s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,GROUP_LIMIT_PRE_CHAIN,  ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
                }else{
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out  %s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN", action ,GROUP_LIMIT_POST_CHAIN,  ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
                }
#else
#if 0
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s -m hashlimit --hashlimit-name %s%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, ipstr, timestr,daystr ,prof->head.name, limittype, ratestr ,ratestr, limittype);
#else
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -m hashlimit --hashlimit-name %d%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr,  timestr,daystr ,prof->ProfIndex, limittype, ratestr ,ratestr, limittype);
                GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -m hashlimit --hashlimit-name %d%s --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode %sip -j RETURN\n",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,prof->ProfIndex, limittype, ratestr ,ratestr, limittype);
#endif
#endif
                doSystem(cmdstr);
            } 
            else 
            {/*共享带宽*/
                sprintf(ratestr , "%.0u", rateTmp*1024 /(8*32));
                /* share rate */
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
                if(i==0){
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in %s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                            action ,GROUP_LIMIT_PRE_CHAIN,  ipstr,timestr,daystr ,ratestr ,ratestr);
                }else{
                    sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out %s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                            action ,GROUP_LIMIT_POST_CHAIN,  ipstr,timestr,daystr ,ratestr ,ratestr);
                }
#else
#if 0
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN", action ,GROUP_LIMIT_CHAIN,ipstr,timestr,daystr ,ratestr ,ratestr);
#else
                sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,ratestr ,ratestr);
                GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s %s -m limit --limit %s/s --limit-burst %s -j RETURN\n",
                        action ,GROUP_LIMIT_CHAIN, if_dir, ipstr, timestr,daystr ,ratestr ,ratestr);
#endif
#endif
                /* make is in use */
                doSystem(cmdstr);
            }
            /* drop packages while rate is overflow */
#if 0
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s -j DROP", action ,GROUP_LIMIT_CHAIN ,ipstr,timestr,daystr);
#else
            /* *   如果已经限速，当速度达到限速值后将数据包丢弃 -j DROP  **/
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
            if(i==0){
                sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-in %s%s%s -j DROP", action , GROUP_LIMIT_PRE_CHAIN , ipstr,timestr,daystr);
            }else{
                sprintf(cmdstr, "iptables -t mangle -%c %s -m uttdev --is-lan-out %s%s%s -j DROP", action ,GROUP_LIMIT_POST_CHAIN , ipstr,timestr,daystr);
            }
#else
            sprintf(cmdstr, "iptables -t mangle -%c %s%s%s%s%s -j DROP", action ,GROUP_LIMIT_CHAIN ,if_dir, ipstr, timestr,daystr);
            GROUP_SE_PRINTF("iptables -t mangle -%c %s%s%s%s%s -j DROP\n", action ,GROUP_LIMIT_CHAIN ,if_dir, ipstr, timestr,daystr);
#endif
            doSystem(cmdstr);
        }
#endif
    }
    return;
}
#endif

void EditOneInstGroup(GroupBaseProfile *prof, char action)
{
    if(action ==  Iptables_ADD && prof->head.active == FALSE)
    {
	/* 假如上网行为管理的实例不被允许，则跳出此条操作*/
        return;
    }

    /* 清除上网行为管理PDB所在的链 */
    if(prof->groupType == GBT_ACT_CONTROL)
    {/*上网行为管理*/
        EditOneActGroup(prof, action);
#ifndef CONFIG_PERFORMANCE_OPTIMIZATION
    }
    else
    {/*精细化带宽*/
        if(prof->groupType == GBT_FINE_LIMIT)
	{
	    EditOneFineLimitGroup(prof, action);
	}
    /* 清除上网行为管理PDB所在的链 */
#endif
    }
    return;
}

void groupiptablesInit(void)
{
    /*精细化限速初始化*/
    doSystem("iptables -t mangle -D FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -D PREROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN);
#endif
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, GROUP_LIMIT_CHAIN);
    utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	 doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN, GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN, GROUP_LIMIT_POST_CHAIN);
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I PREROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_PRE_CHAIN, GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I POSTROUTING -j %s 1>/dev/null 2>&1", GROUP_LIMIT_POST_CHAIN, GROUP_LIMIT_POST_CHAIN);
#endif
#if 0    
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I FORWARD -j %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN, GROUP_LIMIT_CHAIN);
#else
	doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1", GROUP_LIMIT_CHAIN);
#endif
#ifndef CONFIG_UTT_NETFILTER
    /*
     * init L7PROTO
     */
    doSystem("iptables -t mangle -D POSTROUTING -j %s 1>/dev/null 2>&1", L7PROTO_PDB_CHAIN);
    doSystem("iptables -t mangle -F %s  1>/dev/null 2>&1; iptables -t mangle -X %s  1>/dev/null 2>&1", L7PROTO_PDB_CHAIN, L7PROTO_PDB_CHAIN);
    doSystem("iptables -t mangle -N %s 1>/dev/null 2>&1; iptables -t mangle -I POSTROUTING 1 -j %s 1>/dev/null 2>&1", L7PROTO_PDB_CHAIN, L7PROTO_PDB_CHAIN);
    /*
     *init portpdb  and dns forword chain
     */
    doSystem("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN, PORT_DNS_PDB_CHAIN);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", PORT_DNS_PDB_CHAIN, PORT_DNS_PDB_CHAIN);
/*
 * init dns loop chain with input chain
 */
    doSystem("iptables -t filter -D INPUT -j %s 1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP);
    doSystem("iptables -t filter -F %s  1>/dev/null 2>&1; iptables -t filter -X %s  1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP, DNS_PDB_CHAIN_LOOP);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1; iptables -t filter -A INPUT -j %s 1>/dev/null 2>&1", DNS_PDB_CHAIN_LOOP, DNS_PDB_CHAIN_LOOP);
#endif
    groupbaseInit();
}

static void iptablesClearAllPdbChain(const GroupBaseProfile *prof)
{
#ifndef CONFIG_UTT_NETFILTER
    if(prof->groupType == GBT_ACT_CONTROL)
    {
	/* 清除上网行为管理PDB所在的链 */
	doSystem("iptables -t mangle -F %s" ,L7PROTO_PDB_CHAIN);
	doSystem("iptables -t filter -F %s" ,PORT_DNS_PDB_CHAIN);
	doSystem("iptables -t filter -F %s" ,DNS_PDB_CHAIN_LOOP);
    }
#endif
    if(prof->groupType == GBT_FINE_LIMIT)
    {
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_CHAIN);
#if (HTTP_CACHE==FYES || CACHE_SERVER==FYES)
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_PRE_CHAIN);
	doSystem("iptables -t mangle -F %s" ,GROUP_LIMIT_POST_CHAIN);
#endif
	utt_init_GROUP_LIMIT_CHAIN(FIXED_RATE_LIMIT, Iptables_FLUSH);
    }
}

void groupbaseInit(void)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
#if 0
    iptablesClearAllPdbChain();
#endif

    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == TRUE) {
	    prof->ProfIndex = i + 1;
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    if(prof->groupType == GBT_FINE_LIMIT) {/*精细化限速*/
		sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	    } else {
#endif
	    /*
	     *edit one Inst Group 
	     */
	    EditOneInstGroup(prof,Iptables_ADD);
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
	    }
#endif

	    /*
	     *end edit one Inst Group
	     */
	}
    }

}
#ifdef CONFIG_UTT_NETFILTER
extern void reNewPdbChainJumpRule(void);

static void uttNfGlobalProfileChange(Pointer tag, MibProfileType profileType, ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    if(oldProfileDataPtr->UttNfRuleEn !=  newProfileDataPtr->UttNfRuleEn)
	    {
            reNewPdbChainJumpRule(); /* 初始化和utt_polich_chain有关联的规则 */
            initPdbPolicyRule();
            /*按照记录上网管理各功能开启次数的count数组开启对应识别*/
            createIndRuleByCount();
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}
#endif
void funInitGroupBase()
{
    registerForProfileUpdates(MIB_PROF_GROUP_BASE, NULL, (ProfileUpdateFnc)GroupBaseProfileChange);
    groupiptablesInit();
#ifdef CONFIG_UTT_NETFILTER
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)uttNfGlobalProfileChange);/*触发函数*/
    initPdbPolicyRule() ;
#endif

}

#ifdef CONFIG_UTT_NETFILTER
/** 
 * 删除内核对应的 行为管理策略
 */
void actPolicyDelAll(void)
{
    /**
     * 为了编程简单 这里只要行为管理配置改变
     * 则发送netlink消息。让内核清除策略
     * 为了防止其他进程或线程与该进程的冲突这里必须加锁
     * 如speedweb的netlink接受线程 在这里清除时 已经构造消息完成 但未发送。这里在清除。则内核会在清除后生成一条
     * 脏策略
     *
     * 如果 不一次清除 则应goahead加锁。后这里释放锁
     */
    uttSemP(SEM_UTTNF_NO,0);
    uttNfNlPolicyDelAll();
    uttSemV(SEM_UTTNF_NO,0);
    return;
}
/*删除内核对应的所有策略*/
extern int uttNfNlPolicyDelAll(void)
{
    int ret = 0;
    UttNlNfPolicy  nlConf;
    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_POLICY;
    /*增加*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;
    ret = uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*发送消息给内核*/
    return ret;
}


/*
 * 更新策略规则
 * 备注：不包含PDB_NF_TABLE中的PDB_POLICY_CHAIN jump规则的初始化，这个初始化在pdb.c中进行
 */
void initPdbPolicyRule(void) {
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    clearPdbChain(PDB_POLICY_CHAIN);
#if (DHCP_SERVER == FYES)
    MibProfileType mibType = MIB_PROF_DHCP_SERVER;
    DhcpServerProfile *prof = NULL;
    prof = (DhcpServerProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
#if (DHCP_POOL == FYES)
#if (DNS_AGENCY == FYES)
    MibProfileType mibType = MIB_PROF_DNS_AGENCY;
    DnsAgencyProfile *prof = NULL;
    prof = (DnsAgencyProfile *)ProfGetInstPointByIndex(mibType, 0);
    doSystem("iptables -t %s -D INPUT -p udp --dport 53 -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE);
#endif
#endif
    actPolicyDelAll();
    if (profSys->UttNfRuleEn == FUN_ENABLE)
    {/*开启上网行为管理*/
	doSystem("iptables -t %s -A %s -j UTTPOLICY >/dev/null 2>&1", PDB_NF_TABLE, PDB_POLICY_CHAIN);
#if ((DHCP_SERVER == FYES) || (DHCP_POOL == FYES))
#if (DNS_AGENCY == FYES)
	if (prof->DnsProxyEn == FUN_ENABLE)
	{
	    doSystem("iptables -t %s -A INPUT -p udp --dport 53 -j UTTPOLICY", PDB_NF_TABLE);
	}
#endif
#endif
    }

    /*按照GroupBaseProfile中的PdbPolicy1初始化数组，数组是记录哪些上网行为被开启的*/
    changeCountByAllPdbPolicy();
    /*根据记录完的数组建立对应识别规则*/
    createIndRuleByCount(); 
    
    return;
}



/*
 * 函数说明：根据一个GroupBaseProfile中的pdbPolicy1修改数组
 * 创建时间：2012-11-28
 */
static int changeCountByOnePdbPolicy(Uint8 *policy)
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof = NULL;
    int max = 0, min = 0, i = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
        if (ProfInstIsFree(prof) == 1)
        {
            continue;
        }
        if(pdb_policy_match(policy, prof->index) == 1)
        {
            /*某个禁止项被使用则对应位加1，代表被使用次数加1，值为0表示没有被使用*/
            count[i]+=1;
        }
    }
    return 0;
}


/*
 * 函数说明：根据所有GroupBaseProfile中的pdbPolicy1修改数组，数组记录的是各功能被开启次数
 * 创建时间：2012-11-28
 * 返回值：0--正常执行完毕 1--中途退出
 */
static int changeCountByAllPdbPolicy(void)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;    
    int max = 0, min = 0, i = 0;
    

    memset(count, 0, sizeof(count));
    ProfInstLowHigh(mibType, &max, &min);
    for( i = min; i < max; i++)
    {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == GBT_ACT_CONTROL)
	    {
		changeCountByOnePdbPolicy(prof->pdbPolicy1);
	    }
	}
    }      

    return 0;
}


/*
 * 函数功能：判断pdb bit位是否被设置
 * 创建时间：2012-11-28
 * 返回值：1--被设置 0--没有被设置
 */
static int pdb_policy_match(Uint8 *policy, int index)
{
    if (policy[index >> 3] & (1 << (index & 7))) {
	return 1;
    }
    return 0;
}


/*
 * 函数功能：根据数组开启对应识别功能，数组中的数字代表开启次数
 * 创建时间：2012-11-28
 * 返回值：1--不需要单独建立规则 0--建立规则后正常退出
 */
extern int createIndRuleByCount()
{
    SystemProfile *profSys = NULL;
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof = NULL;
    int max = 0, min = 0, i = 0;
    /*记录是否有DNS类被开启的标志*/
    int flagDNS = 0;
    /*记录是否有任何规则被开启的标志*/
    int flagUnknown = 0;

    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);

    /*如果识别统计是开启的，所有识别规则都在pdb.c中开启，不需要单独开启*/
    if (profSys->UttNfIndentEn == FUN_ENABLE)
    {
	return 1;
    }
     

    /*清除utt_indentify_chain链,必须在判断识别统计之后和上网行为之前清空链，顺序不能改变*/
    clearPdbChain(PDB_IND_CHAIN);
    doSystem("iptables -t %s -D PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);

    /*上网行为管理没有开启，不需要建立规则*/
    if (profSys->UttNfRuleEn == FUN_DISABLE)
    {
	return 1;
    }

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
        prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
        if (ProfInstIsFree(prof) == 1)
        {
            continue;
        }
	/*如果当前count[i]即调用次数大于0，则开启对应识别功能*/
	if (count[i] > 0)
	{
	    /*根据不同类型创建不同规则*/
	    if (prof->type == L7Pdb) 
	    {
		createPdbL7IndRule(prof);
	    } 
	    else if(prof->type == PortPdb) 
	    {
		createPdbPortIndRule(prof);
	    }
	    else if(prof->type == DnsPdb )
	    {
#if 0
		createDnsIndRule(prof);
#endif
		flagDNS = 1;/*有任何dns类型的禁止项则创建dns规则*/
	    }
	    flagUnknown = 1;/*有任何行为被禁止则创建unknown规则*/
	}
    }

    if (flagDNS == 1)
    {
	/* dns识别,先删再加 */
	doSystem("iptables -t %s -A PREROUTING -j UTTWEBIND >/dev/null 2>&1", PDB_NF_TABLE);
    }
    if (flagUnknown == 1)
    {
	/* 提高性能，增加unknown规则 和判断会话易识别标识 */
	doSystem("iptables -t %s -I %s -m layer7 --l7proto unknown --l7classified -j RETURN >/dev/null 2>&1", PDB_NF_TABLE, PDB_IND_CHAIN);
    }
    return 0;
}
#endif
#endif
