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

static unsigned char gMac[STR_MAC_SIZE+1];/*ҳ�汣��ʱҪ������Ŀ��mac,��ҳ����ת��"get"ʹ��*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static uint32 isRefresh = 0u;

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);

static void formSysConfUrcp(webs_t wp, char_t *path, char_t *query);
static int aspOutPutGetSysConfUrcp(int eid, webs_t wp, int argc, char **argv);
static int aspOupPutGetSysInfoUrcp(int eid, webs_t wp, int argc, char_t **argv);
/********************************************************************
 * ������ formDefineSysConfUrcp
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2011-05-30
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou 
 *******************************************************************/
extern void formDefineSysConfUrcp(void)
{
    websAspDefine(T("aspOupPutGetSysInfoUrcp"), aspOupPutGetSysInfoUrcp);
    websAspDefine(T("aspOutPutGetSysConfUrcp"), aspOutPutGetSysConfUrcp);
    websFormDefine(T("formSysConfUrcp"), formSysConfUrcp);
}

/********************************************************************
 * ������formSysConfUrcp
 * ���ܣ�����ϵͳ���� 
 * ����������
 * ����� ��
 * ���أ� ��
 * ���ߣ�bhou 
 * ���ڣ� 2011-05-30
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

    /*����ڴ�*/
    memset(&urcp_sys_conf_msg,0,sizeof(urcp_sys_conf_msg_t));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    isRefresh = 0u;/*ˢ�±�־λ��λ*/

    strMac = websGetVar(wp, "macStr", T(""));/*Զ���豸mac��ַ*/
    if(strcmp(strMac, T(""))!=0) /*Ҫ������mac����Ϊ��*/
    {
	/*��ȡҳ�����*/
	strIp = websGetVar(wp, "ipStr", T(""));/*Զ���豸ip*/
	strPasswd = websGetVar(wp, "passStr", T(""));/*Զ���豸����*/

	/*���汾�β�����ַ*/
	memcpy(gMac, strMac, sizeof(gMac)-1u);
	memcpy(gIp, strIp, sizeof(gIp)-1u);
	strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

	/*��ʼ��general msg*/
	urcp_general_msg = &(urcp_sys_conf_msg.general_msg);
	/*ת��mac��ַ*/
	str2mac(strMac, urcp_general_msg->dst_mac);
	strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
	urcp_general_msg->user_ip = ip2int(wp->ipaddr);

	/*��ʼ�����ݲ���*/
	conf_buff = &(urcp_sys_conf_msg.sys_conf_conf);

	/*ȡ��ҳ��ؼ���ֵ*/
	dhcp_relay = websGetVar(wp, T("dhcpOpt82"), T(""));
	dhcp_client = websGetVar(wp, T("dhcpClient"), T(""));

	/*ip���*/
	cipaddr = websGetVar(wp, T("sysIp"), T(""));
	maskaddr = websGetVar(wp, T("sysMask"), T(""));
	gwaddr = websGetVar(wp, T("sysGw"), T(""));

	/*vid*/
	cpu_id = websGetVar(wp, T("vlanId"), T(""));
	mac_age = websGetVar(wp, T("macAgeTime"), T(""));

	dev_uname = websGetVar(wp, T("deviceName"), T(""));
	dev_password = websGetVar(wp, T("pw1"), T(""));

	/*arp ��ƭ*/
	arp_cheap = websGetVar(wp, T("arp"), T(""));

	conf_buff->dhcp_relay_enable = (uint32)(strtoul(dhcp_relay, 0, 10));/*dhcp�м�*/
	conf_buff->dhcp_client_enable = (uint32)(strtoul(dhcp_client, 0, 10));/*dhcp�ͻ���*/
	conf_buff->ip_addr = ip2int(cipaddr);/*ϵͳip*/
	conf_buff->ip_mask =  ip2int(maskaddr);/*ip����*/
	conf_buff->gw_ip =  ip2int(gwaddr);/*����*/
	conf_buff->cpu_vid =  (uint32)(strtoul(cpu_id, 0, 10));/*cpu vid*/
	conf_buff->age_time =  (uint32)(strtoul(mac_age, 0, 10));/*mac�ϻ�ʱ��*/
	strncpy(conf_buff->dev_name, dev_uname, MAX_DEV_NAME_LEN);/*�豸��*/
	conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';
	strncpy(conf_buff->password, dev_password, MAX_PASSWORD_LEN);/*��¼����*/
	conf_buff->password[MAX_PASSWORD_LEN - 1u] = '\0';
	conf_buff->arp_enable = (uint32)(strtoul(arp_cheap, 0, 10));/*�Ƿ���arp*/

	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_SYS_CONF, (char*)(&urcp_sys_conf_msg), sizeof(struct _urcp_sys_conf_msg_t));
    }
    websRedirect(wp, "switch/sysConf.asp?urcpSet=1");
    return;
}

