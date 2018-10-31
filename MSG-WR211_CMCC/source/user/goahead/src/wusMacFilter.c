#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#ifdef CONFIG_DEFAULTS_KERNEL_2_6_21
  #include  <linux/types.h>
  #include  <linux/socket.h>
  #include  <linux/if.h>
#endif

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "management.h"
#include "wireless.h"
#include "linux/autoconf.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if(MAC_FILTER == FYES)

#define  DEBUG_MACFILTER 1
#if DEBUG_MACFILTER
#define MACFILTER_DEBUG(argx) printf argx
#else
#define MACFILTER_DEBUG(argx) 
#endif

#define MF_NAME_H "user_" /*MAC��ַ����Ĭ���û���ǰ׺*/

static struct arpreq arpLan = {
.arp_pa.sa_family = AF_INET,
.arp_ha.sa_family = AF_INET
};

extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
static int getMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv);
static void formConfigMacFilter(webs_t wp, char_t *path, char_t *query);
#if (UTT_SMART_UI != FYES)
static int getMacFilterOneInfo(int eid, webs_t wp, int argc, char **argv);
#endif
static void formConfigMacFilterDel(webs_t wp, char_t *path, char_t *query);
static void formConfigMacFilterDelAll(webs_t wp, char_t *path, char_t *query);
static void formMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query);
char* getMacByIpIoctl(struct arpreq *parp); 


/*�����Ӻ��û�����mac��ַ�Ƿ������profile�ظ�*/
static int repeatNameMacCheck(int indexNew, char *username, char *macAddr)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    int min, max, index;
    int retVal = SUCCESS;
    const char *ErrMsg = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	if (indexNew != index)/*�޸�profile����£�ԭ����profile������ͳ��,�����Ժ�ԭ������ͬ*/
	{
	    prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (ProfInstIsFree(prof) == PROFOK)
	    {
		if(0 == strcmp(prof->head.name, username))/*�û�����ͬ��ʧ��*/
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
		    setTheErrorMsg(ErrMsg);
		    retVal = FAIL;
		}
		if (0 == memcmp(prof->mac.addr, macAddr, sizeof(prof->mac.addr)))/*MAC��ַ��ͬ,ʧ��*/
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_EXIT_ADDR);
		    setTheErrorMsg(ErrMsg);
		    retVal = FAIL;
		}
	    }
	}
    }

    return retVal;
}


/*
 * ��õ�ǰ�����û�MAC��ַ
 */
#if(FEATURE_AC == FYES)
#define ATF_COM_FG 0x02
static char* getCurrentUserMac(webs_t wp, char *macAddr)
{
    char *retVal = NULL;
    MacAddr mac;
    unsigned char buffer[256],ip[32],tmp[128],pmac[18];
    unsigned int arp_flags;
    FILE* fp;

    memset(buffer,0,sizeof(buffer));
    /*���ļ�*/
    fp = fopen("/proc/net/arp", "r");
    if(fp == NULL)
    {
        return retVal;
    }
    fgets(buffer, sizeof(buffer), fp);
    /*��ȡ�ļ�*/
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        /*ip*/
        if(sscanf(buffer, "%s %*s 0x%x %s",  ip,&arp_flags, tmp)<2)
        {
            /*����*/
            fclose(fp);
            return retVal;
        }
        if((strcmp(ip,wp->ipaddr) == 0)&&(arp_flags & ATF_COM_FG))
        {
            strncpy(pmac,tmp,17);
            pmac[17]='\0';
            strRemoveChar(pmac, ':');
            converMac12To6Byte(pmac, &mac);/*ת��Ϊ6�ֽ�����*/
            retVal = &mac.addr;
            memset(macAddr, 0, sizeof(macAddr));
            memcpy(macAddr, retVal, 6);
            //printf("usermac=%s\n",converMac6To12Byte(mac));
            fclose(fp);
            return retVal;
        }
        memset(buffer,0,sizeof(buffer));
    }

    //printf("usermac=%s\n",converMac6To12Byte(mac));
    fclose(fp);
    return retVal;
}

