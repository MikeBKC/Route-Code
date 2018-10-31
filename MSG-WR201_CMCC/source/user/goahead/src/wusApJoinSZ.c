#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "mibUrcp.h"

#if (FEATURE_AP_JOIN_SZ == FYES)

#include    "translate.h"
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "webmsg.h"

#define DEBUG_SZ 0
#if DEBUG_SZ
#define SZ_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define SZ_DEBUG(fmt, argx...)
#endif

#define STR_MAC_SIZE   12
#define STR_IP_SIZE    15
#define MAX_ERROR_TYPE 5u

extern const char *urcpErrorMsg[];

int gCount=4;
static unsigned char gMac[STR_MAC_SIZE+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gSzName[64+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static unsigned char gManaged[64+1];
//static uint32 joinedSZNum = 0u;
static uint32 isRefresh = 0u;

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);

#if (AP_BLACK_WHITE_LIST == FYES)
extern char* getSSIDBySz(char* sz);
extern void change_ssid(char *ssid);
static int send_black_list_by_sz(char *strMac, char *sz)
{
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    ApBlackWhiteListProfile *prof=NULL;
    int min = 0, max = 0 , index = 0, len = 0, num = 0, i = 0;
    unsigned char buffer[1500] = "";
    unsigned char *mac = buffer;
    unsigned char mac_null[MAC_LEN] = "";
    urcp_black_list_msg_t  *msg;
    char ssid[UTT_SSID_LENTH + 1] = "";
    strncpy(ssid, getSSIDBySz(sz), UTT_SSID_LENTH);
    change_ssid(ssid);
    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++)
    {
	memset(buffer, 0 , sizeof(buffer));
	len = 0;
	num = 0;
	prof = (ApBlackWhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (prof->head.active == TRUE) && (prof->num != 0) && (strncmp(prof->sz, sz, UTT_INST_NAME_LEN) == 0))
	{
	    /*遍历到满足条件的黑名单实例，发送整条实例的数据*/
#if 1
	    str2mac(strMac, mac);
	    len = MAC_LEN;
	    msg = (urcp_black_list_msg_t *)(buffer + MAC_LEN);
	    msg->mode = 1;	/*添加*/
	    len += sizeof(msg->mode) + sizeof(msg->staNum);

	    for(i = 0; i < MAX_STA_PER_BLACKLIST; i++)
	    {
		if (memcmp(&(prof->mac[i]), mac_null, MAC_LEN) != 0)
		{
		    memcpy(msg->black_list[num].staMac, &(prof->mac[i]), MAC_LEN);
		    strncpy(msg->black_list[num].ssid, ssid, UTT_SSID_LENTH);
		    num++;
		    len += sizeof(urcp_black_list_t);
		}
	    }
	    msg->staNum = num;
//	    printf("%s,send num:%d, len:%d\n", __func__, num, len);
	    sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_ADD_BLACK_LIST_SZ, buffer, len);
#endif
	}
    }

    return 0;
}

