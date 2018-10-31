#include        <linux/autoconf.h>
#include	<string.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include        "internet.h"
#include	"mib.h"
#include	"profacce.h"
#include        "ifmisc.h"
#include	"uttfunction.h"
#include        "ctype.h"
#include	"translate.h"
#include        "comExeFun.h"
#if (ARP_BINDS == FYES)
#define ARPPD_NAME_H "arpd_" /*arpĬ���û���ǰ׺*/
#define MAX_ARP_FILE_BUF	MAX_USER_PROFILES*(UTT_INST_NAME_LEN + 15/*���ip��ַ*/ + 17/*mac*/ +4)
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName);

static char* getEntryByIpFromSysArpTb(char *ipStr);
extern char* getMacByIpFromSysArpTb(char *arpIp);
static bool getArpAllowOtherEn(void);
static int getArpBindGlobalConfig(int eid, webs_t wp, int argc, char_t **argv);
static int getArpBindList(int eid, webs_t wp, int argc, char_t **argv);
extern char* getNameByIPMac(char *ip, char *mac);
static int getOneArpBindConfig(int eid, webs_t wp, int argc, char_t **argv);
static bool arpBindGlobalConfigCheck(webs_t wp);
static void formConfigArpBindGlobalConfig(webs_t wp, char_t *path, char_t *query);
static int sendArpRequest(IPADDR ipaddr, IPADDR netMask, char*ifName);
extern bool getIpMacFromSysArpEntry(char*arpEntry, char*ipStr ,char* macStr, char*ifName);
static void readScanArp(int eid,webs_t wp,int argc,char_t **argv);
static void formReadArp(webs_t wp,char_t *path,char_t *query);
static int aspOutReadArp(int eid,webs_t wp,int argc,char_t **argv);
static bool parseArpImuFromTaRow(char* rowStr, char** ipStr, char** macStr, char** userName);
static void formAddReadArp(webs_t wp,char_t *path,char_t *query);
static bool arpBindDelAccessCheck(char* clientIp, IPADDR delIp);
static bool arpBindAddAccessCheck(char*userIp,char* newIp, char*newMac);
static bool arpBindUniCheck(char*userName, IPADDR ip, MacAddr mac, int editIndex);
static void arpBindConfigAdd(webs_t wp);
static void arpBindConfigModify(webs_t wp);
static void formConfigArpBindConfig(webs_t wp, char_t *path, char_t *query);
static void formConfigArpBindAllow(webs_t wp, char_t *path, char_t *query);
static void formArpBindDel(webs_t wp, char_t *path, char_t *query);
static void formArpBindDelAll(webs_t wp, char_t *path, char_t *query);
/***********************************************************************
 * �� �� ����   getEntryByIpFromSysArpTb
 * ����������	���ݴ����ip��ַ����arp���е���Ӧ����Ŀ��������
 * �������:    ipStr - �����ҵ�ip��ַ
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
static char* getEntryByIpFromSysArpTb(char *ipStr)
{
    static char ret[96];
    char *retP = NULL;
    int i=0;
    FILE *fp;
    fp=fopen("/proc/net/arp","r");
    if(fp != NULL)
    {
	while(!feof(fp))
	{
	    memset(ret, 0, sizeof(ret));
	    fgets(ret,sizeof(ret),fp);
	    if(strlen(ret) >= ARP_VALID_MIN_LEN)/*�Ϸ�arp��Ŀ*/
	    {
		i=0;
		while((ret[i]!=' ') && (i<sizeof(ret)))
		{
		    i++;
		}
		ret[i]='\0';
		if(!strcmp(ipStr,ret))/*ip�µ�ַ��ͬ*/
		{
		    ret[i]=' ';
		    retP =  ret;
		    break;
		}
	    }
	}
	fclose(fp);
    }
    return retP;
}
/**
 * ����ip��ַ��ϵͳarp���в������Ӧ��mac��ַ
 * retun :�ɹ��򷵻ض�Ӧ��mac��ַ  ���򷵻�NULL
 * bhou
 * 2011-10-26
 */
extern char* getMacByIpFromSysArpTb(char *arpIp)
{
    char *ptr,*arpEntryP,buf[96]={0};
    static char arpMac[32];
    char *tmptok1;

    char *split=" ";
    int ct=0;

    memset(arpMac, 0, sizeof(arpMac));
    arpEntryP = getEntryByIpFromSysArpTb(arpIp);/*��ȡip��ַ��Ӧ��ϵͳarp��Ŀ*/

    if (arpEntryP!=NULL)/*��ȡ�ɹ�*/
    {
	strncpy(buf, arpEntryP, sizeof(buf) -1u);/*��һ�ݿ�����ֹ��ȡ������Ŀ������*/

	tmptok1 = strtok_r(buf,split,&ptr);
	while(tmptok1!=NULL)
	{
	    ct++;
	    if(ct==4) {/*������Ϊ��Ӧ��mac��ַ*/
		strcpy(arpMac,tmptok1);
		break;
	    }
	    tmptok1 = strtok_r(NULL, split,&ptr);
	}
    }

    if(arpMac[0] != '\0')
    {
	ptr = arpMac;/*�ҵ���Ӧ��mac��ַ*/
    }
    else
    {
	ptr = NULL;/*δ�ҵ���Ӧ��mac��ַ*/
    }

    return ptr;

}

/**
 * ���������û�ȡarp���Ƿ�����ǰ��û�����
 * bhou
 * 2011-10-28
 */
static bool getArpAllowOtherEn(void)
{
    InterfaceProfile *prof = NULL;
    bool ret = 1;/*Ĭ��Ϊ����*/


    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*��ȡlan��*/

    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*��������*/
    {
	if(prof->arpAllowOtherEn == FUN_DISABLE)/*������*/
	{
	    ret = 0;
	}
    }

    return ret;

}
/***********************************************************************
 * �� �� ����   getArpBindGlobalConfig
 * ����������   ��ҳ�����ȫ�������еĲ�����Ϣ�������ARP���û���
 * �������ڣ�	2011-10-25
 * �޸����ڣ�
 * ���ߣ�       bhou
 * ����˵����	��
 ***********************************************************************/
static int getArpBindGlobalConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    if(getArpAllowOtherEn()==0) /*������ǰ��û�����*/
    {
	websWrite(wp,"var AllowOtherEnables=\"\";");
    }
    else
    {
	websWrite(wp,"var AllowOtherEnables=\"on\";");
    }
#if (UTT_SMART_UI == FYES) 
    getTheRightMsg(eid,wp,argc,argv);
#endif
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}

/***********************************************************************
 * �� �� ����   getArpBindList
 * ����������   ��ҳ�����ARP��������Ϣ
 * �������ڣ�	2010-3-17
 * �޸����ڣ�   2011-10-24 bhou
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
static int getArpBindList(int eid, webs_t wp, int argc, char_t **argv)
{
    int min = 0, max = 0;
    int i = 0;
    int totalrecs = 0;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    struct in_addr addrIp;

    char_t *UserNames=T("UserNames[%d] = \"%s\";\n");
    char_t *ips =T("arp_IPs[%d] = \"%s\";\n");
    char_t *macs = T("arp_Macs[%d] = \"%s\"; \n");
    char_t *allows = T("Allows[%d] = \"%s\"; \n");
    websWrite(wp,"var UserNames=new Array();\n");
    websWrite(wp,"var arp_IPs = new Array();\n");
    websWrite(wp,"var arp_Macs = new Array();\n");
    websWrite(wp,"var Allows = new Array();\n");


    ProfInstLowHigh(mibType, &max, &min);


    for(i = min; i < max; i++) {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    websWrite(wp, UserNames, totalrecs, prof->head.name);/*�û���*/
	    addrIp.s_addr = prof->ip;

	    websWrite(wp, ips, totalrecs, inet_ntoa(addrIp));/*ip*/

	    websWrite(wp, macs, totalrecs, converMac6To17Byte(prof->mac));/*mac*/

	    websWrite(wp, allows, totalrecs, (prof->bindEn == FUN_DISABLE?"no":"yes"));/*����*/
	    totalrecs++;
	}
    }

    websWrite(wp,"var totalrecs=%d;", totalrecs);/*�Ѱ󶨸���*/
    websWrite(wp,"var max_totalrecs=%d;", max);/*���󶨸���*/
    return 0;
}

