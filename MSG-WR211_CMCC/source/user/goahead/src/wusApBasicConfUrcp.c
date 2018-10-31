#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h> /*inet_addr*/
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    "webs.h"
#include    "utils.h"
#include    "uttfunction.h"
#include    "mib.h"

#if (FEATURE_AP_BASIC_CONF == FYES)

#include    <urcp_ipc.h>
#include    <sw_cfg.h>
#include    "webmsg.h"
#include    <mibUrcp.h>

#define STR_MAC_SIZE   12
#define STR_IP_SIZE    15
#define MAX_ERROR_TYPE 5u

extern const char *urcpErrorMsg[];

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);
extern void aspOutUrcpError(webs_t wp, urcp_status_t ret);

static unsigned char gMac[STR_MAC_SIZE+1];/*ҳ�汣��ʱҪ������Ŀ��mac,��ҳ����ת��"get"ʹ��*/
static unsigned char gSzName[64+1];/*ҳ�汣��ʱҪ������Ŀ��SZ,��ҳ����ת��"get"ʹ��*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static unsigned char gManaged[64+1];/*ҳ�汣��ʱҪ������Ŀ��SZ,��ҳ����ת��"get"ʹ��*/
static uint32 isRefresh = 0u;

/********************************************************************
* ������ getApBasicConfigUrcp
* ���ܣ� ҳ���뽻��ת������
* ������ 2012-06-20
* ������ ��
* ����� ��
* ���أ� ��
* ���ߣ� xhhu 
*******************************************************************/
static int getApBasicConfigUrcp(int eid, webs_t wp, int argc, char **argv)
{
	char buf[20];
    char str_ip[16];
    char str_mask[16];
    char str_gw[16];
    unsigned char std_mac[MAC_LEN];
    misc_opt_assist misc_buff;
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet, *strSzName, *strManaged;
    uint32 is_set=0u;
    urcp_dev_t dev, *tmp= NULL; 
    urcp_ap_basic_conf_t ap_basic_conf;
    int i = 0, val = 0;
    memset(&misc_buff,0,sizeof(misc_buff));

    init_ap_basic_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/

    /*�ж��Ƿ���form��ת������*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    /* �����ǵ�һ�δ򿪣��ǿյĻ�˵���Ǳ�������ת */
    if(strcmp(urcpSet, T(""))==0)
    {
	/*��һ�δ�*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	strSzName = websGetVar(wp, "szName", T(""));
	strManaged = websGetVar(wp, "managedEn", T(""));
	is_set = 0u;
	isRefresh = 0u;/*ˢ�±�־λ��λ*/
    }
    else
    {
	/*ҳ�汣������ת*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	strSzName = gSzName;
	strManaged = gManaged;
	is_set =  1u;
    }
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", strSzName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);
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
	    //printf("\n< %s %d > ret : %d\n", __func__,__LINE__,ret);
	    isRefresh = 1u;/*�������ˢ���򲻶�ȡ���ý���������ý��������ܱ����*/
	}
    }
    if(ret == URCP_STATUS_OK)/*�����ɹ��ŷ�����ȡ*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, GET_AP_BASIC_CONFIG);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL, (uchar*)(&ap_basic_conf), &misc_buff);/*��ȡ*/
	    //printf("\n< %s %d > ret : %d\n", __func__,__LINE__,ret);
    }
    websWrite(wp, "var urcpError = ");
    aspOutUrcpError(wp, ret);
    websWrite(wp, ";\n");

    memset(std_mac,0,sizeof(std_mac));
    str2mac(strMac, std_mac);
    tmp = urcp_dev_table_lookup(std_mac, 0);
    if(tmp==NULL)
    {
	memset(&dev, 0, sizeof(urcp_dev_t));
	tmp = &dev;
    }
    /* ҳ���ӡ */
    /* ����ģʽ */
	sprintf(buf, "%d", tmp->dhcp_en);
    websWrite(wp, "var ipStr = \"%s\";\n", int2ip(tmp->ip, str_ip));
	websWrite(wp, "var dhcp_en = \"%s\";\n", buf);
    if(tmp->dev_type&DEV_TYPE_FAT_AP)
    {
	websWrite(wp, "var workingMode = 0;\n");
    }
    else
    {
	websWrite(wp, "var workingMode = 1;\n");
    }
    /* �豸���� */
    websWrite(wp, "var deviceName = \"%s\";\n", tmp->dev_name);
    /* ip��ַ */
    websWrite(wp, "var ipAddr = \"%s\";\n", int2ip(tmp->ip, str_ip));
    websWrite(wp, "var netMask = \"%s\";\n", int2ip(tmp->netmask, str_mask));
    websWrite(wp, "var gateWay = \"%s\";\n", int2ip(tmp->gateway, str_gw));
    /* �û��� */
    if(strcmp(tmp->user, "")!=0)
    {
	websWrite(wp, "var userName = \"%s\";\n", tmp->user);
    }
    else
    {
	websWrite(wp, "var userName = \"admin\";\n");
    }
    /* ���� */
    /* ���ͻ�����Ŀ */
    //websWrite(wp, "var maxClient = %d;\n", tmp->wl_info.max_clients);
    /* �ŵ� */
    websWrite(wp, "var channel = %d;\n", tmp->wl_info.channel_head);
    websWrite(wp, "var channel5G = %d;\n", tmp->wl_info_5G.channel_head);
    if (((tmp->dev_type)&DEV_TYPE_WIRELESS_5G) != 0)
    {
	websWrite(wp, "var flag_5G = %d;\n", 1);
    }
    else
    { 
	websWrite(wp, "var flag_5G = %d;\n", 0);
    }
    if (tmp->dbm == 0) {
	websWrite(wp, "var roaming_en = 0;\n");
	websWrite(wp, "var roaming_th = \"-%d\";\n", 88);
    } else {
	    websWrite(wp, "var roaming_en = 1;\n");
	    websWrite(wp, "var roaming_th = \"-%d\";\n", tmp->dbm);
    }

    if (tmp->dbm_5g == 0) {
	websWrite(wp, "var roaming_en5G = 0;\n");
	websWrite(wp, "var roaming_th5G = \"-%d\";\n", 88);
    } else {
	    websWrite(wp, "var roaming_en5G = 1;\n");
	    websWrite(wp, "var roaming_th5G = \"-%d\";\n", tmp->dbm_5g);
    }

    //printf("\n< %s %d > conf:%d\n", __func__, __LINE__, ap_basic_conf.effect_conf);
    websWrite(wp, "var configuration = %d;\n", ap_basic_conf.effect_conf);

    websWrite(wp, "var szName = new Array();\n");
    websWrite(wp, "var userNum = new Array();\n");
    websWrite(wp, "var maxUserNum = %d;\n", tmp->wl_info.max_clients);
    val = tmp->dev_type;
    if(val) 
    {
	val = tmp->wl_info.joined_sz_num;
	if(val>MAX_SSID_NUM)
	{
	    val = MAX_SSID_NUM;
	}
    }
    else
    {
	val = 1;
    }
    for(i = 0; i < val; i++)
    {
	if (tmp->wl_info.ssid_info[i].szone[0] != '\0')
	{
	    websWrite(wp, "szName[%d] = \"%s\";\n",i, tmp->wl_info.ssid_info[i].szone);
	    websWrite(wp, "userNum[%d] = %d;\n",i, tmp->wl_info.ssid_info[i].max_clients);
	}
    }
    
