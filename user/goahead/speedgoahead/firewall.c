#include "uttMachine.h"
#if (FIREWALL == FYES)
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "argcmd.h"
#include "a2n.h"
#include "internet.h"
#include "firewall.h"
#include "spdComFun.h"
#ifdef FW_DBG
#define FW_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define FW_PRINTF(fmt, args...)
#endif

static void fw_dns_rule_init(void);
extern const LcdParamEnumValue FilterIpProtocolEnum[];
#if(PORT_GRP == FYES)
extern char* getFwGContentGrp(char* fwGrpName,char* temp);
extern void  getPortFwGrp(char* fwGrpName,char* str,char* proto);

extern void iptablesFilterRun(void);
/*
 * �жϷ������Ƿ񱻷���ǽ����
 */
extern bool checkFwGrpInFireWall(char* groupName)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
    bool flag = FALSE;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
        if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == 1))
        {
            if (prof->fwTypeEn == FW_GRP)
            { 
                //�������������Ƿ���ͬ
                if(strcmp(prof->fwGrpName,groupName) == 0)
                {
                    flag = TRUE;
                    break;
                }
                //�������������Ƿ�Ϊ������
                if(portGrpDepSearchName(prof->fwGrpName,groupName) == 0)
                {
                    flag = TRUE;
                    break;
                }
            }
        }
    }
    return flag;
}
extern void updateFireWall()
{
    if(isFireWallEn() == 1)
    {
        doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
        doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);

        fw_dns_rule_init();
        iptablesFilterRun();
    }
}
#endif
#if 0
/*
 * url, keyword��������
 * keyword������Ҫ����������
 */
static void iptablesUKFilterRun(void) {
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
    char *policy, cmd[FW_CMD_LEN], IPrange[FW_IPRANGE_LEN], days[FW_DAY_LEN], \
	cmd1[FW_CMD_LEN], IPrange1[FW_IPRANGE_LEN];
    char sfIp_a[FW_IP_LEN + 1], seIp_a[FW_IP_LEN + 1];
    struct in_addr sfIp, seIp;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	/*
	 * ��profile�ж�
	 * ��ȡʧ�ܣ�profδ���ã�δ���û��߲���url����ؼ��ֹ������ͣ�����
	 */
	if ((prof == NULL) || (ProfInstIsFree(prof) == 1) || (prof->head.active == 0) || 
		((prof->FilterType != TYPE_FILTER_URL) && (prof->FilterType != TYPE_FILTER_WORD))) {
	    continue;
	}
	/*
	 * ��ʼ��
	 */
	memset(cmd, 0, FW_CMD_LEN);
	memset(IPrange, 0, FW_IPRANGE_LEN);
	memset(days, 0, FW_DAY_LEN);
	memset(sfIp_a, 0, (FW_IP_LEN + 1));
	memset(seIp_a, 0, (FW_IP_LEN + 1));

	if (prof->FilterType == TYPE_FILTER_WORD) {
	    memset(cmd1, 0, FW_CMD_LEN);
	    memset(IPrange1, 0, FW_IPRANGE_LEN);
	}
	/*
	 * ����(����)
	 * �ؼ��ֹ�����Զ��DROP
	 */
	if (prof->Status == 1) {
	    policy = " -j ACCEPT";
	} else {
	    policy = " -j DROP";
	}
	FW_PRINTF("%s: policy = %s\n", __FUNCTION__, policy);

	sfIp.s_addr = prof->srcFromIp;
	seIp.s_addr = prof->srcEndIp;
	/* Դ��ʼIP��ַ��Դ����IP��ַ��Ϊ0ʱ����Ϊ�����е�ַ */
	if ((sfIp.s_addr == 0) && (seIp.s_addr == 0)) {
	    seIp.s_addr = 0xffffffff;
	}
	strncpy(sfIp_a, inet_ntoa(sfIp), FW_IP_LEN);
	strncpy(seIp_a, inet_ntoa(seIp), FW_IP_LEN);

        sprintf(IPrange,"-m iprange --src-range %s-%s", sfIp_a, seIp_a);
	FW_PRINTF("%s: iprange=%s\n", __FUNCTION__, IPrange);

	if (prof->FilterType == TYPE_FILTER_URL) { /* url */
	    sprintf(cmd, "iptables -A %s %s -p tcp -m tcp -m webstr --url %s", 
		    UK_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
	} else { /* �ؼ��� */
	    sprintf(cmd,"iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    UK_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
	    sprintf(IPrange1, "-m iprange --dst-range %s-%s", sfIp_a, seIp_a);
	    sprintf(cmd1, "iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    UK_FILTER_CHAIN, IPrange1, prof->WordFilter.FilterKey);
	    FW_PRINTF("%s: iprange1=%s\n", __FUNCTION__, IPrange1);
	}

	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp(prof->fw_timestart, "All") != 0) {
	    sprintf(cmd+strlen(cmd)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		sprintf(cmd1+strlen(cmd1)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	    }
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd," -m time ");
	    strcat(cmd, days);
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		strcat(cmd1," -m time ");
		strcat(cmd1, days);
	    }
	}
	strcat(cmd, policy);
	doSystem(cmd);
	if (prof->FilterType == TYPE_FILTER_WORD) {
	    strcat(cmd1, policy);
	    doSystem(cmd1);
	}
	FW_PRINTF("%s:cmd=%s\n", __FUNCTION__, cmd);
	FW_PRINTF("%s:cmd1=%s\n", __FUNCTION__, cmd1);
    }
    return;
}
/*
 * IP��������
 * ������Ч˳��ʵ����С����
 */
extern void iptablesIPPortFilterRun(void) {
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
    char *policy, cmd[FW_CMD_LEN], IPrange[FW_IPRANGE_LEN], *proto, days[FW_DAY_LEN];
    char sfIp_a[FW_IP_LEN + 1], seIp_a[FW_IP_LEN + 1], dfIp_a[FW_IP_LEN + 1], 
	 deIp_a[FW_IP_LEN + 1];
    struct in_addr sfIp, seIp, dfIp, deIp;
    InterfaceProfile *profIf = NULL;
#if (PPPOE_SERVER == FYES)
    int global;
    struct in_addr sfIp1, seIp1;
    char cmd1[FW_CMD_LEN], sfIp_a1[FW_IP_LEN + 1], seIp_a1[FW_IP_LEN + 1], IPrange1[FW_IPRANGE_LEN];
    PPPoESrvGloProfile *profS = NULL;
#endif

    /* ȡlan�ں�pppoe server profile,��Դ��ַ��ȫ�ֵ�ַʱʹ�� */
    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
#if (PPPOE_SERVER == FYES)
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO, 0);
#endif

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	/*
	 * ��profile�ж�
	 * ��ȡʧ�ܣ�profδ���ã�δ���û��߲���IP�������ͣ�����
	 */
	if ((prof == NULL) || (ProfInstIsFree(prof) == 1)
		|| (prof->head.active == 0) || (prof->FilterType != TYPE_FILTER_IP)) {
	    continue;
	}
	/*
	 * ���Э��ʧ��
	 */
	proto = (char *)getEnumString(FilterIpProtocolEnum, prof->IpFilter.Protocol);
	if ((prof->fwTypeEn == FW_DIY) && (proto == NULL)) {
	    FW_PRINTF("%s: protoco get error!\n", __FUNCTION__);
	    continue;
	}
	/*
	 * ��ʼ��
	 */
	memset(cmd, 0, FW_CMD_LEN);
