#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    <uttMachine.h>
#include    "webmsg.h"
#include    "webs.h"
#include    "utils.h"
#include    "uttfunction.h"

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#endif

#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
const char *urcpErrorMsg[]={"操作失败","操作超时","操作成功","操作超时","密码错误","操作失败","查找失败"};
const char *urcpErrorMsg_en[]={"The operation failed","The operation timeout","The operation was successful","The operation timeout","Password error","The operation failed","found none"};

#define USB_AP_CONFIG_ROOT "ap_configs/"
#define RAM_AP_CONFIG_ROOT "/tmp/urcp/ap_configs/"

extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);
extern const char *urcpErrorMsg[];
extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);
static int aspOutErrorArr(int eid, webs_t wp, int argc, char **argv);
static int remoteAuth(int eid, webs_t wp, int argc, char **argv);
static int aspRebootRemote(int eid, webs_t wp, int argc, char **argv);
static int aspBindRemote(int eid, webs_t wp, int argc, char **argv);
static int aspResetRemote(int eid, webs_t wp, int argc, char **argv);

static int aspSetPassRemote(int eid, webs_t wp, int argc, char **argv);

void aspOutUrcpError(webs_t wp, urcp_status_t ret);
extern void formDefineAggrUrcp(void);
typedef urcp_status_t (*urcp_fp)(unsigned char mac[6], unsigned int userIp, unsigned int op_type);
extern void getErrorArr(webs_t wp, urcp_fp fp, uint32 op_type);
extern void formDefinePortVlanUrcp(void);
extern void formDefinePortConfigureUrcp(void);
extern void formDefineSysConfUrcp(void);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
extern int isUSBStorageExist(char *usb_mount_path);
#endif
/********************************************************************
 * 函数： formDefineTopoyDiscovery
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
extern void formDefineRemoteUrcp(void)
{
    websAspDefine(T("remoteAuth"), remoteAuth);
    websAspDefine(T("aspOutErrorArr"), aspOutErrorArr);
    websAspDefine(T("aspRebootRemote"), aspRebootRemote);
    websAspDefine(T("aspBindRemote"), aspBindRemote);
    websAspDefine(T("aspSignalRemoteAuth"), aspSignalRemoteAuth);
    websAspDefine(T("aspResetRemote"), aspResetRemote);

    websAspDefine(T("aspSetPassRemote"), aspSetPassRemote);
    //formDefineAggrUrcp();
    //formDefinePortVlanUrcp();
    //formDefinePortConfigureUrcp();
    //formDefineSysConfUrcp();
    return;
}
/*
 *将联动操作结果返回给页面
 *KeHuatao
 *
 */
extern void aspOutUrcpError(webs_t wp, urcp_status_t ret)
{
    int32 i;
    LangProfile *prof=NULL;
    MibProfileType mibType = MIB_PROF_LANG;

    prof = (LangProfile*)ProfGetInstPointByIndex(mibType, 0);
    i = (int32)ret;
    if(ret>=URCP_STATUS_END)
    {
	ret = URCP_STATUS_FAIL;
    }
    if(ret==URCP_STATUS_OK)
    {
	/*成功*/
	websWrite(wp, "\"\"");
    }
    else
    {
	/*失败*/
	if (prof != NULL)
	{
	    if (strcmp(prof->lang,"en")==0)
	    {
		websWrite(wp, "\"%s\"", urcpErrorMsg_en[(i)]);
	    }
	    else
	    {
		websWrite(wp, "\"%s\"", urcpErrorMsg[(i)]);
	    }
	}
    }
    return;
}

/****
 *单机管理密码验证
 *bhou
 */
int aspSignalRemoteAuth(int eid, webs_t wp, int argc, char **argv)
{
    remoteAuth(eid, wp, argc, argv);/*发包*/
    sleep(1u);/*等待一会*/
    getErrorArr(wp,(urcp_fp)(urcp_read_passwd_auth), MISC_GET_GENERAL);/*获取结果*/
    return 0;
}
/********************************************************************
 * 函数： remoteAuth
 * 功能： 点击批量操作后，底层进行发包，验证密码
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-03-18
 ********************************************************************/ 
