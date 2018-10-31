#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    "webs.h"
#include    "utils.h"
#include    "uttfunction.h"

#ifdef CONFIG_SWITCH_EXT 
#include    <urcp_ipc.h>
#include    <sw_cfg.h>
#include    "webmsg.h"
#endif

#if SW_FEATURE_URCP
#define STR_MAC_SIZE   12
#define STR_IP_SIZE    15
#define MAX_ERROR_TYPE 5u

extern const char *urcpErrorMsg[];

static unsigned char gMac[STR_MAC_SIZE+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
extern int test_32_bit(unsigned long value);

static void formQosRatelimitUrcp(webs_t wp, char_t *path, char_t *query);
static int getQosRatelimitListUrcp(int eid, webs_t wp, int argc, char **argv);
static void formStormRatelimitUrcp(webs_t wp, char_t *path, char_t *query);

/*****************************************************************
 * 函数：formDefineRatelimitUrcp
 * 功能：页面于交互转换
 * 参数：略
 * 返回：略
 * 作者：bhou
 ***************************************************************/
extern void formDefineRatelimitUrcp(void)
{
    websFormDefine(T("formQosRatelimitConfUrcp"), formQosRatelimitUrcp);
    websFormDefine(T("formStormRatelimitConfUrcp"), formStormRatelimitUrcp);
    websAspDefine(T("aspOutPutGetQosRatelimitListUrcp"), getQosRatelimitListUrcp);
}

/***************************************************************
 * 函数：formQosRatelimitUrcp
 * 功能：设置qos限速
 * 参数：略
 * 返回：无
 * 作者：bhou
 * **************************************************************/
static void formQosRatelimitUrcp(webs_t wp, char_t *path, char_t *query)
{
    uint32 portCt = 0u;
    uint32 i = 0u;
    urcp_qos_rate_limit_msg_t  urcp_qos_rate_limit_msg;
    urcp_qos_rate_limit_conf_t* conf_buff;
    urcp_general_msg_t* urcp_general_msg;
    char web_name[32];
    uchar *str, *strMac, *strIp, *strPasswd;
    char *_unit = NULL;
    char *_shaper = NULL;
    char *_policer = NULL;

    memset(&urcp_qos_rate_limit_msg,0,sizeof(urcp_qos_rate_limit_msg_t));
    memset(web_name,0,sizeof(web_name));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
    if(strcmp(strMac, T(""))!=0) /*要操作的mac不能为空*/
    {
	str = websGetVar(wp, "numPorts", T(""));/*远程设备端口个数*/
	portCt = (uint32)strtoul(str, NULL, 10);/*转化为数字*/

	if((portCt>0u)&&(portCt<=MAX_URCP_PORT_NUM))/*不在此范围以下操作可能内存溢出*/
	{
	    /*颗粒度*/
	    _unit = websGetVar(wp, T("rateunitReg"), T(""));
	    if ((strcmp(_unit, "") != 0) )
	    {
		/*读取页面变量*/
		strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
		strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/

		/*保存本次操作地址*/
		memcpy(gMac, strMac, sizeof(gMac)-1u);
		memcpy(gIp, strIp, sizeof(gIp)-1u);
		strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

		/*初始化general msg*/
		urcp_general_msg = &(urcp_qos_rate_limit_msg.general_msg);
		/*转换mac地址*/
		str2mac(strMac, urcp_general_msg->dst_mac);
		strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
		urcp_general_msg->user_ip = ip2int(wp->ipaddr);
		urcp_general_msg->port_ct = portCt;

		/*初始化数据部分*/
		conf_buff = &(urcp_qos_rate_limit_msg.qos_rate_limit_conf);
		conf_buff->port_num = portCt;
		conf_buff->rate_unit = (uint32)(strtoul(_unit, 0, 10));
		/*取出页面控件中的值*/
		for (i=0u; i<portCt; i++)
		{
		    sprintf(web_name,"R%x",0x50u+i);/*设置模式*/
		    _shaper = websGetVar(wp,web_name, T(""));

		    sprintf(web_name,"R%x",0x20u+i);/*设置模式*/
		    _policer = websGetVar(wp, web_name, T(""));


		    conf_buff->qos_rate_limit_group[i].shaper = 0xffffffffu; /*无限制-1*/
		    if (strcmp(_shaper, "-1") != 0)/*下行*/
		    {
			conf_buff->qos_rate_limit_group[i].shaper = strtoul(_shaper,0,10);
		    }

		    conf_buff->qos_rate_limit_group[i].policer = 0xffffffffu; /*无限制-1*/
		    if (strcmp(_policer, "-1") != 0)/*上行*/
		    {
			conf_buff->qos_rate_limit_group[i].policer = strtoul(_policer,0,10);
		    }
		}
		/*发消息,通知交换机进程发包*/
		sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_QOS_RATE_LIMIT, (char*)(&urcp_qos_rate_limit_msg), sizeof(struct _urcp_qos_rate_limit_msg_t));

	    }
	    else
		/*颗粒度读取错误*/
	    {
		setTheErrorMsg(T("rate unit wrong"));
	    }
	}
	else
	{
	    /*端口数不对*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    websRedirect(wp, "ratelimit.asp?urcpSet=1");
    return;
}

/***************************************************************
 * 函数：formStormRatelimitUrcp
 * 功能：设置风暴抑制配置
 * 参数：略
 * 返回：略
 * 作者：bhou
 ***************************************************************/
static void formStormRatelimitUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *_uc = NULL;
    char *_bc = NULL;
    char *_mc = NULL;
    char *_icmp = NULL;
    uchar *strMac, *strIp, *strPasswd;
    urcp_storm_rate_limit_msg_t  urcp_storm_rate_limit_msg;
    urcp_storm_rate_limit_conf_t* conf_buff;
    urcp_general_msg_t* urcp_general_msg;

    memset(&urcp_storm_rate_limit_msg,0,sizeof(urcp_storm_rate_limit_msg_t));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
    if(strcmp(strMac, T(""))!=0) /*要操作的mac不能为空*/
    {
	/*读取页面变量*/
	strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
	strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/

	/*保存本次操作地址*/
	memcpy(gMac, strMac, sizeof(gMac)-1u);
	memcpy(gIp, strIp, sizeof(gIp)-1u);
	strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

	/*初始化general msg*/
	urcp_general_msg = &(urcp_storm_rate_limit_msg.general_msg);
	/*转换mac地址*/
	str2mac(strMac, urcp_general_msg->dst_mac);
	strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
	urcp_general_msg->user_ip = ip2int(wp->ipaddr);

	/*初始化数据部分*/
	conf_buff = &(urcp_storm_rate_limit_msg.storm_rate_limit_conf);

	/*取出风暴抑制各项的速率*/
	_uc = websGetVar(wp, T("R10"), T(""));
	_mc = websGetVar(wp, T("R11"), T(""));
	_bc = websGetVar(wp, T("R12"), T(""));
	_icmp = websGetVar(wp, T("R13"), T(""));

	/*单播*/
	conf_buff->uc_rate = 0xffffffffu;/*默认为无限制，即-1*/
	if (strcmp(_uc, "-1") != 0)
	{
	    conf_buff->uc_rate = (unsigned long)0x00000001u << (strtoul(_uc, 0, 10));
	}

	/*组播*/
	conf_buff->mc_rate = 0xffffffffu;/*默认为无限制，即-1*/
	if (strcmp(_mc, "-1") != 0)
	{
	    conf_buff->mc_rate = (unsigned long)0x00000001u << (strtoul(_mc, 0, 10));
	}

	/*广播*/
	conf_buff->bc_rate = 0xffffffffu;/*默认为无限制，即-1*/
	if (strcmp(_bc, "-1") != 0)
	{
	    conf_buff->bc_rate = (unsigned long)0x00000001u << (strtoul(_bc, 0, 10));
	}

	/*icmp*/
	conf_buff->icmp_rate = 0xffffffffu;/*默认为无限制，即-1*/
	if (strcmp(_icmp, "-1") != 0)
	{
	    conf_buff->icmp_rate = (unsigned long)0x00000001u << (strtoul(_icmp, 0, 10));
	}

	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_STORM_RATE_LIMIT, (char*)(&urcp_storm_rate_limit_msg), sizeof(struct _urcp_storm_rate_limit_msg_t));

    }
    websRedirect(wp, "ratelimit.asp?urcpSet=2");/*urcpSet为2用于标示下一步要显示的页面跳转自风暴抑制设置后*/
    return;
}
/********************************************************************
 * 函数： getStormRateLimitMemberUrcp
 * 功能： 页面显示函数（打印联动风暴抑制）
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou
 ********************************************************************/ 
extern urcp_status_t getStormRateLimitMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_status_t pre_ret)
{
    urcp_storm_rate_limit_conf_t conf_buff;
    urcp_status_t ret=pre_ret;/*默认为前一次操作的状态*/
    misc_opt_assist misc_buff;

    memset(&conf_buff,0,sizeof(urcp_storm_rate_limit_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/
    if(pre_ret == URCP_STATUS_OK)/*若之前有一次操作错误，则本次读取没必要*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_STORM_RATE_LIMIT);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*读取*/
    }
    if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
    {
	memset(&conf_buff,0,sizeof(urcp_storm_rate_limit_conf_t));/*失败按空配置处理*/
    }
    /*页面打印*/
    websWrite(wp, "var rates = new Array(%d, %d, %d, %d);\n",
	    test_32_bit(conf_buff.uc_rate) , /*单播*/
	    test_32_bit(conf_buff.mc_rate),/*组播*/
	    test_32_bit(conf_buff.bc_rate),/*广播*/
	    test_32_bit(conf_buff.icmp_rate));  /*icmp*/
    return ret;
}
/********************************************************************
 * 函数： getQosRateLimitMemberUrcp
 * 功能： 页面显示函数（打印联动速率限制)
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou
 ********************************************************************/ 
extern urcp_status_t getQosRateLimitMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{

    uint32 i = 0u;
    urcp_qos_rate_limit_conf_t conf_buff;
    urcp_status_t ret=pre_ret;/*默认为前一次操作的状态*/
    misc_opt_assist misc_buff;

    char strShaper[256];
    char tmpstr[16];

    memset(strShaper, 0, sizeof(strShaper));

    memset(&conf_buff,0,sizeof(urcp_qos_rate_limit_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/
    if(pre_ret == URCP_STATUS_OK)/*若之前有一次操作错误，则本次读取没必要*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_QOS_RATE_LIMIT);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*读取*/
    }
    if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
    {
	memset(&conf_buff,0,sizeof(urcp_qos_rate_limit_conf_t));/*失败按空配置处理*/
    }
    if(portNum != NULL)
    {
	*portNum = conf_buff.port_num;/*远程设备端口个数*/
    }

    sprintf(strShaper, "%s", "var shaper = new Array(");
    /*打印到页面*/
    /*颗粒度*/
    websWrite(wp, "var rateunit_reg = %lu;\n", conf_buff.rate_unit);
    websWrite(wp,"var policer = new Array(");

    for (i=0u; i<conf_buff.port_num;i++)
    {
	/*上传*/
	if (i == conf_buff.port_num - 1u)
	{
	    websWrite(wp,"%d", conf_buff.qos_rate_limit_group[i].policer);/*上行*/
	    sprintf(tmpstr, "%d", conf_buff.qos_rate_limit_group[i].shaper);/*下行*/
	}
	else
	{
	    websWrite(wp,"%d, ", conf_buff.qos_rate_limit_group[i].policer);/*上行*/
	    sprintf(tmpstr, "%d, ", conf_buff.qos_rate_limit_group[i].shaper);/*下行*/
	}
	strcat(strShaper, tmpstr);
    }
    websWrite(wp,");\n");
    websWrite(wp, "%s);\n", strShaper);
    return ret;
}
/***************************************************************
 * 函数：getQosRatelimitListUrcp
 * 功能：打印qos限速信息
 * 参数：略
 * 返回：略
 * 作者：bhou
 * 日期：2011-05-09 
 * **************************************************************/
static int getQosRatelimitListUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*页面打印的端口个数*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*判断是否由formAggrUrcp()跳转过来的*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    
    if(strcmp(urcpSet, T("1"))==0)/*跳转自速率限制设置后*/
    {
	is_set = 1u;
	init_qos_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构信息*/
    }
    else
    {
	if(strcmp(urcpSet, T("2"))==0)/*跳转自风暴抑制设置后*/
	{
	    is_set = 1u;
	    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*初始化辅助结构信息*/
	}
    }
    if(is_set==0u)
    {
	/*第一次打开*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    }
    else
    {
	/*页面保存后的跳转*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
    }
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    if(strcmp(strMac, T(""))==0)
    {
	is_set = 0u;/*此时不用再读取是否设置成功*/
	ret=URCP_STATUS_FAIL;/*按照本次操作为非法处理，则后面只打印，不发联动包*/
	/*后面打印的变量已经没有意义，为了页面的显示不出现错误还得往下走*/
    }
    if(is_set == 1u)
    {
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_SET_GENERAL,(uchar*)NULL, &misc_buff) ;/*读取设置结果*/ 
    }
    ret = getQosRateLimitMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*打印速率限制本页面成员*/
    ret = getStormRateLimitMemberUrcp(wp,strMac,strPasswd,ret);/*打印风暴抑制*/
    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*打印汇聚信息*/
    if(ret==URCP_STATUS_OK)
    {
	/*成功*/
	websWrite(wp, "var urcpError = \"\";\n");
	webPortNum=portNum;/*不正确的情况下，页面打印端口个数为0（否则则可能显示错误）*/
    }
    else if(ret<=MAX_ERROR_TYPE)
    {
	/*失败*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[ret]);
    }
    else
    {
	/*失败*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[0]);
    }
    websWrite(wp, "var numPorts = %d;\n", webPortNum);
    /*vlan信息*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