/***********************************************************************
 * �� �� ����   getArpBindList
 * ����������   ��ҳ�����ARP��������Ϣ
 * �������ڣ�	2010-3-17
 * �޸����ڣ�   2011-10-24 bhou
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
extern char* getNameByIPMac(char *ip, char *mac)
{
    int min = 0, max = 0;
    int i = 0;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    struct in_addr addrIp;

    if ((ip == NULL) || (mac == NULL))
    {
	return NULL;
    }

    ProfInstLowHigh(mibType, &max, &min);

    for(i = min; i < max; i++) {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    addrIp.s_addr = prof->ip;
	    if ((strcmp(inet_ntoa(addrIp), ip) == 0) || (strcmp(converMac6To17Byte(prof->mac), mac) == 0))
	    {
		return prof->head.name;
	    }
	}
    }

    return NULL;
}


/***********************************************************************
 * �� �� ����   getOneArpBindConfig
 * ����������   �����޸�ʱ��ҳ��������޸ĵ�ARP��������Ϣ
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * �޸ģ�       bhou(2011-10-24)
 * ����˵����	��
 ***********************************************************************/
static int getOneArpBindConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t *edit_name = NULL;
    const char *errMsg=NULL;
    UserProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_USER;

    struct in_addr addrS;

    if(ejArgs(argc,argv,T("%s"), &edit_name) <1) 
    {
	errMsg = getTransDataForC(C_LANG_INDEX_READ_INST_NAME_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else if(!edit_name)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_NULL);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {

	prof = (UserProfile*)ProfGetInstPointByName(mibType, edit_name);
	if(prof == NULL) {
	    errMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_USER_NAME);
	    setTheErrorMsgOnFree(errMsg);
	}
	else 
	{
	    websWrite(wp, "var UserNames=\"%s\";", prof->head.name);/*�û���*/
	    addrS.s_addr = prof->ip;
	    websWrite(wp, "var arp_IPs=\"%s\";", inet_ntoa(addrS));/*ip��ַ*/
	    websWrite(wp, "var arp_Macs=\"%s\";", converMac6To17Byte(prof->mac));/*mac��ַ*/
	    websWrite(wp, "var Allows=\"%s\";", (prof->bindEn == FUN_DISABLE?"no":"yes"));/*����*/
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);/*������Ϣ*/
    return 0;
}


/**
 * arp��ȫ�����á������Ϸ�����֤
 * ��������ǰ��û����ӵ��豸ʱ����ǰ�����߱����ڰ��б��У��źϷ�
 * bhou
 * 2011-10-27
 */
static bool arpBindGlobalConfigCheck(webs_t wp)
{
    bool checkRet = 1;/*Ĭ����֤ͨ��*/
    char *allowEnable;
    char *clientIp = wp->ipaddr;/*��ǰ���ӷ��������û�ip*/
    char *clientMac = NULL;/*��ǰ���ӷ��������û�mac*/
    ARP_MATCH matchRet = MATCH_NONE;
    MacAddr macAddr;
    struct in_addr addrS;
    int tmp;
    const char *errMsg=NULL;

    memset(&macAddr, 0, sizeof(MacAddr));
    memset(&addrS, 0, sizeof(addrS));
    allowEnable = websGetVar(wp, T("AllowOtherEnable"), T(""));

    /*��֤���β����Ƿ�Ϸ�*/
    if(0 != strcmp(allowEnable, "on"))/*������ǰ��û�����*/
    {
	/*������ip��ַ�Ƿ���lan��ͬ����*/
#if(FEATURE_AC == FYES)
    tmp = 1;
#else
#if (MULTI_LAN == FYES)
	tmp = isIpInLanNet(clientIp);
#else
	tmp=isIpStrInIfNet(clientIp, getLanIfName());
#endif
#endif
	if(tmp == -1)
	{/*�ж�ʧ��*/
	    checkRet = 0;
	}
	else
	{
	    /*ֻ�з�����ip��ַ��lan�ڼ�arp�󶨵Ķ˿�������ͬ��������֤�ı�Ҫ*/
	    if(tmp == 1)	
	    {/*ͬ����*/
		checkRet = 0;/*���ھ�̬���в��ҵ���ǰ������arp��Ϣ��������֤ͨ��*/
		clientMac = getMacByIpFromSysArpTb(clientIp);/*��ϵͳarp���ȡ��Ӧ��mac��ַ*/
		if(clientMac != NULL)
		{
		    strRemoveChar(clientMac, ':');/*�Ƴ�����*/
		    if (0 != inet_aton(clientIp, &addrS))/*ת��Ϊ��������ʽ*/
		    {
			converMac12To6Byte(clientMac, &macAddr);
			matchRet = matchFromArpBinds(addrS.s_addr, macAddr, NULL);/*��arp��̬�󶨱��в��Ҹ���Ϣ*/
			if(matchRet == MATCH_NONE)
			{
			    /*ip��mac����ƥ��*/
			    errMsg = getTransDataForC(C_LANG_INDEX_ALLOW_BIND_USER);
			    setTheErrorMsgOnFree(errMsg);
			}
			else if(matchRet == MATCH_IP_ONLY)
			{
			    /*�ҵ�ipƥ�����mac��ַ��ƥ��*/
			    errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_MAC);
			    setTheErrorMsgOnFree(errMsg);
			}
			else if(matchRet == MATCH_MAC_ONLY)
			{
			    /*�ҵ�mac��ַƥ�����ip��ַ��ƥ��*/
			    errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_IP);
			    setTheErrorMsgOnFree(errMsg);
			}
			else
			{
			    checkRet = 1;/*ƥ��ip��mac���������β���������ǰ�������û����޷������豸*/
			}

		    }
		}
	    }
	}
    }

    return checkRet;
}
/***********************************************************************
 * �� �� ����   formConfigArpBindGlobalConfig
 * ����������   ����ȫ������
 * �������ڣ�	2010-3-17
 * �޸����ڣ�   2011-10-24 bhou
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
static void formConfigArpBindGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *allowEnable;
    InterfaceProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struProfAlloc *profhead = NULL;
    FunEnableEnum allowEnEnum = FUN_ENABLE;/*Ĭ��Ϊ�������������ӵ��豸*/

    allowEnable = websGetVar(wp, T("AllowOtherEnable"), T(""));

    if(0 != strcmp(allowEnable, "on"))/*������ǰ��û�����*/
    {
	allowEnEnum = FUN_DISABLE;/*������ǰ��û�����*/
    }

    /*�Ϸ���д���ò���Ч*/
    if(arpBindGlobalConfigCheck(wp) != 0)
    {
	prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 0);/*��ȡlan��*/

	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*��������*/
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);/*����*/
	    prof->arpAllowOtherEn = allowEnEnum;/*�Ƿ�����ǰ��û�����*/
	}

	/*ֻ�е��������������ò�����goahead�Żᷢ��Ϣ*/
	ProfUpdate(profhead);/*����Ϣ����*/
	ProfFreeAllocList(profhead);/*�ͷ�malloc���ڴ�*/
	nvramWriteCommit();/*дflash*/
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
}
/*
 * FunctionName:    sendArpRequest
 * Function:        send arp request packet 
 * argvs   :        ipaddr ɨ���ַ
 *                  netMask ɨ��ip����
 *                  ifName  ���Ͱ��Ķ˿�
 * Data:            2011-10-31
 * Author:          bhou
 * */