static int remoteAuth(int eid, webs_t wp, int argc, char **argv)
{
    int i;
    urcp_batch_msg_t urcp_batch_msg;
    char *strMac, *strPasswd;

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));

    memset(&urcp_batch_msg, 0, sizeof(urcp_batch_msg_t));
    /*取mac地址*/
    i = getBatchInfo(wp, strMac, strPasswd, &urcp_batch_msg);
    if(i>0)
    {
	/*发消息,通知交换机进程发包*/
	if((argc==1) && (strcmp(*argv, PAGE_TOPOY)==0))
	{
	    /*拓扑页面的密码验证*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, GET_AUTH_TOPOY, (char*)(&urcp_batch_msg), sizeof(urcp_batch_msg_t));
	}
	else
	{
	    sendwebmsgbuffer(MSG_TYPE_URCP, GET_AUTH, (char*)(&urcp_batch_msg), sizeof(urcp_batch_msg_t));
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
/********************************************************************
 * 函数： remoteAuth
 * 功能： 读取密码验证操作结果
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-03-18
 ********************************************************************/ 
static int aspOutErrorArr(int eid, webs_t wp, int argc, char **argv)
{
    getErrorArr(wp, urcp_read_passwd_auth, MISC_GET_GENERAL);

    return 0;
}
/********************************************************************
 * 函数： getGeneralUrcpSetRet
 * 功能： 获取联动操作结果,通用函数.需在法消息后读取
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： bhou
 *******************************************************************/
extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*转化为微妙*/
    uint32 uip;
    unsigned char std_mac[MAC_LEN];

    memset(std_mac,0,sizeof(std_mac));

    urcp_status_t ret=URCP_STATUS_FAIL;/*默认为读取失败*/

    uip=ip2int(strIp);
    /*转换mac地址*/
    str2mac(strMac, std_mac);
    while(1)
    {
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*再等待*/
	wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
	/*从misc buffer中读取信息*/
	ret = urcp_read_general_conf(std_mac,uip,u_misc_type, misc_buff, conf_buff) ;
	if(ret == URCP_STATUS_WAIT)
	{
	    if(wait_time_us > max_wait_time)
	    {
		ret = URCP_STATUS_TIME_OUT;
	    }
	}
	if(ret != URCP_STATUS_WAIT)
	{
	    break;
	}
    }
    return ret;
}

/********************************************************************
 * 函数： sendGeneralUrcpMsg 
 * 功能： 联动操作发消息通用函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： bhou
 *******************************************************************/
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type)
{
    uint32 uip;

    urcp_general_msg_t  urcp_general_msg;

    uip=ip2int(strIp);
    memset(&urcp_general_msg, 0, sizeof(struct _urcp_general_msg_t));
    /*转换mac地址*/
    str2mac(strMac, urcp_general_msg.dst_mac);
    /*要操作的mac不能为空*/
    strncpy(urcp_general_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_general_msg.user_ip = uip;
    urcp_general_msg.opt_type = u_misc_type;/*操作类型*/
    urcp_general_msg.port_ct = u_port_num;
    /*发消息,通知交换机进程发包*/
    sendwebmsgbuffer(MSG_TYPE_URCP, u_msg_type, (char*)(&urcp_general_msg), sizeof(struct _urcp_general_msg_t));
    return;
}

/********************************************************************
 * 函数： cmd_alone_send_msg
 * 功能： 点击批量操作后，底层进行发包，处理每个单命令（此函数同样适用单机操作）
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou
 * 创建： 2011-05-14
 ********************************************************************/ 
static void cmd_alone_send_msg(webs_t wp, uint32 u_misc_type)
{
    int i;
    urcp_cmd_alone_msg_t urcp_cmd_alone_msg;
    char *strMac, *strPasswd;

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));

    memset(&urcp_cmd_alone_msg, 0, sizeof(struct _urcp_cmd_alone_msg_t));

    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_cmd_alone_msg.batch_info));/*初始化批量操作mac password等*/
    if(i>0)/*初始化成功*/
    {
	urcp_cmd_alone_msg.opt_type = u_misc_type;/*命令类型*/
	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_ALONE, (char*)(&urcp_cmd_alone_msg), sizeof(struct _urcp_cmd_alone_msg_t));
    }
    return;
}
/********************************************************************
 * 函数： getCmdAloneError
 * 功能： 读取单命令操作结果
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou 
 ********************************************************************/ 
