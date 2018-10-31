
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
#include "uttfunction.h"
#include "nat.h"
#include "ifmisc.h"
#ifndef CONFIG_RALINK_RT2880
#include    <linux/netfilter/nf_conntrack_common.h>
#endif
#if (NAT_RULE == FYES)
static int checkPassThroughInst();
static void NatRulePassThroughAorD(char action);
#endif

#if(UTT_NV_1800N != FYES)
extern void wanIpRuleFun(int wanIndex, int action, IPADDR ipS, IPADDR ipE);
extern int doSystem(char_t *fmt, ...);
extern void iptablesNatInit(void);
extern void iptablesNATAllClear(void);
extern char* getWanIfNamePPP(int num);
extern int getIfIp(char *ifname, char *if_addr);
extern void funInitNatRule(void);
extern void InitNatRuleForWordRun(int wanNo, char action);
extern void iptablesEditOneNatRule(const NatRuleProfile* prof, int wanNo, char action);
extern void funInitDMZ(void);
#if (DMZ == FYES)
extern void iptablesDMZForWordRun(void);
#endif
extern void InitStaticNatForWordRun(int wanNo, char action);
extern void iptablesStaticNatForwordRun(const StaticNatProfile* prof, int wanNo, char action);
extern void iptablesStaticNatclear(void);
extern void iptablesStaticNatclearforwanno(int wanno);

void static remoteCtrlForAllWan(const StaticNatProfile* prof, char action);
 /***********************************************************************
 * �� �� ����   checkNatRuleupdatatoStaticNat 
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��NAT����仯��ʱ�򣬸��¾�̬NAT
				
					 nInFromIp     NAT������ڲ���ʼIP
					 nInEndIp	   NAT������ڲ�����IP
					 wanIndex	   NAT����İ󶨽�ں� ��1,2,3,4
 ***********************************************************************/

static void checkNatRuleupdatatoStaticNat(unsigned int nOutIp, unsigned int nInFromIp,unsigned int nInEndIp,int wanIndex)
{
    DBGPRINT("nOutIp[%x] nInFromIp[%x} nInEndIp[%x} wanIndex[%d]:\n",nOutIp,nInFromIp,nInEndIp,wanIndex);
    int min = 0, max = 0,i = 0;
    int nwanIndex; /* static nat wanno */
    unsigned int nIp;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    nIp = htonl(prof->IP); 
	    nwanIndex = prof->NatBind[3] - '0';
	    if((nwanIndex == wanIndex ) &&(nIp>=nInFromIp) && (nIp<=nInEndIp))
	    {
		/* natrule ����staticnat staticnat��Ҫ�޸� */
		iptablesStaticNatclearforwanno(wanIndex);
		InitStaticNatForWordRun(wanIndex, Iptables_ADD);
	    }

	}
    }
}
 /***********************************************************************
 * �� �� ����   getOutOne2OneIp 
 * �������ڣ�	2011-10-14
 * �޸����ڣ�	
 * ���ߣ�       yqyang
 * ����˵����	������ip �鿴��IP�Ƿ���NAT����Χ�ڣ�wanindex �󶨵Ľӿ� 1,2,3,4
				����ֵ�����ص���NAT�����IP ����Ϊ0
				ͨ��staticNAT�� IP����IP��ַ ��ȡNAT_RULE �ж�Ӧ��WAN�ڵ�ַ��NAT�����easyIp�����ȡ��

 ***********************************************************************/
static unsigned int getOutOne2OneIp(char *ip,int wanindex)
{
#if 0
    unsigned int Type = 0;
#endif
    unsigned int nIp,nOutIp,nInFromIp,nInEndIp,i,j, profwanindex;
    int min = 0, max = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;
    nIp=ip2int(ip);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    nOutIp = htonl(prof->OutIp);
	    nInFromIp =  htonl(prof->InFromIP);
	    nInEndIp = htonl(prof->InEndIP);
	    profwanindex = prof->NatBind[3] - '0';
	    if((profwanindex == wanindex) &&(nIp>=nInFromIp) && (nIp<=nInEndIp))
	    {
		j=0;
		/*Mantis11954*/
		if (prof->NatruleType == ONE2ONE) {
		while( (nInFromIp+j)!=nIp )
		{
		    j++;
		}
		}
		return (nOutIp+j);
	    }
	}
    }
    return 0;
}
/***********************************************************************
 * �� �� ����   iptablesAllNATchangeRun 
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	WAN�������ͶϿ�״̬�仯���õ�NAT�ӿڣ�
			    ������wanno ��ʾWAN�ں� 1? action 'A'  'D' wan���������߶Ͽ�
 ***********************************************************************/
void iptablesAllNATchangeRun(int wanNo, char action)
{
    doSystem("nat.sh %d %c", wanNo, action);
    iptablesStaticNatclearforwanno(wanNo);
    InitStaticNatForWordRun(wanNo,action);
    InitNatRuleForWordRun(wanNo,action);
#if (DMZ == FYES)
    iptablesDMZForWordRun();
#endif
}
/***********************************************************************
 * �� �� ����   iptablesNatInit
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��ʼ��IPTABLES NAT ���½���̬NAT,NAT����Ĭ��NAT,UPNP,Զ�̹���DMZ��NAT��������
 ***********************************************************************/
