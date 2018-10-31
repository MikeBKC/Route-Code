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

#define MF_NAME_H "user_" /*MAC地址过滤默认用户名前缀*/

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


/*检查添加和用户名和mac地址是否和其他profile重复*/
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
	if (indexNew != index)/*修改profile情况下，原来的profile不参与统计,即可以和原来的相同*/
	{
	    prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	    if (ProfInstIsFree(prof) == PROFOK)
	    {
		if(0 == strcmp(prof->head.name, username))/*用户名相同，失败*/
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_ALREADY_EXITS);
		    setTheErrorMsg(ErrMsg);
		    retVal = FAIL;
		}
		if (0 == memcmp(prof->mac.addr, macAddr, sizeof(prof->mac.addr)))/*MAC地址相同,失败*/
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
 * 获得当前操作用户MAC地址
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
    /*打开文件*/
    fp = fopen("/proc/net/arp", "r");
    if(fp == NULL)
    {
        return retVal;
    }
    fgets(buffer, sizeof(buffer), fp);
    /*读取文件*/
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        /*ip*/
        if(sscanf(buffer, "%s %*s 0x%x %s",  ip,&arp_flags, tmp)<2)
        {
            /*出错*/
            fclose(fp);
            return retVal;
        }
        if((strcmp(ip,wp->ipaddr) == 0)&&(arp_flags & ATF_COM_FG))
        {
            strncpy(pmac,tmp,17);
            pmac[17]='\0';
            strRemoveChar(pmac, ':');
            converMac12To6Byte(pmac, &mac);/*转化为6字节类型*/
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
    
    if(inet_aton(wp->ipaddr, &addrS) != 0)/*转化ip地址*/ 
    {
	/*访问者ip地址是否与lan口同网段*/
#if (MULTI_LAN == FYES)
	tmp = isIpInLanNet(wp->ipaddr);
#else
	tmp = isIpInIfNet(addrS.s_addr, getLanIfName());
#endif
	if(tmp != 0)/*不同网段则可随便禁止*/
	{	    
	    ((struct sockaddr_in*)&arpLan.arp_pa)->sin_addr.s_addr = addrS.s_addr;
	    memset(arpLan.arp_ha.sa_data, 0, sizeof(arpLan.arp_ha.sa_data));
	    retVal = getMacByIpIoctl(&arpLan);/*通过arp表查询当前ip的对应mac地址*/
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
 * 检查当前用户的MAC地址是否被添加进profile
 * 返回值：SUCCESS--被添加到profile中  FAIL--未被添加
 */
static int currentMacIsSet(webs_t wp, char *macAdd, char *macDel)
{
    int retVal = FAIL;
    int min, max, index = 0;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;
    MacAddr macCurrent;
    if (getCurrentUserMac(wp, (char *)(macCurrent.addr)) != NULL)/*根据操作者ip得到mac地址*/
    {
	if ((macAdd != NULL) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)macAdd))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(macAdd + 2))))) == 0))/*与将要添加到列表中的mac地址相同，成功*/
	{
	    retVal = SUCCESS;
	}
	else if ((macDel != NULL) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)macDel))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(macDel + 2))))) == 0))/*与将要删除的mac地址相同，因为mac地址不重复，所以操作之后一定不再列表中*/
	{
	    retVal = FAIL;
	}
	else/*与添加和删除的都不同，需要到其他prof中查找当前操作者mac地址是否被设置过*/
	{
	    ProfInstLowHigh(mibType, &max, &min);
	    for (index=min;index<max;index++)/*得到所有profile*/
	    {
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if (ProfInstIsFree(prof) == PROFOK)
		{
		    if ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)(prof->mac.addr)))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)(prof->mac.addr + 2))))) == 0)/*找到和操作者相同的mac地址，成功，否则返回失败*/
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
 * 检查当前操作用户是否会被禁止
 */