#else
static char* getCurrentUserMac(webs_t wp, char *macAddr)
{
    int tmp = 0;
    struct in_addr addrS;
    char *retVal = NULL;
    
    if(inet_aton(wp->ipaddr, &addrS) != 0)/*ת��ip��ַ*/ 
    {
	/*������ip��ַ�Ƿ���lan��ͬ����*/
#if (MULTI_LAN == FYES)
	tmp = isIpInLanNet(wp->ipaddr);
#else
	tmp = isIpInIfNet(addrS.s_addr, getLanIfName());
#endif
	if(tmp != 0)/*��ͬ�����������ֹ*/
	{	    
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = addrS.s_addr;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    retVal = getMacByIpIoctl(&arpLan);/*ͨ��arp���ѯ��ǰip�Ķ�Ӧmac��ַ*/
	}    
    }
    if (retVal != NULL)
    {
	memset(macAddr, 0, sizeof(macAddr));
	memcpy(macAddr, retVal, 6);
    }
    return retVal;
}
#endif
/*
 * ��鵱ǰ�û���MAC��ַ�Ƿ���ӽ�profile
 * ����ֵ��SUCCESS--����ӵ�profile��  FAIL--δ�����
 */
static int currentMacIsSet(webs_t wp, char *macAdd, char *macDel)
{
    int retVal = FAIL;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    MacAddr macCurrent;
    if (getCurrentUserMac(wp, (char *)(macCurrent.addr)) != NULL)/*���ݲ�����ip�õ�mac��ַ*/
    {
	if ((macAdd != NULL) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)macAdd))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(macAdd + 2))))) == 0))/*�뽫Ҫ��ӵ��б��е�mac��ַ��ͬ���ɹ�*/
	{
	    retVal = SUCCESS;
	}
	else if ((macDel != NULL) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)macDel))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(macDel + 2))))) == 0))/*�뽫Ҫɾ����mac��ַ��ͬ����Ϊmac��ַ���ظ������Բ���֮��һ�������б���*/
	{
	    retVal = FAIL;
	}
	else/*����Ӻ�ɾ���Ķ���ͬ����Ҫ������prof�в��ҵ�ǰ������mac��ַ�Ƿ����ù�*/
	{
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index=min;index<max;index++)/*�õ�����profile*/
	    {
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if (ProfInstIsFree(prof) == PROFOK)
		{
		    if ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)(prof->mac.addr)))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(prof->mac.addr + 2))))) == 0)/*�ҵ��Ͳ�������ͬ��mac��ַ���ɹ������򷵻�ʧ��*/
		    {
			retVal = SUCCESS;
		    }
		}
	    }
	}
    }
    else
    {
	retVal = -1;
    }
    return retVal;
}

/*
 * ��鵱ǰ�����û��Ƿ�ᱻ��ֹ
 */
static int banCurrentUserCheck(webs_t wp, int active, int action, char *macAdd, char *macDel)
{
    int retVal = SUCCESS;
    int MacIsSet = SUCCESS;
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    
    /*�޸�ȫ������ʱ�����޸ĺ�Ŀ��غ͹��˹���״̬����������´���-1,�ڴ˺����ڲ�ֱ�ӻ�ȡ��ǰ��*/
    if (active == -1)
    {
	profG = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeG, 0);
	if (ProfInstIsFree(profG) == PROFOK)
	{
	    active = profG->head.active;
	    action = profG->action;
	}
    }

    if (active == TRUE)/*���ܲ���������Ҫ�ж��Ƿ�Ϸ�*/
    {
	MacIsSet = currentMacIsSet(wp, macAdd, macDel);/*�жϵ�ǰmac�Ƿ���ӵ�ĳһ��Ŀ��*/
	if (MacIsSet == -1)/*-1��ʾû�ҵ���û�ҵ�������������*/
	{
	    retVal = SUCCESS;
	}
	else if (action == MAC_ALLOW)
	{
	    if (MacIsSet == FAIL)/*ֻ�����б���mac���ʣ���û���Լ���ӽ��б�ʧ��*/
	    {
		retVal = FAIL;
	    }
	}
	else/*ֻ��ֹ*/
	{
	    if (MacIsSet == SUCCESS)/*��ֹ����°��Լ���ӽ��б�ʧ��*/
	    {
		retVal = FAIL;
	    }
	}
    }
    return retVal;
}