void iptablesNatInit(void)
{
    iptablesNATAllClear(); 
    int i;
    if((checkPassThroughInst() !=0)) {
	NatRulePassThroughAorD(Iptables_ADD);
    }
    for( i = 1; i <= getWanSlotCount(); i++) {
	/*del old all nat rule */
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, DMZ_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, DMZ_LOOP_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, DEFAULT_NAT_WAN_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, UPNP_CHAIN_WAN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_PRE_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -X wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_LOOP_CHAIN);
	doSystem("iptables -t nat -D  POSTROUTING  -j wan%d_POSTROUTING 1>/dev/null 2>&1" , i); 
	doSystem("iptables -t nat -X wan%d_POSTROUTING 1>/dev/null 2>&1", i); 
	doSystem("iptables -t nat -D  PREROUTING  -j wan%d_PREROUTING 1>/dev/null 2>&1" , i); 
	doSystem("iptables -t nat -X wan%d_PREROUTING 1>/dev/null 2>&1", i); 
	/* �����µĹ���*/
	doSystem("iptables -t nat -N wan%d_POSTROUTING 1>/dev/null 2>&1", i);
	doSystem("iptables -t nat -I POSTROUTING %d -j wan%d_POSTROUTING 1>/dev/null 2>&1", i, i);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_CHAIN); 
	doSystem("iptables -t nat -I wan%d_POSTROUTING 1 -j wan%d_%s 1>/dev/null 2>&1", i, i, NAT_RULE_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_LOOP_CHAIN);
	doSystem("iptables -t nat -I wan%d_POSTROUTING 2 -j wan%d_%s 1>/dev/null 2>&1", i, i, STATIC_NAT_LOOP_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, DMZ_LOOP_CHAIN);
	doSystem("iptables -t nat -I wan%d_POSTROUTING 3 -j wan%d_%s 1>/dev/null 2>&1", i, i, DMZ_LOOP_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, DEFAULT_NAT_WAN_CHAIN);
	doSystem("iptables -t nat -I wan%d_POSTROUTING 4 -j wan%d_%s 1>/dev/null 2>&1", i, i, DEFAULT_NAT_WAN_CHAIN);
	doSystem("iptables -t nat -N wan%d_PREROUTING 1>/dev/null 2>&1", i);
	doSystem("iptables -t nat -I PREROUTING %d -j wan%d_PREROUTING 1>/dev/null 2>&1", i, i);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_PRE_CHAIN);
	doSystem("iptables -t nat -I wan%d_PREROUTING 1 -j wan%d_%s 1>/dev/null 2>&1", i, i, NAT_RULE_PRE_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -I wan%d_PREROUTING 2 -j wan%d_%s 1>/dev/null 2>&1", i, i, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, DMZ_CHAIN);
	doSystem("iptables -t nat -I wan%d_PREROUTING 3 -j wan%d_%s 1>/dev/null 2>&1", i, i, DMZ_CHAIN);
	doSystem("iptables -t nat -N wan%d_%s 1>/dev/null 2>&1", i, UPNP_CHAIN_WAN);
	doSystem("iptables -t nat -I wan%d_PREROUTING 4 -j wan%d_%s 1>/dev/null 2>&1", i, i, UPNP_CHAIN_WAN);
    }
}


/***********************************************************************
 * �� �� ���   iptablesStaticNatclear
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��������Ժ�ˢ������ static nat 
 ***********************************************************************/
void iptablesStaticNatclear(void)
{
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_LOOP_CHAIN);
    }
}
/*
 *
 * ˢ��staticnat ����  �����������STATIC NAT ���ڸ��������½�STATICNAT
 */
void updataStaticNat(void)
{ 
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, STATIC_NAT_LOOP_CHAIN);
	InitStaticNatForWordRun(i, Iptables_ADD); 
    }
}

 /***********************************************************************
 * �� �� ����   iptablesStaticNatclearforwanno
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	ɾ�� ĳһ���ӿڵ�STATIC NAT
 ***********************************************************************/
void iptablesStaticNatclearforwanno(int wanno)
{
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", wanno, STATIC_NAT_CHAIN);
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", wanno, STATIC_NAT_LOOP_CHAIN);
}


  /***********************************************************************
 * �� �� ����   iptablesNatRuleclear
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	���S NAT rule �����ϵ�ֵ
 ***********************************************************************/
void iptablesNatRuleclear(void)
{
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_CHAIN);
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, NAT_RULE_PRE_CHAIN);
    }
    NatRulePassThroughAorD(Iptables_DEL);
}

 /***********************************************************************
 * �� �� ����   iptablesDMZNatclear
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��� DMZ�����ֵ
 ***********************************************************************/
void iptablesDMZNatclear(void)
{
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, DMZ_CHAIN);
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, DMZ_LOOP_CHAIN);
    }
}

  /***********************************************************************
 * �� �� ����   iptablesDefaultNatclear
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��� default nat �����ֵ
 ***********************************************************************/
void iptablesDefaultNatclear(void)
{
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, DEFAULT_NAT_WAN_CHAIN);
    }
}

 /***********************************************************************
 * �� �� ����   iptablesUpnpNatclear
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����	��� upnp nat �����ֵ
 ***********************************************************************/
void iptablesUpnpNatclear(void)
{
    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_%s 1>/dev/null 2>&1", i, UPNP_CHAIN_WAN);
    }
}