static int banCurrentUserCheck(webs_t wp, int active, int action, char *macAdd, char *macDel)
{
    int retVal = SUCCESS;
    int MacIsSet = SUCCESS;
    MibProfileType mibTypeG = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profG = NULL;
    
    /*修改全局配置时传入修改后的开关和过滤规则状态，其他情况下传入-1,在此函数内部直接获取当前的*/
    if (active == -1)
    {
	profG = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeG, 0);
	if (ProfInstIsFree(profG) == PROFOK)
	{
	    active = profG->head.active;
	    action = profG->action;
	}
    }

    if (active == TRUE)/*功能不开启则不需要判断是否合法*/
    {
	MacIsSet = currentMacIsSet(wp, macAdd, macDel);/*判断当前mac是否被添加到某一条目中*/
	if (MacIsSet == -1)/*-1表示没找到，没找到则可以随意添加*/
	{
	    retVal = SUCCESS;
	}
	else if (action == MAC_ALLOW)
	{
	    if (MacIsSet == FAIL)/*只允许列表中mac访问，又没把自己添加进列表，失败*/
	    {
		retVal = FAIL;
	    }
	}
	else/*只禁止*/
	{
	    if (MacIsSet == SUCCESS)/*禁止情况下把自己添加进列表，失败*/
	    {
		retVal = FAIL;
	    }
	}
    }
    return retVal;
}


/*MacFilter.asp使用，输出列表需要的信息和全局配置信息*/
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
	websWrite(wp,"var MacFilterEnables=\"%d\";", profGlobal->head.active);/*启用MAC地址过滤按钮*/
	websWrite(wp,"var filterRules=\"%d\";", profGlobal->action);/*过滤规则*/
    }
    websWrite(wp,"var filterMacs=new Array();");
    websWrite(wp,"var usernames=new Array();");
    ProfInstLowHigh(mibType, &max, &min);
    num = min;/*num记录实际输出条目数量*/
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
    websWrite(wp,"var max_totalrecs=%d;", MAX_MAC_FILTER_PROFILES);/*最大条目数，即性能指标*/
    getTheErrorMsg(eid, wp, argc, argv);
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif

    return 0;
}

#if (UTT_SMART_UI != FYES)
/*从一行数据中分隔出用户名和密码*/
static int getMacNameFromRow(char *rowStr, char **macStr, char **userName)
{
    int retVal = FAIL;
    char* split =" ";/*页面一行数据中各个单元的分割符*/
    char* ptr = NULL;

    if(rowStr != NULL)
    {
	*macStr  = strtok_r(rowStr, split, &ptr);/*mac字串*/
	if(*macStr != NULL)
	{
	    *userName  = strtok_r(NULL, split, &ptr);/*用户名*/
	    if((*userName != NULL) && (**userName == '\0'))
	    {/*非法用户名*/
		*userName = NULL;
	    }
	    retVal = SUCCESS;/*用户名可以为空。故到此可认为解析成功*/
	}
    }
    return retVal;
}