/********************************************************************
 * ������getIpFlterInfoUrcp
 * ���ܣ���Զ���豸ip������ʾ��ҳ����
 * ��������
 * �������
 * ���أ���
 * ���ߣ�bhou
 * ���ڣ�2011-05-30
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

    init_ip_filter_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/

    if(ret == URCP_STATUS_OK)
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_IP_FILTER);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*��ȡ*/
    }

    if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
    {
	memset(&conf_buff,0,sizeof(urcp_ip_filter_conf_t));/*ʧ�ܰ������ô���*/
    }

    /*ҳ���ӡ*/
    ptr = data;
    websWrite(wp, "var srcIpAddr = new Array(");
    sprintf(ptr, "var srcIpMask = new Array(");

    if(portNum!=NULL)
    {
	*portNum = conf_buff.port_num;
    }

    /*��������bit*/
    for(i=0u; i<conf_buff.port_num; i++)
    {
	/*�ƶ�ָ��*/
	ptr = &(data[strlen(data)]);
	if(i != 0u)
	{
	    websWrite(wp, ",");/*�ָ�*/
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
    /*��β*/
    websWrite(wp, ");\n");
    websWrite(wp, "%s);\n", data);
    return ret;
}

/********************************************************************
 * ������aspOutPutGetSysConfUrcp
 * ���ܣ���Զ���豸������ʾ��ҳ����
 * ��������
 * �������
 * ���أ���
 * ���ߣ�bhou
 * ���ڣ�2011-05-27
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

    init_sys_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/

    /*�ж��Ƿ���form��ת������*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*��һ�δ�*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	is_set = 0u;
	isRefresh = 0u;/*ˢ�±�־λ��λ*/
    }
    else
    {
	/*ҳ�汣������ת*/
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
	is_set = 0u;/*��ʱ�����ٶ�ȡ�Ƿ����óɹ�*/
	ret=URCP_STATUS_FAIL;/*���ձ��β���Ϊ�Ƿ����������ֻ��ӡ������������*/
	/*�����ӡ�ı����Ѿ�û�����壬Ϊ��ҳ�����ʾ�����ִ��󻹵�������*/
    }

    if(is_set == 1u)
    {
	if(isRefresh == 0u)
	{/*���ú���״���ת*/
	    ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_SET_GENERAL,(uchar*)NULL, &misc_buff) ;/*��ȡ���ý��*/ 
	    isRefresh = 1u;/*�������ˢ���򲻶�ȡ���ý���������ý��������ܱ����*/
	}
    }

    if(ret == URCP_STATUS_OK)/*�����ɹ��ŷ�����ȡ*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_SYS_CONF);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*��ȡ*/
    }

    ret=getIpFilterInfoUrcp(wp,strMac,strPasswd,(uint32*)NULL ,ret);/*��ӡip����*/

    if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
    {
	memset(&conf_buff,0,sizeof(urcp_sys_conf_conf_t));/*ʧ�ܰ������ô���*/
    }

     /*ҳ���ӡ*/
    websWrite(wp, "var sysIp = \"%s\";\n", int2ip(conf_buff.ip_addr, str_buf));/*ϵͳip*/
    websWrite(wp, "var sysMask = \"%s\";\n", int2ip(conf_buff.ip_mask, str_buf));/*����*/
    websWrite(wp, "var sysGw = \"%s\";\n", int2ip(conf_buff.gw_ip,str_buf));/*����*/
    websWrite(wp, "var onDhcpClient = %d;\n", conf_buff.dhcp_client_enable);/*dhcp�ͻ���*/

    websWrite(wp, "var onDhcpOpt82 = %u;\n", conf_buff.dhcp_relay_enable);/*dhcp�м�*/
    websWrite(wp, "var vlanId = %u;\n", conf_buff.cpu_vid);/*cpu vid*/
    websWrite(wp, "var macAgeTime = %u;\n", conf_buff.age_time);/*mac�ϻ�ʱ��*/

    websWrite(wp, "var deviceName = \"%s\";\n", conf_buff.dev_name);/*�豸��*/
    websWrite(wp, "var pw = \"%s\";\n", conf_buff.password);/*����*/

    websWrite(wp, "var onArp = %u;\n", conf_buff.arp_enable);/*�Ƿ���arp*/

    if(ret==URCP_STATUS_OK)
    {
	/*�ɹ�*/
	websWrite(wp, "var urcpError = \"\";\n");
    }
    else if(ret<=MAX_ERROR_TYPE)
    {
	/*ʧ��*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[ret]);
    }
    else
    {
	/*ʧ��*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[0]);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}


/********************************************************************
 * ������aspOupPutGetSysInfoUrcp 
 * ���ܣ���ϵͳ��Ϣ��ӡ��ҳ����
 * ��������
 * �������
 * ���أ���
 * ����: bhou 
 * ���ڣ�2011-06-03
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
	init_sys_info_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/

	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_SYS_INFO);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*��ȡ*/

	if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
	{
	    memset(&conf_buff,0,sizeof(urcp_sys_info_conf_t));/*ʧ�ܰ������ô���*/
	}
    }

    sprintf(str_buf, "%lld", conf_buff.outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", str_buf);/*�ܿռ�*/
    sprintf(str_buf, "%lld", conf_buff.outer_flash_unused_space);
    websWrite(wp, "var outerFlashUnusedSpace = \"%s\";\n", str_buf);/*δʹ��*/

    /*��ǰʱ��*/
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

    /*����*/
    websWrite(wp, "var sysGw = \"%s\";\n", int2ip(conf_buff.gw_ip, str_buf));

    /*mac*/ 
    websWrite(wp, "var sysMac = \"%02x-%02x-%02x-%02x-%02x-%02x\";\n",
	    conf_buff.mac_addr[0], conf_buff.mac_addr[1],
	    conf_buff.mac_addr[2], conf_buff.mac_addr[3],
	    conf_buff.mac_addr[4], conf_buff.mac_addr[5]);

    /*���к�*/
    websWrite(wp, "var sysSerial = \"%u\";\n", conf_buff.sys_serialno);


    /*�汾*/
    websWrite(wp, "var sysVersion = \"%s\";\n", conf_buff.sw_version);

    /*cpu vid*/
    websWrite(wp, "var vlanId = %u;\n", conf_buff.cpu_vid);

    /*dev name*/
    websWrite(wp, "var deviceName = \"%s\";\n", conf_buff.dev_name);


    if(ret==URCP_STATUS_OK)
    {
	/*�ɹ�*/
	websWrite(wp, "var urcpError = \"\";\n");
    }
    else if(ret<=MAX_ERROR_TYPE)
    {
	/*ʧ��*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[ret]);
    }
    else
    {
	/*ʧ��*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[0]);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