/* �������NAT ϵͳ��ʼ������*/
void iptablesNATAllClear(void)
{

    int i;
    for( i = 1; i <= getWanSlotCount(); i++) {
	doSystem("iptables -t nat -F wan%d_POSTROUTING 1>/dev/null 2>&1", i); 
	doSystem("iptables -t nat -F wan%d_PREROUTING 1>/dev/null 2>&1", i); 
    }
    iptablesStaticNatclear();
    iptablesNatRuleclear();
    iptablesDefaultNatclear();
    iptablesDMZNatclear();
}

#if (STATIC_NAT == FYES)
void StaticNatProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const StaticNatProfile* oldProfileDataPtr, const StaticNatProfile* newProfileDataPtr )
{
    DBGPRINT("staticNat change \n");
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    iptablesStaticNatForwordRun(newProfileDataPtr,0, Iptables_ADD);
	    break;
	case PROF_CHANGE_EDIT:
	    iptablesStaticNatForwordRun(oldProfileDataPtr,0, Iptables_DEL);
	    iptablesStaticNatForwordRun(newProfileDataPtr,0, Iptables_ADD);
	    break;
	case PROF_CHANGE_DEL:
	    iptablesStaticNatForwordRun(oldProfileDataPtr,0, Iptables_DEL);
	    break;
	case PROF_CHANGE_DELALL:
	    updataStaticNat();
	    break;
	default:
	    break;
    };
    return;
}

/* 
 * remoteCtrInit	-	��ʼ��Զ�̹���
 *
 * ϵͳ����ʱ��û��Զ�̹��������������Զ�̹���ľ�̬ӳ�䣬
 * ����Ĭ�ϲ�����
 *
 */
static void remoteCtrInit(void)
{
	MibProfileType mibType = MIB_PROF_STATIC_NAT;
	StaticNatProfile *staticNatProfile= NULL;
	staticNatProfile = (StaticNatProfile *)ProfGetInstPointByName(mibType, "admin");
    //��Զ�̹�������ò������򴴽�����
	if (staticNatProfile == NULL) {
		staticNatProfile = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, 0);
		ProfSetNameByPoint(staticNatProfile, "admin");
		staticNatProfile->head.active = FALSE;
		staticNatProfile->OutPort= 8081;
		staticNatProfile->protocol= 1;
#if (LAN_DEFAULT_IPADDR == FYES)
        staticNatProfile->IP = htonl(0xc0a80a01);
#elif (RAISECOM_A30W == FYES)
        staticNatProfile->IP = htonl(0xc0a80afd);
#elif (SKYWORTH == FYES)
        staticNatProfile->IP = htonl(0xc0a85901);
#elif (IP_3_254 == FYES)
        staticNatProfile->IP = htonl(0xc0a803fe);
#else
        staticNatProfile->IP = htonl(0xc0a801fe);
#endif
        staticNatProfile->InnerPort= 80;
		staticNatProfile->PortNum= 1;
		strcpy(staticNatProfile->NatBind, "WAN1");
	}
	else
	{//��ֹ�û��Ҹ�����
	    strcpy(staticNatProfile->NatBind, "WAN1");
	}
    remoteCtrlForAllWan(staticNatProfile, staticNatProfile->head.active == TRUE ? Iptables_INS : Iptables_DEL);//Ϊ��wan�ڶ�����natӳ�书��
    return;
}
void funInitStaticNat(void)
{
    registerForProfileUpdates(MIB_PROF_STATIC_NAT, (Pointer)NULL, (ProfileUpdateFnc)StaticNatProfileChange);
    /*mantis11610*/
    remoteCtrInit();
}
void makeStaticNatLoop(char action,unsigned int wanIndex, char *buf, int len, char *ip1, char *ip_address, char *ip, int proto, int prf_int, int prt_int,int in_prf_int,int in_prt_int)
{
    int rc = 0;
    char *pos = buf;

    if( (action==Iptables_ADD || action==Iptables_INS) && (_getWanLinkState(wanIndex)==1)){
    rc = snprintf(pos, len-rc, 
	    "iptables -t nat -%c wan%d_%s -j SNAT -m uttdev --is-lan-in -d %s ",action, wanIndex, STATIC_NAT_LOOP_CHAIN, ip);
    }else if(action==Iptables_DEL){
	rc = snprintf(pos, len-rc, 
		"iptables -t nat -%c wan%d_%s -j SNAT -m uttdev --is-lan-in -d %s ",action, wanIndex, STATIC_NAT_LOOP_CHAIN, ip);
    }else
    {
	return;
    }
    pos = pos + rc;

    // write protocol type
    if(proto == STATIC_NAT_TCP)
	rc = snprintf(pos, len-rc, "-p tcp ");
    else if (proto == STATIC_NAT_UDP)
	rc = snprintf(pos, len-rc, "-p udp ");
    else if (proto == STATIC_NAT_TCP_UDP)
	rc = snprintf(pos, len-rc, " ");
    pos = pos + rc;

    // write port
    if(prt_int != 0)
	rc = snprintf(pos, len-rc, "--dport %d:%d ", in_prf_int, in_prt_int);
    else
	rc = snprintf(pos, len-rc, "--dport %d ", in_prf_int);
    pos = pos + rc;

    // write remote ip
    rc = snprintf(pos, len-rc, "--to %s ", ip1);
}

