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
static uint32 isRefresh = 0u;

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);

static void formSysConfUrcp(webs_t wp, char_t *path, char_t *query);
static int aspOutPutGetSysConfUrcp(int eid, webs_t wp, int argc, char **argv);
static int aspOupPutGetSysInfoUrcp(int eid, webs_t wp, int argc, char_t **argv);
/********************************************************************
 * 函数： formDefineSysConfUrcp
 * 功能： 页面与交互转换函数
 * 创建： 2011-05-30
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： bhou 
 *******************************************************************/
extern void formDefineSysConfUrcp(void)
{
    websAspDefine(T("aspOupPutGetSysInfoUrcp"), aspOupPutGetSysInfoUrcp);
    websAspDefine(T("aspOutPutGetSysConfUrcp"), aspOutPutGetSysConfUrcp);
    websFormDefine(T("formSysConfUrcp"), formSysConfUrcp);
}

/********************************************************************
 * 函数：formSysConfUrcp
 * 功能：设置系统设置 
 * 参数：索引
 * 输出： 无
 * 返回： 无
 * 作者：bhou 
 * 日期： 2011-05-30
 *********************************************************************/ 
static void formSysConfUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *dhcp_relay = NULL;
    char *dhcp_client = NULL;
    char *cipaddr = NULL;
    char *maskaddr = NULL;
    char *gwaddr = NULL;
    char *cpu_id = NULL;
    char *mac_age = NULL;
    char *dev_uname = NULL;
    char *dev_password = NULL;
    char *arp_cheap = NULL;

    uchar *strMac, *strIp, *strPasswd;
    urcp_sys_conf_msg_t  urcp_sys_conf_msg;
    urcp_sys_conf_conf_t* conf_buff;
    urcp_general_msg_t* urcp_general_msg;

    /*清空内存*/
    memset(&urcp_sys_conf_msg,0,sizeof(urcp_sys_conf_msg_t));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    isRefresh = 0u;/*刷新标志位复位*/

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
	urcp_general_msg = &(urcp_sys_conf_msg.general_msg);
	/*转换mac地址*/
	str2mac(strMac, urcp_general_msg->dst_mac);
	strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
	urcp_general_msg->user_ip = ip2int(wp->ipaddr);

	/*初始化数据部分*/
	conf_buff = &(urcp_sys_conf_msg.sys_conf_conf);

	/*取出页面控件的值*/
	dhcp_relay = websGetVar(wp, T("dhcpOpt82"), T(""));
	dhcp_client = websGetVar(wp, T("dhcpClient"), T(""));

	/*ip相关*/
	cipaddr = websGetVar(wp, T("sysIp"), T(""));
	maskaddr = websGetVar(wp, T("sysMask"), T(""));
	gwaddr = websGetVar(wp, T("sysGw"), T(""));

	/*vid*/
	cpu_id = websGetVar(wp, T("vlanId"), T(""));
	mac_age = websGetVar(wp, T("macAgeTime"), T(""));

	dev_uname = websGetVar(wp, T("deviceName"), T(""));
	dev_password = websGetVar(wp, T("pw1"), T(""));

	/*arp 欺骗*/
	arp_cheap = websGetVar(wp, T("arp"), T(""));

	conf_buff->dhcp_relay_enable = (uint32)(strtoul(dhcp_relay, 0, 10));/*dhcp中继*/
	conf_buff->dhcp_client_enable = (uint32)(strtoul(dhcp_client, 0, 10));/*dhcp客户端*/
	conf_buff->ip_addr = ip2int(cipaddr);/*系统ip*/
	conf_buff->ip_mask =  ip2int(maskaddr);/*ip掩码*/
	conf_buff->gw_ip =  ip2int(gwaddr);/*网关*/
	conf_buff->cpu_vid =  (uint32)(strtoul(cpu_id, 0, 10));/*cpu vid*/
	conf_buff->age_time =  (uint32)(strtoul(mac_age, 0, 10));/*mac老化时间*/
	strncpy(conf_buff->dev_name, dev_uname, MAX_DEV_NAME_LEN);/*设备名*/
	conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';
	strncpy(conf_buff->password, dev_password, MAX_PASSWORD_LEN);/*登录密码*/
	conf_buff->password[MAX_PASSWORD_LEN - 1u] = '\0';
	conf_buff->arp_enable = (uint32)(strtoul(arp_cheap, 0, 10));/*是否开启arp*/

	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_SYS_CONF, (char*)(&urcp_sys_conf_msg), sizeof(struct _urcp_sys_conf_msg_t));
    }
    websRedirect(wp, "switch/sysConf.asp?urcpSet=1");
    return;
}