/*
 * MAC地址过滤批量添加
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
    char* delim = "\n";/*textarea行数据分隔符*/
    int bindNum = 0;/*本次要过滤多少个条目*/
    int successNum = 0;/*成功过滤的条目数*/
    int back = 0;
    int maxDeNameNo = 0 , aDNametag = 0;
    const char *errMsg=NULL;
    const char *total=NULL;
    const char *totalNum=NULL;
    const char *failNum=NULL;

    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    textarea=websGetVar(wp,T("data"), T(""));/*多条过滤信息*/

    strRemoveChar(textarea, '\r');/*移除/r*/
    mergeMulSpaEnter(textarea);/*去除多余的空格以及换行*/
    bindNum = strCountChar(textarea, delim[0]) + 1;/*通过查找行数确定用户本次操作条数*/

    rowStr=strtok_r(textarea,delim,&ptr);/*取得一行*/
    /*循环读取textarea数据*/
    while(NULL!=rowStr)
    {
	/*从一行信息中解析出来mac以及用户名*/
	if(getMacNameFromRow(rowStr, &macStr, &userName) != 0)
	{
	    strRemoveChar(macStr, ':');
	    converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
	    instIndex = ProfGetInstIndexFree(mibType);/*获取一个空闲实例*/
	    if(instIndex == PROFFAIL) /*获取不到则证明已达最大条目*/
	    {
		errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		setTheErrorMsgOnFree(errMsg);
		break;
	    }
	    else
	    {/*获取成功*/
		if(banCurrentUserCheck(wp, -1, -1, (char *)macAddr.addr, NULL) == SUCCESS)/*本次过滤是否可能使当前登陆者，无法登录*/
		{
		    aDNametag = 0;/*本次是否使用默认用户名添加*/
		    if(userName == NULL)/*用户名没输入*/
		    {
			if(maxDeNameNo == 0)
			{
			    maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, MF_NAME_H);/*获取最大默认用户名编号*/
			}
			userName = ProfBuildDefaultName(MF_NAME_H, maxDeNameNo);/*生成默认用户名*/
			aDNametag = 1;/*标志生成一个默认用户名*/
		    }
		    if(repeatNameMacCheck(-1, userName, (char *)macAddr.addr) == SUCCESS)/*过滤重复性判断*/
		    {
			if(bindNum < 6)
			{/*若总过滤条目数不是太大。则每次添加都备份并发消息*/
			    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份新创建的空实例*/
			}
			else
			{/*本次过滤条目数过大。则只备份一次。只发一次消息*/
			    if(back == 0 )
			    {/*备份类型为 添加多条。则speedweb会重新初始化arp相关*/
				ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
				back = 1;
			    }
			}
			successNum++;/*操作成功的次数*/
			prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, instIndex);/*获取实例指针*/
			ProfSetNameByPoint(prof, userName);/*设置用户名*/
			memcpy(prof->mac.addr, macAddr.addr, sizeof(prof->mac.addr));
			maxDeNameNo+=aDNametag;/*若默认用户名生成，并被使用了，则最大默认用户名编号加一*/
		    }
		}
	    }
	}
	rowStr=strtok_r(NULL, delim, &ptr);/*读取下一行内容*/
    }    

    ProfUpdate(profhead);/*发消息生效*/
    ProfFreeAllocList(profhead);/*释放备份控件*/
    nvramWriteCommit();/*写flash*/

    if(bindNum > 0)/*多行绑定因无法全部提示，故采用以下提示方式*/
    {
	if(bindNum != successNum)
	{
	    total = getTransDataForC(C_LANG_INDEX_TOTAL_ADD);
	    totalNum = getTransDataForC(C_LANG_INDEX_TOTAL_BIND_NUM);
	    failNum = getTransDataForC(C_LANG_INDEX_FAIL_BIND_NUM);
	    /*本次绑定有失败的条目*/
	    sprintf(tmpStr, "：%s%d%s%d%s", total, bindNum, totalNum, (bindNum - successNum), failNum);
	    setTheErrorMsgCat(tmpStr);
	}
    }
    websRedirect(wp, "MacFilter.asp");
    return ;
}
#endif

/*添加一条MAC地址过滤新条目*/
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
	/*查找未使用过的位置，失败则退出*/
	if (ProfGetInstIndexFree(mibType) == PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_MAC_ADDR_LIST_MAX);
	    setTheErrorMsg(ErrMsg);
	}
	else if ((banCurrentUserCheck(wp, -1, -1, (char *)tmpMac.addr, NULL) == FAIL))/*检查是否会让操作者无法登陆*/
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	    setTheErrorMsgOnFree(ErrMsg);
	}
	else
	{
	    /*检查用户名和mac地址是否和已存在的条目重复*/
	    if (repeatNameMacCheck(-1, username, (char *)tmpMac.addr) == SUCCESS)
	    {
		/* Add new Profile */
		index = ProfNewInst(mibType, username, FALSE);/*根据用户名得到新prof的序号*/
		ProfBackupByIndex(mibType, PROF_CHANGE_ADD, index, &profhead);
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if (ProfInstIsFree(prof) == PROFOK)
		{
		    memcpy(prof->mac.addr, tmpMac.addr, sizeof(prof->mac.addr));
		    ProfUpdate(profhead);/*给speedweb发消息*/
		    ProfFreeAllocList(profhead);
		    nvramWriteCommit();/*保存flash*/
		}
	    }
	}
    }   
}