static int send_black_list_sz(char *strMac, char *sz_all )
{
    char sz_name[64] = "";
    char *str = sz_name;
    char *sz[4];
    int sz_num = 0, i = 0;
    memcpy(sz_name, sz_all, 64);
    memset(sz, 0, sizeof(sz));
    if (sz_name[0] != '\0')
    {
	sz[sz_num] = str;
	sz_num++;
	while(str != NULL)
	{
	    str = strchr(str, ','); 
	    if (str != NULL)
	    {
		*str = '\0';
		str++;
		sz[sz_num++] = str;
	    }
	}
    }
    for(i = 0; i < sz_num; i++)
    {
//	printf("%s, sz:%s\n",__func__, sz[i]);
	send_black_list_by_sz(strMac, sz[i]);
    }

    return 0;
}
#endif
/********************************************************************
* 函数： aspOutPutSzInfo
* 功能： 页面与交互转换函数
* 创建： 2012-07-11
* 参数： 略
* 输出： 无
* 返回： 无
* 作者： xhhu 
*******************************************************************/
static int aspOutPutSzInfo(int eid, webs_t wp, int argc, char **argv)
{
    SZ_DEBUG("\n< %s %d >\n", __func__, __LINE__);
    unsigned int i=0u;
    int tb_index;
    int flag=0;
    uchar *tmpMac;
    uchar *ptr = NULL;
    urcp_dev_t* dev = NULL;
    unsigned char rel_mac[MAC_LEN] = "";
    int flag_5G = 1;		/*默认5G为真*/
    uint32 is_set=0u;
    char buf[48];
    urcp_shm_t *mem_shadow=NULL;
    urcp_dev_t* tmp=NULL;
    urcp_status_t ret = URCP_STATUS_OK;
//    misc_opt_assist misc_buff;
//    unsigned char joinedZones[(UTT_INST_NAME_LEN+1)*6];
    unsigned char *strMac, *strIp, *strPasswd, *waitOver,*strSzName,*strManaged;
//    init_ap_join_sz_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/
    /*判断是否由form跳转过来的*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    /* 空则是第一次打开，非空的话说明是保存后的跳转 */
    if(strcmp(waitOver, T(""))!=0)
    {
	/*页面保存后的跳转*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	strSzName = gSzName;
	strManaged = gManaged;
    websWrite(wp,"var maxSzNum = %d;\n", gCount);
	is_set =  1u;
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	strSzName = websGetVar(wp, "szName", T(""));
	strManaged = websGetVar(wp, "managedEn", T(""));
	getErrorArr(wp, urcp_read_ap_join_sz_conf, MISC_SET_GENERAL);
	is_set = 0u;
	isRefresh = 0u;/*刷新标志位复位*/
    }
    else
    {
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	strSzName = websGetVar(wp, "szName", T(""));
	strManaged = websGetVar(wp, "managedEn", T(""));
	is_set = 0u;
	isRefresh = 0u;/*刷新标志位复位*/
    }
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", strSzName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);


#if 0
    memset(joinedZones,'\0',sizeof(joinedZones));
    ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_SET_GENERAL, (uchar*)(joinedZones), &misc_buff);/*读取*/
    SZ_DEBUG("\n< %s %d > ret:%d OK:%d ip:%s mac:%s joinedZones:%s\n", __func__, __LINE__, ret,URCP_STATUS_OK,wp->ipaddr,strMac,joinedZones);
    if (ret == URCP_STATUS_OK)
    {
	    websWrite(wp, "var joinedZones = \"%s\";\n", joinedZones);
    }
    else
    {
	    websWrite(wp, "var joinedZones = \"\";\n");
    }
#endif

    mem_shadow = get_shm_ptr();
    if(mem_shadow!=NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++) 
	{
	    if (flag == 1)
	    {
		break;
	    }
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]); 
		sprintf(buf, "%02X%02X%02X%02X%02X%02X", tmp->mac[0],tmp->mac[1],tmp->mac[2],tmp->mac[3],tmp->mac[4],tmp->mac[5]);
		SZ_DEBUG("\n< %s %d > strMac:%s buf:%s\n", __func__, __LINE__, strMac, buf);
		if (strcmp(strMac, buf) == 0)
		{
		    flag=1;
		    websWrite(wp,"var maxSzNum = %d;\n", tmp->wl_info.ssid_num);
		    gCount=tmp->wl_info.ssid_num;
		SZ_DEBUG("\n< %s %d > ssid_num:%d\n", __func__, __LINE__, tmp->wl_info.ssid_num);
		    break;
		}
		tb_index = tmp->tb_next;
	    }
	}
    }

    tmpMac=strtok_r(strMac,UTT_SPLIT_SIGN_STR,&ptr);
    while(tmpMac != NULL)
    {
        str2mac(tmpMac, rel_mac); 
	dev = urcp_dev_table_lookup(rel_mac, 0u);
	if ((dev != NULL) && (((dev->dev_type)&DEV_TYPE_WIRELESS_5G)==0))
	{
	    flag_5G = 0;	    /*勾选的MAC中只要有一个不支持5G该变量为假*/
	}
	tmpMac=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
        if( tmpMac != NULL )
	{
            *(tmpMac-1) = ',';
        }
    }
    websWrite(wp, "var flag_5G = \"%d\";\n", flag_5G);

    return 0;
}