void makeStaticNat(char action,unsigned int wanIndex, char *buf, int len, char *wan_ip, char *ip_address, int proto, int prf_int, int prt_int,int in_prf_int,int in_prt_int)
{
    int rc = 0;
    char *pos = buf;
    
    if( (action==Iptables_ADD || action== Iptables_INS) && (_getWanLinkState(wanIndex)==1)){
	rc = snprintf(pos, len-rc, 
		"iptables -t nat -%c wan%d_%s -j DNAT -d %s  ",action, wanIndex, STATIC_NAT_CHAIN, wan_ip);
    }else if(action==Iptables_DEL){
	rc = snprintf(pos, len-rc, 
		"iptables -t nat -%c wan%d_%s -j DNAT -d %s  ",action, wanIndex, STATIC_NAT_CHAIN, wan_ip);
    }
    else
    {
	return;
    }

    pos = pos + rc;

    // write protocol type
    if(proto == STATIC_NAT_TCP)
	rc = snprintf(pos, len-rc, "-p tcp ");
    else if (proto == STATIC_NAT_UDP)
	rc = snprintf(pos, len-rc, "-p udp ");
    else if (proto == STATIC_NAT_TCP_UDP)
	rc = snprintf(pos, len-rc, " ");
    pos = pos + rc;

    // write port
    if(prt_int != 0) {
	rc = snprintf(pos, len-rc, "--dport %d:%d ", prf_int, prt_int);
	pos = pos + rc;
	// write remote ip
	rc = snprintf(pos, len-rc, "--to %s:%d-%d-%d ", ip_address,in_prf_int,in_prt_int,prf_int);
    }
    else {
	rc = snprintf(pos, len-rc, "--dport %d ", prf_int);
	pos = pos + rc;
	rc = snprintf(pos, len-rc, "--to %s:%d-%d ", ip_address,in_prf_int,in_prt_int);
    }

}

/*
 * STATIC NAT ���ýӿ� 
 * wanNo ����ӿں� action ����ɾ���������� A OR D
 *
 */
void InitStaticNatForWordRun(int wanNo, char action)
{
    int min = 0, max = 0,i = 0;
    MibProfileType mibType = MIB_PROF_STATIC_NAT;
    StaticNatProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (StaticNatProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(action == Iptables_ADD) {
		iptablesStaticNatForwordRun(prof,wanNo, Iptables_DEL);
	    }
	    iptablesStaticNatForwordRun(prof,wanNo, action);
	}
    }
}
/*
 * ������wan�ڿ���Զ�̹���
 * 2015-02-11
 */
void static remoteCtrlForAllWan(const StaticNatProfile* prof, char action)
{
    int ifIndex, wanCount;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *profIf = NULL;
    wanCount = getWanSlotCount();
    char wanIp[16], lanIp[16], cmd[256], net_area[16] ={0};
    struct in_addr addr;
    for(ifIndex = 0; ifIndex <= wanCount; ifIndex++) {
        //��ȡwan�ڵ�ip, �ӵڶ���lan�ڿ�ʼ
        profIf = (InterfaceProfile *)ProfGetInstPointByIndex(mibType, ifIndex);
        if(NULL == profIf || 1 == ProfInstIsFree(profIf) || 1 == ifIndex){
            continue;
        }
        if(0 == ifIndex) {//i == 0, ��ȡ����lan�ڵ�ip
            addr.s_addr = profIf->ether.sta.ip; 
            memset(lanIp, 0, sizeof(lanIp));
            snprintf(lanIp, sizeof(lanIp), inet_ntoa(addr));
            continue;
        }
        memset(wanIp, 0, sizeof(wanIp));
        switch(profIf->ether.connMode)
        {
            case ETH_CONN_PPPOE:
                getIfIp(getWanIfNamePPP(ifIndex), wanIp);
                break;
            case ETH_CONN_DHCP:
            case ETH_CONN_STATIC:
                getIfIp(getWanIfName(ifIndex), wanIp);
                break;
            default:
                break;
        }
        if(0 == _getWanLinkState(ifIndex)) {//wan��down״̬��ɾ����Ӧ��natӳ��
            action = Iptables_DEL;
        }
        memset(cmd, 0, sizeof(cmd));
        makeStaticNat(action, ifIndex, cmd, sizeof(cmd), wanIp, lanIp, 1, prof->OutPort, (prof->OutPort+prof->PortNum-1), prof->InnerPort, (prof->InnerPort+prof->PortNum-1));
        doSystem(cmd);
        memset(cmd, 0, sizeof(cmd));
        makeStaticNatLoop(action, ifIndex, cmd, sizeof(cmd), wanIp, net_area, lanIp, 1, prof->OutPort, (prof->OutPort+prof->PortNum-1), prof->InnerPort, (prof->InnerPort+prof->PortNum-1));
        doSystem(cmd);
    }
}

/*
 *
 *  ��̬NAT�ĳ�ʼ����������WAN��״̬��profilechange�ı��ʱ����ã�
 *
 */