#if (PPPOE_SERVER == FYES)
	memset(cmd1, 0, FW_CMD_LEN);
	global = 0;
	memset(sfIp_a1, 0, (FW_IP_LEN + 1));
	memset(seIp_a1, 0, (FW_IP_LEN + 1));
	memset(IPrange1, 0, FW_IPRANGE_LEN);
#endif
	memset(IPrange, 0, FW_IPRANGE_LEN);
	memset(days, 0, FW_DAY_LEN);
	memset(sfIp_a, 0, (FW_IP_LEN + 1));
	memset(seIp_a, 0, (FW_IP_LEN + 1));
	memset(dfIp_a, 0, (FW_IP_LEN + 1));
	memset(deIp_a, 0, (FW_IP_LEN + 1));

	/*
	 * ����(����)
	 */
	if (prof->Status == 1) {
	    policy = " -j ACCEPT";
	} else {
	    policy = " -j DROP";
	}
	FW_PRINTF("%s: policy = %s\n", __FUNCTION__, policy);

	sfIp.s_addr = prof->srcFromIp;
	seIp.s_addr = prof->srcEndIp;
	/* Դ��ʼIP��ַ��Դ����IP��ַ��Ϊ0ʱ����Ϊ�����е�ַ */
	if ((sfIp.s_addr == 0) && (seIp.s_addr == 0)) {
	    /* �����ȫ�ֵ�ַ���ã�����LAN���ε�ַ��pppoe srv��ַ�ص�ַ�������� */
	    if ((ProfInstIsFree(profIf) == 1) 
#if (PPPOE_SERVER == FYES)
		    || (ProfInstIsFree(profS) == 1)
#endif
		    ) {
		continue;
	    }
	    /* lan�����ε�ַ */
	    sfIp.s_addr = profIf->ether.sta.ip & profIf->ether.sta.nm;
	    seIp.s_addr = sfIp.s_addr + (~(profIf->ether.sta.nm));

#if (PPPOE_SERVER == FYES)
	    if (profS->ipStart != 0) {
		/* pppoe��ַ�ص�ַ */
		sfIp1.s_addr = profS->ipStart;
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		seIp1.s_addr = htonl(ntohl(profS->ipStart) + profS->ipcount - 1);
#else
		seIp1.s_addr = htonl(ntohl(profS->ipStart) + profS->maxSessCnt - 1);
#endif
		global = 1;
	    }
#endif
	    FW_PRINTF("%s:sfIp=0x%x,seIp=0x%x,sfIp1=0x%x,seIp1=0x%x\n", __FUNCTION__, sfIp.s_addr, seIp.s_addr, sfIp1.s_addr, seIp1.s_addr);
	}
	strncpy(sfIp_a, inet_ntoa(sfIp), FW_IP_LEN);
	strncpy(seIp_a, inet_ntoa(seIp), FW_IP_LEN);
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    strncpy(sfIp_a1, inet_ntoa(sfIp1), FW_IP_LEN);
	    strncpy(seIp_a1, inet_ntoa(seIp1), FW_IP_LEN);
	}
#endif

	dfIp.s_addr = prof->IpFilter.dstFromIp;
	deIp.s_addr = prof->IpFilter.dstEndIp;
	/* ���iprange */
	if ((dfIp.s_addr != 0) && (deIp.s_addr != 0)) { /* ��Ŀ�ĵ�ַ */
	    strncpy(dfIp_a, inet_ntoa(dfIp), FW_IP_LEN);
	    strncpy(deIp_a, inet_ntoa(deIp), FW_IP_LEN);

	    sprintf(IPrange,"-m iprange --src-range %s-%s -m iprange --dst-range %s-%s", 
		    sfIp_a, seIp_a, dfIp_a, deIp_a);
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(IPrange1,"-m iprange --src-range %s-%s -m iprange --dst-range %s-%s", 
		    sfIp_a1, seIp_a1, dfIp_a, deIp_a);
	    }
#endif
	} else { /* ����Ŀ�ĵ�ַ */
	    sprintf(IPrange,"-m iprange --src-range %s-%s", sfIp_a, seIp_a);
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(IPrange1,"-m iprange --src-range %s-%s", sfIp_a1, seIp_a1);
	    }
#endif
	}
	FW_PRINTF("%s: iprange = %s\n", __FUNCTION__, IPrange);
 	sprintf(cmd, "iptables -A %s -p %s %s ", IPPORT_FILTER_CHAIN, proto, IPrange);
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    sprintf(cmd1, "iptables -A %s -p %s %s ", IPPORT_FILTER_CHAIN, proto, IPrange1);
	}
#endif
	FW_PRINTF("%s: proto=%s\n", __FUNCTION__, proto);

	/*
	 * ��ö˿�
	 */
	if ((strcmp(proto, "TCP") == 0) || (strcmp(proto, "UDP") == 0)) {
   	    sprintf(cmd+strlen(cmd)," --sport %d:%d --dport %d:%d ", 
		    prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort,
		    prof->IpFilter.dstFromPort, prof->IpFilter.dstEndPort); 
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(cmd1+strlen(cmd1)," --sport %d:%d --dport %d:%d ", 
		    prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort,
		    prof->IpFilter.dstFromPort, prof->IpFilter.dstEndPort); 
	    }
#endif
	}
	
	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp(prof->fw_timestart, "All") != 0) {
	    sprintf(cmd+strlen(cmd)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(cmd1+strlen(cmd1)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	    }
#endif
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd," -m time ");
	    strcat(cmd, days);
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		strcat(cmd1," -m time ");
		strcat(cmd1, days);
	    }