/*MacFilter.aspʹ�ã�����б���Ҫ����Ϣ��ȫ��������Ϣ*/
static int getMacFilterMacList(int eid, webs_t wp, int argc, char_t **argv)
{
    int num = 0;
    char *tmpmac = NULL;
    const char *profName = NULL;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    MibProfileType mibTypeGlobal = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlobal = NULL;
   
    profGlobal = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlobal, index);
    if(ProfInstIsFree(profGlobal) == 0)
    {
	websWrite(wp,"var MacFilterEnables=\"%d\";", profGlobal->head.active);/*����MAC��ַ���˰�ť*/
	websWrite(wp,"var filterRules=\"%d\";", profGlobal->action);/*���˹���*/
    }
    websWrite(wp,"var filterMacs=new Array();");
    websWrite(wp,"var usernames=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;/*num��¼ʵ�������Ŀ����*/
    for (index = min; index < max; index++)
    {
	prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	profName = ProfGetInstNameByIndex(mibType, index);
	if (0 != strcmp(profName, ""))
	{
	    tmpmac = converMac6To12Byte(prof->mac);
	    Conversion2StdMac(tmpmac);
	    websWrite(wp, "filterMacs[%d] = \"%s\";", num, tmpmac);
	    websWrite(wp, "usernames[%d] = \"%s\";", num, profName);
	    num++;
	}
	else
	{
	    continue;
	}
    }
    websWrite(wp,"var totalrecs=%d;", num);
    websWrite(wp,"var max_totalrecs=%d;", MAX_MAC_FILTER_PROFILES);/*�����Ŀ����������ָ��*/
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif

    return 0;
}

#if (UTT_SMART_UI != FYES)
/*��һ�������зָ����û���������*/
static int getMacNameFromRow(char *rowStr, char **macStr, char **userName)
{
    int retVal = FAIL;
    char* split =" ";/*ҳ��һ�������и�����Ԫ�ķָ��*/
    char* ptr = NULL;

    if(rowStr != NULL)
    {
	*macStr  = strtok_r(rowStr, split, &ptr);/*mac�ִ�*/
	if(*macStr != NULL)
	{
	    *userName  = strtok_r(NULL, split, &ptr);/*�û���*/
	    if((*userName != NULL) && (**userName == '\0'))
	    {/*�Ƿ��û���*/
		*userName = NULL;
	    }
	    retVal = SUCCESS;/*�û�������Ϊ�ա��ʵ��˿���Ϊ�����ɹ�*/
	}
    }
    return retVal;
}

/*
 * MAC��ַ�����������
 */