static int sendArpRequest(IPADDR ipaddr, IPADDR netMask, char*ifName)
{
    char *ipStr = NULL;
    struct in_addr addrS;
    IPADDR ipStart = 0u, ipEnd = 0u;
    int count = 0;

    ipaddr = ntohl(ipaddr);/*ת��Ϊ�����ֽ���*/
    netMask = ntohl(netMask);/*ת��Ϊ�����ֽ���*/
    ipStart = (ipaddr & netMask) + 1u;/*arpɨ����ʼ��ַ*/
    ipEnd = (ipaddr | (~netMask)) - 1u;/*arpɨ�������ַ*/

    doSystem("ip neigh flush dev %s", ifName);/*�����arp��*/
    while(ipStart <= ipEnd) 
    {
	addrS.s_addr = htonl(ipStart);/*ת��Ϊ�����ֽ���*/
	ipStr = inet_ntoa(addrS);/*ת��Ϊ�ַ�����ʽ*/
	if(ipStr != NULL)
	{
	    if (++count == 30) {
		sleep(1);
		count = 0;
	    }
	    doSystem("arping -q -I %s %s -c 1 &",ifName,ipStr);/*����arping���͹㲥��*/
	}
	ipStart++;
    }

    return 0;
}

/*
 * FunctionName:    readScanArp
 * Function:        ��ϵͳarp���ж�ȡarpɨ����Ϣ 
 * Data:            2011-11-01
 * Author:          bhou
 * */
static void readScanArp(int eid,webs_t wp,int argc,char_t **argv)
{
    int i = 0;
    FILE *fp;
    char *scanNetStr = NULL;
    char buf[96], ipStr[32], macStr[32], tmpMacStr[32];
    MacAddr macAddr;
    struct in_addr addrS;
    IPADDR arpScanNet = 0u;
    IPADDR netMask = convertLenToNetMask(24u);/*ת��Ϊ������ip��ַ��ʽ����*/

    memset(ipStr, 0, sizeof(ipStr));
    memset(macStr, 0, sizeof(macStr));
    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    memset(&tmpMacStr, 0, sizeof(tmpMacStr));

    scanNetStr = websGetVar(wp, T("scanNet"), T(""));/*ɨ������*/
    if(*scanNetStr != '\0')/*ɨ�����ת������*/
    {
	arpScanNet = strtoul(scanNetStr, NULL, 16);/*ת��Ϊ�޷�������*/
	fp=fopen("/proc/net/arp","r");/*��ȡϵͳarp��*/
	while(!feof(fp))
	{
	    memset(buf, 0, sizeof(buf));
	    fgets(buf, sizeof(buf), fp);/*��ȡһ��arp��Ŀ*/
	    if(strlen(buf) >= ARP_VALID_MIN_LEN)/*�Ϸ�arp��Ŀ*/
	    {
		/*��arp��Ŀ�н�������ip�Լ�mac��ַ*/
		if(getIpMacFromSysArpEntry(buf, ipStr, macStr, getLanIfName())!=0)
		{
		    strncpy(tmpMacStr, macStr, sizeof(tmpMacStr) - 1u);
		    strRemoveChar(tmpMacStr, ':');/*�Ƴ�mac��ַ�е����ӷ�*/
		    converMac12To6Byte(tmpMacStr, &macAddr);/*ת��Ϊ6�ֽ���ʽmac��ַ*/
		    if(inet_aton(ipStr, &addrS)!=0)/*ת��Ϊ������ip��ַ*/
		    {
			if((addrS.s_addr & netMask) == arpScanNet)/*��ɨ��ipͬ����*/
			{
			    /*�뾲̬����Ŀ����ͬ����Ŀ����ʾ��ҳ����*/
			    if(matchFromArpBinds(addrS.s_addr, macAddr, NULL) == MATCH_NONE)
			    {
				websWrite(wp,"i_flag[%d]=1;i_ips[%d]=\"%s   \";",i,i,ipStr);
				websWrite(wp,"i_macs[%d]=\"%s\";\n",i,macStr);
				i++;
			    }
			}
		    }
		}
	    }
	}
	websWrite(wp,"i_totalrecs = %d;\n\n",i);
	fclose(fp);/*�ر��ļ���*/
    }
    return ;
}

/*
 * FunctionName:    formReadArp
 * Function:        scan the mac addr
 * Data:            2011-10-31 
 * Author:          bhou
 * */
static void formReadArp(webs_t wp,char_t *path,char_t *query)
{
    char *ipStr;
    struct in_addr addrS;
    IPADDR netMask = 0u;
    char retUrl[64];
    const char *errMsg=NULL;

    memset(&addrS, 0, sizeof(addrS));
    memset(retUrl,0,sizeof(retUrl));

    ipStr=websGetVar(wp,T("ipAddr"), T(""));/*��ȡɨ��ip��ַ*/
    if(0 == inet_aton(ipStr, &addrS))/*ת��Ϊ��������ʽ*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	/*Ĭ�ϰ�����24ɨ��*/
	netMask = convertLenToNetMask(24u);/*ת��Ϊ������ip��ַ��ʽ����*/
	sendArpRequest(addrS.s_addr, netMask, getLanIfName());/*����arp��ѯ�㲥��*/
	sleep(3);/*�ȴ�arp�ظ���Ϣ*/
    }
#if (UTT_SMART_UI == FYES)
    sprintf(retUrl, "IPMacConfigSui.asp?scanNet=%08x", (netMask&addrS.s_addr));/*��ɨ�����μ�¼*/
#else 
    sprintf(retUrl, "IPMacConfig.asp?scanNet=%08x", (netMask&addrS.s_addr));/*��ɨ�����μ�¼*/
#endif   
    websRedirect(wp,retUrl );/*ҳ����ת*/

    return ;
}

/*
 * FunctionName:    aspOutReadArp
 * Function:        output the mac addr which is got by formReadArp
 * Data:            2011-06 
 * Author:          malei
 * */
static int aspOutReadArp(int eid,webs_t wp,int argc,char_t **argv)
{
    int min = 0, max = 0;
    char lan_addr[16];

    if (-1 == getIfIp(getLanIfName(), lan_addr)) /*��ȡlan��ip��ַ*/
    {
	lan_addr[0]='\0';
    }

    ProfInstLowHigh(MIB_PROF_USER, &max, &min);/*��ȡʵ����Χ*/
    websWrite(wp,"i_old_ip[0] = \"%s\";\n",lan_addr);
    websWrite(wp,"var totalrecs= %d;\n", ProfCountInstNotFree(MIB_PROF_USER));/*����ʵ������*/ 
    websWrite(wp,"var maxtotalrecs = %d;\n",max);/*���ʵ������*/
    readScanArp(eid,wp,argc,argv);
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}
/**
 * ��ҳ��textarea��һ���н������� ip��ַ��mac��ַ���Լ��û���
 * bhou
 * 2011-11-03
 * ����˵����rowStr��ֵ�ᱻ�ı�
 */
static bool parseArpImuFromTaRow(char* rowStr, char** ipStr, char** macStr, char** userName)
{
    bool parseRet = 0;/*�������Ĭ��Ϊʧ��*/
    char* split =" ";/*ҳ��һ�������и�����Ԫ�ķָ��*/
    char* ptr = NULL;

    if(rowStr != NULL)
    {
	if(strlen(rowStr)>= ARP_VALID_MIN_LEN) /*�Ϸ�arp��Ŀ*/
	{
	    *ipStr  = strtok_r(rowStr, split, &ptr);/*ip�ִ�*/
	    if(*ipStr != NULL)
	    {

		*macStr  = strtok_r(NULL, split, &ptr);/*mac�ִ�*/
		if(*macStr != NULL)
		{
		    *userName  = strtok_r(NULL, split, &ptr);/*�û���*/
		    if((*userName != NULL) && (**userName == '\0'))
		    {/*�Ƿ��û���*/
			*userName = NULL;
		    }
		    parseRet = 1;/*�û�������Ϊ�ա��ʵ��˿���Ϊ�����ɹ�*/
		}

	    }
	}
    }
    return parseRet;
}

/*
 * FunctionName:    formAddReadArp
 * Function:        һ�����Ӷ���ip/mac��̬�� 
 * Data:            2011-11-03 
 * Author:          bhou 
 * */
static void formAddReadArp(webs_t wp,char_t *path,char_t *query)
{
    char *textarea = NULL, *ptr = NULL, *rowStr = NULL;
    char tmpStr[64];
    char *ipStr = NULL, *macStr = NULL, *userName = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
#if (DHCP_POOL ==FYES)
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profDhcp = NULL;
    int max,min;
    int index;
#endif 
    int instIndex = 0;
    MacAddr macAddr;
    struct in_addr addrS;
    struProfAlloc *profhead = NULL;
    char* delim = "\n";/*textarea�����ݷָ���*/
    int bindNum = 0;/*����Ҫ�󶨶��ٸ���Ŀ*/
    int successNum = 0;/*�ɹ��󶨵���Ŀ��*/
    int back = 0;
    int maxDeNameNo = 0 , aDNametag = 0;
    char* userIp = wp->ipaddr;
    const char *errMsg=NULL;
    const char *total=NULL;
    const char *totalNum=NULL;
    const char *failNum=NULL;

    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    textarea=websGetVar(wp,T("data"), T(""));/*ip/mac��������Ϣ*/

    strRemoveChar(textarea, '\r');/*�Ƴ�/r*/
    mergeMulSpaEnter(textarea);/*ȥ������Ŀո��Լ�����*/
    bindNum = strCountChar(textarea, delim[0]) + 1;/*ͨ����������ȷ���û����β��������arp������*/

    rowStr=strtok_r(textarea,delim,&ptr);/*ȡ��һ��arp������*/
    /*ѭ����ȡtextarea���ݲ����а�*/
    while(NULL!=rowStr)
    {
	/*��һ����Ϣ�н�������ip,mac�Լ��û���*/
	if(parseArpImuFromTaRow(rowStr, &ipStr, &macStr, &userName) != 0)
	{
	    strRemoveChar(macStr, ':');
	    if (0 != inet_aton(ipStr, &addrS))/*ת��Ϊ��������ʽ*/
	    {
#if (DHCP_POOL == FYES)
		ProfInstLowHigh(mibTypeDhcp, &max, &min); /* ���ڴ���Ѱ�ҵ�ַ����Ϣ */
		for (index = min; index < max; index++) {
		    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
		    if ((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)){

			if (!strcmp(profDhcp->head.name,"default")){
			    if ((ntohl(addrS.s_addr)&ntohl(profDhcp->netmask)) ^ (ntohl(profDhcp->ipStart)&ntohl(profDhcp->netmask))){
				continue;
			    }else{
				break;
			    }
			}
			if ((ntohl(addrS.s_addr) >= ntohl(profDhcp->ipStart)) && (ntohl(addrS.s_addr) <= ntohl(profDhcp->ipEnd))){
			    if ( profDhcp->dhcpEn == PROF_ENABLE){
				break;
			    }else{
				if (bindNum == 1){
				    errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP1);

				    setTheErrorMsgOnFree(errMsg);
				    goto out;
				}
			    }
			}
		    }
		}
		if ( index >= max ){
		    if ( bindNum == 1){
			errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP2);
			setTheErrorMsgOnFree(errMsg);
			goto out;
		    }else{
			continue;
		    }
		}
#endif

		converMac12To6Byte(macStr, &macAddr);/*ת��Ϊ6�ֽ�����*/
		instIndex = ProfGetInstIndexFree(mibType);/*��ȡһ������ʵ��*/
		if(instIndex == PROFFAIL) /*��ȡ������֤���Ѵ�������Ŀ*/
		{
		    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		    setTheErrorMsgOnFree(errMsg);
		    break;
		}
		else
		{/*��ȡ�ɹ�*/
		    if(arpBindAddAccessCheck(userIp, ipStr, macStr)!=0)/*���ΰ��Ƿ����ʹ��ǰ��½�ߣ��޷���¼*/
		    {
			userIp = NULL;/*�´���֤�����ض�ϵͳarp��*/
			aDNametag = 0;/*�����Ƿ�ʹ��Ĭ���û������*/
			if(userName == NULL)/*�û���û����*/
			{
			    if(maxDeNameNo == 0)
			    {
				maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, ARPPD_NAME_H);/*��ȡ���Ĭ���û������*/
			    }
			    userName = ProfBuildDefaultName(ARPPD_NAME_H, maxDeNameNo);/*����Ĭ���û���*/
			    aDNametag = 1;/*��־����һ��Ĭ���û���*/
			}
			if(arpBindUniCheck(userName, addrS.s_addr, macAddr, PROF_UNUSED_INDEX)!=0)/*��̬���ظ����ж�*/
			{
#if (DHCP_POOL == FYES)
			    ProfInstLowHigh(mibTypeDhcp, &max, &min); /* ���ڴ���Ѱ�ҵ�ַ����Ϣ */
			    for (index = min; index < max; index++) {
				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
				if ((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
				    if (!strcmp(profDhcp->head.name,"default")){
					if ((ntohl(addrS.s_addr)&ntohl(profDhcp->netmask)) == (ntohl(profDhcp->ipStart)&ntohl(profDhcp->netmask))){
					    break;
					}
				    }

				    if ((ntohl(addrS.s_addr) >= ntohl(profDhcp->ipStart)) && (ntohl(addrS.s_addr) <= ntohl(profDhcp->ipEnd))){
					break;  /*�ҵ���ַ�أ��˳�ѭ��*/
				    }
				}
			    }
			    if( index<max ){
#if 0
				if( profDhcp->ipStart>addrS.s_addr ||  profDhcp->ipEnd < addrS.s_addr){
				    setTheErrorMsgOnFree(T("IP����DHCP��ַ���ڣ�"));
				    break;
				}
#endif
#endif
				if(bindNum < 6)
				{/*���ܰ���Ŀ������̫����ÿ����Ӷ����ݲ�����Ϣ*/
				    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*�����´����Ŀ�ʵ��*/
				}
				else
				{/*���ΰ���Ŀ��������ֻ����һ�Ρ�ֻ��һ����Ϣ*/
				    if(back == 0 )
				    {/*��������Ϊ ��Ӷ�������speedweb�����³�ʼ��arp���*/
					ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
					back = 1;
				    }
				}
				successNum++;/*�����ɹ��Ĵ���*/
				prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡʵ��ָ��*/
				ProfSetNameByPoint(prof, userName);/*�����û���*/
				prof->bindEn = FUN_ENABLE;/*Ĭ��Ϊ�������*/
				prof->ip = addrS.s_addr;/*��̬��ip��ַ*/
				prof->mac = macAddr;/*��̬��mac��ַ*/
				maxDeNameNo+=aDNametag;/*��Ĭ���û������ɣ�����ʹ���ˣ������Ĭ���û�����ż�һ*/
#if (DHCP_POOL ==FYES)
				strcpy(prof->dhcpPoolName, profDhcp->head.name);
				prof->vid = profDhcp->vid;
			    } else {
				if(bindNum < 6)
				{/*���ܰ���Ŀ������̫����ÿ����Ӷ����ݲ�����Ϣ*/
				    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*�����´����Ŀ�ʵ��*/
				}
				else
				{/*���ΰ���Ŀ��������ֻ����һ�Ρ�ֻ��һ����Ϣ*/
				    if(back == 0 )
				    {/*��������Ϊ ��Ӷ�������speedweb�����³�ʼ��arp���*/
					ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
					back = 1;
				    }
				}
				successNum++;/*�����ɹ��Ĵ���*/
				prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡʵ��ָ��*/
				ProfSetNameByPoint(prof, userName);/*�����û���*/
				prof->bindEn = FUN_ENABLE;/*Ĭ��Ϊ�������*/
				prof->ip = addrS.s_addr;/*��̬��ip��ַ*/
				prof->mac = macAddr;/*��̬��mac��ַ*/
				maxDeNameNo+=aDNametag;/*��Ĭ���û������ɣ�����ʹ���ˣ������Ĭ���û�����ż�һ*/
			    }
#endif
			}
		    }

		}

	    }
	    else
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
		setTheErrorMsgOnFree(errMsg);
	    }

	}
	rowStr=strtok_r(NULL, delim, &ptr);/*��ȡ��һ������*/
    }    

    ProfUpdate(profhead);/*����Ϣ��Ч*/
    ProfFreeAllocList(profhead);/*�ͷű��ݿؼ�*/
    nvramWriteCommit();/*дflash*/

    if(bindNum > 1)/*���а����޷�ȫ����ʾ���ʲ���������ʾ��ʽ*/
    {
	if(bindNum != successNum)
	{
	    total = getTransDataForC(C_LANG_INDEX_TOTAL_BIND);
	    totalNum = getTransDataForC(C_LANG_INDEX_TOTAL_BIND_NUM);
	    failNum = getTransDataForC(C_LANG_INDEX_FAIL_BIND_NUM);
	    /*���ΰ���ʧ�ܵ���Ŀ*/
	    sprintf(tmpStr, "��%s%d%s%d%s", total, bindNum, totalNum, (bindNum - successNum), failNum);
	    setTheErrorMsgCat(tmpStr);
	}
    }
out:
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
    return ;
}
/*
 * arp��̬�󶨲�����ɾ����Ŀʱ���ж�ɾ������Ŀ�Ƿ���ܵ��£���ǰ��¼�ߣ��޷����ʸ��豸
 * bhou
 * clientIp:��ǰ��¼��ip��ַ delIp:Ҫɾ����ip��ַ
 * 2011-10-29
 */
static bool arpBindDelAccessCheck(char* clientIp, IPADDR delIp)
{
    bool ret = 1;
    int tmp = 0;
    const char *errMsg=NULL;

    struct in_addr addrS;
    memset(&addrS, 0, sizeof(addrS));

    /*ֻ���ڲ�����ǰ��û����ӵ�����¡��Ų��������ɾ������*/
    if(getArpAllowOtherEn()==0)
    {
	if(inet_aton(clientIp, &addrS)!=0)/*ת��ip��ַ*/
	{
	    if(delIp == addrS.s_addr)/*��֮ǰ������ip��ַ��ͬ��mac����ͬ���ʲ����ж�mac��ַ�Ƿ���ͬ*/
	    {
#if(FEATURE_AC == FYES)
        tmp = 1;
#else
#if (MULTI_LAN == FYES)
		tmp = isIpInLanNet(clientIp);
#else
		tmp=isIpInIfNet(addrS.s_addr, getLanIfName());/*�Ƿ���lan��ipͬ����*/
#endif
#endif
		if(tmp == 0)
		{/*��ͬ���Ρ���������ɾ��*/
		    ret = 1;
		}
		else  
		{
		    ret = 0;
		    errMsg = getTransDataForC(C_LANG_INDEX_ALLOW_BIND_USER);
		    setTheErrorMsgOnFree(errMsg);
		}
	    }
	}
    }

    return ret;/*�ɹ��򷵻���ɾ��������*/

}

/**
 * arp��̬�󶨡�������Ŀʱ���жϸò����Ƿ���ܵ��µ�ǰ��¼�ߡ��޷����ʸ��豸
 * bhou
 * userIp:��ǰ��¼��ip��ַ newIp:����ӵ�ip��ַ newMac:����ӵ�mac��ַ
 * ��userIpΪNULLʱ����ʾ������֤���ϴ���֤��ȡ����绷���ޱ仯��������arp��Ϣ�ޱ仯��lan��ip�����ޱ仯��
 * 2011-10-29
 */
static bool arpBindAddAccessCheck(char*userIp,char* newIp, char*newMac)
{
    static char *clientMac = NULL;/*��ǰ���ӷ��������û�mac*/
    static char clientIp[32];
    static int cmpLanIpNet = 0;
    int ipCmpFlag = 0,macCmpFlag = 0;
    bool ret = 0;
    const char *errMsg=NULL;

    if(userIp != NULL)
    {
	memset(clientIp, 0, sizeof(clientIp));
	strncpy(clientIp, userIp, sizeof(clientIp) - 1u);
#if(FEATURE_AC == FYES)
    cmpLanIpNet = 1;
#else
#if (MULTI_LAN == FYES)
	cmpLanIpNet = isIpInLanNet(clientIp);
#else
	cmpLanIpNet = isIpStrInIfNet(clientIp, getLanIfName());/*�Ƿ���lan��ipͬ����*/
#endif
#endif
	if(cmpLanIpNet == 1)/*ͬ��������֤*/
	{
	    clientMac = getMacByIpFromSysArpTb(clientIp);/*��ϵͳarp���ȡ��Ӧ��mac��ַ*/
	    if(clientMac != NULL)
	    {
		strRemoveChar(clientMac, ':');/*�Ƴ�����*/
	    }
	}
    }
    if(cmpLanIpNet == 0)
    {/*��ͬ���Ρ�������*/
	ret = 1;
    }
    else
    {
	if((cmpLanIpNet == 1) && (clientMac!=NULL))
	{/*ͬ������arp��Ϣ��ȡ��ȷ*/
	    ipCmpFlag = strcmp(clientIp, newIp);
	    macCmpFlag = strncasecmp(clientMac, newMac ,UTT_MAC_STR_LEN);

	    if((ipCmpFlag == 0) && (macCmpFlag != 0))
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_MAC);
		setTheErrorMsgOnFree(errMsg);
	    }
	    else if((ipCmpFlag != 0) && (macCmpFlag == 0))
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_IP);
		setTheErrorMsgOnFree(errMsg);
	    }
	    else
	    {
		ret = 1;
	    }
	}
    }

    return ret;

}
/**
 * arp��̬��༭���ʱ���ж� userName, ip, mac�Ƿ���������Ŀ��ͻ
 * editIndex:��ǰ�༭��Ŀ����������Ϊ���ӿ�ΪPROF_UNUSED_INDEX
 * userName,ip,mac ��ǰ������������Ϊ��ֵ
 * bhou
 * 2011-10-29
 */
static bool arpBindUniCheck(char*userName, IPADDR ip, MacAddr mac, int editIndex)
{
    bool ret = 0;
    ARP_MATCH matchRet = MATCH_NONE;
    int instIndex = 0;
    const char *errMsg=NULL;

    instIndex = ProfGetInstIndexByName(MIB_PROF_USER, userName);

    if((instIndex != PROFFAIL) && (instIndex != editIndex) )
    {
	errMsg = getTransDataForC(C_LANG_INDEX_USER_INPUT_REPEAT);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	/*ÿ��ip��mac��ַ��ֻ�ܰ�һ��*/
	matchRet = matchFromArpBinds(ip, mac, &editIndex);/*ƥ��arp��̬�󶨱�*/
	if(matchRet == MATCH_IP_ONLY)/*ip��ַƥ�䡣mac��ַ��ƥ��*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else if(matchRet == MATCH_MAC_ONLY)/*ip��ַ��ƥ�䡣mac��ַƥ��*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAC_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else if(matchRet == MATCH_ALL)/*ip��ַƥ�䡣mac��ַƥ��*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_IPMAC_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else
	{
	    /*����ƥ��,���������ظ���*/
	    ret = 1;
	}
    }
    return ret;
}
/**
 * arp��̬����Ӳ���
 * bhou
 * 2011-10-29
 */
static void arpBindConfigAdd(webs_t wp)
{
    char *userName,*ipStr,*macStr, *dhcpPoolName;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int instIndex = 0;
    MacAddr macAddr;
    struct in_addr addrS;
    struProfAlloc *profhead = NULL;
    const char *errMsg=NULL;
#if (DHCP_POOL == FYES)
    DhcpPoolProfile *profDhcp = NULL;
    int min, max, index;
#endif
    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));

    /*��ҳ���ȡ���ؼ���ֵ*/

    dhcpPoolName = websGetVar(wp, T("pools"), T(""));
#if (DHCP_POOL == FYES)
    char *dhcpVid;
    dhcpVid = websGetVar(wp, T("dhcpVid"), T(""));
#endif
    userName=websGetVar(wp,T("UserName"), T(""));
    ipStr=websGetVar(wp,T("IP"), T(""));
    macStr=websGetVar(wp,T("Mac"), T(""));

    if (0 == inet_aton(ipStr, &addrS))/*ת��Ϊ��������ʽ*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	converMac12To6Byte(macStr, &macAddr);/*ת��Ϊ6�ֽ�����*/
	if(arpBindAddAccessCheck(wp->ipaddr, ipStr, macStr)!=0)/*���ΰ��Ƿ����ʹ��ǰ��½�ߣ��޷���¼*/
	{
	    if(arpBindUniCheck(userName, addrS.s_addr, macAddr, PROF_UNUSED_INDEX)!=0)/*��̬���ظ����ж�*/
	    {

#if (DHCP_POOL == FYES)
		ProfInstLowHigh(MIB_PROF_DHCP_POOL, &max, &min);
		for (index = min; index < max; index++) {
		    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(MIB_PROF_DHCP_POOL, index);
		    if((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp))
			    && (TRUE == profDhcp->head.active) && profDhcp->dhcpEn  ) {
			if( !strcmp(dhcpPoolName,profDhcp->head.name) ){
#if 0
			    printf("index: %d\n",index);
			    printf("profDhcp->interface: %s\n",profDhcp->interface);
			    printf("%s : %d\n",__FILE__,__LINE__);
#endif
			    break;
			}
		    }
		}
		if ( index >= max ){
		    errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP3);

		    setTheErrorMsg(errMsg);
		    return;
		}
#endif
		instIndex = ProfNewInst(mibType, userName, FALSE);/*�½�ʵ��*/

		if(instIndex == PROFFAIL) /*�������ظ����жϡ�������ֻ����ʱ�ﵽ�����Ŀ*/
		{
		    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		    setTheErrorMsgOnFree(errMsg);
		}
		else
		{
		    prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡʵ��ָ��*/

		    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*�����´����Ŀ�ʵ��*/
		    strcpy(prof->dhcpPoolName, dhcpPoolName);
		    prof->bindEn = FUN_ENABLE;/*Ĭ��Ϊ�������*/
#if (DHCP_POOL == FYES)
		    prof->vid = strtol(dhcpVid, NULL, 10);/*��̬��vid*/
#endif
		    prof->ip = addrS.s_addr;/*��̬��ip��ַ*/
		    prof->mac = macAddr;/*��̬��mac��ַ*/
		    ProfUpdate(profhead);/*����Ϣ��Ч*/
		    ProfFreeAllocList(profhead);/*�ͷű��ݿؼ�*/
		    nvramWriteCommit();/*дflash*/
		}
	    }
	}
    }
    return;
}
/**
 * arp��̬���޸Ĳ���
 * bhou
 * 2011-10-29
 */
static void arpBindConfigModify(webs_t wp)
{
    char *userName,*ipStr,*macStr,*userNameOld, *dhcpPoolName;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int instIndex = 0;
    bool errorTag = 0;
    MacAddr macAddr;
    struct in_addr addrS;
    struProfAlloc *profhead = NULL;
    const char *errMsg=NULL;

#if (DHCP_POOL ==FYES)
    MibProfileType mibTypeDhcp = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *profDhcp = NULL;
    int max,min;
    int index;
#endif 

    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));

    /*��ȡҳ��ؼ�ֵ*/
    dhcpPoolName = websGetVar(wp, T("pools"), T(""));
#if (DHCP_POOL == FYES)
    char *dhcpVid;
    dhcpVid = websGetVar(wp, T("dhcpVid"), T(""));
#endif
    userName=websGetVar(wp,T("UserName"), T(""));/*���û���*/
    userNameOld=websGetVar(wp,T("UserNameold"), T(""));/*���û���*/
    ipStr=websGetVar(wp,T("IP"), T(""));
    macStr=websGetVar(wp,T("Mac"), T(""));

    if (0 == inet_aton(ipStr, &addrS))/*ת��Ϊ��������ʽ*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	converMac12To6Byte(macStr, &macAddr);/*ת��Ϊ6�ֽ�����*/
	instIndex = ProfGetInstIndexByName(mibType, userNameOld);/*���Ҹ�����*/

	if(instIndex == PROFFAIL) 
	{/*���޸ĵ����ò�����*/
	    errMsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	    setTheErrorMsgOnFree(errMsg);
	}
	else 
	{/*����Ҫ�޸ĵ�����*/
	    if(arpBindUniCheck(userName, addrS.s_addr, macAddr, instIndex)!=0)/*��̬���ظ��Լ��*/
	    {


#if (DHCP_POOL == FYES)
		ProfInstLowHigh(MIB_PROF_DHCP_POOL, &max, &min);
		for (index = min; index < max; index++) {
		    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(MIB_PROF_DHCP_POOL, index);
		    if((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
			if ( '\0' == *dhcpPoolName ){

			    if (!strcmp(profDhcp->head.name,"default")){
				if ((ntohl(addrS.s_addr)&ntohl(profDhcp->netmask)) ^ (ntohl(profDhcp->ipStart)&ntohl(profDhcp->netmask))){
				    continue;
				}else{
				    break;
				}
			    }

			    if ((ntohl(addrS.s_addr) >= ntohl(profDhcp->ipStart)) && (ntohl(addrS.s_addr) <= ntohl(profDhcp->ipEnd))){
				if ( profDhcp->dhcpEn == PROF_ENABLE){
				    break;
				}else{
				    errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP4);
				    setTheErrorMsgOnFree(errMsg);
				    return;
				}
			    }

			}else if( !strcmp(dhcpPoolName,profDhcp->head.name) ){
			    //                            printf("index: %d\n",index);
			    //                            printf("profDhcp->interface: %s\n",profDhcp->interface);
			    //                            printf("%s : %d\n",__FILE__,__LINE__);
			    if ( profDhcp->dhcpEn == PROF_ENABLE){
				break;
			    }
			}
		    }
		}
		if ( index >= max ){
		    if ( '\0' == *dhcpPoolName ){
			errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP2);
			setTheErrorMsg(errMsg);
		    }else{
			errMsg = getTransDataForC(C_LANG_INDEX_ARPBIND_TIP3);

			setTheErrorMsg(errMsg);
		    }
		    return;
		}
#endif

		errorTag = 0;
		prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡҪ�޸ĵ�ʵ��ָ��*/
		if((prof->ip != addrS.s_addr) || (macAddrCmp(&prof->mac, &macAddr))!=0)/*�����޸��漰��ip��mac*/
		{
		    /*��ip��mac���޸ġ�����ܵ��µ�ǰ��½��arp��Ϣ��ɾ��*/
		    if(arpBindDelAccessCheck(wp->ipaddr, prof->ip)==0)/*��ǰ��¼���Ƿ��ܵ�¼*/
		    {
			errorTag = 1;/*�谭�˵�ǰ��½�ߵ�¼*/
		    }
		    else
		    {
			/*�ж��޸ĺ�İ���Ϣ�Ƿ���ܵ��µ�ǰ��¼���޷���¼*/
			if(arpBindAddAccessCheck(wp->ipaddr, ipStr, macStr)==0)
			{
			    errorTag = 1;/*�谭�˵�ǰ��½�ߵ�¼*/
			}
		    }
		}

		if(errorTag == 0)/*�޴�*/
		{
		    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);/*�����޸�֮ǰ������*/
		    ProfSetNameByPoint(prof, userName);/*�����û���*/


		    if( dhcpPoolName == "" ){                 /* ����IP/MAC��ҳ���޸�ʱ����ַ���������ڴ�����Ϣ������  */
#if (DHCP_POOL == FYES)
			ProfInstLowHigh(mibTypeDhcp, &max, &min);     
			for (index = min; index < max; index++) {
			    profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
			    if ((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
				if ((ntohl(addrS.s_addr) >= ntohl(profDhcp->ipStart)) && (ntohl(addrS.s_addr) <= ntohl(profDhcp->ipEnd))){
				    break;
				}
			    }
			}
			if(index<max){
			    strcpy(prof->dhcpPoolName, profDhcp->head.name);
			    prof->vid = profDhcp->vid;/*��̬��vid*/
			}
#endif
		    }else{  /* ��̬DHCP�б����޸� */
			strcpy(prof->dhcpPoolName, dhcpPoolName);
#if (DHCP_POOL == FYES)
			prof->vid = strtol(dhcpVid, NULL, 10);/*��̬��vid*/
#endif
		    }
		    prof->ip = addrS.s_addr;/*ip��ַ*/
		    prof->mac = macAddr;/*mac��ַ*/
		    ProfUpdate(profhead);/*����Ϣ����*/
		    ProfFreeAllocList(profhead);/*�ͷű��ݿռ�*/
		    nvramWriteCommit();/*дflash*/

		}
	    }
	}

    }
    return;
}
/***********************************************************************
 * �� �� ����   webConfigArpBindConfig
 * ����������   ��ӻ��޸�ARP������
 * �������ڣ�	2010-10-29
 * �޸����ڣ�

 * ���ߣ�       bhou
 ***********************************************************************/
extern void webConfigArpBindConfig(webs_t wp)
{

    char* action = websGetVar(wp, T("Action"), T("")); 

    if(strncmp(action, "add", 3) == 0) 
    {/*���*/
	arpBindConfigAdd(wp);
    } 
    else
    {/*�޸�*/
	arpBindConfigModify(wp);
    }

    return;
}
/***********************************************************************
 * �� �� ����   formConfigArpBindConfig
 * ����������   ��ӻ��޸�ARP������
 * �������ڣ�	2010-10-29
 * �޸����ڣ�

 * ���ߣ�       bhou
 ***********************************************************************/
static void formConfigArpBindConfig(webs_t wp, char_t *path, char_t *query)
{
    webConfigArpBindConfig(wp);

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
    return;
}
/***********************************************************************
 * �� �� ����   formConfigArpBindAllow
 * ����������   ������ֹARP���û�����
 * �������ڣ�	2011-10-29
 * �޸����ڣ�
 * ���ߣ�       bhou
 * ����˵����	��
 ***********************************************************************/
static void formConfigArpBindAllow(webs_t wp, char_t *path, char_t *query)
{
    char_t *UserName, *allow;
    struct in_addr addrS;
    struProfAlloc *profhead = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int instIndex = 0, tmp = 0;
    bool errorFlag = 0;
    FunEnableEnum allowEn = FUN_ENABLE;
    const char *errMsg=NULL;

    memset(&addrS, 0, sizeof(addrS));

    UserName=websGetVar(wp,T("AllowID"), T(""));/*�û���*/
    allow=websGetVar(wp,T("Allow"), T(""));/*����*/

    instIndex = ProfGetInstIndexByName(mibType, UserName);
    if(instIndex == PROFFAIL) 
    {/*���޸ĵ����ò�����*/
	errMsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {/*Ҫ���������ô���*/
        prof = ProfGetInstPointByIndex(mibType, instIndex);/*��ȡ������*/
        if (0 == strcmp("no",allow))/*��ֹ�ð��û�*/
        {
            allowEn = FUN_DISABLE;
            if(inet_aton(wp->ipaddr, &addrS)!=0)/*ת��ip��ַ*/
            {
                if(prof->ip == addrS.s_addr)/*����ֹ����Ŀ�뵱ǰ��¼��arp��Ϣ��ͬ*/
                {
                    /*������ip��ַ�Ƿ���lan��ͬ����*/
#if(FEATURE_AC == FYES)
                    tmp = 1;
#else
#if (MULTI_LAN == FYES)
		    tmp = isIpInLanNet(wp->ipaddr);
#else
		    tmp=isIpInIfNet(addrS.s_addr, getLanIfName());
#endif
#endif
                    if(tmp == 0)
                    {/*��ͬ�����������ֹ*/
                        errorFlag = 0;
                    }
                    else
                    {
                        errorFlag = 1;
                        errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
                        setTheErrorMsgOnFree(errMsg);
                    }
                }
            }
        }
        if(errorFlag == 0)/*�����Ϸ�*/
        {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);/*�����޸�֮ǰ������*/
            prof->bindEn = allowEn ;/*������ֹ*/
            ProfUpdate(profhead);/*����Ϣ����*/
            ProfFreeAllocList(profhead);/*�ͷű��ݿռ�*/
            nvramWriteCommit();/*дflash*/
        }
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
}
/**
 * ҳ��arp��ɾ������
 * bhou
 * 2011-11-16
 */
extern void webArpBindDel(webs_t wp)
{
    char *UserName = NULL, *tmp = NULL, *ptr = NULL;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile* prof;
    struProfAlloc *profList  = NULL;
    struct in_addr addrS;
    int tmpInt = 0;
    bool allowOtherEn = getArpAllowOtherEn();/*�Ƿ�����ǰ��û�����*/
    const char *errMsg=NULL;

    memset(&addrS, 0, sizeof(addrS));

    inet_aton(wp->ipaddr, &addrS);/*ת����ǰ��¼��ip��ַΪ������*/

    UserName = websGetVar(wp,T("delstr"),T("")); /*Ҫɾ�����û����б�*/
    if(!UserName)
    {   
	errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);/*����ɾ��������¼*/
	while(tmp!=NULL)
	{
	    prof = ProfGetInstPointByName(mibType, tmp);/*��ȡ���õ�ַ*/
	    if(prof != NULL) /*��������*/
	    {
		/*��������ǰ��û����ӡ���ǰ������arp��Ϣ����ɾ��*/
		if((allowOtherEn ==0) && (prof->ip == addrS.s_addr))
		{
#if(FEATURE_AC == FYES)
		    tmpInt = 1;
#else
#if (MULTI_LAN == FYES)
		    tmpInt = isIpInLanNet(wp->ipaddr);
#else
		    tmpInt = isIpInIfNet(addrS.s_addr, getLanIfName());/*�ж��Ƿ��lan��ͬ����*/
#endif
#endif
		    /*��ʱֻ��tmpIntΪ0.����lan�ڲ�ͬ���β���ɾ��*/
		}
		if(tmpInt == 0)
		{
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
		    ProfDelInstByName(mibType, tmp);/*ɾ������ʵ��*/
		}
		else
		{
		    errMsg = getTransDataForC( C_LANG_INDEX_ALLOW_BIND_USER);
		    setTheErrorMsgOnFree(errMsg);
		    tmpInt = 0;
		}

	    }
	    tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);/*������һ���û���*/
	}
	ProfUpdate(profList);/*����Ϣɾ��*/
	ProfFreeAllocList(profList);/*�ͷű��ݿռ�*/
	nvramWriteCommit();/*дflash*/
    }
    return;
}
/***********************************************************************
 * �� �� ����   formArpBindDel
 * ����������	ɾ��һ���������¼
 * �������ڣ�	2011-10-31
 * �޸����ڣ�
 * ���ߣ�       bhou
 ***********************************************************************/