#if 0
    if(is_set == 1u)
    {
	/* ��Ч���� */
	websWrite(wp, "var configuration = %d;\n", ntohl(ap_basic_conf.effect_conf));
    }
    else
    {
	printf("\n< %s %d > flag:%d\n", __func__, __LINE__, tmp->wl_info.ap_flag);
	if(tmp->wl_info.ap_flag & AP_PROFILE_LOCAL)
	{
	    websWrite(wp, "var configuration = 1;\n");
	}
	else
	{
	    websWrite(wp, "var configuration = 0;\n");
	}
    }
#endif

    getTheErrorMsg(eid, wp, argc, argv);
	
    return 0;
}

/********************************************************************
* ������ formApBasicProcessForUrcp
* ���ܣ� ҳ���뽻��ת������
* ������ 2012-06-20
* ������ ��
* ����� ��
* ���أ� ��
* ���ߣ� xhhu 
*******************************************************************/
#if 0
static uint32 formApBasicProcessForUrcp(webs_t wp,urcp_ ap_basic_msg_t *urcp_ ap_basic_msg)
{
    urcp_ ap_basic_msg ->opt_type = MISC_SET_GENERAL;/*����*/
}
#endif
/********************************************************************
* ������ formApBasicConfigUrcp
* ���ܣ� ҳ���뽻��ת������
* ������ 2012-06-20
* ������ ��
* ����� ��
* ���أ� ��
* ���ߣ� xhhu 
*******************************************************************/
static void formApBasicConfigUrcp(int eid, webs_t wp, int argc, char **argv)
{
    char *workingMode = NULL;/* ����ģʽ */
    char *deviceName = NULL;/* �豸�� */
    char *ipAddr = NULL;/* ip��ַ */
    char *netMask = NULL;
    char *gateWay = NULL;/* ���ص�ַ */
    char *userName = NULL;/* �û��� */
    char *pswd = NULL;/* ���� */
	char *dhcp_en = NULL;/* dhcp�ͻ��� */
    char *pswdModify = NULL;/* ���� */
//    char *maxClient = NULL;/* ���ͻ�����Ŀ */
    char *chnl = NULL;/* �ŵ� */
    char *channel_5g = NULL;
    char *conf = NULL;/* ��Ч���� */
    char *needReboot = NULL;
	char *roamEn = NULL, *roamEn5g = NULL ; /* ������ֵ */
	char *roamTh = NULL, *roamTh5g = NULL ; /* ������ֵ */
    char *str = NULL;	    /*���������ͻ����������ַ���*/
    uint32 i = 0, j = 0;
    unsigned char std_mac[MAC_LEN];
    urcp_dev_t dev, *tmp= NULL; 
    int val = 0, user_num = 0, num = 0;
    char name[32] = "";

    uchar *strMac, *strIp, *strPasswd,*strSzName,*strManaged;
    urcp_ap_basic_conf_msg_t urcp_ap_basic_msg;
    urcp_ap_basic_conf_t *ap_basic_conf_buff;

    /*����ڴ�*/
    memset(&urcp_ap_basic_msg,0,sizeof(urcp_ap_basic_conf_msg_t));
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    memset(gSzName, 0, sizeof(gSzName));
    memset(gManaged, 0, sizeof(gManaged));
    isRefresh = 0u;/*ˢ�±�־λ��λ*/

    strMac = websGetVar(wp, "macStr", T(""));/*Զ���豸mac��ַ*/
    if(strcmp(strMac, T(""))!=0) /*Ҫ������mac����Ϊ��*/
    {
	/*��ȡҳ�����*/
	strIp = websGetVar(wp, "ipStr", T(""));/*Զ���豸ip*/
	strPasswd = websGetVar(wp, "passStr", T(""));/*Զ���豸����*/
	strSzName = websGetVar(wp, "szName", T(""));/*Զ���豸������*/
	strManaged = websGetVar(wp, "managedEn", T(""));/*Զ���豸������*/

	/*���汾�β�����ַ*/
	memcpy(gMac, strMac, sizeof(gMac)-1u);
	memcpy(gIp, strIp, sizeof(gIp)-1u);
	strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);
	memcpy(gSzName, strSzName, sizeof(gSzName)-1u);
	memcpy(gManaged, strManaged, sizeof(gManaged)-1u);

	/*��ʼ�� msg*/
	//printf("\nstrMac : %s\n", strMac);
	str2mac(strMac, urcp_ap_basic_msg.general_msg.dst_mac);
#if 0
	printf("\nDmac:\n");
	for (i=0; i<6; i++)
	{
	    printf(" %02x", urcp_ap_basic_msg.general_msg.dst_mac[i]);
	}
	    printf(" \n");
#endif
	strncpy(urcp_ap_basic_msg.general_msg.password, strPasswd, MAX_PASSWORD_LEN);
	urcp_ap_basic_msg.general_msg.user_ip = ip2int(wp->ipaddr);
	urcp_ap_basic_msg.general_msg.opt_type = MISC_SET_GENERAL;/* ������Ϣ���� */

	/*��ʼ�����ݲ���*/
	ap_basic_conf_buff = &(urcp_ap_basic_msg.ap_basic_conf);

	/*ȡ��ҳ��ؼ���ֵ*/
	workingMode = websGetVar(wp, "ap_mode", T(""));
	deviceName = websGetVar(wp, "deviceName", T(""));
	ipAddr = websGetVar(wp, "sysIp", T(""));
	netMask = websGetVar(wp, "subMask", T(""));
	gateWay = websGetVar(wp, "gateWay", T(""));
	userName = websGetVar(wp, "userName", T(""));
	pswd = websGetVar(wp, "pw1", T(""));
	dhcp_en = websGetVar(wp, "dhcp_en", T(""));
	roamEn = websGetVar(wp, "roaming_en", T(""));
	roamTh = websGetVar(wp, "roaming_th", T(""));
	roamEn5g = websGetVar(wp, "roaming_en5G", T(""));

	roamTh5g = websGetVar(wp, "roaming_th5G", T(""));
	pswdModify = websGetVar(wp, "modifyPw", T(""));
	//maxClient = websGetVar(wp, "max_user", T(""));
	chnl = websGetVar(wp, "channel", T(""));
	channel_5g = websGetVar(wp, "channel5G", T(""));
	conf = websGetVar(wp, "config", T(""));
	needReboot = websGetVar(wp, "reboot", T(""));

	/* �����ṹ */
	ap_basic_conf_buff->work_mode = (uint32)(strtoul(workingMode, 0, 10));
	strncpy(ap_basic_conf_buff->dev_name, deviceName, MAX_DEV_NAME_LEN);
	ap_basic_conf_buff->dev_name[MAX_DEV_NAME_LEN - 1u] = '\0';
	ap_basic_conf_buff->ip_addr = ip2int(ipAddr);
	strncpy(gIp, ipAddr, (STR_IP_SIZE+1));
	ap_basic_conf_buff->sub_mask = ip2int(netMask);
	ap_basic_conf_buff->gate_way = ip2int(gateWay);
	if(strcmp(dhcp_en, "1")==0)
	{
	    ap_basic_conf_buff->dhcp_en = 1;
	} else {
	    ap_basic_conf_buff->dhcp_en = 0;
	}

	if(strcmp(roamEn, "1")==0)
	{
		ap_basic_conf_buff->dbm = 0 - (uint32)strtol(roamTh, NULL, 10);
	} else {
		ap_basic_conf_buff->dbm = 0;
	}

	if(strcmp(roamEn5g, "1")==0)
	{
		ap_basic_conf_buff->dbm_5g = 0 - (uint32)strtol(roamTh5g, NULL, 10);
	} else {
		ap_basic_conf_buff->dbm_5g = 0;
	}


	if(strcmp(pswdModify, "1")==0)
	{
	    ap_basic_conf_buff->pass_modify = 1;
	    strncpy(ap_basic_conf_buff->password, pswd, MAX_PASSWORD_LEN);
	    ap_basic_conf_buff->password[MAX_PASSWORD_LEN - 1u] = '\0';
	}
	//ap_basic_conf_buff->max_clt = (uint32)(strtoul(maxClient, 0, 10));
	ap_basic_conf_buff->channel = (uint32)(strtoul(chnl, 0, 10));
	if (channel_5g != NULL)
	{
	    ap_basic_conf_buff->channel_5g = (uint32)(strtoul(channel_5g, 0, 10));
	}
	if(strcmp(conf, "local")==0)
	{
	    ap_basic_conf_buff->effect_conf = 1;
	}
	else
	{
	    ap_basic_conf_buff->effect_conf = 0;
	}

	/*�ӽ����ȡ����������*/
	ap_basic_conf_buff->sz_max_client.sz_num = 0;
	memset(std_mac,0,sizeof(std_mac));
	str2mac(strMac, std_mac);
	tmp = urcp_dev_table_lookup(std_mac, 0);
	if (tmp != NULL)
	{
	    str = websGetVar(wp, "szUserNums", T(""));
	    val = tmp->dev_type;
	    if(val) 
	    {
		val = tmp->wl_info.joined_sz_num;
		if(val>MAX_SSID_NUM)
		{
		    val = MAX_SSID_NUM;
		}
	    }
	    else
	    {
		val = 1;
	    }
	    for(num = 0,i = 0; i < val; i++)
	    {
		if (sscanf(str, "%[^:]:%d", name, &user_num) == 2)
		{
		    if ( (strncmp(name, tmp->wl_info.ssid_info[i].szone, sizeof(tmp->wl_info.ssid_info[i].szone)) == 0) && \
			    (user_num != 0) && (user_num != tmp->wl_info.ssid_info[i].max_clients))
		    {
			strncpy(ap_basic_conf_buff->sz_max_client.sz[num].sz_name, name, sizeof(ap_basic_conf_buff->sz_max_client.sz[num].sz_name));
			ap_basic_conf_buff->sz_max_client.sz[num].user_num = user_num;
			num++;
//			printf("%s,%s,%d\n", __func__,ap_basic_conf_buff->sz_max_client.sz[num-1].sz_name, ap_basic_conf_buff->sz_max_client.sz[num-1].user_num);
		    }
		}
		else
		{
		    break;
		}
		str = strchr(str, ',');
		if (str == NULL)
		{
		    break;
		}
		else
		{
		    str+=1;
		}
	    }
	    ap_basic_conf_buff->sz_max_client.sz_num = num;
	}

	/* ����Ϣ */
	sendwebmsgbuffer(MSG_TYPE_URCP, SET_AP_BASIC_CONFIG, (char*)(&urcp_ap_basic_msg), sizeof(urcp_ap_basic_conf_msg_t));
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return;
}
/********************************************************************
* ������ aspChangeChannel
* ���ܣ� ���������ŵ�����
* ������ 2012-06-20
* ������ ��
* ����� ��
* ���أ� ��
* ���ߣ�
*******************************************************************/
static int aspChangeChannel(int eid, webs_t wp, int argc, char **argv)
{
    int i=0;
    char *strMac, *strPasswd, *strValue=NULL;
    urcp_send_channel_msg_t urcp_send_channel_msg;

    memset(&urcp_send_channel_msg, 0, sizeof(urcp_send_channel_msg_t));

    /*��ҳ������*/
    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
	strValue = websGetVar(wp, "channelSelect", T(""));
    /*ȡmac��ַ*/
    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_send_channel_msg.batch_info));
    if(i>0)
    {

    urcp_send_channel_msg.channel = strtoul(strValue,NULL,10);
	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, CHANGE_CHANNEL, (char*)(&urcp_send_channel_msg), sizeof(urcp_send_channel_msg_t));
    }

	sleep(2u);
    getErrorArr(wp, urcp_read_ap_basic_conf, MISC_SET_GENERAL);

    return 0;

}
/********************************************************************
 * ������ formDefineApBasicConfUrcp
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2012-06-20
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� xhhu 
 *******************************************************************/
extern void formDefineApBasicConfUrcp(void)
{
    websAspDefine(T("getApBasicConfigUrcp"), getApBasicConfigUrcp);
    websAspDefine(T("formApBasicConfigUrcp"), formApBasicConfigUrcp);
    websAspDefine(T("aspChangeChannel"), aspChangeChannel);
}


#endif
