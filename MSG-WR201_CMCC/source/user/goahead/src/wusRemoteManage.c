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
const char *urcpErrorMsg[]={"����ʧ��","������ʱ","�����ɹ�","������ʱ","�������","����ʧ��","����ʧ��"};
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
 * ������ formDefineTopoyDiscovery
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
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
 *����������������ظ�ҳ��
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
	/*�ɹ�*/
	websWrite(wp, "\"\"");
    }
    else
    {
	/*ʧ��*/
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
 *��������������֤
 *bhou
 */
int aspSignalRemoteAuth(int eid, webs_t wp, int argc, char **argv)
{
    remoteAuth(eid, wp, argc, argv);/*����*/
    sleep(1u);/*�ȴ�һ��*/
    getErrorArr(wp,(urcp_fp)(urcp_read_passwd_auth), MISC_GET_GENERAL);/*��ȡ���*/
    return 0;
}
/********************************************************************
 * ������ remoteAuth
 * ���ܣ� ������������󣬵ײ���з�������֤����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2011-03-18
 ********************************************************************/ 
static int remoteAuth(int eid, webs_t wp, int argc, char **argv)
{
    int i;
    urcp_batch_msg_t urcp_batch_msg;
    char *strMac, *strPasswd;

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));

    memset(&urcp_batch_msg, 0, sizeof(urcp_batch_msg_t));
    /*ȡmac��ַ*/
    i = getBatchInfo(wp, strMac, strPasswd, &urcp_batch_msg);
    if(i>0)
    {
	/*����Ϣ,֪ͨ���������̷���*/
	if((argc==1) && (strcmp(*argv, PAGE_TOPOY)==0))
	{
	    /*����ҳ���������֤*/
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
 * ������ remoteAuth
 * ���ܣ� ��ȡ������֤�������
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2011-03-18
 ********************************************************************/ 
static int aspOutErrorArr(int eid, webs_t wp, int argc, char **argv)
{
    getErrorArr(wp, urcp_read_passwd_auth, MISC_GET_GENERAL);

    return 0;
}
/********************************************************************
 * ������ getGeneralUrcpSetRet
 * ���ܣ� ��ȡ�����������,ͨ�ú���.���ڷ���Ϣ���ȡ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;
    unsigned char std_mac[MAC_LEN];

    memset(std_mac,0,sizeof(std_mac));

    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(strIp);
    /*ת��mac��ַ*/
    str2mac(strMac, std_mac);
    while(1)
    {
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ٵȴ�*/
	wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
	/*��misc buffer�ж�ȡ��Ϣ*/
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
 * ������ sendGeneralUrcpMsg 
 * ���ܣ� ������������Ϣͨ�ú���
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type)
{
    uint32 uip;

    urcp_general_msg_t  urcp_general_msg;

    uip=ip2int(strIp);
    memset(&urcp_general_msg, 0, sizeof(struct _urcp_general_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_general_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_general_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_general_msg.user_ip = uip;
    urcp_general_msg.opt_type = u_misc_type;/*��������*/
    urcp_general_msg.port_ct = u_port_num;
    /*����Ϣ,֪ͨ���������̷���*/
    sendwebmsgbuffer(MSG_TYPE_URCP, u_msg_type, (char*)(&urcp_general_msg), sizeof(struct _urcp_general_msg_t));
    return;
}

/********************************************************************
 * ������ cmd_alone_send_msg
 * ���ܣ� ������������󣬵ײ���з���������ÿ��������˺���ͬ�����õ���������
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 * ������ 2011-05-14
 ********************************************************************/ 
static void cmd_alone_send_msg(webs_t wp, uint32 u_misc_type)
{
    int i;
    urcp_cmd_alone_msg_t urcp_cmd_alone_msg;
    char *strMac, *strPasswd;

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));

    memset(&urcp_cmd_alone_msg, 0, sizeof(struct _urcp_cmd_alone_msg_t));

    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_cmd_alone_msg.batch_info));/*��ʼ����������mac password��*/
    if(i>0)/*��ʼ���ɹ�*/
    {
	urcp_cmd_alone_msg.opt_type = u_misc_type;/*��������*/
	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_CMD_ALONE, (char*)(&urcp_cmd_alone_msg), sizeof(struct _urcp_cmd_alone_msg_t));
    }
    return;
}
/********************************************************************
 * ������ getCmdAloneError
 * ���ܣ� ��ȡ������������
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou 
 ********************************************************************/ 
static void getCmdAloneError(webs_t wp, uint32 u_misc_type)
{
    getErrorArr(wp,(urcp_fp)(urcp_read_cmd_alone_conf), u_misc_type);
    return;
}
/********************************************************************
 * ������ aspRebootRemote
 * ���ܣ� ��������
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou 
 ********************************************************************/ 
static int aspRebootRemote(int eid, webs_t wp, int argc, char **argv)
{
    cmd_alone_send_msg(wp,MISC_REBOOT);/*����Ϣ����*/
    sleep(2u);/*�ȴ�����*/
    getCmdAloneError(wp,MISC_REBOOT);/*��ӡ���*/
    return 0;
}
/********************************************************************
 * ������ aspBindRemote
 * ���ܣ� ������ȫ��
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou 
 * ����ʱ��:2011-05-16
 ********************************************************************/ 
static int aspBindRemote(int eid, webs_t wp, int argc, char **argv)
{
    cmd_alone_send_msg(wp,MISC_SAFEBIND);/*����Ϣ*/
    sleep(2u);/*�ȴ�����*/
    getCmdAloneError(wp,MISC_SAFEBIND);/*��ӡ���*/

    return 0;
}
/********************************************************************
 * ������ aspResetRemote
 * ���ܣ� �����ָ���������
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chenshiyu
 * ����ʱ��:2011-05-26
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
    cmd_alone_send_msg(wp,MISC_RESET);/*����Ϣ*/
    sleep(2u);/*�ȴ�����*/
    getCmdAloneError(wp,MISC_RESET);/*��ӡ���*/

#if (UTT_NV_D908W == FYES)
    /*��������AP���õ�FALSH��*/
    apConfigWriteToFlash();
#endif
    return 0;
}



/********************************************************************
 * ������ aspSetPassRemote
 * ���ܣ� ���������޸�
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� chenshiyu
 * ����ʱ��:2011-05-30
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

    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_pwd_msg.batch_info));/*��ʼ����������mac password��*/
    if(i>0)/*��ʼ���ɹ�*/
    {	
		urcp_pwd_msg.opt_type = MISC_SET_GENERAL;/*��������*/

		/* Get the password from web */
		dev_passwd = websGetVar(wp, T("pw1"), T(""));

		strncpy(urcp_pwd_msg.dev_password, dev_passwd, MAX_PASSWORD_LEN);
	
		/*����Ϣ,֪ͨ���������̷���*/
		sendwebmsgbuffer(MSG_TYPE_URCP, PASSWORD_CONFIG, (char*)(&urcp_pwd_msg), sizeof(struct _urcp_pwd_msg_t));
	}

	sleep(2u);
	getErrorArr(wp, (urcp_fp)(urcp_read_password), MISC_SET_GENERAL);/*��ӡ����������Ϣ*/

    return 0;
}
#endif