void iptablesStaticNatForwordRun(const StaticNatProfile* prof, int wanNo, char action)
{

    char ip[16]={0},ip1[16]={0},ip2[16]={0};
    char cmd[1024], net_area[32]={0};
    unsigned int  nOutPort,nInFromPort,PortNum,OutIp,wanIndex;
    int proto;
#if (MULTI_LAN == FYES)
    int num = 0;
    char LanIfName[11];
#endif
    if(strlen(prof->NatBind)<=0) {
        wanIndex = 1;
    } else {
        wanIndex = prof->NatBind[3] - '0';
    }
    /* Զ�̹���NAT��Ҫ�����ڵ�һ��λ��  */
    if((action == Iptables_ADD) && (strcmp(prof->head.name, "admin") == 0)) {
        action = Iptables_INS;
        if(prof->head.active == TRUE){
            remoteCtrlForAllWan(prof, Iptables_INS);
        }
    }
    if((action == Iptables_DEL) && (strcmp(prof->head.name, "admin") == 0)){
        remoteCtrlForAllWan(prof, Iptables_DEL);
    }
    
    if(prof != NULL && ProfInstIsFree((Pointer)prof) == 0 && prof->head.active == TRUE  ) {
        if((wanNo==0) || (wanIndex==wanNo)) {
            proto = prof->protocol;
            nOutPort = prof->OutPort;
            PortNum = prof->PortNum;
            nInFromPort = prof->InnerPort;
            int2ip(htonl(prof->IP), ip); /* nat ipaddress */
#if (MULTI_LAN == FYES)
            get_netarea(getLanIfName(), net_area, ip, &num);
            if(num == 0)
            {
                return;
            }
            else
            {
                memset(LanIfName, 0, sizeof(LanIfName));
                if(num == 1)
                {
                    snprintf(LanIfName, 10, "%s", getLanIfName());
                }
                else
                {
                    snprintf(LanIfName, 10, "%s:%d", getLanIfName(), num);
                }
            }
            getIfIp(LanIfName, ip2);
#else
            get_netarea(getLanIfName(),net_area);
            getIfIp((char*)getLanIfName(),ip2);
#endif
            OutIp=getOutOne2OneIp(ip, wanIndex);
            if(!OutIp) {
                getIfIp(getWanIfNamePPP(wanIndex),ip1);
            } else {
                int2ip(OutIp,ip1);
            }

            switch(proto){
                case STATIC_NAT_TCP:
                case STATIC_NAT_UDP:
                    makeStaticNat(action,wanIndex, cmd, sizeof(cmd), ip1, ip, proto, 
                            nOutPort,(nOutPort+PortNum-1),nInFromPort,(nInFromPort+PortNum-1));
                    doSystem(cmd);
                    break;
                case STATIC_NAT_TCP_UDP:
                    makeStaticNat(action,wanIndex, cmd, sizeof(cmd), ip1, ip, 
                            STATIC_NAT_TCP, nOutPort,(nOutPort+PortNum-1),nInFromPort,
                            (nInFromPort+PortNum-1));
                    doSystem(cmd);
                    makeStaticNat(action,wanIndex, cmd, sizeof(cmd), ip1, ip, STATIC_NAT_UDP, nOutPort,
                            (nOutPort+PortNum-1),nInFromPort,(nInFromPort+PortNum-1));
                    doSystem(cmd);
                    break;
                default:
                    break;
            }
            switch(proto){
                case STATIC_NAT_TCP:
                case STATIC_NAT_UDP:
                    makeStaticNatLoop(action,wanIndex, cmd, sizeof(cmd), ip2, net_area, ip, proto, 
                            nOutPort,(nOutPort+PortNum-1),nInFromPort,(nInFromPort+PortNum-1));
                    doSystem(cmd);
                    break;
                case STATIC_NAT_TCP_UDP:
                    makeStaticNatLoop(action,wanIndex, cmd, sizeof(cmd), ip2, net_area, ip, STATIC_NAT_TCP, 
                            nOutPort,(nOutPort+PortNum-1),nInFromPort,(nInFromPort+PortNum-1));
                    doSystem(cmd);
                    makeStaticNatLoop(action,wanIndex, cmd, sizeof(cmd), ip2, net_area, ip, STATIC_NAT_UDP, 
                            nOutPort,(nOutPort+PortNum-1),nInFromPort,(nInFromPort+PortNum-1));
                    doSystem(cmd);
                default:
                    break;
            }
        }
    }
}


#endif

#if (NAT_RULE == FYES)
 /***********************************************************************
 * �� �� ����   NatRuleDelAll
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����   ɾ�����е�NAT����
 ***********************************************************************/
void  NatRuleDelAll(void)
{
    iptablesNatRuleclear();
    int i,j;
    for (j=1; j<=getWanSlotCount(); j++ ) {
	char *wanIfName=getWanIfName(j);
	for(i=0; i<MAX_NAT_BIND_IP*2; i++ ) {
	    doSystem("ifconfig %s:%d down  2>/dev/null &",wanIfName, i);
	}
    }
    /*  ����static nat iptables       */
    updataStaticNat(); 
}


 /***********************************************************************
 * �� �� ����   NatRuleDelAll
 * �������ڣ�	2013-12-04
 * �޸����ڣ�
 * ���ߣ�       dyguo
 * ����˵����   null
 ***********************************************************************/
static int checkPassThroughInst()
{
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof=NULL;
    int min = 0, max = 0;
    int i = 0,count = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile *)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
	    if(prof->NatruleType == PASSTHROUGH) {
		count++;
	    }
	}
    }
    return count;
}
 /***********************************************************************
 * �� �� ����   NatRulePassThroughAorD
 * �������ڣ�	2013-12-04
 * �޸����ڣ�
 * ���ߣ�       dyguo
 * ����˵����   passthrough ������ӻ�ɾ��
 ***********************************************************************/