/********************************************************************
 * 函数：getIpFlterInfoUrcp
 * 功能：将远程设备ip过滤显示在页面上
 * 参数：无
 * 输出：无
 * 返回：略
 * 作者：bhou
 * 日期：2011-05-30
 * ******************************************************************/
extern urcp_status_t getIpFilterInfoUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 i = 0u;
    urcp_ip_filter_conf_t conf_buff;
    misc_opt_assist misc_buff;
    urcp_status_t ret = pre_ret;
    char data[768], *ptr = NULL;
    char str_buf[32];

    memset(data, 0, sizeof(data));
    memset(str_buf,0,sizeof(str_buf));
    memset(&conf_buff,0,sizeof(urcp_ip_filter_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_ip_filter_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/

    if(ret == URCP_STATUS_OK)
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_IP_FILTER);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*读取*/
    }

    if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
    {
	memset(&conf_buff,0,sizeof(urcp_ip_filter_conf_t));/*失败按空配置处理*/
    }

    /*页面打印*/
    ptr = data;
    websWrite(wp, "var srcIpAddr = new Array(");
    sprintf(ptr, "var srcIpMask = new Array(");

    if(portNum!=NULL)
    {
	*portNum = conf_buff.port_num;
    }

    /*分析各个bit*/
    for(i=0u; i<conf_buff.port_num; i++)
    {
	/*移动指针*/
	ptr = &(data[strlen(data)]);
	if(i != 0u)
	{
	    websWrite(wp, ",");/*分隔*/
	    ptr[0] = ',';
	    ptr++;/*ptr++*/
	}

	/*ip*/
	int2ip(conf_buff.filter_src_ip[i], str_buf);
	websWrite(wp, "\"%s\"", str_buf);
	/*mask*/
	int2ip(conf_buff.filter_src_ip_mask[i], str_buf);
	sprintf(ptr, "\"%s\"", str_buf);
    }
    /*结尾*/
    websWrite(wp, ");\n");
    websWrite(wp, "%s);\n", data);
    return ret;
}

/********************************************************************
 * 函数：aspOutPutGetSysConfUrcp
 * 功能：将远程设备设置显示在页面上
 * 参数：无
 * 输出：无
 * 返回：略
 * 作者：bhou
 * 日期：2011-05-27
 * ******************************************************************/
static int aspOutPutGetSysConfUrcp(int eid, webs_t wp, int argc, char **argv)
{
    char str_buf[64];
    urcp_sys_conf_conf_t conf_buff;
    misc_opt_assist misc_buff;
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;

    memset(str_buf,0,sizeof(str_buf));
    memset(&conf_buff,0,sizeof(urcp_sys_conf_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_sys_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/

    /*判断是否由form跳转过来的*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*第一次打开*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	is_set = 0u;
	isRefresh = 0u;/*刷新标志位复位*/
    }
    else
    {
	/*页面保存后的跳转*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	is_set =  1u;
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
	if(isRefresh == 0u)
	{/*设置后的首次跳转*/
	    ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_SET_GENERAL,(uchar*)NULL, &misc_buff) ;/*读取设置结果*/ 
	    isRefresh = 1u;/*保存后。再刷新则不读取设置结果。因设置结果缓存可能被清空*/
	}
    }

    if(ret == URCP_STATUS_OK)/*操作成功才发包获取*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_SYS_CONF);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*读取*/
    }

    ret=getIpFilterInfoUrcp(wp,strMac,strPasswd,(uint32*)NULL ,ret);/*打印ip过滤*/

    if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
    {
	memset(&conf_buff,0,sizeof(urcp_sys_conf_conf_t));/*失败按空配置处理*/
    }

     /*页面打印*/
    websWrite(wp, "var sysIp = \"%s\";\n", int2ip(conf_buff.ip_addr, str_buf));/*系统ip*/
    websWrite(wp, "var sysMask = \"%s\";\n", int2ip(conf_buff.ip_mask, str_buf));/*掩码*/
    websWrite(wp, "var sysGw = \"%s\";\n", int2ip(conf_buff.gw_ip,str_buf));/*网关*/
    websWrite(wp, "var onDhcpClient = %d;\n", conf_buff.dhcp_client_enable);/*dhcp客户端*/

    websWrite(wp, "var onDhcpOpt82 = %u;\n", conf_buff.dhcp_relay_enable);/*dhcp中继*/
    websWrite(wp, "var vlanId = %u;\n", conf_buff.cpu_vid);/*cpu vid*/
    websWrite(wp, "var macAgeTime = %u;\n", conf_buff.age_time);/*mac老化时间*/

    websWrite(wp, "var deviceName = \"%s\";\n", conf_buff.dev_name);/*设备名*/
    websWrite(wp, "var pw = \"%s\";\n", conf_buff.password);/*密码*/

    websWrite(wp, "var onArp = %u;\n", conf_buff.arp_enable);/*是否开启arp*/

    if(ret==URCP_STATUS_OK)
    {
	/*成功*/
	websWrite(wp, "var urcpError = \"\";\n");
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
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}