#endif
	}
	strcat(cmd, policy);
	doSystem(cmd);
	FW_PRINTF("%s:cmd=%s\n", __FUNCTION__, cmd);
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    strcat(cmd1, policy);
	    doSystem(cmd1);
	    FW_PRINTF("%s:cmd1=%s\n", __FUNCTION__, cmd1);
	}
#endif
    }
    return;
}
#endif


#if 0
/*�������ǽ�ܿ��غ󣬴˴����޸����ܵĴ����Ѳ���Ҫ*/
static void utt_init_ACL_FILTER_CHAIN(void)
{
    static char initOver = 0;
    if(initOver == 0) {
        doSystem("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
        initOver = 1;
    }
    return;
}
#endif
#if(PORT_GRP == FYES)
void do_work__fw_iptables(char* head,char* str,char* time,char* day,char* policy){
    char* tmp = NULL;
    char cmd[200]={0};
    char temp_str[36*100]={0};

    if(str == NULL || strlen(str) > sizeof(temp_str))
    {
        return ;
    }
    memcpy(temp_str,str,strlen(str));
    tmp = strtok(temp_str,UTT_SPLIT_SIGN_RECORDS);
    while(tmp != NULL)
    {
        memset(cmd,0,sizeof(cmd));
        snprintf(cmd,sizeof(cmd),"%s \"%s\" %s %s %s",head,tmp,time,day,policy);
        doSystem(cmd);
        tmp = strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
}
#endif
/*
 * url, keyword��������
 * keyword������Ҫ����������
 */
static void iptablesUKFilterRun(FireWallProfile *prof) {
#if 0
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
#endif
    char *policy, cmd[FW_CMD_LEN + 100], IPrange[FW_IPRANGE_LEN], days[FW_DAY_LEN], \
	cmd1[FW_CMD_LEN + 100], IPrange1[FW_IPRANGE_LEN], cmd2[FW_CMD_LEN + 100]; 
#ifdef HTTP_CACHE
	char cmd_input[FW_CMD_LEN],cmd1_input[FW_CMD_LEN];
#endif
    char sfIp_a[FW_IP_LEN + 1], seIp_a[FW_IP_LEN + 1];
    struct in_addr sfIp, seIp;
    char tempp[36*100] = {0};
#if (PORT_GRP == FYES)
    char cmd_time[FW_IPRANGE_LEN]={0},cmd2_time[FW_IPRANGE_LEN]={0},cmd1_time[FW_IPRANGE_LEN]={0},cmd_input_time[FW_IPRANGE_LEN]={0},cmd1_input_time[FW_IPRANGE_LEN]={0};
    char cmd_day[FW_IPRANGE_LEN]={0},cmd2_day[FW_IPRANGE_LEN]={0},cmd1_day[FW_IPRANGE_LEN]={0},cmd_input_day[FW_IPRANGE_LEN]={0},cmd1_input_day[FW_IPRANGE_LEN]={0};
#endif
#if 0
/*�������ǽ�ܿ��غ󣬴˴����޸����ܵĴ����Ѳ���Ҫ*/
    utt_init_ACL_FILTER_CHAIN();
#endif
#if 0
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	/*
	 * ��profile�ж�
	 * ��ȡʧ�ܣ�profδ���ã�δ���û��߲���url����ؼ��ֹ������ͣ�����
	 */
	if ((prof == NULL) || (ProfInstIsFree(prof) == 1) || (prof->head.active == 0) || 
		((prof->FilterType != TYPE_FILTER_URL) && (prof->FilterType != TYPE_FILTER_WORD))) {
	    continue;
	}
#endif
	/*
	 * ��ʼ��
	 */
	memset(cmd, 0, FW_CMD_LEN);
#ifdef HTTP_CACHE
	memset(cmd_input, 0, FW_CMD_LEN);
#endif
	memset(cmd2, 0, FW_CMD_LEN);
	memset(IPrange, 0, FW_IPRANGE_LEN);
	memset(days, 0, FW_DAY_LEN);
	memset(sfIp_a, 0, (FW_IP_LEN + 1));
	memset(seIp_a, 0, (FW_IP_LEN + 1));

	if (prof->FilterType == TYPE_FILTER_WORD) {
	    memset(cmd1, 0, FW_CMD_LEN);
#ifdef HTTP_CACHE
	    memset(cmd1_input, 0, FW_CMD_LEN);
#endif
	    memset(IPrange1, 0, FW_IPRANGE_LEN);
	}
	/*
	 * ����(����)
	 * �ؼ��ֹ�����Զ��DROP
	 */
	if (prof->Status == 1) {
	    policy = " -j RETURN";
	} else {
	    policy = " -j DROP";
	}
	FW_PRINTF("%s: policy = %s\n", __FUNCTION__, policy);

	sfIp.s_addr = prof->srcFromIp;
	seIp.s_addr = prof->srcEndIp;
	/* Դ��ʼIP��ַ��Դ����IP��ַ��Ϊ0ʱ����Ϊ�����е�ַ */
	if (((prof-> addrTypeEn == ADDR_IPRANGE) && (sfIp.s_addr == 0) && (seIp.s_addr == 0)) 
#if (IP_GRP == FYES)
		||((prof-> addrTypeEn == ADDR_IPGRP) && (strlen(prof->ipGrpName) == 0))
#endif
		)
	{
	    sfIp.s_addr = 0;
	    seIp.s_addr = 0xffffffff;
	}
	strncpy(sfIp_a, inet_ntoa(sfIp), FW_IP_LEN);
	strncpy(seIp_a, inet_ntoa(seIp), FW_IP_LEN);

	if((prof-> addrTypeEn == ADDR_IPRANGE)
#if (IP_GRP == FYES)
		||((prof-> addrTypeEn == ADDR_IPGRP) && (strlen(prof->ipGrpName) == 0))
#endif
		)
	{
	    sprintf(IPrange,"-m iprange --src-range %s-%s", sfIp_a, seIp_a);
	    sprintf(IPrange1, "-m iprange --dst-range %s-%s", sfIp_a, seIp_a);
	}
#if (IP_GRP == FYES)
	else
	{
	    if(strlen(prof->ipGrpName) != 0)
	    {
		sprintf(IPrange,"-m set --set %s src", prof->ipGrpName);
		sprintf(IPrange1,"-m set --set %s dst", prof->ipGrpName);
	    }
	}
#endif
	FW_PRINTF("%s: iprange=%s\n", __FUNCTION__, IPrange);
	FW_PRINTF("%s-%d: prof->FilterType = %d\n", __FUNCTION__, __LINE__, prof->FilterType);

    if(prof->fwTypeEn == FW_DIY){
	if (prof->FilterType == TYPE_FILTER_URL) { /* url */
	    sprintf(cmd, "iptables -A %s %s -p tcp -m tcp -m webstr --url %s", 
		    ACL_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
#ifdef HTTP_CACHE
	    sprintf(cmd_input, "iptables -A %s %s -p tcp -m tcp -m webstr --url %s", 
		    ACL_FILTER_INPUT_CHAIN, IPrange, prof->WordFilter.FilterKey);
#endif
	}
	else if(prof->FilterType == TYPE_FILTER_DNS)
	{
	    sprintf(cmd, "iptables -A %s %s -m domain --name \"%s\"", 
		    ACL_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
	    sprintf(cmd2, "iptables -A %s %s -m domain --name \"%s\"", 
		    ACL_FILTER_INPUT_CHAIN, IPrange, prof->WordFilter.FilterKey);
#if 0
	}
	else if(prof->FilterType == TYPE_FILTER_MAC)
	{
	    sprintf(cmd, "iptables -A %s %s -m mac --mac-source %s",
		    ACL_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
#endif
	} else { /* �ؼ��� */
	    sprintf(cmd,"iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    ACL_FILTER_CHAIN, IPrange, prof->WordFilter.FilterKey);
	    sprintf(cmd1, "iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    ACL_FILTER_CHAIN, IPrange1, prof->WordFilter.FilterKey);
#ifdef HTTP_CACHE
	    sprintf(cmd_input,"iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    ACL_FILTER_INPUT_CHAIN, IPrange, prof->WordFilter.FilterKey);
	    sprintf(cmd1_input, "iptables -A %s %s -p tcp -m string --algo bm --string %s", 
		    ACL_FILTER_INPUT_CHAIN, IPrange1, prof->WordFilter.FilterKey);
#endif
	    FW_PRINTF("%s: iprange1=%s\n", __FUNCTION__, IPrange1);
	}

	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp((char *)prof->fw_timestart, "All") != 0) {
	    sprintf(cmd+strlen(cmd)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#ifdef HTTP_CACHE
	    sprintf(cmd_input+strlen(cmd_input)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#endif
	    if (prof->FilterType == TYPE_FILTER_DNS) 
	    {
		sprintf(cmd2+strlen(cmd2)," -m time --timestart %s --timestop %s", 
			prof->fw_timestart, prof->fw_timestop);
	    }
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		sprintf(cmd1+strlen(cmd1)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#ifdef HTTP_CACHE
		sprintf(cmd1_input+strlen(cmd1_input)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#endif
	    }
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd," -m time ");
	    strcat(cmd, days);
#ifdef HTTP_CACHE
	    strcat(cmd_input," -m time ");
	    strcat(cmd_input, days);
#endif
	    if (prof->FilterType == TYPE_FILTER_DNS) 
	    {
		strcat(cmd2," -m time ");
		strcat(cmd2, days);
	    }
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		strcat(cmd1," -m time ");
		strcat(cmd1, days);
#ifdef HTTP_CACHE
		strcat(cmd1_input," -m time ");
		strcat(cmd1_input, days);
#endif
	    }
	}
	strcat(cmd, policy);
	doSystem(cmd);
#ifdef HTTP_CACHE
	strcat(cmd_input, policy);
	doSystem(cmd_input);
#endif
	if (prof->FilterType == TYPE_FILTER_DNS) 
	{
	    strcat(cmd2, policy);
	    doSystem(cmd2);
	}
	if (prof->FilterType == TYPE_FILTER_WORD) {
	    strcat(cmd1, policy);
	    doSystem(cmd1);
#ifdef HTTP_CACHE
	    strcat(cmd1_input, policy);
	    doSystem(cmd1_input);
#endif
	}
	FW_PRINTF("%s:cmd=%s\n", __FUNCTION__, cmd);
	FW_PRINTF("%s:cmd1=%s\n", __FUNCTION__, cmd1);
#if 0
    }
#endif
    }
#if(PORT_GRP == FYES)
    else if(prof->fwTypeEn == FW_GRP){
	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp((char *)prof->fw_timestart, "All") != 0) {
	    sprintf(cmd_time," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#ifdef HTTP_CACHE
	    sprintf(cmd_input_time," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#endif
	    if (prof->FilterType == TYPE_FILTER_DNS) 
	    {
		sprintf(cmd2_time," -m time --timestart %s --timestop %s", 
			prof->fw_timestart, prof->fw_timestop);
	    }
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		sprintf(cmd1_time," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#ifdef HTTP_CACHE
		sprintf(cmd1_input_time," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#endif
	    }
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd_day," -m time ");
	    strcat(cmd_day, days);
#ifdef HTTP_CACHE
	    strcat(cmd_input_day," -m time ");
	    strcat(cmd_input_day, days);
#endif
	    if (prof->FilterType == TYPE_FILTER_DNS) 
	    {
		strcat(cmd2_day," -m time ");
		strcat(cmd2_day, days);
	    }
	    if (prof->FilterType == TYPE_FILTER_WORD) {
		strcat(cmd1_day," -m time ");
		strcat(cmd1_day, days);
#ifdef HTTP_CACHE
		strcat(cmd1_input_day," -m time ");
		strcat(cmd1_input_day, days);
#endif
	    }
	}

        getFwGContentGrp(prof->fwGrpName,tempp);
        if (prof->FilterType == TYPE_FILTER_URL) { /* url */
            sprintf(cmd, "iptables -A %s %s -p tcp -m tcp -m webstr --url ",
                    ACL_FILTER_CHAIN, IPrange);
            do_work__fw_iptables(cmd,tempp,cmd_time,cmd_day,policy);
#ifdef HTTP_CACHE
            sprintf(cmd_input, "iptables -A %s %s -p tcp -m tcp -m webstr --url ", 
                    ACL_FILTER_INPUT_CHAIN, IPrange);
            do_work__fw_iptables(cmd_input,tempp,cmd_input_time,cmd_input_day,policy);
#endif
        }
        else if(prof->FilterType == TYPE_FILTER_DNS)
        {
            sprintf(cmd, "iptables -A %s %s -m domain --name ", 
                    ACL_FILTER_CHAIN, IPrange);
            do_work__fw_iptables(cmd,tempp,cmd_time,cmd_day,policy);
            sprintf(cmd2, "iptables -A %s %s -m domain --name ", 
                    ACL_FILTER_INPUT_CHAIN, IPrange);
            do_work__fw_iptables(cmd2,tempp,cmd2_time,cmd2_day,policy);
        } else { /* �ؼ��� */
            sprintf(cmd,"iptables -A %s %s -p tcp -m string --algo bm --string ", 
                    ACL_FILTER_CHAIN, IPrange);
            do_work__fw_iptables(cmd,tempp,cmd_time,cmd_day,policy);
            sprintf(cmd1, "iptables -A %s %s -p tcp -m string --algo bm --string ", 
                    ACL_FILTER_CHAIN, IPrange1);
            do_work__fw_iptables(cmd1,tempp,cmd1_time,cmd1_day,policy);

#ifdef HTTP_CACHE
            sprintf(cmd_input,"iptables -A %s %s -p tcp -m string --algo bm --string ", 
                    ACL_FILTER_INPUT_CHAIN, IPrange);
            do_work__fw_iptables(cmd_input,tempp,cmd_input_time,cmd_input_day,policy);
            sprintf(cmd1_input, "iptables -A %s %s -p tcp -m string --algo bm --string ", 
                    ACL_FILTER_INPUT_CHAIN, IPrange);
            do_work__fw_iptables(cmd1,tempp,cmd1_input_time,cmd1_input_day,policy);
#endif
        }
    }
#endif
    return;
}
bool checkPortIn(char* portlist,int port)
{
    char src[20] ={0};
    char* tmp = NULL;
    int start,end;    
    memcpy(src,portlist,sizeof(src));
    tmp = strchr(src,':');
    if(tmp != NULL)
        *tmp = '\0';
    start = atoi(src);
    end = atoi(tmp + 1);
    if((start <= port) &&(port <= end )&& (port != 0))
        return TRUE;
   return FALSE; 
}
/*
 * IP��������
 * ������Ч˳��ʵ����С����
 */
static void iptablesIPPortFilterRun(FireWallProfile *prof) {
#if 0
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
#endif
    char *policy, cmd[FW_CMD_LEN], IPrange[FW_IPRANGE_LEN], *proto, days[FW_DAY_LEN];
    char desIPStr[200] = {0};
#ifdef HTTP_CACHE
	char cmd_input[FW_CMD_LEN];
#endif
    char sfIp_a[FW_IP_LEN + 1], seIp_a[FW_IP_LEN + 1], dfIp_a[FW_IP_LEN + 1], 
	 deIp_a[FW_IP_LEN + 1];
    struct in_addr sfIp, seIp, dfIp, deIp;
    InterfaceProfile *profIf = NULL;
#if (MULTI_LAN == FYES)
    int zeroFlag = 0;
#endif
#if 0
/*�������ǽ�ܿ��غ󣬴˴����޸����ܵĴ����Ѳ���Ҫ*/
    utt_init_ACL_FILTER_CHAIN();
#endif
#if (PPPOE_SERVER == FYES)
    int global;
    struct in_addr sfIp1, seIp1;
    char cmd1[FW_CMD_LEN], sfIp_a1[FW_IP_LEN + 1], seIp_a1[FW_IP_LEN + 1], IPrange1[FW_IPRANGE_LEN];
    PPPoESrvGloProfile *profS = NULL;
    memset(&sfIp1, 0, sizeof(struct in_addr));
    memset(&seIp1, 0, sizeof(struct in_addr));
#endif
#if (PORT_GRP == FYES)
    char cmd_head[100] = {0},cmp_input_head[100]={0},cmd_pppoe_head[100]={0};
    char cmd_time[100] = {0},cmp_input_time[100]={0},cmd_pppoe_time[100]={0};
    char cmd_day[100] = {0},cmp_input_day[100]={0},cmd_pppoe_day[100]={0};
    char src[20*100]={0},protop[10*100] = {0};
    char *ptl_p = NULL;
    char * plist  = NULL;
    char * dot    = NULL;
    char port[50] = {0},port_input[50] = {0};
    char ptl[10]  = {0};
#endif

    /* ȡlan�ں�pppoe server profile,��Դ��ַ��ȫ�ֵ�ַʱʹ�� */
    profIf = (InterfaceProfile *)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);
#if (PPPOE_SERVER == FYES)
    profS = (PPPoESrvGloProfile *)ProfGetInstPointByIndex(MIB_PROF_PPPOE_SRV_GLO, 0);
#endif

#if 0
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	/*
	 * ��profile�ж�
	 * ��ȡʧ�ܣ�profδ���ã�δ���û��߲���IP�������ͣ�����
	 */
	if ((prof == NULL) || (ProfInstIsFree(prof) == 1)
		|| (prof->head.active == 0) || (prof->FilterType != TYPE_FILTER_IP)) {
	    continue;
	}
#endif
	/*
	 * ���Э��ʧ��
	 */
	proto = (char *)getEnumString(FilterIpProtocolEnum, prof->IpFilter.Protocol);
	if ((prof->fwTypeEn == FW_DIY) && (proto == NULL)) {
	    FW_PRINTF("%s: protoco get error!\n", __FUNCTION__);
#if 0
	    continue;
#endif
	    return;
	}
	/*
	 * ��ʼ��
	 */
	memset(cmd, 0, FW_CMD_LEN);
#ifdef HTTP_CACHE
	memset(cmd_input, 0, FW_CMD_LEN);
#endif
#if (PPPOE_SERVER == FYES)
	memset(cmd1, 0, FW_CMD_LEN);
	global = 0;
	memset(sfIp_a1, 0, (FW_IP_LEN + 1));
	memset(seIp_a1, 0, (FW_IP_LEN + 1));
	memset(IPrange1, 0, FW_IPRANGE_LEN);
#endif
	memset(IPrange, 0, FW_IPRANGE_LEN);
	memset(days, 0, FW_DAY_LEN);
	memset(sfIp_a, 0, (FW_IP_LEN + 1));
	memset(seIp_a, 0, (FW_IP_LEN + 1));
	memset(dfIp_a, 0, (FW_IP_LEN + 1));
	memset(deIp_a, 0, (FW_IP_LEN + 1));

	/*
	 * ����(����)
	 */
	if (prof->Status == 1) {
	    policy = " -j RETURN";
	} else {
	    policy = " -j DROP";
	}
	FW_PRINTF("%s: policy = %s\n", __FUNCTION__, policy);

	sfIp.s_addr = prof->srcFromIp;
	seIp.s_addr = prof->srcEndIp;
	/* Դ��ʼIP��ַ��Դ����IP��ַ��Ϊ0ʱ����Ϊ�����е�ַ */
	if (((prof-> addrTypeEn == ADDR_IPRANGE) && (sfIp.s_addr == 0) && (seIp.s_addr == 0))
#if (IP_GRP == FYES)
	    || ((prof-> addrTypeEn == ADDR_IPGRP) && (strlen(prof->ipGrpName) == 0))
#endif
	    ) {
	    /* �����ȫ�ֵ�ַ���ã�����LAN���ε�ַ��pppoe srv��ַ�ص�ַ�������� */
	    if ((ProfInstIsFree(profIf) == 1) 
#if (PPPOE_SERVER == FYES)
		    || (ProfInstIsFree(profS) == 1)
#endif
		    ) {
#if 0
		continue;
#endif
		return;
	    }
	    /* lan�����ε�ַ */
	    sfIp.s_addr = profIf->ether.sta.ip & profIf->ether.sta.nm;
	    seIp.s_addr = sfIp.s_addr + (~(profIf->ether.sta.nm));
#if (MULTI_LAN == FYES)
	    zeroFlag = 1;
#endif

#if (PPPOE_SERVER == FYES)
	    if (profS->ipStart != 0) {
		/* pppoe��ַ�ص�ַ */
		sfIp1.s_addr = profS->ipStart;
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
		seIp1.s_addr = htonl(ntohl(profS->ipStart) + profS->ipcount - 1);
#else
		seIp1.s_addr = htonl(ntohl(profS->ipStart) + profS->maxSessCnt - 1);
#endif
		global = 1;
	    }
	    FW_PRINTF("%s:sfIp=0x%x,seIp=0x%x,sfIp1=0x%x,seIp1=0x%x\n", __FUNCTION__, sfIp.s_addr, seIp.s_addr, sfIp1.s_addr, seIp1.s_addr);
#endif
	}
	strncpy(sfIp_a, inet_ntoa(sfIp), FW_IP_LEN);
	strncpy(seIp_a, inet_ntoa(seIp), FW_IP_LEN);
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    strncpy(sfIp_a1, inet_ntoa(sfIp1), FW_IP_LEN);
	    strncpy(seIp_a1, inet_ntoa(seIp1), FW_IP_LEN);
	}
#endif

	dfIp.s_addr = prof->IpFilter.dstFromIp;
	deIp.s_addr = prof->IpFilter.dstEndIp;
#if 1
    if(((prof-> destAddrTypeEn == ADDR_IPRANGE) && ((dfIp.s_addr != 0) && (deIp.s_addr != 0)))
            ||(prof->destAddrTypeEn == ADDR_IPGRP))
    {
#else
	/* ���iprange */
	if ((dfIp.s_addr != 0) && (deIp.s_addr != 0)) { /* ��Ŀ�ĵ�ַ */
#endif
        if((prof-> destAddrTypeEn == ADDR_IPRANGE) && ((dfIp.s_addr != 0) && (deIp.s_addr != 0))) {
	    strncpy(dfIp_a, inet_ntoa(dfIp), FW_IP_LEN);
	    strncpy(deIp_a, inet_ntoa(deIp), FW_IP_LEN);
        snprintf(desIPStr, 100, "-m iprange --dst-range %s-%s",dfIp_a,deIp_a);
        }
#if (IP_GRP == FYES)
        else if ((prof-> destAddrTypeEn == ADDR_IPGRP) && (strlen(prof->destIpGrpName) != 0)) {
                snprintf(desIPStr, 100, "-m set --set %s dst ",prof->destIpGrpName);
        }
#endif

	    FW_PRINTF("%s-----%d,prof->addrTypeEn = %d\n", __func__, __LINE__, prof->addrTypeEn);

	if((prof-> addrTypeEn == ADDR_IPRANGE)
#if (IP_GRP == FYES)
	    ||((prof-> addrTypeEn == ADDR_IPGRP) && (strlen(prof->ipGrpName) == 0))
#endif
		)
	{
#if (MULTI_LAN == FYES)
	    if(zeroFlag == 1)
	    {
#if (IP_GRP == FYES)
#if 0
		sprintf(IPrange,"-m set --set %s src -m iprange --dst-range %s-%s", 
			SYS_LAN_RANGE_GROUP, dfIp_a, deIp_a);
#else
		sprintf(IPrange,"-m set --set %s src %s", 
			SYS_LAN_RANGE_GROUP, desIPStr);
#endif
#else
#error ipset has been moved
#endif
	    }
	    else
	    {
#if 0
		sprintf(IPrange,"-m iprange --src-range %s-%s -m iprange --dst-range %s-%s", 
			sfIp_a, seIp_a, dfIp_a, deIp_a);
#else
		sprintf(IPrange,"-m iprange --src-range %s-%s %s", 
			sfIp_a, seIp_a, desIPStr);
#endif
	    }
#else
#if 0
	    sprintf(IPrange,"-m iprange --src-range %s-%s -m iprange --dst-range %s-%s", 
		    sfIp_a, seIp_a, dfIp_a, deIp_a);
#else
	    sprintf(IPrange,"-m iprange --src-range %s-%s %s", 
		    sfIp_a, seIp_a, desIPStr);
#endif
#endif
	}
#if (IP_GRP == FYES)
	else
	{
#if 0
	    sprintf(IPrange,"-m set --set %s src -m iprange --dst-range %s-%s", 
		    prof->ipGrpName, dfIp_a, deIp_a);
#else
	    sprintf(IPrange,"-m set --set %s src %s", 
		    prof->ipGrpName, desIPStr);
#endif
	}
#endif
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
#if 0
		sprintf(IPrange1,"-m iprange --src-range %s-%s -m iprange --dst-range %s-%s", 
		    sfIp_a1, seIp_a1, desIPStr);
#else
		sprintf(IPrange1,"-m iprange --src-range %s-%s %s ",
		    sfIp_a1, seIp_a1, desIPStr);
#endif
	    }
#endif
	} else { /* ����Ŀ�ĵ�ַ */
	    FW_PRINTF("%s-----%d,prof->addrTypeEn = %d\n", __func__, __LINE__, prof->addrTypeEn);
	    if((prof-> addrTypeEn == ADDR_IPRANGE)
#if (IP_GRP == FYES)
		    ||((prof-> addrTypeEn == ADDR_IPGRP) && (strlen(prof->ipGrpName) == 0))
#endif
		    )
	    {
#if (MULTI_LAN == FYES)
		if(zeroFlag == 1)
		{
#if (IP_GRP == FYES)
		    sprintf(IPrange,"-m set --set %s src", SYS_LAN_RANGE_GROUP);
#else
#error ipset has been moved
#endif
		}
		else
		{
		    sprintf(IPrange,"-m iprange --src-range %s-%s", sfIp_a, seIp_a);
		}
#else
		sprintf(IPrange,"-m iprange --src-range %s-%s", sfIp_a, seIp_a);
#endif
	    }
#if (IP_GRP == FYES)
	    else
	    {
		if(strlen(prof->ipGrpName) != 0)
		{
		    sprintf(IPrange,"-m set --set %s src ", prof->ipGrpName);
		}
	    }
#endif

#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(IPrange1,"-m iprange --src-range %s-%s", sfIp_a1, seIp_a1);
	    }
#endif
	}
    if(prof->fwTypeEn == FW_DIY)
    {

	FW_PRINTF("%s: iprange = %s\n", __FUNCTION__, IPrange);
 	sprintf(cmd, "iptables -A %s -p %s %s ", ACL_FILTER_CHAIN, proto, IPrange);
#ifdef HTTP_CACHE
	if((strcmp(proto, "TCP") == 0)&&(prof->IpFilter.dstEndPort==80)){
		sprintf(cmd_input, "iptables -A %s -p %s %s ", ACL_FILTER_INPUT_CHAIN, proto, IPrange);
	}
#endif
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    sprintf(cmd1, "iptables -A %s -p %s %s ", ACL_FILTER_CHAIN, proto, IPrange1);
	}
#endif
	FW_PRINTF("%s: proto=%s\n", __FUNCTION__, proto);

	/*
	 * ��ö˿�
	 */
	if ((strcmp(proto, "TCP") == 0) || (strcmp(proto, "UDP") == 0)) {
   	    sprintf(cmd+strlen(cmd)," --sport %d:%d --dport %d:%d ", 
		    prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort,
		    prof->IpFilter.dstFromPort, prof->IpFilter.dstEndPort); 
#ifdef HTTP_CACHE
		//��Ϊ80�˿ڱ�ת���������3128�˿�,����������80�˿�ʱ�����˿�ת��Ϊ3128����˿�
		if((strcmp(proto, "TCP") == 0)&&(prof->IpFilter.dstEndPort==80)){
			sprintf(cmd_input+strlen(cmd_input)," --sport %d:%d --dport 3128:3128", 
		    prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort);
		}
#endif
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(cmd1+strlen(cmd1)," --sport %d:%d --dport %d:%d ", 
		    prof->IpFilter.srcFromPort, prof->IpFilter.srcEndPort,
		    prof->IpFilter.dstFromPort, prof->IpFilter.dstEndPort); 
	    }
#endif
	}
	
	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp((char *)prof->fw_timestart, "All") != 0) {
	    sprintf(cmd+strlen(cmd)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
#ifdef HTTP_CACHE		
	if((strcmp(proto, "TCP") == 0)&&(prof->IpFilter.dstEndPort==80)){
	    sprintf(cmd_input+strlen(cmd_input)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	}
#endif
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		sprintf(cmd1+strlen(cmd1)," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	    }
#endif
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd," -m time ");
	    strcat(cmd, days);
#ifdef HTTP_CACHE
	if((strcmp(proto, "TCP") == 0)&&(prof->IpFilter.dstEndPort==80)){
	    strcat(cmd_input," -m time ");
	    strcat(cmd_input, days);
	}
#endif
#if (PPPOE_SERVER == FYES)
	    if (global == 1) {
		strcat(cmd1," -m time ");
		strcat(cmd1, days);
	    }
#endif
	}
	strcat(cmd, policy);
	doSystem(cmd);
#ifdef HTTP_CACHE
	if((strcmp(proto, "TCP") == 0)&&(prof->IpFilter.dstEndPort==80)){
		strcat(cmd_input, policy);
		doSystem(cmd_input);
	}
#endif
	FW_PRINTF("%s:cmd=%s\n", __FUNCTION__, cmd);
#if (PPPOE_SERVER == FYES)
	if (global == 1) {
	    strcat(cmd1, policy);
	    doSystem(cmd1);
	    FW_PRINTF("%s:cmd1=%s\n", __FUNCTION__, cmd1);
	}
#endif
#if 0
    }
#endif
    }
#if(PORT_GRP == FYES)
    else if(prof->fwTypeEn == FW_GRP)
    {
	/*
	 * ��Чʱ��
	 */
	/* ʱ�� */
	if (strcmp((char *)prof->fw_timestart, "All") != 0) {
	    sprintf(cmd_time," -m time --timestart %s --timestop %s", 
		    prof->fw_timestart, prof->fw_timestop);
	}
	/* ���� */
	if (prof->fw_day != 0x7f) {
	    strcpy(days,getDayForIptables(prof->fw_day));
	    strcat(cmd_day," -m time ");
	    strcat(cmd_day, days);
	}

    getPortFwGrp( prof->fwGrpName, src,protop);
    plist = strtok(src,UTT_SPLIT_SIGN_RECORDS);
    while(plist != NULL)
    {
        memset(cmd,0,sizeof(cmd));
#if(PPPOE_SERVER == FYES)
        memset(cmd1,0,sizeof(cmd1));
#endif
        memset(port,0,sizeof(port)); memset(ptl,0,sizeof(ptl));

        if(plist[0] == 'T')
        { 
            strcpy(ptl,"TCP"); 
        }else{ 
            strcpy(ptl,"UDP"); 
        }

        dot = strchr(plist,','); 
        if(dot != NULL)
        {
            *dot = '\0'; 
        }else{ 
            break; 
        }

        /* ��ö˿� */
        sprintf(port," --sport %s --dport %s ", plist + 2, dot + 1);
#ifdef HTTP_CACHE
        if((strcmp(ptl, "TCP") == 0)&&(checkPortIn(plist + 2,80)))
        {
			sprintf(port_input," --sport %d:%d --dport 3128:3128", plist + 2);
        }
#endif

        sprintf(cmd, "iptables -A %s -p %s %s %s %s %s %s", ACL_FILTER_CHAIN, ptl, IPrange,port,cmd_time,cmd_day,policy);
        doSystem(cmd);
#ifdef HTTP_CACHE
        if((strcmp(ptl, "TCP") == 0)&&(checkPortIn(plist + 2,80)))
        {
            sprintf(cmd_input, "iptables -A %s -p %s %s %s %s %s %s", ACL_FILTER_CHAIN, ptl, IPrange,port_input,cmd_time,cmd_day,policy);
            doSystem(cmd_input);
        }
#endif
#if (PPPOE_SERVER == FYES)
        if (global == 1) {
            sprintf(cmd1, "iptables -A %s -p %s %s %s %s %s %s", ACL_FILTER_CHAIN, ptl, IPrange1,port,cmd_time,cmd_day,policy);
            doSystem(cmd1);
        }
#endif
        plist=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);

    }
    plist=strtok(protop, UTT_SPLIT_SIGN_RECORDS);
    while(plist != NULL)
    {
        memset(cmd,0,sizeof(cmd));
#if(PPPOE_SERVER == FYES)
        memset(cmd1,0,sizeof(cmd1));
#endif
        sprintf(cmd, "iptables -A %s -p %s %s %s %s %s", ACL_FILTER_CHAIN, plist, IPrange, cmd_time, cmd_day, policy);
        doSystem(cmd);
#if (PPPOE_SERVER == FYES)
        if (global == 1) {
            sprintf(cmd1, "iptables -A %s -p %s %s %s %s %s", ACL_FILTER_CHAIN, plist, IPrange1, cmd_time, cmd_day, policy);
            doSystem(cmd1);
        }
#endif
        plist=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
    }
    }
#endif /*PORT_GRP*/
    return;
}

/*
 * ���ʿ��Ʋ���������Ч
 * ��Ч˳��ʵ����С����
 */
extern void iptablesFilterRun(void) {
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof == NULL) || (ProfInstIsFree(prof) == 1) || (prof->head.active == 0)) {
	    continue;
	}

	/* ip���� */
	if (prof->FilterType == TYPE_FILTER_IP) {
	    iptablesIPPortFilterRun(prof);
	} 
	/* url mac key word���� */
	else if ((prof->FilterType == TYPE_FILTER_URL)
		|| (prof->FilterType == TYPE_FILTER_DNS)
		|| (prof->FilterType == TYPE_FILTER_WORD)
#if 0
		|| (prof->FilterType == TYPE_FILTER_MAC)
#endif
	){
	    iptablesUKFilterRun(prof);
	} else {
	    ;
	}
    }
}