/*修改一条信息*/
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


    /*在mac地址修改过情况下，判断新mac地址是否会让操作者无法登陆*/
    if ((strcmp(mac, oldMac) != 0) && (banCurrentUserCheck(wp, -1, -1, (char *)tmpMac.addr, (char *)tmpOldMac.addr) == FAIL))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	setTheErrorMsgOnFree(ErrMsg);
    }
    else/*新mac地址可以设置*/
    {
	index = ProfGetInstIndexByName(mibType, oldName);
	if (index != PROFFAIL)
	{
	    /*检查新的用户名和mac地址是否和其他信息重复*/
	    if (repeatNameMacCheck(index, username, (char *)tmpMac.addr) == SUCCESS)
	    {
		ProfBackupByName(mibType, PROF_CHANGE_EDIT, oldName, &profhead);/*备份修改之前的*/
		prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if(ProfInstIsFree(prof) == PROFOK)
		{
		    ProfSetNameByPoint(prof, username);/*重新设置用户名*/
		    memcpy(prof->mac.addr, tmpMac.addr, sizeof(tmpMac.addr));
		    ProfUpdate(profhead);/*给speedweb发消息,创建对应规则*/
		    ProfFreeAllocList(profhead);
		    nvramWriteCommit();/*保存flash*/
		}
	    }
	}
    }

}


/*添加或修改单条信息对应执行函数,MacFilter_edit.asp使用*/
static void formConfigMacFilter(webs_t wp, char_t *path, char_t *query)
{
    char *action = NULL;

    action = websGetVar(wp, T("Action"), T("")); 

    if(strcmp(action, "add") == 0)/*判断是添加或者修改*/
    {
	addMacFilterProf(wp);/*添加*/
    }
    else
    {
	editMacFilterProf(wp);/*修改*/

    }

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif
}


/*删除按钮执行函数*/
static void formConfigMacFilterDel(webs_t wp, char_t *path, char_t *query)
{
    char_t *delStr, *tmp = NULL, *ptr = NULL;
    const char *ErrMsg = NULL;
    MacFilterProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    struProfAlloc *profhead = NULL;
    MibProfileType mibTypeGlo = MIB_PROF_MAC_FILTER_GLOBAL;
    MacFilterGlobalProfile *profGlo = NULL;

    delStr = websGetVar(wp, T("delstr"), T(""));/*要删除的prof用户名组成的字符串*/
    profGlo = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlo, 0);

    if (ProfInstIsFree(profGlo) == PROFOK)/*检查指针*/
    {
	tmp = strtok_r(delStr, UTT_SPLIT_SIGN_STR, &ptr);/*分割出第一个用户名*/
	while(tmp != NULL)
	{
	    prof = ProfGetInstPointByName(mibType, tmp);/*获取配置地址*/
	    if (ProfInstIsFree(prof) == PROFOK)
	    {
		/*判断是否会让操作者无法登陆*/
		if (banCurrentUserCheck(wp, -1, -1, NULL, (char *)(prof->mac.addr)) == SUCCESS)
		{
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
		    ProfDelInstByName(mibType, tmp);/*删除已有实例*/
		}
		else
		{
		    ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		    setTheErrorMsgOnFree(ErrMsg);
		}
	    }
	    tmp=strtok_r(NULL, ",", &ptr);/*每次去除一个用户名，返回NULL退出*/
	}

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*保存flash*/
    }
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif
}