/********************************************************************
* 函数： formApJoinSZ
* 功能： 页面与交互转换函数
* 创建： 2012-07-12
* 参数： 无
* 输出： 无
* 返回： 无
* 作者： xhhu 
*******************************************************************/
static int aspRemoteApJoinSZ(int eid, webs_t wp, int argc, char_t **argv)
{
    SZ_DEBUG("\n< %s %d >\n", __func__, __LINE__);
#if 0
    int min, max, index, poolmin, poolmax, poolindex;
    int i=0;
    char *ptr = NULL;
    char *tmpStr = NULL;
    char SZNames[64];/* 服务区名称 */
    ServiceZoneProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    urcp_ap_join_sz_t *ap_join_sz_buff = NULL;
    DhcpPoolProfile *profpool = NULL;
    MibProfileType mibTypepool = MIB_PROF_DHCP_POOL;
#endif 
    uchar *strMac, *strIp, *strPasswd,*strSzName,*strManaged, *rf24G, *rf5G;
    urcp_ap_join_sz_msg_t urcp_ap_join_sz_msg;
    urcp_dev_t* dev = NULL; 
    const char *ErrMsg = NULL;

    /*清空内存*/
    memset(&urcp_ap_join_sz_msg,0,sizeof(urcp_ap_join_sz_msg));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gSzName, 0, sizeof(gSzName));
    memset(gPasswd, 0, sizeof(gPasswd));
    memset(gManaged, 0, sizeof(gManaged));
    //memset(joinedZones, 0, sizeof(joinedZones));

    //memset(SZNames, 0, sizeof(SZNames));
    isRefresh = 0u;/*刷新标志位复位*/

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
    SZ_DEBUG("\n< %s %d > sz_name:%s strMac:%s\n", __func__, __LINE__, urcp_ap_join_sz_msg.sz_name, strMac);
    if(strcmp(strMac, T(""))!=0) /*要操作的mac不能为空*/
    {
	/*读取页面变量*/
	strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
	strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/
	strSzName = websGetVar(wp, "szName", T(""));/*远程设备密码*/
	strManaged = websGetVar(wp, "managedEn", T(""));/*远程设备密码*/
	rf24G = websGetVar(wp, "fre2", T(""));/*远程设备密码*/
	rf5G = websGetVar(wp, "fre5", T(""));/*远程设备密码*/

	if(strcmp(rf24G, "on") == 0) {
	    urcp_ap_join_sz_msg.rf = 1;
	}

	if(strcmp(rf5G, "on") == 0) {
	    if(urcp_ap_join_sz_msg.rf == 1) {
		urcp_ap_join_sz_msg.rf = RF_FLAG_24G_5G;
	    }
	    else {
		urcp_ap_join_sz_msg.rf = RF_FLAG_5G;
	    }
	}
	else {
	    if(urcp_ap_join_sz_msg.rf == 1) {
		urcp_ap_join_sz_msg.rf = RF_FLAG_24G;
	    }
	    else {
		setTheErrorMsg("请至少选择一个射频单元！");
		return 0;
	    }
	}

	/*保存本次操作地址*/
	memcpy(gMac, strMac, sizeof(gMac)-1u);
	memcpy(gSzName, strSzName, sizeof(gSzName)-1u);
	memcpy(gIp, strIp, sizeof(gIp)-1u);
	strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);
	memcpy(gManaged, strManaged, sizeof(gManaged)-1u);

	strncpy(urcp_ap_join_sz_msg.sz_name, websGetVar(wp, "SZNameArray", T("")), 64);
	//strncpy(joinedZones, urcp_ap_join_sz_msg.sz_name, sizeof(joinedZones));
    SZ_DEBUG("\n< %s %d > sz_name:%s\n", __func__, __LINE__, urcp_ap_join_sz_msg.sz_name);
	str2mac(strMac, urcp_ap_join_sz_msg.general_msg.dst_mac);
	dev = urcp_dev_table_lookup(urcp_ap_join_sz_msg.general_msg.dst_mac, 0u); 
	if ((urcp_ap_join_sz_msg.rf != RF_FLAG_24G) && (dev != NULL) && (((dev->dev_type)&DEV_TYPE_WIRELESS_5G)==0) )
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_AP_WIRELESS_NOT_SUPPORT_5G);
            setTheErrorMsgOnFree(ErrMsg);      
	    return 0;
	}
	strncpy(urcp_ap_join_sz_msg.general_msg.password, strPasswd, MAX_PASSWORD_LEN);
	urcp_ap_join_sz_msg.general_msg.user_ip = ip2int(wp->ipaddr);
	urcp_ap_join_sz_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */
    SZ_DEBUG("\n< %s %d > sz_name:%s ip:%s mac:%s\n", __func__, __LINE__, urcp_ap_join_sz_msg.sz_name, wp->ipaddr, strMac);