static void NatRulePassThroughAorD(char action)
{
    if(action==Iptables_ADD){
	doSystem("iptables -t mangle -N %s 1>dev/null 2>&1",NAT_RULE_PASS_THROUGH);
	doSystem("iptables -t mangle -I PREROUTING -m uttdev --is-lan-in -j %s 1>/dev/null 2>&1",NAT_RULE_PASS_THROUGH);
    }else if(action==Iptables_DEL){
	doSystem("iptables -t mangle -D PREROUTING -m uttdev --is-lan-in -j %s 1>/dev/null 2>&1",NAT_RULE_PASS_THROUGH);
	doSystem("iptables -t mangle -F %s 1>dev/null 2>&1",NAT_RULE_PASS_THROUGH);
	doSystem("iptables -t mangle -X %s 1>dev/null 2>&1",NAT_RULE_PASS_THROUGH);
    }
}

 /***********************************************************************
 * �� �� ����   NatRuleProfileChange
 * �������ڣ�	2011-10-14
 * �޸����ڣ�
 * ���ߣ�       yqyang
 * ����˵����   NAT�����profilechange����
 ***********************************************************************/
void NatRuleProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const NatRuleProfile* oldProfileDataPtr, const NatRuleProfile* newProfileDataPtr )
{
    DBGPRINT("NatRule change \n");
#if (CACHE_SERVER == FYES)
	buildCacheExceptionGrp();
#endif
    switch(changeType) {
	case PROF_CHANGE_ADD:
	    if((checkPassThroughInst() !=0) && (checkPassThroughInst() == 1)) {
		NatRulePassThroughAorD(Iptables_ADD);
	    }
	    iptablesEditOneNatRule(newProfileDataPtr,0, Iptables_ADD);
		/*
		 * ��NATRULE ProfileChange �ı�� �鿴�Ƿ���Ҫ�޸�STATICNAT
		 */
		checkNatRuleupdatatoStaticNat(htonl(newProfileDataPtr->OutIp),htonl(newProfileDataPtr->InFromIP),htonl(newProfileDataPtr->InEndIP),(newProfileDataPtr->NatBind[3]-'0'));
	    break;
	case PROF_CHANGE_EDIT:
	    iptablesEditOneNatRule(oldProfileDataPtr,0, Iptables_DEL);
	    iptablesEditOneNatRule(newProfileDataPtr,0, Iptables_ADD);
		    /* ����NATRULE �İ󶨽ӿ�û�б仯 ֻҪ����ˢ�� ��ǰ�ӿ�  ,����󶨽ӿڱ仯����Ҫˢ����ǰ�Ľӿں͸ı��Ժ�Ľӿ�      */
		if(oldProfileDataPtr->NatBind[3]-'0' != newProfileDataPtr->NatBind[3]-'0'){
		    /*  �ӿڸı� ˢ���޸���ǰ�Ľӿ� */

		checkNatRuleupdatatoStaticNat(htonl(oldProfileDataPtr->OutIp),htonl(oldProfileDataPtr->InFromIP),htonl(oldProfileDataPtr->InEndIP),(oldProfileDataPtr->NatBind[3]-'0'));
		}
		/*      �󶨽ӿ�Ϊ�ı� ֻ��Ҫˢ�����ڵĽӿ�  */

		checkNatRuleupdatatoStaticNat(htonl(newProfileDataPtr->OutIp),htonl(newProfileDataPtr->InFromIP),htonl(newProfileDataPtr->InEndIP),(newProfileDataPtr->NatBind[3]-'0'));
	    break;
	case PROF_CHANGE_DEL:
	    iptablesEditOneNatRule(oldProfileDataPtr,0, Iptables_DEL);
		checkNatRuleupdatatoStaticNat(htonl(oldProfileDataPtr->OutIp),htonl(oldProfileDataPtr->InFromIP),htonl(oldProfileDataPtr->InEndIP),(oldProfileDataPtr->NatBind[3]-'0'));
	    break;
	case PROF_CHANGE_DELALL:
	    NatRuleDelAll();
	    break;
	case PROF_CHANGE_SPECIAL:
	    wanIpRuleFun((oldProfileDataPtr->NatBind[3]-'0'), UTT_SPEED_WEB_SEND_MSG_ACT_DEL,
		    ntohl(oldProfileDataPtr->InFromIP), ntohl(oldProfileDataPtr->InEndIP));
	    break;
	default:
	    break;
    };
    if((changeType == PROF_CHANGE_DEL)||(changeType == PROF_CHANGE_DELALL)) {
	if((checkPassThroughInst() ==0)) {
	    NatRulePassThroughAorD(Iptables_DEL);
	}
    }
    return;
}