static void getCmdAloneError(webs_t wp, uint32 u_misc_type)
{
    getErrorArr(wp,(urcp_fp)(urcp_read_cmd_alone_conf), u_misc_type);
    return;
}
/********************************************************************
 * 函数： aspRebootRemote
 * 功能： 联动重启
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou 
 ********************************************************************/ 
static int aspRebootRemote(int eid, webs_t wp, int argc, char **argv)
{
    cmd_alone_send_msg(wp,MISC_REBOOT);/*发消息重启*/
    sleep(2u);/*等待两秒*/
    getCmdAloneError(wp,MISC_REBOOT);/*打印结果*/
    return 0;
}
/********************************************************************
 * 函数： aspBindRemote
 * 功能： 联动安全绑定
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： bhou 
 * 创建时间:2011-05-16
 ********************************************************************/ 
static int aspBindRemote(int eid, webs_t wp, int argc, char **argv)
{
    cmd_alone_send_msg(wp,MISC_SAFEBIND);/*发消息*/
    sleep(2u);/*等待两秒*/
    getCmdAloneError(wp,MISC_SAFEBIND);/*打印结果*/

    return 0;
}
/********************************************************************
 * 函数： aspResetRemote
 * 功能： 联动恢复出厂配置
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chenshiyu
 * 创建时间:2011-05-26
 ********************************************************************/ 
static int aspResetRemote(int eid, webs_t wp, int argc, char **argv)
{
#if (FEATURE_AC == FYES)
    char *serialNo;
    char *ptr;
    char tmp_path[256];
    char cmd[256];
    char *tmp;

    memset(tmp_path, '\0', sizeof(tmp_path));
    memset(cmd, '\0', sizeof(cmd));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    if (1 == isUSBStorageExist(tmp_path)) {
	    strcat(tmp_path, USB_AP_CONFIG_ROOT);
    } else {
	    strcat(tmp_path, RAM_AP_CONFIG_ROOT);
    }
#else
#if (UTT_WX_2000 == FYES)
	strcat(tmp_path, "/ff/");
	strcat(tmp_path, USB_AP_CONFIG_ROOT);
#else
	strcat(tmp_path, RAM_AP_CONFIG_ROOT);
#endif
#endif

    serialNo = websGetVar(wp, "serialNo", T(""));
    tmp=strtok_r(serialNo,UTT_SPLIT_SIGN_STR,&ptr);
    while(tmp != NULL)
    {
	sprintf(cmd,"rm -f %s%s.xml",tmp_path,tmp);
	printf("cmd = %s\n", cmd);
	system(cmd);
	tmp=strtok_r(NULL,UTT_SPLIT_SIGN_STR,&ptr);
    }
#endif
    cmd_alone_send_msg(wp,MISC_RESET);/*发消息*/
    sleep(2u);/*等待两秒*/
    getCmdAloneError(wp,MISC_RESET);/*打印结果*/

#if (UTT_NV_D908W == FYES)
    /*保存所有AP配置到FALSH中*/
    apConfigWriteToFlash();
#endif
    return 0;
}



/********************************************************************
 * 函数： aspSetPassRemote
 * 功能： 联动密码修改
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： chenshiyu
 * 创建时间:2011-05-30
 ********************************************************************/ 
static int aspSetPassRemote(int eid, webs_t wp, int argc, char **argv)
{
	char *dev_passwd = NULL;
	urcp_pwd_msg_t urcp_pwd_msg;
    char *strMac, *strPasswd;
    int i;

	memset(&urcp_pwd_msg, 0, sizeof(struct _urcp_pwd_msg_t));

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));

    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_pwd_msg.batch_info));/*初始化批量操作mac password等*/
    if(i>0)/*初始化成功*/
    {	
		urcp_pwd_msg.opt_type = MISC_SET_GENERAL;/*命令类型*/

		/* Get the password from web */
		dev_passwd = websGetVar(wp, T("pw1"), T(""));

		strncpy(urcp_pwd_msg.dev_password, dev_passwd, MAX_PASSWORD_LEN);
	
		/*发消息,通知交换机进程发包*/
		sendwebmsgbuffer(MSG_TYPE_URCP, PASSWORD_CONFIG, (char*)(&urcp_pwd_msg), sizeof(struct _urcp_pwd_msg_t));
	}

	sleep(2u);
	getErrorArr(wp, (urcp_fp)(urcp_read_password), MISC_SET_GENERAL);/*打印联动错误信息*/

    return 0;
}
#endif