/********************************************************************
 * 函数：aspOupPutGetSysInfoUrcp 
 * 功能：将系统信息打印到页面上
 * 参数：无
 * 输出：无
 * 返回：略
 * 作者: bhou 
 * 日期：2011-06-03
 * ******************************************************************/
static int aspOupPutGetSysInfoUrcp(int eid, webs_t wp, int argc, char_t **argv)
{
    char str_buf[32];
    urcp_sys_info_conf_t conf_buff;
    misc_opt_assist misc_buff;
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd;

    memset(str_buf,0,sizeof(str_buf));
    memset(&conf_buff,0,sizeof(urcp_sys_info_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));

    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);

    if(strcmp(strMac, T(""))!=0)
    {
	init_sys_info_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/

	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_SYS_INFO);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*读取*/

	if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
	{
	    memset(&conf_buff,0,sizeof(urcp_sys_info_conf_t));/*失败按空配置处理*/
	}
    }

    sprintf(str_buf, "%lld", conf_buff.outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", str_buf);/*总空间*/
    sprintf(str_buf, "%lld", conf_buff.outer_flash_unused_space);
    websWrite(wp, "var outerFlashUnusedSpace = \"%s\";\n", str_buf);/*未使用*/

    /*当前时间*/
    websWrite(wp, "var sysDateTimes = \"%s\";\n", conf_buff.now_time);

    /*cpu*/
    websWrite(wp, "var cpuStat = \"%d\";\n", conf_buff.cpu_rate);

    /*memory*/
    websWrite(wp, "var memStat = \"%d\";\n", conf_buff.mem_rate);

    /*uptime*/
    websWrite(wp, "var upTime = \"%s\";\n", conf_buff.up_time);

    /*ip*/
    websWrite(wp, "var sysIp = \"%s\";\n", int2ip(conf_buff.ip_addr, str_buf));

    /*mask*/
    websWrite(wp, "var sysMask = \"%s\";\n", int2ip(conf_buff.ip_mask, str_buf));

    /*网关*/
    websWrite(wp, "var sysGw = \"%s\";\n", int2ip(conf_buff.gw_ip, str_buf));

    /*mac*/ 
    websWrite(wp, "var sysMac = \"%02x-%02x-%02x-%02x-%02x-%02x\";\n",
	    conf_buff.mac_addr[0], conf_buff.mac_addr[1],
	    conf_buff.mac_addr[2], conf_buff.mac_addr[3],
	    conf_buff.mac_addr[4], conf_buff.mac_addr[5]);

    /*序列号*/
    websWrite(wp, "var sysSerial = \"%u\";\n", conf_buff.sys_serialno);


    /*版本*/
    websWrite(wp, "var sysVersion = \"%s\";\n", conf_buff.sw_version);

    /*cpu vid*/
    websWrite(wp, "var vlanId = %u;\n", conf_buff.cpu_vid);

    /*dev name*/
    websWrite(wp, "var deviceName = \"%s\";\n", conf_buff.dev_name);


    if(ret==URCP_STATUS_OK)
    {
	/*成功*/
	websWrite(wp, "var urcpError = \"\";\n");
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
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