static void macFilterBulkAdd(webs_t wp, char_t *path, char_t *query)
{
    char *textarea = NULL, *ptr = NULL, *rowStr = NULL;
    char tmpStr[64];
    char *macStr = NULL, *userName = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    int instIndex = 0;
    MacAddr macAddr;
    struct in_addr addrS;
    struProfAlloc *profhead = NULL;
    char* delim = "\n";/*textarea�����ݷָ���*/
    int bindNum = 0;/*����Ҫ���˶��ٸ���Ŀ*/
    int successNum = 0;/*�ɹ����˵���Ŀ��*/
    int back = 0;
    int maxDeNameNo = 0 , aDNametag = 0;
    const char *errMsg=NULL;
    const char *total=NULL;
    const char *totalNum=NULL;
    const char *failNum=NULL;

    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    textarea=websGetVar(wp,T("data"), T(""));/*����������Ϣ*/

    strRemoveChar(textarea, '\r');/*�Ƴ�/r*/
    mergeMulSpaEnter(textarea);/*ȥ������Ŀո��Լ�����*/
    bindNum = strCountChar(textarea, delim[0]) + 1;/*ͨ����������ȷ���û����β�������*/

    rowStr=strtok_r(textarea,delim,&ptr);/*ȡ��һ��*/
    /*ѭ����ȡtextarea����*/
    while(NULL!=rowStr)
    {
	/*��һ����Ϣ�н�������mac�Լ��û���*/
	if(getMacNameFromRow(rowStr, &macStr, &userName) != 0)
	{
	    strRemoveChar(macStr, ':');
	    converMac12To6Byte(macStr, &macAddr);/*ת��Ϊ6�ֽ�����*/
	    instIndex = ProfGetInstIndexFree(mibType);/*��ȡһ������ʵ��*/
	    if(instIndex == PROFFAIL) /*��ȡ������֤���Ѵ������Ŀ*/
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		setTheErrorMsgOnFree(errMsg);
		break;
	    }
	    else
	    {/*��ȡ�ɹ�*/
		if(banCurrentUserCheck(wp, -1, -1, (char *)macAddr.addr, NULL) == SUCCESS)/*���ι����Ƿ����ʹ��ǰ��½�ߣ��޷���¼*/
		{
		    aDNametag = 0;/*�����Ƿ�ʹ��Ĭ���û������*/
		    if(userName == NULL)/*�û���û����*/
		    {
			if(maxDeNameNo == 0)
			{
			    maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, MF_NAME_H);/*��ȡ���Ĭ���û������*/
			}
			userName = ProfBuildDefaultName(MF_NAME_H, maxDeNameNo);/*����Ĭ���û���*/
			aDNametag = 1;/*��־����һ��Ĭ���û���*/
		    }
		    if(repeatNameMacCheck(-1, userName, (char *)macAddr.addr) == SUCCESS)/*�����ظ����ж�*/
		    {
			if(bindNum < 6)
			{/*���ܹ�����Ŀ������̫����ÿ����Ӷ����ݲ�����Ϣ*/
			    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*�����´����Ŀ�ʵ��*/
			}
			else
			{/*���ι�����Ŀ��������ֻ����һ�Ρ�ֻ��һ����Ϣ*/
			    if(back == 0 )
			    {/*��������Ϊ ��Ӷ�������speedweb�����³�ʼ��arp���*/
				ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
				back = 1;
			    }
			}
			successNum++;/*�����ɹ��Ĵ���*/
			prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, instIndex);/*��ȡʵ��ָ��*/
			ProfSetNameByPoint(prof, userName);/*�����û���*/
			memcpy(prof->mac.addr, macAddr.addr, sizeof(prof->mac.addr));
			maxDeNameNo+=aDNametag;/*��Ĭ���û������ɣ�����ʹ���ˣ������Ĭ���û�����ż�һ*/
		    }
		}
	    }
	}
	rowStr=strtok_r(NULL, delim, &ptr);/*��ȡ��һ������*/
    }    

    ProfUpdate(profhead);/*����Ϣ��Ч*/
    ProfFreeAllocList(profhead);/*�ͷű��ݿؼ�*/
    nvramWriteCommit();/*дflash*/

    if(bindNum > 0)/*���а����޷�ȫ����ʾ���ʲ���������ʾ��ʽ*/
    {
	if(bindNum != successNum)
	{
	    total = getTransDataForC(C_LANG_INDEX_TOTAL_ADD);
	    totalNum = getTransDataForC(C_LANG_INDEX_TOTAL_BIND_NUM);
	    failNum = getTransDataForC(C_LANG_INDEX_FAIL_BIND_NUM);
	    /*���ΰ���ʧ�ܵ���Ŀ*/
	    sprintf(tmpStr, "��%s%d%s%d%s", total, bindNum, totalNum, (bindNum - successNum), failNum);
	    setTheErrorMsgCat(tmpStr);
	}
    }
    websRedirect(wp, "MacFilter.asp");
    return ;
}
#endif

