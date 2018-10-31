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

extern char * mirror_port;
extern char * mirror_source[PORT_COUNT]; 
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);

static unsigned char gMac[STR_MAC_SIZE+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static uint32 isRefresh = 0u;
static void formMirrorUrcp(webs_t wp, char_t *path, char_t *query);
static int getMirrorUrcp(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * 函数： formDefineTopoyDiscovery
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
extern void formDefineMirrorUrcp(void)
{
    websAspDefine(T("aspOutPutMirrorUrcp"), getMirrorUrcp);
    websFormDefine(T("formMirrorUrcp"), formMirrorUrcp);
}

/*******************************************************************
 *函数： formMirror
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-10-12
 ******************************************************************/
static void formMirrorUrcp(webs_t wp, char_t *path, char_t *query)
{
    uint32 i, portCt, port, ip;
    port_bit_mask_t pm; 
    char *str, *strMac, *strIp, *strPasswd;
    urcp_mirror_msg_t  urcp_mirror_msg;

    /*读取镜像端口*/
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    isRefresh = 0u;/*刷新标志复位*/

    strMac = websGetVar(wp, "macStr", T(""));
    if(strcmp(strMac, T(""))!=0)
    {
	/*镜像源端口*/
	str = websGetVar(wp, "numPorts", T(""));
	portCt = (uint32)strtoul(str, NULL, 10);
	/*读取mask*/
	if((portCt>0u)&&(portCt<=PORT_COUNT))
	{
	    pm = 0u;
	    for(i=0u;i<portCt;i++)
	    {
		/*取页面数值*/
		str = websGetVar(wp, mirror_source[i], T(""));
		if(strtoul(str, NULL, 10) == 1u)
		{
		    pm |= 1u<<i;
		}
	    }

	    /*读取页面变量*/
	    strIp = websGetVar(wp, "ipStr", T(""));
	    strPasswd = websGetVar(wp, "passStr", T(""));
	    str = websGetVar(wp, mirror_port, T(""));
	    port = (uint32)strtoul(str, NULL, 10);
	    ip=ip2int(strIp);

	    /*保存本次操作地址*/
	    i=(uint32)sizeof(gMac)-1u;
	    memcpy(gMac, strMac, i);
	    i=(uint32)sizeof(gIp)-1u;
	    memcpy(gIp, strIp, i);
	    i=(uint32)sizeof(gPasswd)-1u;
	    strncpy(gPasswd, strPasswd, i);

	    /*发消息*/
	    memset(&urcp_mirror_msg, 0, sizeof(struct _urcp_mirror_msg_t));
	    /*页面读的mac地址格式转换*/
            str2mac((uchar*)strMac, urcp_mirror_msg.dst_mac);
	    strncpy(urcp_mirror_msg.password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_mirror_msg.user_ip = ip2int(wp->ipaddr);
	    urcp_mirror_msg.port_ct = portCt;
	    urcp_mirror_msg.mirror_port = port;
	    urcp_mirror_msg.mirror_source= pm;
	    /*发消息,通知交换机进程发包*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, SET_MIRROR, (char*)(&urcp_mirror_msg), sizeof(struct _urcp_mirror_msg_t));
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
    /*跳转，设置urcpSet表示不用再发包读取镜像信息了,直接读本次保存到misc buffer中的信息*/
    websRedirect(wp, "switch/mirror.asp?urcpSet=1");

    return;
}

/********************************************************************
 * 函数： getUrcpMirrorConf
 * 功能： 获取联动镜像数据结构
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： kehuatao / edit by bhou
 *******************************************************************/
extern urcp_status_t getUrcpMirrorConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_mirror_conf_t *mirror_buff,uint32 misc_type)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000 *1000 ;/*转化为微妙*/
    uint32 ip;

    urcp_mirror_msg_t  urcp_mirror_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*默认为读取失败*/

    ip=ip2int(wp->ipaddr);
    memset(&urcp_mirror_msg, 0, sizeof(struct _urcp_mirror_msg_t));
    /*转换mac地址*/
    str2mac(strMac, urcp_mirror_msg.dst_mac);
    /*要操作的mac不能为空*/
    strncpy(urcp_mirror_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_mirror_msg.user_ip = ip;
    /*get操作则重新获取。否者直接读取本地设置信息即可*/
    if(misc_type == MISC_GET_GENERAL)
    {
	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, GET_MIRROR, (char*)(&urcp_mirror_msg), sizeof(struct _urcp_mirror_msg_t));
    }
    /*等待1会*/
    while(1)
    {	 
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*等待100ms即0.1s*/  
	wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC; /*从misc buffer中读取信息*/	   
	memset(mirror_buff, 0, sizeof(struct _urcp_mirror_conf_t));     
	ret = urcp_read_mirror_conf(urcp_mirror_msg.dst_mac, ip, misc_type , mirror_buff);	 
	if((ret == URCP_STATUS_WAIT) || (ret==URCP_STATUS_FOUND_NONE))
	{
	    /*URCP_STATUS_FOUND_NONE 表示可能urcp程序还没来得及创建节点信息与发包，系统就又调度执行goahead程序了*/
	    if(wait_time_us > max_wait_time)	  
	    {      
		ret = URCP_STATUS_TIME_OUT;	 
	    } 
	}	
	if((ret!=URCP_STATUS_WAIT) && (ret!=URCP_STATUS_FOUND_NONE))
	{     
	    break; 
	} 
    }

    return ret;
}


/********************************************************************
 * 函数： getMirror
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2010-10-12
 ********************************************************************/ 
static int getMirrorUrcp(int eid, webs_t wp, int argc, char **argv)
{
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 i,portNum;
    port_bit_mask_t  pm;
    urcp_mirror_conf_t mirror_buff;
    urcp_status_t ret = URCP_STATUS_OK ;

    memset(&mirror_buff, 0, sizeof(struct _urcp_mirror_conf_t));
    /*判断是否由formMirrorUrcp()跳转过来的*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*第一次打开
	 *按此方式打开的页面。进行刷新则 数据被重新post给页面*/
	isRefresh = 0u;
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	i = MISC_GET_GENERAL;
    }
    else
    {
	/*页面保存后的跳转*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	if(isRefresh == 0u)
	{/*保存后的首次跳转*/
	    i = MISC_SET_GENERAL;
	    isRefresh = 1u;
	}
	else
	{/*页面保存后进行刷新
	  *这时页面是被再次请求。但 远程设备密码，mac等信息未被重新post*/
	    i = MISC_GET_GENERAL;
	}
    }
    /*重新打印到页面，供下次"form"使用*/
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    /*要操作的mac不能为空*/
    if(strcmp(strMac, T(""))!=0)
    {
	/*从misc buffer中读取镜像信息*/
	ret = getUrcpMirrorConf(wp,strMac,strPasswd,&mirror_buff,i);
    }
    else/*出错*/
    {
	ret=URCP_STATUS_FAIL;/*按照本次操作为非法处理，则后面只打印，不发联动包*/
	websWrite(wp, "urcpError = \"get the destination host MAC error!\";\n");
	/*后面打印的变量已经没有意义，为了页面的显示不出现错误还得往下走*/
    }
    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*打印汇聚信息*/
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

    /*镜像源端口*/
    portNum = mirror_buff.port_num;
    if(portNum>PORT_COUNT)
    {
	/*目前页面显示不了那么多*/
	portNum = PORT_COUNT;
    }
    websWrite(wp, "var numPorts = %d;\n", portNum);

    /*镜像端口*/
    websWrite(wp, "var mirrorPort = %d;\n", mirror_buff.mirror_port);

    pm = mirror_buff.mirror_source;
    /*镜像源端口*/
    websWrite(wp, "var mirrorSource = new Array(");
    /*分析各个bit*/
    for(i=0u;i<portNum; i++)
    {
	if(i!=0u)
	{
	    websWrite(wp, ",");
	}

	if((pm & (uint32)(1u<<i))>0u)/*启用*/
	{
	    websWrite(wp, "\"checked\"");
	}
	else/*禁用*/
	{
	    websWrite(wp, "\"\"");
	}
    }
    /*结尾*/
    websWrite(wp, ");\n");
   getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif
