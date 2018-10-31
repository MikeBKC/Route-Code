#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if (FEATURE_AP_MAC_FILTER == FYES)

#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "webmsg.h"


#define DEBUG_MF 0
#if DEBUG_MF
#define MF_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define MF_DEBUG(fmt, argx...)
#endif



#define STR_MAC_SIZE   12
#define STR_IP_SIZE    15
#define MAX_ERROR_TYPE 5u
#define ATF_COM_F 0x02

extern char* converMac6To12Byte(MacAddr macAddr);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern const char *urcpErrorMsg[];

int mac_filter_gCount=4;
static unsigned char gMac[STR_MAC_SIZE+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static unsigned char gManaged[64+1];
//static uint32 joinedSZNum = 0u;
static uint32 isRefresh = 0u;

/*
 * 获得当前操作用户MAC地址
 * 通过AP登陆，不判断是否同网段
 */
char* getCurrentUserMacFromAc(webs_t wp, char *macAddr)
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
        if((strcmp(ip,wp->ipaddr) == 0)&&(arp_flags & ATF_COM_F))
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
/********************************************************************
* 函数： aspOutPutApMacFilter
* 功能： 页面与交互转换函数
* 创建： 2012-07-11
* 参数： 略
* 输出： 无
* 返回： 无
* 作者： xhhu 
*******************************************************************/
static int aspOutPutApMacFilterConfUrcp(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrNum = 0,i;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;
	char *szName=NULL,*p=NULL;
    unsigned char *strMac, *strIp, *strPasswd, *waitOver,*strManaged;

	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    szName = websGetVar(wp, "szName", T(""));
    strManaged = websGetVar(wp, "managedEn", T(""));
    websWrite(wp, "var szName = \"%s\";\n", szName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);

	websWrite(wp, "var macConfName = new Array();\n");
	websWrite(wp, "var macNum = new Array();\n");
	websWrite(wp, "var Rules = new Array();\n");
	websWrite(wp, "var SZname = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) 
	{
		prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE))
		{
			websWrite(wp, "macConfName[%d] = \"%s\";", arrNum, prof->head.name);
			websWrite(wp, "macNum[%d] = %d;", arrNum, prof->num);
			if(prof->mode == 1)
			{
				websWrite(wp, "Rules[%d] = \"允许\";", arrNum);
			}
			else if(prof->mode == 2)
			{
				websWrite(wp, "Rules[%d] = \"禁止\";", arrNum);
			}
			/*服务区信息*/
			websWrite(wp, "var SZname%d = new Array();\n",arrNum);
			for(i = 0; i< MAX_SERVICE_ZONE_PROFILES; i++)
			{
				if(prof->sz[i][0] != '\0')
				{
					websWrite(wp, "SZname%d[%d] = \"%s\";",arrNum,i,prof->sz[i]);
				}
			}
			websWrite(wp, "SZname[%d] = SZname%d;",arrNum,arrNum);
			arrNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;", arrNum);
    websWrite(wp,"var max_totalrecs = %d;\n", MAX_SERVICE_ZONE_PROFILES);
	getTheErrorMsg(eid, wp, argc, argv);
}
static int aspOutPutApMacFilter(int eid, webs_t wp, int argc, char **argv)
{
    unsigned int i=0u;
    int tb_index;
    int flag=0;
    uint32 is_set=0u;
    char buf[48];
    unsigned char *strMac=NULL, *strIp=NULL, *strPasswd=NULL,*szName=NULL, *waitOver=NULL,*strManaged = NULL;
    /*判断是否由form跳转过来的*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    /* 空则是第一次打开，非空的话说明是保存后的跳转 */
    if(strcmp(waitOver, T(""))!=0)
    {
	/*页面保存后的跳转*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	strManaged = gManaged;
//    websWrite(wp,"var maxSzNum = %d;\n", mac_filter_gCount);
	is_set =  1u;
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    szName = websGetVar(wp, "szName", T(""));
    strManaged = websGetVar(wp, "managedEn", T(""));
	getErrorArr(wp, urcp_read_ap_mac_filter_conf, MISC_SET_GENERAL);
	is_set = 0u;
	isRefresh = 0u;/*刷新标志位复位*/
    }
    else
    {
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    szName = websGetVar(wp, "szName", T(""));
    strManaged = websGetVar(wp, "managedEn", T(""));
	is_set = 0u;
	isRefresh = 0u;/*刷新标志位复位*/
    }
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", szName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);
    websWrite(wp,"maxrecs[0] = %d;\n", MAX_SERVICE_ZONE_PROFILES);
    return 0;
}
static int aspRemoteApMacFiter(int eid, webs_t wp, int argc, char_t **argv)
{
	uchar *strMac, *strIp, *strPasswd,*strManaged;
	urcp_ap_mac_filter_msg_t urcp_ap_mac_filter_msg;

	memset(&urcp_ap_mac_filter_msg,0,sizeof(urcp_ap_mac_filter_msg));
	memset(gMac, 0, sizeof(gMac));
	memset(gIp, 0, sizeof(gIp));
	memset(gPasswd, 0, sizeof(gPasswd));
	memset(gManaged, 0, sizeof(gManaged));

	isRefresh = 0u;/*刷新标志位复位*/

	strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
	if(strcmp(strMac, T(""))!=0) /*要操作的mac不能为空*/
	{
		/*读取页面变量*/
		strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
		strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/
		strManaged = websGetVar(wp, "managedEn", T(""));/*远程设备密码*/

		/*保存本次操作地址*/
		memcpy(gMac, strMac, sizeof(gMac)-1u);
		memcpy(gIp, strIp, sizeof(gIp)-1u);
		strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);
		strncpy(gManaged, strManaged, sizeof(gManaged)-1u);

		strncpy(urcp_ap_mac_filter_msg.name, websGetVar(wp, "mfNameArray", T("")), 64);
		str2mac(strMac, urcp_ap_mac_filter_msg.general_msg.dst_mac);
		strncpy(urcp_ap_mac_filter_msg.general_msg.password, strPasswd, MAX_PASSWORD_LEN);
		urcp_ap_mac_filter_msg.general_msg.user_ip = ip2int(wp->ipaddr);
		urcp_ap_mac_filter_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */
		sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_AP_MAC_FILTER, (char*)&urcp_ap_mac_filter_msg, sizeof(urcp_ap_mac_filter_msg));
	}
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

#define MAX_IP_LEN 16
/*
 * 批量下发MAC地址过滤
 *
 *
 */
static int aspApBatchMacFilter(int eid, webs_t wp, int argc, char **argv)
{
    uchar *strMac, *strIp, *strPasswd,*strManaged;
    uchar *tmpMac;
    uchar *ptr = NULL;
    urcp_ap_mac_filter_msg_t urcp_ap_mac_filter_msg;

    uchar *ip1,*ip2;
    uchar *sp1,*sp2;

    uchar tmpIp[MAX_IP_LEN];
    uchar tmpSp[MAX_PASSWORD_LEN];
    /*清空内存*/
    memset(&urcp_ap_mac_filter_msg,0,sizeof(urcp_ap_mac_filter_msg));

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
	strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
	strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/
	strManaged = websGetVar(wp, "managedEn", T(""));/*远程设备密码*/


    ip1 = strIp;
    sp1 = strPasswd;

	strncpy(urcp_ap_mac_filter_msg.name, websGetVar(wp, "mfNameArray", T("")), 64);
    tmpMac=strtok_r(strMac,UTT_SPLIT_SIGN_STR,&ptr);
//  printf("tmpMac=%s,szName=%s\n",tmpMac,urcp_ap_mac_filter_msg.name);
    while(tmpMac != NULL)
    {
        memset(tmpIp,0,sizeof(tmpIp));
        memset(tmpSp,0,sizeof(tmpSp));

        ip2 = strchr(ip1,UTT_SPLIT_SIGN_CHAR);
        sp2 = strchr(sp1,UTT_SPLIT_SIGN_SPACES_CHAR);
        sp1++;
        if ( ip2 != NULL ){
            memcpy(tmpIp,ip1,ip2-ip1);
        }else{
            memcpy(tmpIp,ip1,strlen(ip1));
        }
        if ( sp2 != NULL ){
            memcpy(tmpSp,sp1,sp2-sp1);
        }else{
            memcpy(tmpSp,sp1,strlen(sp1));
        }

        str2mac(tmpMac, urcp_ap_mac_filter_msg.general_msg.dst_mac);
        strncpy(urcp_ap_mac_filter_msg.general_msg.password, tmpSp, MAX_PASSWORD_LEN);
	    urcp_ap_mac_filter_msg.general_msg.user_ip = ip2int(wp->ipaddr);
        urcp_ap_mac_filter_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */

        sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_AP_MAC_FILTER, (char*)&urcp_ap_mac_filter_msg, sizeof(urcp_ap_mac_filter_msg));

        tmpMac=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
        if( tmpMac != NULL ){
            *(tmpMac-1) = ',';
        }
        ip1 = ip2 + 1;;
        sp1 = sp2 + 1;
    }
//	getErrorArr(wp, (urcp_fp)(urcp_read_password), MISC_SET_GENERAL);/*打印联动错误信息*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}

typedef struct recoard_filter_info
{
	char szName[UTT_INST_NAME_LEN + 1];
	int macNum;
	int rule;
}szMacInfo;
static void formFitApMacFilterConf(webs_t wp, char_t *path, char_t *query)
{
    MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
	int max, min, index, indexTmp,i,j,k,sz_index;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL,*proftmp=NULL;
	struProfAlloc *profhead = NULL;
	ProfChangeType profType = PROF_CHANGE_ADD;
	char *SZNames=NULL,*szNameTmp=NULL,*cfName=NULL,*mode=NULL,*textarea=NULL,*ptr=NULL,*macStr=NULL,*action=NULL,*oldCfName=NULL;
	char ErrMsg[128],szNameBack[256];
    char* delim = "\n";/*textarea行数据分隔符*/
    int bindNum = 0;/*本次要过滤多少个条目*/
	const char *errMsg = NULL;
	MacAddr macAddr;
	szMacInfo szInfos[10];
	memset(&macAddr, 0, sizeof(macAddr));
	memset(szInfos, 0, sizeof(szInfos));
	memset(ErrMsg, 0, sizeof(ErrMsg));
	memset(szNameBack, 0, sizeof(szNameBack));

	action = websGetVar(wp, "Action", T(""));
	oldCfName = websGetVar(wp, "oldConfName", T(""));
	cfName = websGetVar(wp, "confName", T(""));
	mode = websGetVar(wp, "mode", T(""));
	SZNames = websGetVar(wp, "servicezone", T(""));/*多个匹配服务区*/
	strcpy(szNameBack,SZNames);

    MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
    textarea=websGetVar(wp,T("data"), T(""));/*多条过滤信息*/
//    printf("SZNames=%s,cfName=%s,mode=%s,action=%s,oldCfName=%s\n",SZNames,cfName,mode,action,oldCfName);
//    printf("textarea=%s\n",textarea);
    strRemoveChar(textarea, '\r');/*移除/r*/
    mergeMulSpaEnter(textarea);/*去除多余的空格以及换行*/
    bindNum = strCountChar(textarea, delim[0]) + 1;/*通过查找行数确定用户本次操作条数*/

	/*限制条件判断,填充szInfo[10],统计现有配置信息*/

	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++)
	{
    MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
		proftmp = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, i);
		if((proftmp != NULL)  && (ProfInstIsFree(proftmp) == 0) && (proftmp->head.active == TRUE))
		{
            if ( !strcmp(oldCfName, proftmp->head.name) && ('\0' != *oldCfName) ){
                continue;
            }else{
			sz_index=0;
			while(proftmp->sz[sz_index][0] != '\0')
			{
				for(j = 0; j < 10; j++)
				{
					if(strcmp(proftmp->sz[i],szInfos[j].szName)==0)
					{
						szInfos[j].macNum += proftmp->num;
						szInfos[j].rule = proftmp->mode;
					}
				}
				/*szInfos未记录，添加新纪录*/
				if(j == 10)
				{
					for(k = 0; k < 10; k++)
					{
						if(szInfos[k].szName[0] == '\0')
						{
							strcpy(szInfos[k].szName,proftmp->sz[sz_index]);
							szInfos[k].macNum = proftmp->num;
							szInfos[k].rule = proftmp->mode;
							break;
						}
					}
				}
				sz_index++;
			}
			}
		}
	}

    szNameTmp = strtok_r(SZNames,",",&ptr);/*取得一个服务区名*/
	while(NULL != szNameTmp)
	{
		for(i = 0; i < 10; i++)
		{
			if((szInfos[i].szName[0] != '\0') && (strcmp(szNameTmp,szInfos[i].szName) == 0))
			{
				if( (szInfos[i].macNum + bindNum) > MAX_MAC_FILTER_SIGNAL_AP )
				{
					sprintf(ErrMsg,"%s服务区MAC地址数超限，最大允许%d个MAC地址！",szInfos[i].szName,MAX_MAC_FILTER_SIGNAL_AP);
					setTheErrorMsg(ErrMsg);
					websRedirect(wp, "FitApMacFilter.asp");
					return;
				}
				if( szInfos[i].rule != strtol(mode, NULL, 10))
				{
					sprintf(ErrMsg,"%s服务区规则冲突！",szInfos[i].szName);
					setTheErrorMsg(ErrMsg);
					websRedirect(wp, "FitApMacFilter.asp");
					return;
				}
			}
		}

		szNameTmp=strtok_r(NULL,",",&ptr);/*取得一个服务区名*/
	}

		MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
		/*判断是否导致当前登录者不能登录*/
	char macbuf[512],*macTmpStr=NULL;
	int tmpRule,tmpMacFlag = 0;
    MacAddr macCurrent;

    if (getCurrentUserMacFromAc(wp, (char *)(macCurrent.addr)) != NULL)/*根据操作者ip得到mac地址*/
	{
	    memset(macbuf,0,sizeof(macbuf));
		tmpRule = strtol(mode, NULL, 10);
	    strcpy(macbuf,textarea);
        macTmpStr=strtok_r(macbuf,delim,&ptr);/*取得一行*/
        while(NULL!=macTmpStr)
        {
	    strRemoveChar(macTmpStr, ':');
//printf("usermac=%s,mac=%s\n",converMac6To12Byte(macCurrent),macTmpStr);
	    converMac12To6Byte(macTmpStr, &macAddr);/*转化为6字节类型*/
		if(memcmp(&macCurrent,&macAddr,sizeof(MacAddr)) == 0)
		{
		    tmpMacFlag = 1;
            if(tmpRule == 2)//禁止规则，判断mac地址相同则返回
			{
	            errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	            setTheErrorMsgOnFree(errMsg);
				websRedirect(wp, "FitApMacFilter.asp");
				return;
			}
		}
		macTmpStr=strtok_r(NULL, delim, &ptr);/*读取下一行内容*/
		}
//printf("tmpMacFlag = %d\n",tmpMacFlag);
		if(tmpMacFlag == 0)//没有找到与登录MAC相同的MAC地址
		{
            if(tmpRule == 1)//允许规则,导致无法登陆
			{
	            errMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	            setTheErrorMsgOnFree(errMsg);
				websRedirect(wp, "FitApMacFilter.asp");
				return;
			}
		}
	}

	if(strcmp(action,"modify") == 0)
	{
		/*修改*/
		indexTmp = ProfGetInstIndexByName(mibType, oldCfName);

		index = indexTmp;
		profType = PROF_CHANGE_EDIT;
	}else if(strcmp(action,"add") == 0)
	{
		/*添加*/
		index = ProfNewInst(mibType, cfName, FALSE);
		profType = PROF_CHANGE_ADD;
	}
    prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType,index);
	ProfBackupByIndex(mibType, profType, index, &profhead);/*备份修改之前的*/
	prof->head.active = TRUE;

	MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
	/*保存MAC地址*/

	memset(prof->mac,'\0',sizeof(prof->mac));
    macStr=strtok_r(textarea,delim,&ptr);/*取得一行*/
    /*循环读取textarea数据*/
	i = 0;
    while(NULL!=macStr)
    {

	    strRemoveChar(macStr, ':');
	    converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
//        printf("macStr=%s\n",macStr);
		memcpy(prof->mac[i].addr,macAddr.addr,sizeof(macAddr.addr));
		macStr=strtok_r(NULL, delim, &ptr);/*读取下一行内容*/
		i++;
	}
		MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);

    szNameTmp=strtok_r(szNameBack,",",&ptr);/*取得一个服务区名*/
	i = 0;
	memset(prof->sz,'\0',sizeof(prof->sz));
    while(NULL != szNameTmp)
    {

 MF_DEBUG("szNameTmp=%s\n",szNameTmp);
		MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
		strcpy(prof->sz[i],szNameTmp);
		szNameTmp=strtok_r(NULL,",",&ptr);/*取得一个服务区名*/
		i++;
	}
	strcpy(prof->head.name,cfName);
	prof->num = bindNum;
	prof->mode = strtol(mode, NULL, 10);
    ProfFreeAllocList(profhead);/*释放备份控件*/

    nvramWriteCommit();/*写flash*/
    websRedirect(wp, "FitApMacFilter.asp");
	return;

}
static void formApMacFilterDel(webs_t wp, char_t *path, char_t *query)
{
	char *delStr = NULL, *tmp=NULL;
	int min, max, index = 0, change = 0;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;
    MacAddr macCurrent;
	int i,delFlag = 1;

    MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);
	delStr = websGetVar(wp,T("delstr"),T(""));
	if (NULL == delStr) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_GROUP);
		setTheErrorMsg((char *)ErrMsg);
        websRedirect(wp, "FitApMacFilter.asp");
	    return;
	}
    MF_DEBUG("\n---> %s <---%d\n", __func__,__LINE__);

	ProfInstLowHigh(mibType, &max, &min);

	tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
	while(tmp != NULL) {
		for (index = min; index < max; index++)
		{
		    delFlag = 1;//是否删除的标志
			prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
			if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active))
			{
    MF_DEBUG("\n---> %s <---%d,cfName=%s,tmp=%s\n", __func__,__LINE__,prof->head.name,tmp);
                if (strcmp(prof->head.name, tmp) == 0)
				{
                    if (getCurrentUserMacFromAc(wp, (char *)(macCurrent.addr)) != NULL)/*根据操作者ip得到mac地址*/
					{
			           for(i = 0; i< prof->num; i++)
					   {
		                   if(memcmp(&macCurrent,&prof->mac[i],sizeof(MacAddr)) == 0)//操作地址是否在删除的地址组内
						   {
							   if(prof->mode == 1)//该prof的规则为允许，则删除失败
							   {
							       delFlag = 0;
	                               ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
	                               setTheErrorMsgOnFree(ErrMsg);
							   }
						   }
					   }
					}
                    if(delFlag == 1)
					{
                    ProfBackupByIndex(mibType, PROF_CHANGE_DEL, index, &profhead);
					ProfDelInstByIndex(mibType, index);
                    change = 1;
					}
                }
			}
		}

		tmp = strtok(NULL, UTT_SPLIT_SIGN_STR);
	}
	if (1 == change) {
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
	}
    websRedirect(wp, "FitApMacFilter.asp");
	return;
}
static void formApMacFilterDelAll(webs_t wp, char_t *path, char_t *query)
{
	int min, max, index = 0;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;
    MacAddr macCurrent;
	int i,delFlag = 1;

	ProfInstLowHigh(mibType, &max, &min);

	for (index = (min); index < max; index++) {
		delFlag = 1;//是否删除的标志
		prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)) && (TRUE == prof->head.active))
		{
            if(getCurrentUserMacFromAc(wp, (char *)(macCurrent.addr)) != NULL)/*根据操作者ip得到mac地址*/
			{
	           for(i = 0; i< prof->num; i++)
			   {
                   if(memcmp(&macCurrent,&prof->mac[i],sizeof(MacAddr)) == 0)//操作地址是否在删除的地址组内
				   {
					   if(prof->mode == 1)//该prof的规则为允许，则删除失败
					   {
					       delFlag = 0;
                           ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
                           setTheErrorMsgOnFree(ErrMsg);
					   }
				   }
			   }
			}
            if(delFlag == 1)
			{
			ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, index, &profhead);
			ProfDelInstByIndex(mibType, index);
			}
		}
	}

	ProfFreeAllocList(profhead);
	nvramWriteCommit();

	websRedirect(wp, "FitApMacFilter.asp");
	return;
}
static int aspOutPutFitApMacFilterConf(int eid, webs_t wp, int argc, char **argv)
{
	int i, min, max, index, arrNum = 0;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;
	char *tmpmac = NULL;

	websWrite(wp, "var SZname = new Array();\n");
	websWrite(wp, "var macConfName = new Array();\n");
	websWrite(wp, "var macData = new Array();\n");
	websWrite(wp, "var Rules = new Array();\n");

	ProfInstLowHigh(mibType, &max, &min);

	for (index = min; index < max; index++) {
		prof = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE))
		{
			websWrite(wp, "macConfName[%d] = \"%s\";", arrNum, prof->head.name);
			if(prof->mode == 1)
			{
				websWrite(wp, "Rules[%d] = \"允许\";", arrNum);
			}
			else if(prof->mode == 2)
			{
				websWrite(wp, "Rules[%d] = \"禁止\";", arrNum);
			}
			/*服务区信息*/
			websWrite(wp, "var SZname%d = new Array();\n",arrNum);
			for(i = 0; i< MAX_SERVICE_ZONE_PROFILES; i++)
			{
				if(prof->sz[i][0] != '\0')
				{
					websWrite(wp, "SZname%d[%d] = \"%s\";",arrNum,i,prof->sz[i]);
				}
			}
			websWrite(wp, "SZname[%d] = SZname%d;",arrNum,arrNum);

			/*MAC地址信息，复制按钮用到*/
			websWrite(wp, "var macData%d = new Array();\n",arrNum);
			for(i = 0; i< prof->num; i++)
			{
				tmpmac = converMac6To12Byte(prof->mac[i]);
				websWrite(wp, "macData%d[%d] = \"%s\";",arrNum,i,tmpmac);
			}
			websWrite(wp, "macData[%d] = macData%d;",arrNum,arrNum);
			arrNum++;
		}
	}

	websWrite(wp, "var totalrecs = %d;", arrNum);
    websWrite(wp,"var max_totalrecs = %d;\n", MAX_SERVICE_ZONE_PROFILES);
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;

}
static int getOneServiceZoneMacFilter(int eid, webs_t wp, int argc, char **argv)
{
	int index,i=0;
	char *edit_Name;
	char *tmpmac = NULL;
	MibProfileType mibType = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *prof = NULL;


	ejArgs(argc, argv, T("%s"), &edit_Name);

	prof = (ApMacFilterProfile*) ProfGetInstPointByName(mibType, edit_Name);
	if (prof != NULL) {
		websWrite(wp, "var confName = \"%s\";", prof->head.name);
		websWrite(wp, "var macNum = \"%d\";", prof->num);
		websWrite(wp, "var mode = \"%d\";", prof->mode);
		websWrite(wp, "var SZnames = new Array();\n");
		for(index = 0;index<MAX_SERVICE_ZONE_PROFILES;index++)
		{
			if(prof->sz[index][0] != '\0')
			{
				websWrite(wp, "SZnames[%d] = \"%s\";",i, prof->sz[index]);
				i++;
			}
		}
		websWrite(wp, "var macData = new Array();\n");/*打印MAC地址*/
		for(index = 0;index < prof->num;index++)
		{
			tmpmac = converMac6To12Byte(prof->mac[index]);
			websWrite(wp, "macData[%d] = \"%s\";",index,tmpmac);
		}
	}

	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}