/*���һ��MAC��ַ��������Ŀ*/
static void addMacFilterProf(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    const char *ErrMsg = NULL;
    char *mac = NULL, *username = NULL;
    int index = 0;
    struProfAlloc *profhead = NULL;
    MacAddr tmpMac;

    username = websGetVar(wp, T("username"), "");
    mac = websGetVar(wp, T("filterMac"), "");
    converMac12To6Byte(mac, &tmpMac);

    if((username[0] != '\0') && (mac[0] != '\0'))
    {
	/*����δʹ�ù���λ�ã�ʧ�����˳�*/
	if (ProfGetInstIndexFree(mibType) == PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_MAX);
	    setTheErrorMsg(ErrMsg);
	}
	else if ((banCurrentUserCheck(wp, -1, -1, (char *)tmpMac.addr, NULL) == FAIL))/*����Ƿ���ò������޷���½*/
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	    setTheErrorMsgOnFree(ErrMsg);
	}
	else
	{
	    /*����û�����mac��ַ�Ƿ���Ѵ��ڵ���Ŀ�ظ�*/
	    if (repeatNameMacCheck(-1, username, (char *)tmpMac.addr) == SUCCESS)
	    {
		/* Add new Profile */
		index = ProfNewInst(mibType, username, FALSE);/*�����û����õ���prof�����*/
		ProfBackupByIndex(mibType, PROF_CHANGE_ADD, index, &profhead);
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if (ProfInstIsFree(prof) == PROFOK)
		{
		    memcpy(prof->mac.addr, tmpMac.addr, sizeof(prof->mac.addr));
		    ProfUpdate(profhead);/*��speedweb����Ϣ*/
		    ProfFreeAllocList(profhead);
		    nvramWriteCommit();/*����flash*/
		}
	    }
	}
    }   
}


/*�޸�һ����Ϣ*/
static void editMacFilterProf(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    MacAddr tmpMac, tmpOldMac;
    char *mac = NULL, *oldMac = NULL;
    char *username = NULL, *oldName = NULL;
    int index = 0;
    const char *ErrMsg = NULL;

    username = websGetVar(wp, T("username"), "");
    mac = websGetVar(wp, T("filterMac"), "");
    oldName = websGetVar(wp, T("oldName"), "");
    oldMac = websGetVar(wp, T("oldMac"), "");
    converMac12To6Byte(mac, &tmpMac);
    converMac12To6Byte(oldMac, &tmpOldMac);


    /*��mac��ַ�޸Ĺ�����£��ж���mac��ַ�Ƿ���ò������޷���½*/
    if ((strcmp(mac, oldMac) != 0) && (banCurrentUserCheck(wp, -1, -1, (char *)tmpMac.addr, (char *)tmpOldMac.addr) == FAIL))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	setTheErrorMsgOnFree(ErrMsg);
    }
    else/*��mac��ַ��������*/
    {
	index = ProfGetInstIndexByName(mibType, oldName);
	if (index != PROFFAIL)
	{
	    /*����µ��û�����mac��ַ�Ƿ��������Ϣ�ظ�*/
	    if (repeatNameMacCheck(index, username, (char *)tmpMac.addr) == SUCCESS)
	    {
		ProfBackupByName(mibType, PROF_CHANGE_EDIT, oldName, &profhead);/*�����޸�֮ǰ��*/
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if(ProfInstIsFree(prof) == PROFOK)
		{
		    ProfSetNameByPoint(prof, username);/*���������û���*/
		    memcpy(prof->mac.addr, tmpMac.addr, sizeof(tmpMac.addr));
		    ProfUpdate(profhead);/*��speedweb����Ϣ,������Ӧ����*/
		    ProfFreeAllocList(profhead);
		    nvramWriteCommit();/*����flash*/
		}
	    }
	}
    }

}


