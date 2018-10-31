#include "uttMachine.h"
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <syslog.h>

#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "time.h"
 
#if(MAKE_MONEY == FYES)
#include    <linux/netfilter/nf_conntrack_common.h>
#include "utt_conf_define.h"

#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
#endif

#define MONEY 1
#ifdef MONEY_DBG
#define MONEY_PRINTF(fmt, args...)		printf(fmt, ## args)	
#else
#define MONEY_PRINTF(fmt, args...)
#endif

#define MIN_ACTIVE_TIME 1356998400u

#if (WIFIDOG_MAKE_MONEY == FYES)

#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
/* ����������
 * isAdd: 0 ɾ�� ����0 ��� */
static void whiteDnsInit(int isAdd)
{

    UttNlNfwifiDnsConf  nlConf;
    
    memset(&nlConf, 0 ,sizeof(nlConf));
    
    nlConf.msgHead.nlType = UTT_NLNF_WIFIDOG_DNS_WHITE;	    /*wifidog����������*/
    if(isAdd != 0)
    {
	nlConf.confType = UTT_NFCONF_ADD;
    }
    else
    {
	nlConf.confType = UTT_NFCONF_DEL;
    }

    /*֧��������*/
    strncpy(nlConf.entryVal.domainName, "*.alipay*.com", MAX_DOMAIN_NAME_LENGTH);/*����*/
    nlConf.entryVal.ipEntry = NULL;
    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*������Ϣ���ں�*/

}
#endif

#ifdef CONFIG_UTT_NETFILTER
/**
 *  ���ͻ����쳣�����ˡ��ں�hash��ʱʱ��֪ͨӦ��
 *  �ô�ip����
 */
static void uttNfNlStaleTimeSet(int enable)
{
    UttNlNfHashConf conf;
    memset(&conf, 0, sizeof(UttNlNfHashConf));
    conf.msgHead.nlType = UTT_NLNF_HASH;
    conf.confType = UTT_NFCONF_ADD;
    conf.staleEn = 1;
    /*���ؿ���,����wifidog*/
    conf.staleTime = 15U;/*��ʱʱ��15����*/
    if(enable != 0)
    {
	conf.trigger =  1U<<UTT_NF_HASH_WIFIDOG_DES_NOTIFY;
    } else {
	conf.trigger =  0U;
    }
    uttNfNlSend(spdNfNlSock, (char *)&conf, sizeof(conf), 0, 0);/*������Ϣ���ں�*/
    return;
}
#endif

/*
 * wifidogStart()
 * wifidog ��ʼ��
 * */
static void wifidogStart()
{
    doSystem("wifidog-init start ");
    return;
}

/*
 * wifidogClose()
 * wifidog exit
 * kill wifidog 
 * */
static void wifidogClose()
{
    doSystem("wifidog-init stop ");
    sleep(1);
    kill_process("wifidog");
    kill_process("wdctl");
    return ;
}
/**
 * lan�ڱ仯��Ҫ����һЩ׬Ǯwifi����
 * */
int  makeMoneyOnLan(void)
{
    /*��Ҫ�����ж�׬Ǯwifi��ǰ�Ƿ���Ч���Դﵽ����ʱ���̻߳���*/
    MakeMoneyProfile *prof = NULL;

    uttSemP(SEM_SALE_WIFI,0);
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
    if((prof != NULL)&&(prof->MakeMoneyEn == 1)&&(prof->wifitime_active == 1)) //׬Ǯwifi�����ҵ�ǰʱ����Ч
    {
	sleep(2);
	wifidogClose();
	sleep(1);
	wifidogStart();
    }
    uttSemV(SEM_SALE_WIFI,0);
}
static int makeMoneyRuleAdd(void)
{
    wifidogStart();
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
    whiteDnsInit(1);/*��֧ͨ����������*/
#endif
#ifdef CONFIG_UTT_NETFILTER
    uttNfNlStaleTimeSet(1);
#endif
}
static int makeMoneyRuleExit(void)
{
    uttNfNlStaleTimeSet(0);/*��ɾ���ں�ע���netlink֪ͨ����ɱwifidog��������ܻ���ᱻ��Ч��֪ͨ����*/
    wifidogClose();
#ifdef CONFIG_UTT_WIFIDOG_XT_WIFI_MATCH
    whiteDnsInit(0);/*ɾ��֧����������*/
#endif
}
#endif