/***********************************************************************
* �� �� ����   iptablesEditOneNatRule
* �������ڣ�	2011-10-14
* �޸����ڣ�
* ���ߣ�       yqyang
* ����˵����   һ��ʵ����NAT����
***********************************************************************/
void iptablesEditOneNatRule(const NatRuleProfile* prof, int wanNo, char action)
{
    char ip1[16]={0},ip2[16]={0},ip3[16]={0},wanIp[16]={0},*wanName, NatBind[8];
    unsigned int nOutIp,nInFromIp,nInEndIp,ethId,i,ipNum,nWanIp, wanIndex;
    unsigned int Type = 0;
    DBGPRINT("InitNatRuleForWordRun begin\n");
    char lanIp[16]={0}, net_area[32]={0};;
#if (MULTI_LAN == FYES)
    int num = 0;
    char LanIfName[11];
#endif
    if(prof != NULL && ProfInstIsFree((Pointer)prof) == 0) {
	Type = prof->NatruleType;
	ethId = prof->EthId;
	nOutIp = htonl(prof->OutIp);
	nInFromIp =  htonl(prof->InFromIP);
	nInEndIp = htonl(prof->InEndIP);
	strcpy(NatBind,prof->NatBind);
	wanIndex = prof->NatBind[3] - '0';
	if((wanNo==0) || (wanIndex==wanNo)) {
	    wanName=getWanIfName(wanIndex);
	    getIfIp(wanName,wanIp);
	    nWanIp=ip2int(wanIp);
	    ipNum=1;
	    if(Type == ONE2ONE)
	    {
		ipNum=nInEndIp-nInFromIp+1;
	    }
	    if( (action==Iptables_ADD) && (_getWanLinkState(wanIndex)==1)){
		/*  ���Դ��ַ·��   */
		wanIpRuleFun(wanIndex, UTT_SPEED_WEB_SEND_MSG_ACT_ADD,
			ntohl(prof->InFromIP), ntohl(prof->InEndIP));
	    }
	    else if(action==Iptables_DEL){
		/* ɾ��Դ��ַ·�� */
		wanIpRuleFun(wanIndex, UTT_SPEED_WEB_SEND_MSG_ACT_DEL,
			ntohl(prof->InFromIP), ntohl(prof->InEndIP));
	    }
	    if(Type == PASSTHROUGH) { /*passthrough*/
		if( (action==Iptables_ADD) && (_getWanLinkState(wanIndex)==1)) {
		    doSystem("iptables -t mangle -I %s -m iprange --src-range %s-%s -j CONNMARK --set-mark 0x%d0/0x%x  1>/dev/null 2>&1",NAT_RULE_PASS_THROUGH,int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2),wanIndex,POLICYROUTE_CONNMARK_MSK);
		    doSystem("iptables -t nat -I POSTROUTING  -m iprange --src-range %s-%s -o %s -j ACCEPT 1>/dev/null 2>&1",int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2),wanName);
		} else { /*delete*/
		    doSystem("iptables -t mangle -D %s -m iprange --src-range %s-%s -j CONNMARK --set-mark 0x%d0/0x%x  1>/dev/null 2>&1",NAT_RULE_PASS_THROUGH,int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2),wanIndex,POLICYROUTE_CONNMARK_MSK);
		    doSystem("iptables -t nat -D POSTROUTING  -m iprange --src-range %s-%s -o %s -j ACCEPT 1>/dev/null 2>&1",int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2),wanName);
		
		}
	    } else {

	    for(i=0;i<ipNum;i++)
	    {

		if(Type == ONE2ONE)
		{
		    memset(lanIp, 0, sizeof(lanIp));
#if (MULTI_LAN == FYES)
		    get_netarea(getLanIfName(), net_area, int2ip((nInFromIp+i),ip1), &num);
		    if(num == 0)
		    {
			return;
		    }
		    else
		    {
			memset(LanIfName, 0, sizeof(LanIfName));
			if(num == 1)
			{
			    snprintf(LanIfName, 10, "%s", getLanIfName());
			}
			else
			{
			    snprintf(LanIfName, 10, "%s:%d", getLanIfName(), num);
			}
		    }
		    getIfIp(LanIfName, lanIp);
#else
		    getIfIp((char*)getLanIfName(),lanIp);
#endif
		}
		if( (action==Iptables_ADD) && (_getWanLinkState(wanIndex)==1))
		{ 
		    if(nWanIp!=nOutIp+i)//���IP 
		    {
			doSystem("ifconfig %s:%d down  1>/dev/null 2>&1",wanName,(ethId+i));
			doSystem("ifconfig %s:%d %s netmask 255.255.255.255  1>/dev/null 2>&1",wanName,(ethId+i),int2ip((nOutIp+i),ip1));
		    }
		}
		else//delete
		{
		    if(nWanIp!=nOutIp+i)
		    {
			doSystem("ifconfig %s:%d down  1>/dev/null 2>&1",wanName,(ethId+i));
		    }
		}
		/* clear the nat session */
		doSystem("echo %xallwan >/proc/nf_conntrack_wan ", nOutIp+i);

		if( (action==Iptables_ADD) && (_getWanLinkState(wanIndex)==1)){
		    if(Type == ONE2ONE)
		    {
			doSystem("iptables -t nat -A wan%d_"NAT_RULE_CHAIN"  -m uttdev --is-lan-in -d %s -j SNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nInFromIp+i),ip1), lanIp);

			doSystem("iptables -t nat -A wan%d_"NAT_RULE_CHAIN" -s %s -j SNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nInFromIp+i),ip1),int2ip((nOutIp+i),ip2));
			doSystem("iptables -t nat -A wan%d_"NAT_RULE_PRE_CHAIN"  -d %s -j DNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nOutIp+i),ip2),int2ip((nInFromIp+i),ip1));
		    }
		    else
		    {
			doSystem("iptables -t nat -A wan%d_"NAT_RULE_CHAIN" -m iprange --src-range %s-%s -j SNAT --to %s  1>/dev/null 2>&1",\
				wanIndex,int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2), int2ip(nOutIp,ip3));
		    }
		}
		else if(action==Iptables_DEL){
		    if(Type == ONE2ONE)
		    {
			doSystem("iptables -t nat -D wan%d_"NAT_RULE_CHAIN"  -m uttdev --is-lan-in -d %s -j SNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nInFromIp+i),ip1), lanIp);

			doSystem("iptables -t nat -D wan%d_"NAT_RULE_CHAIN" -s %s -j SNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nInFromIp+i),ip1),int2ip((nOutIp+i),ip2));
			doSystem("iptables -t nat -D wan%d_"NAT_RULE_PRE_CHAIN"  -d %s -j DNAT --to %s  1>/dev/null 2>&1",\
				 wanIndex,int2ip((nOutIp+i),ip2),int2ip((nInFromIp+i),ip1));
		    }
		    else
		    {
			doSystem("iptables -t nat -D wan%d_"NAT_RULE_CHAIN" -m iprange --src-range %s-%s -j SNAT --to %s  1>/dev/null 2>&1",\
				wanIndex,int2ip(nInFromIp,ip1),int2ip(nInEndIp,ip2), int2ip(nOutIp,ip3));
		    }
		}
	    }
	    }
	}
    }

}
/***********************************************************************
* �� �� ����   iptablesEditOneNatRule
* �������ڣ�	2011-10-14
* �޸����ڣ�
* ���ߣ�       yqyang
* ����˵����   nat rule ���ýӿ�
		wanno  wan�ں�  action ������������ɾ��    A �������� D����ر�
***********************************************************************/
void InitNatRuleForWordRun(int wanNo, char action)
{
    int min = 0, max = 0,i = 0;
    MibProfileType mibType = MIB_PROF_NAT_RULE;
    NatRuleProfile *prof= NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (NatRuleProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    if(action == Iptables_ADD) {
		iptablesEditOneNatRule(prof,wanNo, Iptables_DEL);
	    }
	    iptablesEditOneNatRule(prof,wanNo, action);
	}
    }

}
/*  NatRule Nat init */
void funInitNatRule(void)
{
    registerForProfileUpdates(MIB_PROF_NAT_RULE, (Pointer)NULL, (ProfileUpdateFnc)NatRuleProfileChange);
}
#endif