/*��ӻ��޸ĵ�����Ϣ��Ӧִ�к���,MacFilter_edit.aspʹ��*/
static void formConfigMacFilter(webs_t wp, char_t *path, char_t *query)
{
    char *action = NULL;

    action = websGetVar(wp, T("Action"), T("")); 

    if(strcmp(action, "add") == 0)/*�ж�����ӻ����޸�*/
    {
	addMacFilterProf(wp);/*���*/
    }
    else
    {
	editMacFilterProf(wp);/*�޸�*/

    }

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif
}


/*ɾ����ťִ�к���*/
static void formConfigMacFilterDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *delStr, *tmp = NULL, *ptr = NULL;
    const char *ErrMsg = NULL;
    MacFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeGlo = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlo = NULL;

    delStr = websGetVar(wp, T("delstr"), T(""));/*Ҫɾ����prof�û�����ɵ��ַ���*/
    profGlo = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlo, 0);

    if (ProfInstIsFree(profGlo) == PROFOK)/*���ָ��*/
    {
	tmp = strtok_r(delStr, UTT_SPLIT_SIGN_STR, &ptr);/*�ָ����һ���û���*/
	while(tmp != NULL)
	{
	    prof = ProfGetInstPointByName(mibType, tmp);/*��ȡ���õ�ַ*/
	    if (ProfInstIsFree(prof) == PROFOK)
	    {
		/*�ж��Ƿ���ò������޷���½*/
		if (banCurrentUserCheck(wp, -1, -1, NULL, (char *)(prof->mac.addr)) == SUCCESS)
		{
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
		    ProfDelInstByName(mibType, tmp);/*ɾ������ʵ��*/
		}
		else
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		    setTheErrorMsgOnFree(ErrMsg);
		}
	    }
	    tmp=strtok_r(NULL, ",", &ptr);/*ÿ��ȥ��һ���û���������NULL�˳�*/
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*����flash*/
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif
}


/*ɾ��������Ŀִ�к���*/
static void formConfigMacFilterDelAll(webs_t wp, char_t *path, char_t *query)
{
    int min = 0, max = 0;
    int index = 0, bakIndex = 0;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL, profBak;
    MibProfileType mibTypeGlo = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlo = NULL;
    ProfChangeType profAction = PROF_CHANGE_DELALL;
    struProfAlloc *profhead = NULL;
    int back = 0;
    const char *ErrMsg = NULL;
    MacAddr macCurrent;
    int needCompare = 0;/*�Ƿ���ҵ���ǰmac��û�ҵ�����Ҫ�Ƚ�*/

    memset(&macCurrent, 0, sizeof(macCurrent));
    memset(&profBak, 0, sizeof(profBak));
    profGlo = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlo, 0);

    if ((ProfInstIsFree(profGlo) == PROFOK) && (profGlo->head.active == TRUE) && (profGlo->action == MAC_ALLOW) && (getCurrentUserMac(wp, (char *)(macCurrent.addr)) != NULL))/*���ݲ�����ip�õ�mac��ַ*/
    {
	needCompare = 1;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if(ProfInstIsFree(prof) == PROFOK)
	{
	    /*�ж��Ƿ��������MAC��ͬ*/
	    if ((needCompare == 1) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)(prof->mac.addr)))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)((prof->mac.addr) + 2))))) == 0))
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(ErrMsg);
		bakIndex = index;/*��¼����*/
		ProfGetInstByIndex(mibType, index, &profBak);/*��ȡʵ���Ŀ�����*/
	    }
	    if (back == 0)
	    {
		ProfBackupByIndex(mibType, profAction, index, &profhead);
		back = 1;
	    }
	    ProfDelInstByIndex(mibType, index);/*ɾ��prof*/
	}
    }
    ProfUpdate(profhead);/*����Ϣ*/
    ProfFreeAllocList(profhead);
	
    sleep(2);/*�ȴ�ɾ�����еĲ��������ȴ�����Ϣ���ƿ����ȴ������Ϣ��ɾ�����У�*/
    profhead = NULL;
    /*���ﲻ�½�ʵ������Ϊ�˷�ֹ�û�ȫ������Ĭ���û���ʱ����ͻ�����Ե����*/
    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, bakIndex, &profhead);/*�����´����Ŀ�ʵ��*/
    prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, bakIndex);/*��ȡʵ��ָ��*/
    if(prof != NULL)
    {
	*prof = profBak;/*ֱ�Ӹ�ֵ*/
    }
    ProfUpdate(profhead);/*����Ϣ���ʵ��*/
    ProfFreeAllocList(profhead);/*�ͷű��ݿռ�*/

    nvramWriteCommit();/*����flash*/
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif

}