int moneywifi_switch_change(int status,MakeMoneyProfile *prof)
{
    int ret = 0;
    if(prof->wifitime_active != status)
    {
	prof->wifitime_active = status;
printf("wifitime_active = %d\n",prof->wifitime_active);
	ret = 1;
    }
    return ret;
}
int moneywifi_timeCheck(void)
{
    MakeMoneyProfile *prof = NULL;
    time_t daytime=0u,tmCurrent=0u;
    unsigned int minute1 = 0u, minute2 = 0u, hour1 = 0u, hour2 = 0u;
    unsigned int timestart = 0u, timestop = 0u;
    int status = -1;

    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);

    tmCurrent = time(NULL);
    daytime = tmCurrent % 86400;
    if(prof != NULL){
	sscanf(prof->TimeStart,"%u:%u",&hour1,&minute1);	
	sscanf(prof->TimeStop,"%u:%u",&hour2,&minute2);	
	timestart = hour1 * 3600 + minute1 * 60;
	timestop = hour2 * 3600 + minute2 * 60 + 59;
	if (tmCurrent < MIN_ACTIVE_TIME)
	{
	    status = 0;//ϵͳʱ�䲻ͬ��׬Ǯwifi�ر�
	}
	/*�ж��Ƿ��ڿ�ʼʱ�䵽����ʱ��֮��*/
	if(timestart <= timestop)
	{
	    if ((daytime >= timestart ) && (daytime <= timestop))
	    {
		status = 1;
	    }else{
		status = 0;
	    }
	}else{
	    if ((daytime >= timestart ) || (daytime <= timestop))
	    {
		printf("%s-%d\n",__func__,__LINE__);
		status = 1;
	    }else{
		status = 0;
	    }

	}

    }
    return status;
}
static void MoneywifiTimeEdit()
{
    int status=0;
    MakeMoneyProfile *prof = NULL;
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
    if(prof != NULL)
    {
	status = moneywifi_timeCheck();
	printf("%s-%d,status=%d\n",__func__,__LINE__,status);
	if(status != -1)
	{
	    moneywifi_switch_change(status,prof);
	}
    }
}
static void AddLimtRule(MakeMoneyProfile *prof)
{
    char ratestr[10];
    doSystem("iptables -t mangle -F salewifi_limit");
    doSystem("iptables -t mangle -D FORWARD  -m connmark --mark %u/0x%x -j salewifi_limit",SALEWIFI_MARK,NOTICE_CONNMARK_MSK);
    doSystem("iptables -t mangle -X salewifi_limit");

    /*��ɾ�������ֹ�ظ�����*/
    doSystem("iptables -t mangle -D PREROUTING -m uttdev --is-salewifi-in -j CONNMARK --set-mark %u/0x%x",SALEWIFI_MARK,NOTICE_CONNMARK_MSK);


    if((prof->MakeMoneyEn == 1)&&(prof->wifitime_active == 1)) {  //׬Ǯwifi��Чʱ����������

	doSystem("iptables -t mangle -N salewifi_limit");
	doSystem("iptables -t mangle -A FORWARD  -m connmark --mark %u/0x%x -j salewifi_limit",SALEWIFI_MARK,NOTICE_CONNMARK_MSK);

	/*��salewifi��������ݰ�����0x4000���*/
	doSystem("iptables -t mangle -A PREROUTING -m uttdev --is-salewifi-in -j CONNMARK --set-mark %u/0x%x",SALEWIFI_MARK,NOTICE_CONNMARK_MSK);

	if(prof->uRate != 0){
	    memset(ratestr,0,sizeof(ratestr));
	    sprintf(ratestr , "%.0u", prof->uRate*1024 /8);
	    printf("%s-%d,%s\n",__func__,__LINE__,ratestr);
	    doSystem("iptables -t mangle -A salewifi_limit -m uttdev --lan-to-wan  -m hashlimit --hashlimit-name wifisrc --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode srcip -j RETURN",ratestr,ratestr);
	    doSystem("iptables -t mangle -A salewifi_limit -m uttdev --lan-to-wan -j DROP");
	}
	if(prof->dRate != 0){
	    memset(ratestr,0,sizeof(ratestr));
	    sprintf(ratestr , "%.0u", prof->dRate*1024 /8);
	    printf("%s-%d,%s\n",__func__,__LINE__,ratestr);
	    doSystem("iptables -t mangle -A salewifi_limit -m uttdev --wan-to-lan -m hashlimit --hashlimit-name wifidst --hashlimit %s/sec --hashlimit-burst %s --hashlimit-mode dstip -j RETURN",ratestr,ratestr);
	    doSystem("iptables -t mangle -A salewifi_limit -m uttdev --wan-to-lan -j DROP");
	}
    }

}