static int getServiceZone(int eid, webs_t wp, int argc, char **argv)
{
	int min, max, index, arrNum = 0;
	MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *prof = NULL;

	ProfInstLowHigh(mibType, &max, &min);
	websWrite(wp, "var SZones= new Array();\n");

	for (index = min; index < max; index++) {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE)) {
			if (prof->serviceEn == PROF_ENABLE) {
		        websWrite(wp, "SZones[%d] = \"%s\";", arrNum, prof->head.name);
			    arrNum++;
			}
		}
	}

    websWrite(wp,"var max_totalrecs = %d;\n", MAX_MAC_FILTER_SIGNAL_AP);/*MAC地址可添加的最大条目数*/
	getTheErrorMsg(eid, wp, argc, argv);
	return 0;
}

extern void formDefineApMacFilter(void)
{
	websFormDefine(T("formFitApMacFilter"), formFitApMacFilterConf);
    websFormDefine(T("formApMacFilterDel"), formApMacFilterDel);
    websFormDefine(T("formApMacFilterDelAll"), formApMacFilterDelAll);
    websAspDefine(T("aspOutPutFitApMacFilterConf"), aspOutPutFitApMacFilterConf);
    websAspDefine(T("getServiceZone"), getServiceZone);
    websAspDefine(T("getOneServiceZoneMacFilter"), getOneServiceZoneMacFilter);

    websAspDefine(T("aspOutPutApMacFilter"), aspOutPutApMacFilter);
    websAspDefine(T("aspOutPutApMacFilterConfUrcp"), aspOutPutApMacFilterConfUrcp);
    websAspDefine(T("aspRemoteApMacFiter"), aspRemoteApMacFiter);
    websAspDefine(T("aspApBatchMacFilter"), aspApBatchMacFilter);
}
#endif