/*MAC��ַ����ȫ�����ú���*/
static void formMacFilterGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char_t *macfiteren, *filterRule;
    int index = 0;
    int activeTmp = FALSE, actionTmp = MAC_DENY;
    const char *errMsg = NULL;
    
    MibProfileType mibType = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *prof = NULL;
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    macfiteren=websGetVar(wp, "MacFilterEnable", T(""));
    filterRule=websGetVar(wp, "filterRule", T(""));
    if (0 == strncmp(macfiteren, "on", 3))/*����MAC��ַ���˰�ť״̬*/
    {
	activeTmp = TRUE;
    }
    if (0 == strcmp(filterRule, "0"))/*���˹���*/
    {
	actionTmp = MAC_ALLOW;
    }

    /*����Ƿ����Լ���ֹ*/
    if (banCurrentUserCheck(wp, activeTmp, actionTmp, NULL, NULL) == FAIL)
    {
	errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	prof = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibType, index);
	if(ProfInstIsFree(prof) == PROFOK)
	{
	    ProfBackupByIndex(mibType, profAction, index, &profhead);
	    prof->head.active = activeTmp;
	    prof->action = actionTmp;
	}
	ProfUpdate(profhead);/*��speedweb����Ϣ*/
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*����flash*/
    }    

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif

}

	
#if (UTT_SMART_UI != FYES)
/*MacFilter_edit.aspҳ��ʹ�ã���ҳ�����һ��prof������*/
static int getMacFilterOneInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;

    char *editName = NULL;
    const char *ErrMsg = NULL;
    char *tmpmac = NULL;

    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*�鿴ʵ�����Ƿ����*/ 
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	prof = ProfGetInstPointByName(mibType, editName);
	if (prof != NULL)
	{
	    websWrite(wp, "username = \"%s\";", prof->head.name);/*�û���*/
	    tmpmac = converMac6To12Byte(prof->mac);
	    websWrite(wp, "macaddr = \"%s\";", tmpmac);/*MAC��ַ*/
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

/***********************************************************************
 * �� �� ����   formDefineMacFilter
 * �������ڣ�	2013-4-25
 * �޸����ڣ�
 * ����˵����	��
***********************************************************************/
void formDefineMacFilter(void)
{
    websFormDefine(T("ConfigMacFilter"), formConfigMacFilter);
#if (UTT_SMART_UI != FYES)
    websFormDefine(T("formMacFilterBulkAdd"), macFilterBulkAdd);
#endif
    websFormDefine(T("ConfigMacFilterDel"), formConfigMacFilterDel);
    websFormDefine(T("ConfigMacFilterDelAll"), formConfigMacFilterDelAll);
    websAspDefine(T("outputMacFilterConfig"), getMacFilterMacList);
#if (UTT_SMART_UI != FYES)
    websAspDefine(T("outPutMacFilterOneInfo"), getMacFilterOneInfo);
#endif
    websFormDefine(T("ConfigMacFilterGlobalConfig"), formMacFilterGlobalConfig);
    strcpy(arpLan.arp_dev, getLanIfName());
}
#endif
