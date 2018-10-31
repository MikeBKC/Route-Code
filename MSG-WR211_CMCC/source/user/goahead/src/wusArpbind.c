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
#define ARPPD_NAME_H "arpd_" /*arp默认用户名前缀*/
#define MAX_ARP_FILE_BUF	MAX_USER_PROFILES*(UTT_INST_NAME_LEN + 15/*最长的ip地址*/ + 17/*mac*/ +4)
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
 * 函 数 名：   getEntryByIpFromSysArpTb
 * 功能描述：	根据传入的ip地址查找arp表中的相应的条目，并返回
 * 输入参数:    ipStr - 待查找的ip地址
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
	    if(strlen(ret) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
	    {
		i=0;
		while((ret[i]!=' ') && (i<sizeof(ret)))
		{
		    i++;
		}
		ret[i]='\0';
		if(!strcmp(ipStr,ret))/*ip新地址相同*/
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
 * 根据ip地址从系统arp表中查找其对应的mac地址
 * retun :成功则返回对应的mac地址  否则返回NULL
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
    arpEntryP = getEntryByIpFromSysArpTb(arpIp);/*读取ip地址对应的系统arp条目*/

    if (arpEntryP!=NULL)/*读取成功*/
    {
	strncpy(buf, arpEntryP, sizeof(buf) -1u);/*做一份拷贝防止读取到的条目被更改*/

	tmptok1 = strtok_r(buf,split,&ptr);
	while(tmptok1!=NULL)
	{
	    ct++;
	    if(ct==4) {/*第四项为对应的mac地址*/
		strcpy(arpMac,tmptok1);
		break;
	    }
	    tmptok1 = strtok_r(NULL, split,&ptr);
	}
    }

    if(arpMac[0] != '\0')
    {
	ptr = arpMac;/*找到对应的mac地址*/
    }
    else
    {
	ptr = NULL;/*未找到对应的mac地址*/
    }

    return ptr;

}

/**
 * 从现有配置获取arp绑定是否允许非绑定用户连接
 * bhou
 * 2011-10-28
 */
static bool getArpAllowOtherEn(void)
{
    InterfaceProfile *prof = NULL;
    bool ret = 1;/*默认为允许*/


    prof = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, 0);/*获取lan口*/

    if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*配置已用*/
    {
	if(prof->arpAllowOtherEn == FUN_DISABLE)/*不允许*/
	{
	    ret = 0;
	}
    }

    return ret;

}
/***********************************************************************
 * 函 数 名：   getArpBindGlobalConfig
 * 功能描述：   向页面输出全局配置中的参数信息（允许非ARP绑定用户）
 * 创建日期：	2011-10-25
 * 修改日期：
 * 作者：       bhou
 * 附加说明：	无
 ***********************************************************************/
static int getArpBindGlobalConfig(int eid, webs_t wp, int argc, char_t **argv)
{
    if(getArpAllowOtherEn()==0) /*不允许非绑定用户连接*/
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
 * 函 数 名：   getArpBindList
 * 功能描述：   向页面输出ARP绑定配置信息
 * 创建日期：	2010-3-17
 * 修改日期：   2011-10-24 bhou
 * 作者：       柏德秀
 * 附加说明：	无
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
	    websWrite(wp, UserNames, totalrecs, prof->head.name);/*用户名*/
	    addrIp.s_addr = prof->ip;

	    websWrite(wp, ips, totalrecs, inet_ntoa(addrIp));/*ip*/

	    websWrite(wp, macs, totalrecs, converMac6To17Byte(prof->mac));/*mac*/

	    websWrite(wp, allows, totalrecs, (prof->bindEn == FUN_DISABLE?"no":"yes"));/*允许*/
	    totalrecs++;
	}
    }

    websWrite(wp,"var totalrecs=%d;", totalrecs);/*已绑定个数*/
    websWrite(wp,"var max_totalrecs=%d;", max);/*最大绑定个数*/
    return 0;
}

/***********************************************************************
 * 函 数 名：   getArpBindList
 * 功能描述：   向页面输出ARP绑定配置信息
 * 创建日期：	2010-3-17
 * 修改日期：   2011-10-24 bhou
 * 作者：       柏德秀
 * 附加说明：	无
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
 * 函 数 名：   getOneArpBindConfig
 * 功能描述：   用于修改时向页面输出所修改的ARP绑定配置信息
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 修改：       bhou(2011-10-24)
 * 附加说明：	无
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
	    websWrite(wp, "var UserNames=\"%s\";", prof->head.name);/*用户名*/
	    addrS.s_addr = prof->ip;
	    websWrite(wp, "var arp_IPs=\"%s\";", inet_ntoa(addrS));/*ip地址*/
	    websWrite(wp, "var arp_Macs=\"%s\";", converMac6To17Byte(prof->mac));/*mac地址*/
	    websWrite(wp, "var Allows=\"%s\";", (prof->bindEn == FUN_DISABLE?"no":"yes"));/*允许*/
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);/*错误信息*/
    return 0;
}