static void formArpBindDel(webs_t wp, char_t *path, char_t *query)
{
    webArpBindDel(wp);
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
}
/***********************************************************************
 * �� �� ����   webArpBindDelAll
 * ����������	ɾ�����м�¼.������ɾ������ǰ������arp��ͬ�ļ�¼
 * �������ڣ�	2011-10-31
 * �޸����ڣ�
 * ���ߣ�       bhou
 * ����˵����	��
 ***********************************************************************/
extern void webArpBindDelAll(webs_t wp)
{
    struct in_addr addrS;
    int min = 0, max = 0;
    int i = 0, bakIndex = 0;
    int back = 0, tmpInt = 0;
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile* prof, profBak;
    struProfAlloc *profList  = NULL;

    memset(&addrS, 0, sizeof(addrS));
    memset(&profBak, 0, sizeof(profBak));

    inet_aton(wp->ipaddr, &addrS);/*ת��������ip��ַΪ������*/

    if(getArpAllowOtherEn()==0)/*������ǰ��û�����*/
    {
#if(FEATURE_AC == FYES)
    tmpInt = 1;
#else
#if (MULTI_LAN == FYES)
	tmpInt = isIpInLanNet(wp->ipaddr);
#else
	tmpInt = isIpInIfNet(addrS.s_addr, getLanIfName());/*�ж��Ƿ��lan��ͬ����*/
#endif
#endif
	/*ֻ�в�ͬ����������ɾ��*/
    }
    ProfInstLowHigh(mibType, &max, &min);/*��ȡʵ����Χ*/

    /*����ɾ��*/
    for(i = min; i < max; i++) 
    {
	prof = ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*��������*/
	{
	    if(tmpInt!=0)/*�ж�ʧ�ܻ�ͬ����*/
	    {
		if(prof->ip == addrS.s_addr)/*��arp��Ϣ����ǰ��¼��arp��ͬ*/
		{
		    /*���ﲻ��ֱ��PorfBackupһ��PROF_CHANGE_ADD
		     *�򱸷���������Ϣ��˳��ȷ���ҿ��ܸ���*/
		    bakIndex = i;/*��¼����*/
		    ProfGetInstByIndex(mibType, i, &profBak);/*��ȡʵ���Ŀ�����*/
#if 0
		    /*��ֻ��һ��ʵ��ʱ�˷����в�ͨ*/
		    ProfDelInstByIndex(mibType, i);/*ɾ������ʵ��*/
		    continue;/*ɾ������,���ܱ��ݸ�ʵ������������ʵ�����������ɾ�����ݵ�������ͬ������ܲ�������Ϣ*/
#endif
		}
	    }
#if (DHCP_SERVICE == FYES)
	    if(back == 0) 
	    {
		/*ֻ����һ�Σ���ֹ��Ϣ�෢���ϵͳ����*/
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
#else
	    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
#endif
	    ProfDelInstByIndex(mibType, i);/*ɾ������ʵ��*/
	}
    }
    /*����Ϣɾ�����а�*/
    ProfUpdate(profList);
    ProfFreeAllocList(profList);/*�ͷű��ݿռ�*/

    if(tmpInt != 0)
    {
	sleep(2);/*�ȴ�ɾ�����еĲ��������ȴ�����Ϣ���ƿ����ȴ������Ϣ��ɾ�����У�*/
	profList = NULL;
	/*���ﲻ�½�ʵ������Ϊ�˷�ֹ�û�ȫ������Ĭ���û���ʱ����ͻ�����Ե����*/
	ProfBackupByIndex(mibType, PROF_CHANGE_ADD, bakIndex, &profList);/*�����´����Ŀ�ʵ��*/
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, bakIndex);/*��ȡʵ��ָ��*/
	if(prof != NULL)
	{
	    *prof = profBak;/*ֱ�Ӹ�ֵ*/
	}

	ProfUpdate(profList);/*����Ϣ���ʵ��*/
	ProfFreeAllocList(profList);/*�ͷű��ݿռ�*/
    }
    nvramWriteCommit();/*дflash*/
    return;
}

/***********************************************************************
 * �� �� ����   formArpBindDelAll
 * ����������	ɾ�����м�¼.������ɾ������ǰ������arp��ͬ�ļ�¼
 * �������ڣ�	2011-10-31
 * �޸����ڣ�
 * ���ߣ�       bhou
 * ����˵����	��
 ***********************************************************************/
static void formArpBindDelAll(webs_t wp, char_t *path, char_t *query)
{
    webArpBindDelAll(wp);
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "IPMac_Sui.asp");
#else
    websRedirect(wp, "IPMac.asp");
#endif
    return;
}

/* * ReadIPMacFromProfile()
 * 2013-2-1
 *��ȡIP/MAC����Ϣ��buf��
 * zheng.xiao
 */
static Boolean ReadIPMacFromProfile(char *buf)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    struct in_addr addrIp;
    int min = 0, max = 0, i = 0;
    char *name=NULL,*ip=NULL,*mac=NULL;

    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);
	if(prof != NULL && ProfInstIsFree(prof) == 0) {
	    name = prof->head.name;
	    addrIp.s_addr = prof->ip;
	    ip = inet_ntoa(addrIp);			/*convert ip addr*/
	    mac = converMac6To17Byte(prof->mac);	/*convert mac addr*/
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen( ip ) ) )
	    {
		strcat(buf,ip);	    /*copy inst ip*/
	    }
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen(" ") ) )
	    {
		strcat(buf," ");
	    }
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen( mac ) ) )
	    {
		strcat(buf,mac);    /*copy inst mac*/
	    }
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen(" ") ) )
	    {
		strcat(buf," ");
	    }
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen( name ) ) )
	    {
		strcat(buf,name);   /*copy inst name */
	    }
	    if( MAX_ARP_FILE_BUF > ( strlen(buf) + strlen("\r\n") ) )
	    {
		strcat(buf,"\r\n");
	    }
	}
    }

    return TRUE;
}
/***
 *2013-2-1
 *IP/MAC����Ϣ����
 *zheng.xiao
 */
static void formConfigIPMacExport(webs_t wp, char *path, char *query)
{
    char filename[20] = {0};
    char buf[MAX_ARP_FILE_BUF + 1];
    int len = 0;

    memset( buf, 0, MAX_ARP_FILE_BUF );
    memset( filename, 0, 20);
    strcpy(filename, "IPMacinfo.txt");

    ReadIPMacFromProfile(buf);
    len = strlen(buf);
    wimDownloadFile(wp,filename,buf,len);
    return;
}

/***********************************************************************
 * �� �� ����   formDefineArpBindConfig
 * ����������	����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * �������ڣ�	2010-3-17
 * �޸����ڣ�
 * ���ߣ�       �ص���
 * ����˵����	��
 ***********************************************************************/
void formDefineArpBindConfig(void)
{
    websFormDefine(T("formArpBindGlobalConfig"), formConfigArpBindGlobalConfig);
    websFormDefine(T("formArpBindConfig"), formConfigArpBindConfig);
    websFormDefine(T("formReadArp"), formReadArp);
    websFormDefine(T("formAddReadArp"),formAddReadArp);
    websFormDefine(T("formArpBindDelAll"), formArpBindDelAll);
    websFormDefine(T("formArpBindDel"), formArpBindDel);
    websFormDefine(T("formArpBindAllow"), formConfigArpBindAllow);
    websAspDefine(T("aspOutOneArpBindConfig"), getOneArpBindConfig);
    websAspDefine(T("aspOutArpBindList"), getArpBindList);
    websAspDefine(T("aspOutReadArp"), aspOutReadArp);
    websAspDefine(T("aspOutArpBindGlobalConfig"), getArpBindGlobalConfig);
    websFormDefine(T("formIPMacExport"), formConfigIPMacExport);
}

#endif