/*
 *����ǽ����profile change����
 */
static void firewallProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const Pointer oldProfileDataPtr, const Pointer newProfileDataPtr )
{
    switch(changeType) {
	case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
#if 0
	    /* ��������ipport����url_keyword�� */
	    /* ���� */
	    doSystem("iptables -F %s 1>/dev/null 2>&1", UK_FILTER_CHAIN);
	    doSystem("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);

	    /* ������ */
	    iptablesUKFilterRun();
	    iptablesIPPortFilterRun();
#endif
	    if(isFireWallEn() == 1)
	    {
	    doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
	    doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);

	    fw_dns_rule_init();
	    iptablesFilterRun();
	    }
            break;
        default:
            break;
    }
    return;
}

/*
 * ��ʼ������ǽ��forward���е�˳��ֻ�ڳ�ʼ��speedwebʱ����.mantis 9618
 */
extern void fw_rule_init(void) {
    doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);

    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);

    doSystem("iptables -t filter -I FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);

#ifdef HTTP_CACHE
    doSystem("iptables -D INPUT -j %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
    doSystem("iptables -t filter -I INPUT -j %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
#endif
    fw_dns_rule_init();

}
static void fw_dns_rule_init(void)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int i, min = 0, max = 0;
    int flag = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (FireWallProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == 1)) 
	{
	    if (prof->FilterType == TYPE_FILTER_DNS)
	    {/*������������������˴���*/
		flag = 1;
		break;
	    }
	}
    }
    if(flag == 1)
    {
	doSystem("iptables -D INPUT -j %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
	doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
	doSystem("iptables -t filter -I INPUT -j %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
    }
    return;
}

/*
 * ����ǽ��ʼ������
 * ����ǽĿǰ�������������Ƿŵ�forward���У���ʵ���Ժϲ�Ϊһ����
 * Ϊ�˱��ֺ�520һ������Ȼʹ��������
 */
extern void fw_init(void) {
#if 0
    /*
     * url �ؼ��������
     */
    doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", UK_FILTER_CHAIN);
    doSystem("iptables -F %s 1>/dev/null 2>&1", UK_FILTER_CHAIN);

    /*
     * ip���������
     */
    doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
    doSystem("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);

    /* url keyword����ʼ�� */
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", UK_FILTER_CHAIN);
    doSystem("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", UK_FILTER_CHAIN);

    /* ip��������ʼ�� */
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
    doSystem("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);

    /* ip���������� */
    iptablesIPPortFilterRun();

    /* url keyword������ */
    iptablesUKFilterRun();
#endif

    /*
     * uk����ip���ϲ�
     * acl�����
     */
#if 0
    doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
#endif
    doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
    /* acl����ʼ�� */
    doSystem("iptables -t filter -N %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
#if 0
    doSystem("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
#endif
    
    /* acl������ */
    iptablesFilterRun();

    return;
}

#if (DNS_FILTER == FYES)
extern void dnsFilter_rule_init();
#endif

/**����ǽ���ʲ��Կ���ҳ��ȫ�ֿ��ش�������
 *zhengxiao
 * 2013.2
 */
static void firewallGlobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
            if(((oldProfileDataPtr->firewallEn == FUN_DISABLE))&&(newProfileDataPtr->firewallEn == FUN_ENABLE))
            {
	        fw_rule_init();
 	        fw_init();
#if (DNS_FILTER == FYES)
		/* ȷ�������������ȼ����ڷ���ǽ���ȼ� */
		dnsFilter_rule_init();
#endif
            }
            if(((newProfileDataPtr->firewallEn == FUN_DISABLE))&&(oldProfileDataPtr->firewallEn == FUN_ENABLE))
	    {
                doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
                doSystem("iptables -F %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
		doSystem("iptables -D INPUT -j %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
    		doSystem("iptables -D FORWARD -j %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
		doSystem("iptables -X %s 1>/dev/null 2>&1", ACL_FILTER_CHAIN);
		doSystem("iptables -X %s 1>/dev/null 2>&1", ACL_FILTER_INPUT_CHAIN);
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

/**�жϷ���ǽ���ʲ��Կ���ҳ�棬�жϿ����Ƿ���
 *zhengxiao
 *2013.2
 */
int isFireWallEn(void)
{
    SystemProfile *profSys = NULL;
    int ret = 0;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if(profSys->firewallEn == FUN_ENABLE )
    {
        ret = 1;
    }     
    return ret;
}

/*
 * ��ʼ��profiel change����
 */
extern void funInitFirewall(void) {
    registerForProfileUpdates(MIB_PROF_FIRE_WALL, NULL, firewallProfileChange);
    registerForProfileUpdates(MIB_PROF_SYSTEM, NULL, (ProfileUpdateFnc)firewallGlobalProfileChange);
    /* ��ʼ������˳��, mansis 9618 */
    if(isFireWallEn() == 1)
    {
    	fw_rule_init();
    	fw_init();
    }
    return;
}
#endif