/**
 * arp绑定全局设置。操作合法性验证
 * 当不允许非绑定用户连接到设备时。当前访问者必须在绑定列表中，才合法
 * bhou
 * 2011-10-27
 */
static bool arpBindGlobalConfigCheck(webs_t wp)
{
    bool checkRet = 1;/*默认验证通过*/
    char *allowEnable;
    char *clientIp = wp->ipaddr;/*当前连接服务器的用户ip*/
    char *clientMac = NULL;/*当前连接服务器的用户mac*/
    ARP_MATCH matchRet = MATCH_NONE;
    MacAddr macAddr;
    struct in_addr addrS;
    int tmp;
    const char *errMsg=NULL;

    memset(&macAddr, 0, sizeof(MacAddr));
    memset(&addrS, 0, sizeof(addrS));
    allowEnable = websGetVar(wp, T("AllowOtherEnable"), T(""));

    /*验证本次操作是否合法*/
    if(0 != strcmp(allowEnable, "on"))/*不允许非绑定用户连接*/
    {
	/*访问者ip地址是否与lan口同网段*/
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
	{/*判断失败*/
	    checkRet = 0;
	}
	else
	{
	    /*只有访问者ip地址与lan口即arp绑定的端口网段相同。才有验证的必要*/
	    if(tmp == 1)	
	    {/*同网段*/
		checkRet = 0;/*需在静态绑定中查找到当前访问者arp信息。才算验证通过*/
		clientMac = getMacByIpFromSysArpTb(clientIp);/*从系统arp表读取对应的mac地址*/
		if(clientMac != NULL)
		{
		    strRemoveChar(clientMac, ':');/*移除：号*/
		    if (0 != inet_aton(clientIp, &addrS))/*转换为二进制形式*/
		    {
			converMac12To6Byte(clientMac, &macAddr);
			matchRet = matchFromArpBinds(addrS.s_addr, macAddr, NULL);/*在arp静态绑定表中查找该信息*/
			if(matchRet == MATCH_NONE)
			{
			    /*ip和mac都不匹配*/
			    errMsg = getTransDataForC(C_LANG_INDEX_ALLOW_BIND_USER);
			    setTheErrorMsgOnFree(errMsg);
			}
			else if(matchRet == MATCH_IP_ONLY)
			{
			    /*找到ip匹配项。但mac地址不匹配*/
			    errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_MAC);
			    setTheErrorMsgOnFree(errMsg);
			}
			else if(matchRet == MATCH_MAC_ONLY)
			{
			    /*找到mac地址匹配项。但ip地址不匹配*/
			    errMsg = getTransDataForC(C_LANG_INDEX_NO_SAME_IP);
			    setTheErrorMsgOnFree(errMsg);
			}
			else
			{
			    checkRet = 1;/*匹配ip和mac，则允许本次操作。否则当前操作的用户将无法访问设备*/
			}

		    }
		}
	    }
	}
    }

    return checkRet;
}
/***********************************************************************
 * 函 数 名：   formConfigArpBindGlobalConfig
 * 功能描述：   保存全局配置
 * 创建日期：	2010-3-17
 * 修改日期：   2011-10-24 bhou
 * 作者：       柏德秀
 * 附加说明：	无
 ***********************************************************************/
static void formConfigArpBindGlobalConfig(webs_t wp, char_t *path, char_t *query)
{
    char *allowEnable;
    InterfaceProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_INTERFACE;
    struProfAlloc *profhead = NULL;
    FunEnableEnum allowEnEnum = FUN_ENABLE;/*默认为允许其他人连接到设备*/

    allowEnable = websGetVar(wp, T("AllowOtherEnable"), T(""));

    if(0 != strcmp(allowEnable, "on"))/*不允许非绑定用户连接*/
    {
	allowEnEnum = FUN_DISABLE;/*不允许非绑定用户连接*/
    }

    /*合法则写配置并生效*/
    if(arpBindGlobalConfigCheck(wp) != 0)
    {
	prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 0);/*获取lan口*/

	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*配置已用*/
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/
	    prof->arpAllowOtherEn = allowEnEnum;/*是否允许非绑定用户访问*/
	}

	/*只有当备份数据与配置不符，goahead才会发消息*/
	ProfUpdate(profhead);/*发消息更新*/
	ProfFreeAllocList(profhead);/*释放malloc的内存*/
	nvramWriteCommit();/*写flash*/
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
 * argvs   :        ipaddr 扫描地址
 *                  netMask 扫描ip掩码
 *                  ifName  发送包的端口
 * Data:            2011-10-31
 * Author:          bhou
 * */