/*删除所有条目执行函数*/
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
    int needCompare = 0;/*是否查找到当前mac，没找到则不需要比较*/

    memset(&macCurrent, 0, sizeof(macCurrent));
    memset(&profBak, 0, sizeof(profBak));
    profGlo = (MacFilterGlobalProfile *)ProfGetInstPointByIndex(mibTypeGlo, 0);

    if ((ProfInstIsFree(profGlo) == PROFOK) && (profGlo->head.active == TRUE) && (profGlo->action == MAC_ALLOW) && (getCurrentUserMac(wp, (char *)(macCurrent.addr)) != NULL))/*根据操作者ip得到mac地址*/
    {
	needCompare = 1;
    }
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
	if(ProfInstIsFree(prof) == PROFOK)
	{
	    /*判断是否与操作者MAC相同*/
	    if ((needCompare == 1) && ((((*(unsigned short*)(macCurrent.addr)) ^ (*((unsigned short*)(prof->mac.addr)))) || ((*((UINT32*)(macCurrent.addr + 2))) ^ (*((UINT32*)((prof->mac.addr) + 2))))) == 0))
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
		setTheErrorMsgOnFree(ErrMsg);
		bakIndex = index;/*记录索引*/
		ProfGetInstByIndex(mibType, index, &profBak);/*获取实例的拷贝体*/
	    }
	    if (back == 0)
	    {
		ProfBackupByIndex(mibType, profAction, index, &profhead);
		back = 1;
	    }
	    ProfDelInstByIndex(mibType, index);/*删除prof*/
	}
    }
    ProfUpdate(profhead);/*发消息*/
    ProfFreeAllocList(profhead);
	
    sleep(2);/*等待删除所有的操作（不等待则消息机制可能先处理该消息再删除所有）*/
    profhead = NULL;
    /*这里不新建实例，是为了防止用户全部采用默认用户名时，冲突可能性的提高*/
    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, bakIndex, &profhead);/*备份新创建的空实例*/
    prof = (MacFilterProfile *)ProfGetInstPointByIndex(mibType, bakIndex);/*获取实例指针*/
    if(prof != NULL)
    {
	*prof = profBak;/*直接赋值*/
    }
    ProfUpdate(profhead);/*发消息添加实例*/
    ProfFreeAllocList(profhead);/*释放备份空间*/

    nvramWriteCommit();/*保存flash*/
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif

}


/*MAC地址过滤全局配置函数*/
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
    if (0 == strncmp(macfiteren, "on", 3))/*启用MAC地址过滤按钮状态*/
    {
	activeTmp = TRUE;
    }
    if (0 == strcmp(filterRule, "0"))/*过滤规则*/
    {
	actionTmp = MAC_ALLOW;
    }

    /*检查是否会把自己禁止*/
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
	ProfUpdate(profhead);/*给speedweb发消息*/
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*保存flash*/
    }    

#if (UTT_SMART_UI == FYES)
    setSuccessTips();
    websRedirect(wp, "MacFilterSui.asp");
#else
    websRedirect(wp, "MacFilter.asp");
#endif

}

	
#if (UTT_SMART_UI != FYES)
/*MacFilter_edit.asp页面使用，向页面输出一个prof的内容*/
static int getMacFilterOneInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_MAC_FILTER;
    MacFilterProfile *prof = NULL;

    char *editName = NULL;
    const char *ErrMsg = NULL;
    char *tmpmac = NULL;

    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ 
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	prof = ProfGetInstPointByName(mibType, editName);
	if (prof != NULL)
	{
	    websWrite(wp, "username = \"%s\";", prof->head.name);/*用户名*/
	    tmpmac = converMac6To12Byte(prof->mac);
	    websWrite(wp, "macaddr = \"%s\";", tmpmac);/*MAC地址*/
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif

/***********************************************************************
 * 函 数 名：   formDefineMacFilter
 * 创建日期：	2013-4-25
 * 修改日期：
 * 附加说明：	无
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