extern void moneywifi_separate(void)
{
    MakeMoneyProfile *prof = NULL;
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);

    if(prof != NULL){
	doSystem("ebtables -F FORWARD");
	doSystem("iptables -t mangle -D INPUT -p tcp -m uttdev --is-salewifi-in -m multiport ! --dport 53,2060 -j DROP");   //ɾ����ֹ׬Ǯwifi�����豸 

	if ((prof->MakeMoneyEn == 1)&&(prof->SaftyEn == 1)&&(prof->wifitime_active == 1))
	{
	    doSystem("iptables -t mangle -A INPUT -p tcp -m uttdev --is-salewifi-in -m multiport ! --dport 53,2060 -j DROP");   //��ֹ׬Ǯwifi�����豸 
	    /*׬Ǯwifi ssid��lan�ڼ���ͨssid����*/
	    doSystem("ebtables -A FORWARD -i ra2 -o ra0 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra2 -o ra1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra2 -o rai0 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra2 -o rai1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra2 -o eth2.1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra0 -o ra2 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra1 -o ra2 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai0 -o ra2 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai1 -o ra2 -j DROP");
	    doSystem("ebtables -A FORWARD -i eth2.1 -o ra2 -j DROP");

	    doSystem("ebtables -A FORWARD -i ra3 -o ra0 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra3 -o ra1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra3 -o rai0 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra3 -o rai1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra3 -o eth2.1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra0 -o ra3 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra1 -o ra3 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai0 -o ra3 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai1 -o ra3 -j DROP");
	    doSystem("ebtables -A FORWARD -i eth2.1 -o ra3 -j DROP");

	    doSystem("ebtables -A FORWARD -i rai2 -o ra0 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai2 -o ra1 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai2 -o rai0 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai2 -o rai1 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai2 -o eth2.1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra0 -o rai2 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra1 -o rai2 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai0 -o rai2 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai1 -o rai2 -j DROP");
	    doSystem("ebtables -A FORWARD -i eth2.1 -o rai2 -j DROP");

	    doSystem("ebtables -A FORWARD -i rai3 -o ra0 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai3 -o ra1 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai3 -o rai0 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai3 -o rai1 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai3 -o eth2.1 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra0 -o rai3 -j DROP");
	    doSystem("ebtables -A FORWARD -i ra1 -o rai3 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai0 -o rai3 -j DROP");
	    doSystem("ebtables -A FORWARD -i rai1 -o rai3 -j DROP");
	    doSystem("ebtables -A FORWARD -i eth2.1 -o rai3 -j DROP");

	}
    }
}
/*
 *������flag-�Ƿ������������߹��ܵı�ʶ
 */