#if 0
	SZ_DEBUG("\n< %s %d > SZNames:%s strIp:%s strPasswd:%s strMac:%s\n", __func__, __LINE__, SZNames, strIp, strPasswd, strMac);
	tmp=strtok_r(SZNames,UTT_SPLIT_SIGN_STR,&ptr);
	ProfInstLowHigh(mibType, &max, &min);
	ProfInstLowHigh(mibTypepool, &poolmax, &poolmin);
	while(tmp != NULL)
	{
	    for (index = min; index < max; index++)
	    {
		prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
		if (prof != NULL)
		{
		    if (strcmp(tmp, prof->head.name) == 0)
		    {
			SZ_DEBUG("\n< %s %d >\n", __func__, __LINE__);
			/*初始化 msg*/
			str2mac(strMac, urcp_ap_join_sz_msg[i].general_msg.dst_mac);
			strncpy(urcp_ap_join_sz_msg[i].general_msg.password, strPasswd, MAX_PASSWORD_LEN);
			urcp_ap_join_sz_msg[i].general_msg.user_ip = ip2int(wp->ipaddr);
			urcp_ap_join_sz_msg[i].general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */
			ap_join_sz_buff = &(urcp_ap_join_sz_msg[i].ap_join_sz);
			if (ap_join_sz_buff != NULL)
			{
			    ap_join_sz_buff->serviceEn = prof->serviceEn;
			    strncpy(ap_join_sz_buff->szName, prof->head.name, (UTT_INST_NAME_LEN + 1));
			    strncpy(ap_join_sz_buff->ssid, prof->ssid, (UTT_SSID_LENTH + 1));
			    ap_join_sz_buff->ssidBcast = prof->ssidBcast;
			    ap_join_sz_buff->vid = prof->vid;
			    ap_join_sz_buff->securMode = prof->securMode;
			    /* wep */
			    ap_join_sz_buff->wepSecurity.auMethod = prof->wepSecurity.auMethod;
			    ap_join_sz_buff->wepSecurity.kType = prof->wepSecurity.kType;
			    ap_join_sz_buff->wepSecurity.kNum1.key_state_action = prof->wepSecurity.kNum1.key_state_action;
			    strncpy(ap_join_sz_buff->wepSecurity.kNum1.passwd, prof->wepSecurity.kNum1.passwd, UTT_PASSWD_LEN);
			    ap_join_sz_buff->wepSecurity.kNum1.kFmt = prof->wepSecurity.kNum1.kFmt;
			    ap_join_sz_buff->wepSecurity.kNum2.key_state_action = prof->wepSecurity.kNum2.key_state_action;
			    strncpy(ap_join_sz_buff->wepSecurity.kNum2.passwd, prof->wepSecurity.kNum2.passwd, UTT_PASSWD_LEN);
			    ap_join_sz_buff->wepSecurity.kNum2.kFmt = prof->wepSecurity.kNum2.kFmt;
			    ap_join_sz_buff->wepSecurity.kNum3.key_state_action = prof->wepSecurity.kNum3.key_state_action;
			    strncpy(ap_join_sz_buff->wepSecurity.kNum3.passwd, prof->wepSecurity.kNum3.passwd, UTT_PASSWD_LEN);
			    ap_join_sz_buff->wepSecurity.kNum3.kFmt = prof->wepSecurity.kNum3.kFmt;
			    /* wpa */
			    ap_join_sz_buff->wpaSecurity.wVersion = prof->wpaSecurity.wVersion;
			    ap_join_sz_buff->wpaSecurity.sAlg = prof->wpaSecurity.sAlg;
			    ap_join_sz_buff->wpaSecurity.radiusIp = prof->wpaSecurity.radiusIp;
			    ap_join_sz_buff->wpaSecurity.radiusPort = prof->wpaSecurity.radiusPort;
			    strncpy(ap_join_sz_buff->wpaSecurity.radiusPasswd, prof->wpaSecurity.radiusPasswd, UTT_PASSWD_LEN);
			    ap_join_sz_buff->wpaSecurity.updataTime = prof->wpaSecurity.updataTime;
			    /* wpa-psk */
			    ap_join_sz_buff->wpaPskSecurity.wVersion = prof->wpaPskSecurity.wVersion;
			    ap_join_sz_buff->wpaPskSecurity.sAlg = prof->wpaPskSecurity.sAlg;
			    strncpy(ap_join_sz_buff->wpaPskSecurity.sharePasswd, prof->wpaPskSecurity.sharePasswd, UTT_PASSWD_LEN);
			    ap_join_sz_buff->wpaPskSecurity.updataTime = prof->wpaPskSecurity.updataTime;
			    /* DHCP */
			    ap_join_sz_buff->dhcpEn = prof->dhcpEn;
			    for (poolindex = poolmin; poolindex < poolmax; poolindex++)
			    {
				profpool = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibTypepool, poolindex);
				if (profpool != NULL)
				{
				    if (strcmp(profpool->head.name, prof->dhcpPool) == 0)
				    {
					ap_join_sz_buff->startIp = profpool->ipStart; /* 已经是网络字节序 */
					ap_join_sz_buff->endIp = profpool->ipEnd; /* 已经是网络字节序 */
					break;
				    }
				}
			    }
			    ap_join_sz_buff->gw = prof->gw; /* 已经是网络字节序 */
			    ap_join_sz_buff->netmask = prof->netmask; /* 已经是网络字节序 */
			    i++;
			}
		    }
		}
	    }
	    tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
	}
	urcp_ap_join_sz_msg[0].num = i;
