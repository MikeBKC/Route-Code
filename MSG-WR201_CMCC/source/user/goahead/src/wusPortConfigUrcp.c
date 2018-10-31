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

static void formPortConfigureUrcp(webs_t wp, char_t *path, char_t *query);
int getPortConfigureUrcp(int eid, webs_t wp, int argc, char **argv);

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);

/********************************************************************
 * 函数： fromDefinePortStatics
 * 功能： 页面与交互转换函数
 * 创建： 2010-08-16
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： jfgu
 * 维护： jfgu
 * 日期： 2010-09-01
 *******************************************************************/
void formDefinePortConfigureUrcp(void)
{
	websAspDefine(T("aspOutPortConfigureUrcp"), getPortConfigureUrcp);
	websFormDefine(T("formPortConfigureUrcp"), formPortConfigureUrcp);	
}

/********************************************************************
 * 函数： getportconfmemberurcp
 * 功能： 页面显示函数（打印联动端口管理信息）
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou
 ********************************************************************/ 
extern urcp_status_t getPortConfMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    char m_buffer[64];
    char cur_speed[512] ;
    char _max_frame[256];
    char str_protect[128] ;
    char speed_mode[128];
    char str_fc[128];
    char port_info[768] ;
    char str_flood[128];

    uint32 i = 0u;
    uint32 start_index=0u;
    urcp_port_conf_conf_t port_conf_buff;
    urcp_status_t ret=pre_ret;/*默认为前一次操作的状态*/
    misc_opt_assist misc_buff;

    memset(cur_speed,0,sizeof(cur_speed));
    memset(_max_frame,0,sizeof(_max_frame));
    memset(str_protect,0,sizeof(str_protect));
    memset(speed_mode,0,sizeof(speed_mode));
    memset(str_fc,0,sizeof(str_fc));
    memset(port_info,0,sizeof(port_info));
    memset(str_flood,0,sizeof(str_flood));
    memset(&port_conf_buff,0,sizeof(urcp_port_conf_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_port_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构体信息*/
    if(pre_ret == URCP_STATUS_OK)/*若之前有一次操作错误，则本次读取没必要*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_PORT_CONF);/*发消息*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&port_conf_buff), &misc_buff);/*读取*/
    }
    if(ret != URCP_STATUS_OK)/*操作失败仍需打印，否则页面可能出错*/
    {
	memset(&port_conf_buff,0,sizeof(urcp_port_conf_conf_t));/*失败按空配置处理*/
    }
    if(portNum != NULL)
    {
	*portNum = port_conf_buff.port_num;/*远程设备端口个数*/
    }

    for (i=0u; i<port_conf_buff.port_num ; i++)
    {
	if (i > 0u)
	{
	    start_index = 1u;
	    m_buffer[0] = ',';
	}
	/*模式*/
	sprintf(&m_buffer[start_index], "\"%d\"", port_conf_buff.port_conf_group[i].selected_mode);
	strcat(speed_mode, m_buffer);

	/*最大帧*/
	sprintf(&m_buffer[start_index], "\"%d\"", port_conf_buff.port_conf_group[i].max_frame);
	strcat(_max_frame, m_buffer);

	/*流控*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].rate_limit);
	strcat(str_fc, m_buffer);

	/*端口保护*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].protect);
	strcat(str_protect, m_buffer);

	/*泛洪*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].flooding);
	strcat(str_flood, m_buffer);

	/*端口状态*/
	sprintf(&m_buffer[start_index], "\"%s\"", port_conf_buff.port_conf_group[i].link_status);
	strcat(cur_speed, m_buffer);

	/*端口描述*/
	sprintf(&m_buffer[start_index], "\"%s\"", port_conf_buff.port_conf_group[i].remark);
	strcat(port_info, m_buffer);
    }

    /*打印到页面*/
    websWrite(wp, "var curSpeed = new Array(%s);\n", cur_speed);/*连接状态*/
    websWrite(wp, "var speedMode = new Array(%s);\n", speed_mode);/*模式*/
    websWrite(wp, "var fc = new Array(%s);\n", str_fc);/*流控*/
    websWrite(wp, "var maxFrame = new Array(%s);\n", _max_frame);/*最大帧*/
    websWrite(wp, "var portprotect = new Array(%s);\n", str_protect);/*端口保护*/
    websWrite(wp, "var portInfo = new Array(%s); \n",  port_info);/*端口名称*/
    websWrite(wp, "var floodings = new Array(%s); \n", str_flood); /*泛洪*/

    return ret;
}
/********************************************************************
 * 函数： getPortConfigureUrcp
 * 功能： 页面显示函数（显示联动端口管理页面所有信息）
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou 
 ********************************************************************/ 
int getPortConfigureUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*页面打印的端口个数*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    init_port_conf_misc_opt_assist(&misc_buff);/*初始化辅助结构信息*/

    
    /*判断是否由formAggrUrcp()跳转过来的*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*第一次打开*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	isRefresh = 0u;/*刷新标志位复位*/
	is_set = 0u;
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
	    if(ret == URCP_STATUS_OK)
	    {
		sleep(4);/*休眠3秒。因涉及到端口的操作，可能该端口需重启链接*/
	    }
	    isRefresh = 1u;/*保存后。再刷新则不读取设置结果。因设置结果缓存可能被清空*/
	}
    }
    ret = getPortConfMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*打印端口设置本页面成员*/
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

/*******************************************************************
 *函数： formPortConfigureUrcp
 *功能： 页面提交函数
 *参数： 略
 *返回： 无
 *作者： bhou 
 ******************************************************************/
static void formPortConfigureUrcp(webs_t wp, char_t *path, char_t *query)
{
    uint32 portCt = 0u;
    uint32 i = 0u;
    uint32 u_protect = 0u;
    uint32 u_flooding = 0u;
    char *m_portInfo = NULL;
    urcp_port_conf_conf_t _port_config;
    char web_name[32];
    uchar *str, *strMac, *strIp, *strPasswd;

    memset(&_port_config,0,sizeof(urcp_port_conf_conf_t));
    memset(web_name,0,sizeof(web_name));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    isRefresh = 0u;/*刷新标志位复位*/

    strMac = websGetVar(wp, "macStr", T(""));/*远程设备mac地址*/
    if(strcmp(strMac, T(""))!=0)
    {
	str = websGetVar(wp, "numPorts", T(""));/*远程设备端口个数*/
	portCt = (uint32)strtoul(str, NULL, 10);/*转化为数字*/

	if((portCt>0u)&&(portCt<=MAX_URCP_PORT_NUM))/*不在此范围以下操作可能内存溢出*/
	{

	    /*读取页面变量*/
	    strIp = websGetVar(wp, "ipStr", T(""));/*远程设备ip*/
	    strPasswd = websGetVar(wp, "passStr", T(""));/*远程设备密码*/

	    /*保存本次操作地址*/
	    memcpy(gMac, strMac, sizeof(gMac)-1u);
	    memcpy(gIp, strIp, sizeof(gIp)-1u);
	    strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

	    str = websGetVar(wp, "hiddenvalue", T(""));/*端口保护*/
	    u_protect = (uint32)(strtoul(str, 0, 10));
	    str = websGetVar(wp, "hiddenvalue2", T(""));/*泛洪*/
	    u_flooding = (uint32)(strtoul(str, 0, 10));

	    _port_config.port_num = portCt;
	    /*取出页面控件中的值*/
	    for (i=0u; i<portCt; i++)
	    {
		sprintf(web_name,"R%x",0x50u+i);/*设置模式*/
		_port_config.port_conf_group[i].selected_mode = (int)(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0));	

		sprintf(web_name,"R%x",0x11u+i);/*允许最大帧*/
		_port_config.port_conf_group[i].max_frame = (int)(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0));	

		sprintf(web_name,"R%x",0xd0u+i);/*流控*/

		if(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0)>0u)
		{
		    _port_config.port_conf_group[i].rate_limit = 1;	
		}
		if((u_protect & ((uint32)1u << i))>0u)/*端口保护*/
		{
		    _port_config.port_conf_group[i].protect = 1;
		}
		else/*泛洪与端口保护不能同时开启*/
		{
		    /*泛洪*/
		    if((u_flooding & ((uint32)1u << i))>0u)
		    {
			_port_config.port_conf_group[i].flooding = 1;
		    }
		}
		/*端口备注*/
		sprintf(web_name,"portInfo%u",i+1u);
		m_portInfo = websGetVar(wp, web_name, T(""));
		strncpy(_port_config.port_conf_group[i].remark,m_portInfo,MAX_PORT_REMARK_LEN);
	    }
	    cfg_write_urcp_swap_conf((char*)(&_port_config),sizeof(urcp_port_conf_conf_t));/*写入共享内存*/
	    sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,portCt,MISC_SET_GENERAL, URCP_SET_PORT_CONF);/*发消息*/
	    pend_sem(0); /*等待交换机进程处理完消息*/
	}
	else
	{
	    /*端口数不对*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    else
    {
	/*没有目的交换机mac，出错*/
	setTheErrorMsg("get the destination host MAC error!");
    }
    websRedirect(wp, "switch/ports.asp?urcpSet=1");
    return;
}
#endif