static int sendArpRequest(IPADDR ipaddr, IPADDR netMask, char*ifName)
{
    char *ipStr = NULL;
    struct in_addr addrS;
    IPADDR ipStart = 0u, ipEnd = 0u;
    int count = 0;

    ipaddr = ntohl(ipaddr);/*转换为主机字节序*/
    netMask = ntohl(netMask);/*转换为主机字节序*/
    ipStart = (ipaddr & netMask) + 1u;/*arp扫描起始地址*/
    ipEnd = (ipaddr | (~netMask)) - 1u;/*arp扫描结束地址*/

    doSystem("ip neigh flush dev %s", ifName);/*先清空arp表*/
    while(ipStart <= ipEnd) 
    {
	addrS.s_addr = htonl(ipStart);/*转换为网络字节序*/
	ipStr = inet_ntoa(addrS);/*转换为字符串形式*/
	if(ipStr != NULL)
	{
	    if (++count == 30) {
		sleep(1);
		count = 0;
	    }
	    doSystem("arping -q -I %s %s -c 1 &",ifName,ipStr);/*利用arping发送广播包*/
	}
	ipStart++;
    }

    return 0;
}

/*
 * FunctionName:    readScanArp
 * Function:        从系统arp表中读取arp扫描信息 
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
    IPADDR netMask = convertLenToNetMask(24u);/*转化为二进制ip地址形式掩码*/

    memset(ipStr, 0, sizeof(ipStr));
    memset(macStr, 0, sizeof(macStr));
    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    memset(&tmpMacStr, 0, sizeof(tmpMacStr));

    scanNetStr = websGetVar(wp, T("scanNet"), T(""));/*扫描网段*/
    if(*scanNetStr != '\0')/*扫描后跳转过来的*/
    {
	arpScanNet = strtoul(scanNetStr, NULL, 16);/*转化为无符号整形*/
	fp=fopen("/proc/net/arp","r");/*读取系统arp表*/
	while(!feof(fp))
	{
	    memset(buf, 0, sizeof(buf));
	    fgets(buf, sizeof(buf), fp);/*读取一条arp条目*/
	    if(strlen(buf) >= ARP_VALID_MIN_LEN)/*合法arp条目*/
	    {
		/*从arp条目中解析出来ip以及mac地址*/
		if(getIpMacFromSysArpEntry(buf, ipStr, macStr, getLanIfName())!=0)
		{
		    strncpy(tmpMacStr, macStr, sizeof(tmpMacStr) - 1u);
		    strRemoveChar(tmpMacStr, ':');/*移除mac地址中的连接符*/
		    converMac12To6Byte(tmpMacStr, &macAddr);/*转化为6字节形式mac地址*/
		    if(inet_aton(ipStr, &addrS)!=0)/*转换为二进制ip地址*/
		    {
			if((addrS.s_addr & netMask) == arpScanNet)/*与扫描ip同网段*/
			{
			    /*与静态绑定条目不相同的条目才显示到页面上*/
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
	fclose(fp);/*关闭文件流*/
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

    ipStr=websGetVar(wp,T("ipAddr"), T(""));/*获取扫描ip地址*/
    if(0 == inet_aton(ipStr, &addrS))/*转换为二进制形式*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	/*默认按网段24扫描*/
	netMask = convertLenToNetMask(24u);/*转化为二进制ip地址形式掩码*/
	sendArpRequest(addrS.s_addr, netMask, getLanIfName());/*发送arp查询广播包*/
	sleep(3);/*等待arp回复信息*/
    }
#if (UTT_SMART_UI == FYES)
    sprintf(retUrl, "IPMacConfigSui.asp?scanNet=%08x", (netMask&addrS.s_addr));/*将扫描网段记录*/
#else 
    sprintf(retUrl, "IPMacConfig.asp?scanNet=%08x", (netMask&addrS.s_addr));/*将扫描网段记录*/
#endif   
    websRedirect(wp,retUrl );/*页面跳转*/

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

    if (-1 == getIfIp(getLanIfName(), lan_addr)) /*获取lan口ip地址*/
    {
	lan_addr[0]='\0';
    }

    ProfInstLowHigh(MIB_PROF_USER, &max, &min);/*获取实例范围*/
    websWrite(wp,"i_old_ip[0] = \"%s\";\n",lan_addr);
    websWrite(wp,"var totalrecs= %d;\n", ProfCountInstNotFree(MIB_PROF_USER));/*已用实例个数*/ 
    websWrite(wp,"var maxtotalrecs = %d;\n",max);/*最大实例个数*/
    readScanArp(eid,wp,argc,argv);
    getTheErrorMsg(eid, wp, argc, argv);  
    return 0;
}
/**
 * 从页面textarea的一行中解析出来 ip地址。mac地址。以及用户名
 * bhou
 * 2011-11-03
 * 附加说明：rowStr的值会被改变
 */
static bool parseArpImuFromTaRow(char* rowStr, char** ipStr, char** macStr, char** userName)
{
    bool parseRet = 0;/*解析结果默认为失败*/
    char* split =" ";/*页面一行数据中各个单元的分割符*/
    char* ptr = NULL;

    if(rowStr != NULL)
    {
	if(strlen(rowStr)>= ARP_VALID_MIN_LEN) /*合法arp条目*/
	{
	    *ipStr  = strtok_r(rowStr, split, &ptr);/*ip字串*/
	    if(*ipStr != NULL)
	    {

		*macStr  = strtok_r(NULL, split, &ptr);/*mac字串*/
		if(*macStr != NULL)
		{
		    *userName  = strtok_r(NULL, split, &ptr);/*用户名*/
		    if((*userName != NULL) && (**userName == '\0'))
		    {/*非法用户名*/
			*userName = NULL;
		    }
		    parseRet = 1;/*用户名可以为空。故到此可认为解析成功*/
		}

	    }
	}
    }
    return parseRet;
}

/*
 * FunctionName:    formAddReadArp
 * Function:        一次增加多条ip/mac静态绑定 
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
    char* delim = "\n";/*textarea行数据分隔符*/
    int bindNum = 0;/*本次要绑定多少个条目*/
    int successNum = 0;/*成功绑定的条目数*/
    int back = 0;
    int maxDeNameNo = 0 , aDNametag = 0;
    char* userIp = wp->ipaddr;
    const char *errMsg=NULL;
    const char *total=NULL;
    const char *totalNum=NULL;
    const char *failNum=NULL;

    memset(&macAddr, 0, sizeof(macAddr));
    memset(&addrS, 0, sizeof(addrS));
    textarea=websGetVar(wp,T("data"), T(""));/*ip/mac多条绑定信息*/

    strRemoveChar(textarea, '\r');/*移除/r*/
    mergeMulSpaEnter(textarea);/*去除多余的空格以及换行*/
    bindNum = strCountChar(textarea, delim[0]) + 1;/*通过查找行数确定用户本次操作共需绑定arp的条数*/

    rowStr=strtok_r(textarea,delim,&ptr);/*取得一行arp绑定内容*/
    /*循环读取textarea数据并进行绑定*/
    while(NULL!=rowStr)
    {
	/*从一行信息中解析出来ip,mac以及用户名*/
	if(parseArpImuFromTaRow(rowStr, &ipStr, &macStr, &userName) != 0)
	{
	    strRemoveChar(macStr, ':');
	    if (0 != inet_aton(ipStr, &addrS))/*转换为二进制形式*/
	    {
#if (DHCP_POOL == FYES)
		ProfInstLowHigh(mibTypeDhcp, &max, &min); /* 在内存中寻找地址池信息 */
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

		converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
		instIndex = ProfGetInstIndexFree(mibType);/*获取一个空闲实例*/
		if(instIndex == PROFFAIL) /*获取不到则证明已达最大绑定条目*/
		{
		    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		    setTheErrorMsgOnFree(errMsg);
		    break;
		}
		else
		{/*获取成功*/
		    if(arpBindAddAccessCheck(userIp, ipStr, macStr)!=0)/*本次绑定是否可能使当前登陆者，无法登录*/
		    {
			userIp = NULL;/*下次验证不用重读系统arp表*/
			aDNametag = 0;/*本次是否使用默认用户名添加*/
			if(userName == NULL)/*用户名没输入*/
			{
			    if(maxDeNameNo == 0)
			    {
				maxDeNameNo = ProfGetMaxDefaultNameNo(mibType, ARPPD_NAME_H);/*获取最大默认用户名编号*/
			    }
			    userName = ProfBuildDefaultName(ARPPD_NAME_H, maxDeNameNo);/*生成默认用户名*/
			    aDNametag = 1;/*标志生成一个默认用户名*/
			}
			if(arpBindUniCheck(userName, addrS.s_addr, macAddr, PROF_UNUSED_INDEX)!=0)/*静态绑定重复性判断*/
			{
#if (DHCP_POOL == FYES)
			    ProfInstLowHigh(mibTypeDhcp, &max, &min); /* 在内存中寻找地址池信息 */
			    for (index = min; index < max; index++) {
				profDhcp = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypeDhcp, index);
				if ((NULL!= profDhcp) && (0 == ProfInstIsFree((Pointer)profDhcp)) && (TRUE == profDhcp->head.active)) {
				    if (!strcmp(profDhcp->head.name,"default")){
					if ((ntohl(addrS.s_addr)&ntohl(profDhcp->netmask)) == (ntohl(profDhcp->ipStart)&ntohl(profDhcp->netmask))){
					    break;
					}
				    }

				    if ((ntohl(addrS.s_addr) >= ntohl(profDhcp->ipStart)) && (ntohl(addrS.s_addr) <= ntohl(profDhcp->ipEnd))){
					break;  /*找到地址池，退出循环*/
				    }
				}
			    }
			    if( index<max ){
#if 0
				if( profDhcp->ipStart>addrS.s_addr ||  profDhcp->ipEnd < addrS.s_addr){
				    setTheErrorMsgOnFree(T("IP不在DHCP地址池内！"));
				    break;
				}
#endif
#endif
				if(bindNum < 6)
				{/*若总绑定条目数不是太大。则每次添加都备份并发消息*/
				    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份新创建的空实例*/
				}
				else
				{/*本次绑定条目数过大。则只备份一次。只发一次消息*/
				    if(back == 0 )
				    {/*备份类型为 添加多条。则speedweb会重新初始化arp相关*/
					ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
					back = 1;
				    }
				}
				successNum++;/*操作成功的次数*/
				prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*获取实例指针*/
				ProfSetNameByPoint(prof, userName);/*设置用户名*/
				prof->bindEn = FUN_ENABLE;/*默认为允许访问*/
				prof->ip = addrS.s_addr;/*静态绑定ip地址*/
				prof->mac = macAddr;/*静态绑定mac地址*/
				maxDeNameNo+=aDNametag;/*若默认用户名生成，并被使用了，则最大默认用户名编号加一*/
#if (DHCP_POOL ==FYES)
				strcpy(prof->dhcpPoolName, profDhcp->head.name);
				prof->vid = profDhcp->vid;
			    } else {
				if(bindNum < 6)
				{/*若总绑定条目数不是太大。则每次添加都备份并发消息*/
				    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份新创建的空实例*/
				}
				else
				{/*本次绑定条目数过大。则只备份一次。只发一次消息*/
				    if(back == 0 )
				    {/*备份类型为 添加多条。则speedweb会重新初始化arp相关*/
					ProfBackupByIndex(mibType, PROF_CHANGE_ADD_ALL, instIndex, &profhead);
					back = 1;
				    }
				}
				successNum++;/*操作成功的次数*/
				prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*获取实例指针*/
				ProfSetNameByPoint(prof, userName);/*设置用户名*/
				prof->bindEn = FUN_ENABLE;/*默认为允许访问*/
				prof->ip = addrS.s_addr;/*静态绑定ip地址*/
				prof->mac = macAddr;/*静态绑定mac地址*/
				maxDeNameNo+=aDNametag;/*若默认用户名生成，并被使用了，则最大默认用户名编号加一*/
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
	rowStr=strtok_r(NULL, delim, &ptr);/*读取下一行内容*/
    }    

    ProfUpdate(profhead);/*发消息生效*/
    ProfFreeAllocList(profhead);/*释放备份控件*/
    nvramWriteCommit();/*写flash*/

    if(bindNum > 1)/*多行绑定因无法全部提示，故采用以下提示方式*/
    {
	if(bindNum != successNum)
	{
	    total = getTransDataForC(C_LANG_INDEX_TOTAL_BIND);
	    totalNum = getTransDataForC(C_LANG_INDEX_TOTAL_BIND_NUM);
	    failNum = getTransDataForC(C_LANG_INDEX_FAIL_BIND_NUM);
	    /*本次绑定有失败的条目*/
	    sprintf(tmpStr, "：%s%d%s%d%s", total, bindNum, totalNum, (bindNum - successNum), failNum);
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
 * arp静态绑定操作。删除条目时。判断删除该条目是否可能导致，当前登录者，无法访问该设备
 * bhou
 * clientIp:当前登录者ip地址 delIp:要删除的ip地址
 * 2011-10-29
 */
static bool arpBindDelAccessCheck(char* clientIp, IPADDR delIp)
{
    bool ret = 1;
    int tmp = 0;
    const char *errMsg=NULL;

    struct in_addr addrS;
    memset(&addrS, 0, sizeof(addrS));

    /*只有在不允许非绑定用户连接的情况下。才不能随便做删除操作*/
    if(getArpAllowOtherEn()==0)
    {
	if(inet_aton(clientIp, &addrS)!=0)/*转化ip地址*/
	{
	    if(delIp == addrS.s_addr)/*因之前绑定项若ip地址相同则mac必相同。故不必判断mac地址是否相同*/
	    {
#if(FEATURE_AC == FYES)
        tmp = 1;
#else
#if (MULTI_LAN == FYES)
		tmp = isIpInLanNet(clientIp);
#else
		tmp=isIpInIfNet(addrS.s_addr, getLanIfName());/*是否与lan口ip同网段*/
#endif
#endif
		if(tmp == 0)
		{/*不同网段。则可以随便删除*/
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

    return ret;/*成功则返回欲删除的索引*/

}

/**
 * arp静态绑定。增加条目时。判断该操作是否可能导致当前登录者。无法访问该设备
 * bhou
 * userIp:当前登录者ip地址 newIp:新添加的ip地址 newMac:新添加的mac地址
 * 当userIp为NULL时。表示本次验证跟上次验证相比。网络环境无变化（访问者arp信息无变化且lan口ip网段无变化）
 * 2011-10-29
 */
static bool arpBindAddAccessCheck(char*userIp,char* newIp, char*newMac)
{
    static char *clientMac = NULL;/*当前连接服务器的用户mac*/
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
	cmpLanIpNet = isIpStrInIfNet(clientIp, getLanIfName());/*是否与lan口ip同网段*/
#endif
#endif
	if(cmpLanIpNet == 1)/*同网段需验证*/
	{
	    clientMac = getMacByIpFromSysArpTb(clientIp);/*从系统arp表读取对应的mac地址*/
	    if(clientMac != NULL)
	    {
		strRemoveChar(clientMac, ':');/*移除：号*/
	    }
	}
    }
    if(cmpLanIpNet == 0)
    {/*不同网段。则随便绑定*/
	ret = 1;
    }
    else
    {
	if((cmpLanIpNet == 1) && (clientMac!=NULL))
	{/*同网段且arp信息读取正确*/
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
 * arp静态表编辑添加时。判断 userName, ip, mac是否与其他项目冲突
 * editIndex:当前编辑条目的索引。若为增加可为PROF_UNUSED_INDEX
 * userName,ip,mac 当前新增或欲更改为的值
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
	/*每个ip与mac地址都只能绑定一次*/
	matchRet = matchFromArpBinds(ip, mac, &editIndex);/*匹配arp静态绑定表*/
	if(matchRet == MATCH_IP_ONLY)/*ip地址匹配。mac地址不匹配*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_IP_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else if(matchRet == MATCH_MAC_ONLY)/*ip地址不匹配。mac地址匹配*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_MAC_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else if(matchRet == MATCH_ALL)/*ip地址匹配。mac地址匹配*/
	{
	    errMsg = getTransDataForC(C_LANG_INDEX_IPMAC_HAVE_BINDED);
	    setTheErrorMsgOnFree(errMsg);
	}
	else
	{
	    /*都不匹配,即不存在重复项*/
	    ret = 1;
	}
    }
    return ret;
}
/**
 * arp静态绑定添加操作
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

    /*从页面读取各控件的值*/

    dhcpPoolName = websGetVar(wp, T("pools"), T(""));
#if (DHCP_POOL == FYES)
    char *dhcpVid;
    dhcpVid = websGetVar(wp, T("dhcpVid"), T(""));
#endif
    userName=websGetVar(wp,T("UserName"), T(""));
    ipStr=websGetVar(wp,T("IP"), T(""));
    macStr=websGetVar(wp,T("Mac"), T(""));

    if (0 == inet_aton(ipStr, &addrS))/*转换为二进制形式*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
	if(arpBindAddAccessCheck(wp->ipaddr, ipStr, macStr)!=0)/*本次绑定是否可能使当前登陆者，无法登录*/
	{
	    if(arpBindUniCheck(userName, addrS.s_addr, macAddr, PROF_UNUSED_INDEX)!=0)/*静态绑定重复性判断*/
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
		instIndex = ProfNewInst(mibType, userName, FALSE);/*新建实例*/

		if(instIndex == PROFFAIL) /*因以作重复性判断。故这里只可能时达到最大条目*/
		{
		    errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
		    setTheErrorMsgOnFree(errMsg);
		}
		else
		{
		    prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*获取实例指针*/

		    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份新创建的空实例*/
		    strcpy(prof->dhcpPoolName, dhcpPoolName);
		    prof->bindEn = FUN_ENABLE;/*默认为允许访问*/
#if (DHCP_POOL == FYES)
		    prof->vid = strtol(dhcpVid, NULL, 10);/*静态绑定vid*/
#endif
		    prof->ip = addrS.s_addr;/*静态绑定ip地址*/
		    prof->mac = macAddr;/*静态绑定mac地址*/
		    ProfUpdate(profhead);/*发消息生效*/
		    ProfFreeAllocList(profhead);/*释放备份控件*/
		    nvramWriteCommit();/*写flash*/
		}
	    }
	}
    }
    return;
}
/**
 * arp静态绑定修改操作
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

    /*获取页面控件值*/
    dhcpPoolName = websGetVar(wp, T("pools"), T(""));
#if (DHCP_POOL == FYES)
    char *dhcpVid;
    dhcpVid = websGetVar(wp, T("dhcpVid"), T(""));
#endif
    userName=websGetVar(wp,T("UserName"), T(""));/*新用户名*/
    userNameOld=websGetVar(wp,T("UserNameold"), T(""));/*老用户名*/
    ipStr=websGetVar(wp,T("IP"), T(""));
    macStr=websGetVar(wp,T("Mac"), T(""));

    if (0 == inet_aton(ipStr, &addrS))/*转换为二进制形式*/
    {
	errMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_IP_ADDR_ERR);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	converMac12To6Byte(macStr, &macAddr);/*转化为6字节类型*/
	instIndex = ProfGetInstIndexByName(mibType, userNameOld);/*查找该配置*/

	if(instIndex == PROFFAIL) 
	{/*欲修改的配置不存在*/
	    errMsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	    setTheErrorMsgOnFree(errMsg);
	}
	else 
	{/*找了要修改的配置*/
	    if(arpBindUniCheck(userName, addrS.s_addr, macAddr, instIndex)!=0)/*静态绑定重复性检测*/
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
		prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*获取要修改的实例指针*/
		if((prof->ip != addrS.s_addr) || (macAddrCmp(&prof->mac, &macAddr))!=0)/*本次修改涉及到ip，mac*/
		{
		    /*若ip，mac被修改。则可能导致当前登陆者arp信息被删除*/
		    if(arpBindDelAccessCheck(wp->ipaddr, prof->ip)==0)/*当前登录者是否还能登录*/
		    {
			errorTag = 1;/*阻碍了当前登陆者登录*/
		    }
		    else
		    {
			/*判断修改后的绑定信息是否可能导致当前登录者无法登录*/
			if(arpBindAddAccessCheck(wp->ipaddr, ipStr, macStr)==0)
			{
			    errorTag = 1;/*阻碍了当前登陆者登录*/
			}
		    }
		}

		if(errorTag == 0)/*无错*/
		{
		    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);/*备份修改之前的配置*/
		    ProfSetNameByPoint(prof, userName);/*设置用户名*/


		    if( dhcpPoolName == "" ){                 /* 当在IP/MAC绑定页面修改时，地址池名称有内存中信息来输入  */
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
			    prof->vid = profDhcp->vid;/*静态绑定vid*/
			}
#endif
		    }else{  /* 静态DHCP列表中修改 */
			strcpy(prof->dhcpPoolName, dhcpPoolName);
#if (DHCP_POOL == FYES)
			prof->vid = strtol(dhcpVid, NULL, 10);/*静态绑定vid*/
#endif
		    }
		    prof->ip = addrS.s_addr;/*ip地址*/
		    prof->mac = macAddr;/*mac地址*/
		    ProfUpdate(profhead);/*发消息更改*/
		    ProfFreeAllocList(profhead);/*释放备份空间*/
		    nvramWriteCommit();/*写flash*/

		}
	    }
	}

    }
    return;
}
/***********************************************************************
 * 函 数 名：   webConfigArpBindConfig
 * 功能描述：   添加或修改ARP绑定配置
 * 创建日期：	2010-10-29
 * 修改日期：

 * 作者：       bhou
 ***********************************************************************/
extern void webConfigArpBindConfig(webs_t wp)
{

    char* action = websGetVar(wp, T("Action"), T("")); 

    if(strncmp(action, "add", 3) == 0) 
    {/*添加*/
	arpBindConfigAdd(wp);
    } 
    else
    {/*修改*/
	arpBindConfigModify(wp);
    }

    return;
}
/***********************************************************************
 * 函 数 名：   formConfigArpBindConfig
 * 功能描述：   添加或修改ARP绑定配置
 * 创建日期：	2010-10-29
 * 修改日期：

 * 作者：       bhou
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
 * 函 数 名：   formConfigArpBindAllow
 * 功能描述：   允许，禁止ARP绑定用户上网
 * 创建日期：	2011-10-29
 * 修改日期：
 * 作者：       bhou
 * 附加说明：	无
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

    UserName=websGetVar(wp,T("AllowID"), T(""));/*用户名*/
    allow=websGetVar(wp,T("Allow"), T(""));/*允许*/

    instIndex = ProfGetInstIndexByName(mibType, UserName);
    if(instIndex == PROFFAIL) 
    {/*欲修改的配置不存在*/
	errMsg = getTransDataForC(C_LANG_INDEX_THE_CONFIG_NOT_FIND);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {/*要操作的配置存在*/
        prof = ProfGetInstPointByIndex(mibType, instIndex);/*获取该配置*/
        if (0 == strcmp("no",allow))/*禁止该绑定用户*/
        {
            allowEn = FUN_DISABLE;
            if(inet_aton(wp->ipaddr, &addrS)!=0)/*转化ip地址*/
            {
                if(prof->ip == addrS.s_addr)/*欲禁止的条目与当前登录者arp信息相同*/
                {
                    /*访问者ip地址是否与lan口同网段*/
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
                    {/*不同网段则可随便禁止*/
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
        if(errorFlag == 0)/*操作合法*/
        {
            ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);/*备份修改之前的配置*/
            prof->bindEn = allowEn ;/*允许或禁止*/
            ProfUpdate(profhead);/*发消息更改*/
            ProfFreeAllocList(profhead);/*释放备份空间*/
            nvramWriteCommit();/*写flash*/
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
 * 页面arp绑定删除函数
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
    bool allowOtherEn = getArpAllowOtherEn();/*是否允许非绑定用户连接*/
    const char *errMsg=NULL;

    memset(&addrS, 0, sizeof(addrS));

    inet_aton(wp->ipaddr, &addrS);/*转化当前登录者ip地址为二进制*/

    UserName = websGetVar(wp,T("delstr"),T("")); /*要删除的用户名列表*/
    if(!UserName)
    {   
	errMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_NOT_FIND);
	setTheErrorMsgOnFree(errMsg);
    }
    else
    {
	tmp=strtok_r(UserName,UTT_SPLIT_SIGN_STR,&ptr);/*可能删除多条记录*/
	while(tmp!=NULL)
	{
	    prof = ProfGetInstPointByName(mibType, tmp);/*获取配置地址*/
	    if(prof != NULL) /*配置已用*/
	    {
		/*若不允许非绑定用户连接。则当前访问者arp信息不能删除*/
		if((allowOtherEn ==0) && (prof->ip == addrS.s_addr))
		{
#if(FEATURE_AC == FYES)
		    tmpInt = 1;
#else
#if (MULTI_LAN == FYES)
		    tmpInt = isIpInLanNet(wp->ipaddr);
#else
		    tmpInt = isIpInIfNet(addrS.s_addr, getLanIfName());/*判断是否跟lan口同网段*/
#endif
#endif
		    /*此时只有tmpInt为0.即跟lan口不同网段才能删除*/
		}
		if(tmpInt == 0)
		{
		    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profList);
		    ProfDelInstByName(mibType, tmp);/*删除已有实例*/
		}
		else
		{
		    errMsg = getTransDataForC( C_LANG_INDEX_ALLOW_BIND_USER);
		    setTheErrorMsgOnFree(errMsg);
		    tmpInt = 0;
		}

	    }
	    tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);/*解析下一个用户名*/
	}
	ProfUpdate(profList);/*发消息删除*/
	ProfFreeAllocList(profList);/*释放备份空间*/
	nvramWriteCommit();/*写flash*/
    }
    return;
}
/***********************************************************************
 * 函 数 名：   formArpBindDel
 * 功能描述：	删除一条或多条记录
 * 创建日期：	2011-10-31
 * 修改日期：
 * 作者：       bhou
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
 * 函 数 名：   webArpBindDelAll
 * 功能描述：	删除所有记录.但不会删除跟当前访问则arp相同的记录
 * 创建日期：	2011-10-31
 * 修改日期：
 * 作者：       bhou
 * 附加说明：	无
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

    inet_aton(wp->ipaddr, &addrS);/*转化访问者ip地址为二进制*/

    if(getArpAllowOtherEn()==0)/*不允许非绑定用户连接*/
    {
#if(FEATURE_AC == FYES)
    tmpInt = 1;
#else
#if (MULTI_LAN == FYES)
	tmpInt = isIpInLanNet(wp->ipaddr);
#else
	tmpInt = isIpInIfNet(addrS.s_addr, getLanIfName());/*判断是否跟lan口同网段*/
#endif
#endif
	/*只有不同网段则可随便删除*/
    }
    ProfInstLowHigh(mibType, &max, &min);/*获取实例范围*/

    /*遍历删除*/
    for(i = min; i < max; i++) 
    {
	prof = ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*配置已用*/
	{
	    if(tmpInt!=0)/*判断失败或同网段*/
	    {
		if(prof->ip == addrS.s_addr)/*该arp信息跟当前登录者arp相同*/
		{
		    /*这里不能直接PorfBackup一个PROF_CHANGE_ADD
		     *因备份链表触发消息的顺序不确定且可能更改*/
		    bakIndex = i;/*记录索引*/
		    ProfGetInstByIndex(mibType, i, &profBak);/*获取实例的拷贝体*/
#if 0
		    /*当只有一个实例时此方法行不通*/
		    ProfDelInstByIndex(mibType, i);/*删除已有实例*/
		    continue;/*删除所有,不能备份该实例。否则若该实例重新添加与删除备份的索引相同，便可能不触发消息*/
#endif
		}
	    }
#if (DHCP_SERVICE == FYES)
	    if(back == 0) 
	    {
		/*只备份一次，防止消息多发造成系统过慢*/
		ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
		back = 1;
	    }
#else
	    ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profList);
#endif
	    ProfDelInstByIndex(mibType, i);/*删除已有实例*/
	}
    }
    /*发消息删除所有绑定*/
    ProfUpdate(profList);
    ProfFreeAllocList(profList);/*释放备份空间*/

    if(tmpInt != 0)
    {
	sleep(2);/*等待删除所有的操作（不等待则消息机制可能先处理该消息再删除所有）*/
	profList = NULL;
	/*这里不新建实例，是为了防止用户全部采用默认用户名时，冲突可能性的提高*/
	ProfBackupByIndex(mibType, PROF_CHANGE_ADD, bakIndex, &profList);/*备份新创建的空实例*/
	prof = (UserProfile*)ProfGetInstPointByIndex(mibType, bakIndex);/*获取实例指针*/
	if(prof != NULL)
	{
	    *prof = profBak;/*直接赋值*/
	}

	ProfUpdate(profList);/*发消息添加实例*/
	ProfFreeAllocList(profList);/*释放备份空间*/
    }
    nvramWriteCommit();/*写flash*/
    return;
}

/***********************************************************************
 * 函 数 名：   formArpBindDelAll
 * 功能描述：	删除所有记录.但不会删除跟当前访问则arp相同的记录
 * 创建日期：	2011-10-31
 * 修改日期：
 * 作者：       bhou
 * 附加说明：	无
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
 *读取IP/MAC绑定信息到buf中
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
 *IP/MAC绑定信息导出
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
 * 函 数 名：   formDefineArpBindConfig
 * 功能描述：	把网页上调用的asp函数转换成该文件中定义的函数
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
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