#endif
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_AP_JOIN_SZ, (char*)&urcp_ap_join_sz_msg, sizeof(urcp_ap_join_sz_msg));

#if (AP_BLACK_WHITE_LIST == FYES) 
	/*下发黑名单*/
	send_black_list_sz(strMac, websGetVar(wp, "SZNameArray", T("")));
#endif
    }
    //sprintf(tmpStr,"serviceZone.asp?macStr=%s",strMac);
    //websRedirect(wp, tmpStr);
    getTheErrorMsg(eid, wp, argc, argv);
    //websRedirect(wp, "serviceZone.asp?urcpSet=1");
    SZ_DEBUG("\n%s %d\n", __func__, __LINE__);
    return 0;
}

#define MAX_IP_LEN 16
/*
 *批量下发服务区
 *
 *
 *
 */
static int aspJoinSz(int eid, webs_t wp, int argc, char **argv)
{
    uchar *strMac, *strIp, *strPasswd, *rf24G, *rf5G;
    uchar *tmpMac;
    uchar *ptr = NULL;
    urcp_ap_join_sz_msg_t urcp_ap_join_sz_msg;

    uchar *ip1,*ip2;
    uchar *sp1,*sp2;

    uchar tmpIp[MAX_IP_LEN];
    uchar tmpSp[MAX_PASSWORD_LEN];
    /*清空内存*/
    memset(&urcp_ap_join_sz_msg,0,sizeof(urcp_ap_join_sz_msg));

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
    strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
    strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/
    rf24G = websGetVar(wp, "fre2", T(""));/*远程设备密码*/
    rf5G = websGetVar(wp, "fre5", T(""));/*远程设备密码*/

    if(strcmp(rf24G, "on") == 0) {
	urcp_ap_join_sz_msg.rf = 1;
    }

    if(strcmp(rf5G, "on") == 0) {
	if(urcp_ap_join_sz_msg.rf == 1) {
	    urcp_ap_join_sz_msg.rf = RF_FLAG_24G_5G;
	}
	else {
	    urcp_ap_join_sz_msg.rf = RF_FLAG_5G;
	}
    }
    else {
	if(urcp_ap_join_sz_msg.rf == 1) {
	    urcp_ap_join_sz_msg.rf = RF_FLAG_24G;
	}
	else {
	    setTheErrorMsg("请至少选择一个射频单元！");
	    return 0;
	}
    }

    ip1 = strIp;
    sp1 = strPasswd;

	strncpy(urcp_ap_join_sz_msg.sz_name, websGetVar(wp, "SZNameArray", T("")), 64);
    tmpMac=strtok_r(strMac,UTT_SPLIT_SIGN_STR,&ptr);

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

        str2mac(tmpMac, urcp_ap_join_sz_msg.general_msg.dst_mac);
        strncpy(urcp_ap_join_sz_msg.general_msg.password, tmpSp, MAX_PASSWORD_LEN);
	    urcp_ap_join_sz_msg.general_msg.user_ip = ip2int(wp->ipaddr);
        urcp_ap_join_sz_msg.general_msg.opt_type = MISC_SET_GENERAL;/* 设置消息类型 */

        sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_AP_JOIN_SZ, (char*)&urcp_ap_join_sz_msg, sizeof(urcp_ap_join_sz_msg));
	/*下发黑名单*/
#if (AP_BLACK_WHITE_LIST == FYES)
	send_black_list_sz(tmpMac, websGetVar(wp, "SZNameArray", T("")));	
#endif
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
/********************************************************************
 * 函数： formDefineApJoinSZ
 * 功能： 页面与交互转换函数
 * 创建： 2012-06-20
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： xhhu 
 *******************************************************************/
extern void formDefineApJoinSZ(void)
{
    websAspDefine(T("aspOutPutSzInfo"), aspOutPutSzInfo);
    websAspDefine(T("aspRemoteApJoinSZ"), aspRemoteApJoinSZ);
    websAspDefine(T("aspJoinSz"), aspJoinSz);
}
#endif