#if (DMZ == FYES)
void DMZProfileChange(Pointer tag, MibProfileType profileType, 
	ProfChangeType changeType, Uint32 index,
	const DMZProfile* oldProfileDataPtr, const DMZProfile* newProfileDataPtr )
{
    DBGPRINT("DMZProfileChange !\n");
    switch(changeType) {
	case PROF_CHANGE_ADD:
	case PROF_CHANGE_EDIT:
	case PROF_CHANGE_DEL:
	case PROF_CHANGE_DELALL:
	    iptablesDMZForWordRun();
	    break;
	default:
	    break;
    };
    return;
}

/***********************************************************************
* �� �� ����   makeiptablesDMZ
* �������ڣ�	2011-10-14
* �޸����ڣ�
* ���ߣ�       yqyang
* ����˵����   DMZ��itpables����ʵ��
************************************************************************/
void makeiptablesDMZ(unsigned int wanIndex, char *buf, int len, char *wan_name, char *ip_address)
{
    char_t lanIp[16]={0}, wanIp[16]={0}, net_area[32]={0};
#if (MULTI_LAN == FYES)
    int num = 0;
    char LanIfName[11];
#endif
    getIfIp(wan_name ,wanIp);
#if (MULTI_LAN == FYES)
    get_netarea(getLanIfName(), net_area, ip_address, &num);
    if(num == 0)
    {
	return;
    }
    else
    {
	memset(LanIfName, 0, sizeof(LanIfName));
	snprintf(LanIfName, 10, "%s:%d", getLanIfName(), num);
    }
    getIfIp(getLanIfName(), lanIp);
#else
    get_netarea(getLanIfName(),net_area);
    getIfIp(getLanIfName(),lanIp);
#endif
#if (IP_SEC == FYES)
    doSystem("iptables -t nat -A wan%d_%s -d %s -p udp -m multiport --dport 500,4500 -j RETURN ",wanIndex,DMZ_CHAIN,wanIp);
#endif
    int rc = snprintf(buf, len, "iptables -t nat -A wan%d_%s -d %s -j DNAT --to %s", wanIndex, DMZ_CHAIN, wanIp ,ip_address);
    snprintf(buf+rc, len, ";iptables -t nat -A wan%d_%s -s %s -d %s -j SNAT --to %s", wanIndex, DMZ_LOOP_CHAIN, net_area,ip_address ,lanIp);
}

/***********************************************************************
* �� �� ����	iptablesDMZForWordRun
* �������ڣ�	2011-10-14
* �޸����ڣ�
* ���ߣ�       yqyang
* ����˵����   DMZ�Ľӿں���
************************************************************************/
void iptablesDMZForWordRun(void)
{
    int i, DMZIP;
    char cmd[1024], ip_address[16];
    iptablesDMZNatclear();

    MibProfileType mibType = MIB_PROF_DMZ_NAT;
    DMZProfile *prof= NULL;
    prof = (DMZProfile*)ProfGetInstPointByIndex(mibType, 0);
    DMZIP = htonl(prof->DMZIP);
    int2ip(DMZIP,ip_address);
    if(prof != NULL && ProfInstIsFree(prof) == 0 && prof->head.active == 1 && DMZIP != 0) {
	for(i =1; i<=getWanSlotCount();i++) {
	    if(_getWanLinkState(i)) {
	    makeiptablesDMZ(i,cmd, sizeof(cmd), getWanIfNamePPP(i), ip_address);
	    doSystem(cmd);
	    }
	}
    }
}
/* DMZ init */
void funInitDMZ(void)
{
    registerForProfileUpdates(MIB_PROF_DMZ_NAT, (Pointer)NULL, (ProfileUpdateFnc)DMZProfileChange);
}
#endif
#endif