void MakeMoneyInit(MakeMoneyProfile *prof,int flag)
{
    if(flag == 1){
	doSystem("wlan.sh");
#if (WIRELESS_5G == FYES)
	doSystem("wlan_5g.sh");
#endif
    }else{
	if((prof->MakeMoneyEn == 1)&&(prof->wifitime_active == 1))   //׬Ǯwifi�ر�-������
	{
	    doSystem("ifconfig ra2 up");
	    doSystem("ifconfig ra3 up");
	    doSystem("brctl addif br0 ra2");
	    doSystem("brctl addif br0 ra3");
#if (WIRELESS_5G == FYES)
	    doSystem("ifconfig rai2 up");
	    doSystem("ifconfig rai3 up");
	    doSystem("brctl addif br0 rai2");
	    doSystem("brctl addif br0 rai3");
#endif

	}else{   //׬Ǯwifi����-���ر�
	    doSystem("brctl delif br0 ra2");
	    doSystem("brctl delif br0 ra3");
	    doSystem("ifconfig ra2 down");
	    doSystem("ifconfig ra3 down");
#if (WIRELESS_5G == FYES)
	    doSystem("brctl delif br0 rai2");
	    doSystem("brctl delif br0 rai3");
	    doSystem("ifconfig rai2 down");
	    doSystem("ifconfig rai3 down");
#endif
	}
    }

#if (WIFIDOG_MAKE_MONEY)
    if((prof != NULL)&&(prof->MakeMoneyEn == 1)&&(prof->wifitime_active == 1))
    {//׬Ǯwifi�����ҵ�ǰʱ����Ч������wifidog
	makeMoneyRuleAdd();
    }else{
	makeMoneyRuleExit();//�˳�wifidog
    }
#endif

    sleep(3);
    moneywifi_separate();
    AddLimtRule(prof);

}
static void makeMoneyProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const MakeMoneyProfile *oldProfileDataPtr, const MakeMoneyProfile *newProfileDataPtr )
{
    MONEY_PRINTF("%s\n", __FUNCTION__);

    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
	    uttSemP(SEM_SALE_WIFI,0);

//	    AddLimtRule((MakeMoneyProfile *)newProfileDataPtr);
            if((strcmp(oldProfileDataPtr->TimeStart,newProfileDataPtr->TimeStart)!=0)
	      || (strcmp(oldProfileDataPtr->TimeStop,newProfileDataPtr->TimeStop)!=0))//Ӫҵʱ��仯
	    {
		MoneywifiTimeEdit();
	    }
	    if(strcmp(oldProfileDataPtr->charge,newProfileDataPtr->charge)!=0)//���۱仯������wifidog
	    {
		makeMoneyRuleExit();//���˳�wifidog 
	    }
	    if((strcmp(oldProfileDataPtr->ssid,newProfileDataPtr->ssid)!=0)||(strcmp(oldProfileDataPtr->ssid2,newProfileDataPtr->ssid2)!=0)||(strcmp(oldProfileDataPtr->ssid_5g,newProfileDataPtr->ssid_5g)!=0)||(strcmp(oldProfileDataPtr->ssid2_5g,newProfileDataPtr->ssid2_5g)!=0)||(strcmp(oldProfileDataPtr->encodeType,newProfileDataPtr->encodeType)!=0)||(strcmp(oldProfileDataPtr->encodeType2,newProfileDataPtr->encodeType2)!=0)||(strcmp(oldProfileDataPtr->encodeType_5g,newProfileDataPtr->encodeType_5g)!=0)||(strcmp(oldProfileDataPtr->encodeType2_5g,newProfileDataPtr->encodeType2_5g)!=0))
		MakeMoneyInit(newProfileDataPtr,1);     //ssid�仯��������������
	    else
		MakeMoneyInit(newProfileDataPtr,0);     //ֻ��رջ���׬Ǯwifi�ӿ�
	    uttSemV(SEM_SALE_WIFI,0);
	    break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:
            break;
    };
    return;
}




/*
 * ��ʼ��
 */
extern void funInitMakeMoney(void) {
    MakeMoneyProfile *prof = NULL;
    registerForProfileUpdates(MIB_PROF_MAKE_MONEY, NULL, makeMoneyProfileChange);
  //  doSystem("iptables -t mangle -A INPUT -m uttdev --is-salewifi-in -j DROP");   //��ֹ׬Ǯwifi�����豸

    uttSemP(SEM_SALE_WIFI,0);
    prof = (MakeMoneyProfile *)ProfGetInstPointByIndex(MIB_PROF_MAKE_MONEY,0);
    if(prof != NULL){
	if(prof->isBind == 0)
	{
	    prof->MakeMoneyEn = 0;
	}
	MakeMoneyInit(prof,1);
	printf("%s-%d\n",__func__,__LINE__);
//	AddLimtRule(prof);
    }    
    uttSemV(SEM_SALE_WIFI,0);
    return;
}

#endif
