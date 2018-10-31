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

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
extern int test_32_bit(unsigned long value);

static void formQosRatelimitUrcp(webs_t wp, char_t *path, char_t *query);
static int getQosRatelimitListUrcp(int eid, webs_t wp, int argc, char **argv);
static void formStormRatelimitUrcp(webs_t wp, char_t *path, char_t *query);

/*****************************************************************
 * ������formDefineRatelimitUrcp
 * ���ܣ�ҳ���ڽ���ת��
 * ��������
 * ���أ���
 * ���ߣ�bhou
 ***************************************************************/
extern void formDefineRatelimitUrcp(void)
{
    websFormDefine(T("formQosRatelimitConfUrcp"), formQosRatelimitUrcp);
    websFormDefine(T("formStormRatelimitConfUrcp"), formStormRatelimitUrcp);
    websAspDefine(T("aspOutPutGetQosRatelimitListUrcp"), getQosRatelimitListUrcp);
}

/***************************************************************
 * ������formQosRatelimitUrcp
 * ���ܣ�����qos����
 * ��������
 * ���أ���
 * ���ߣ�bhou
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

    strMac = websGetVar(wp, "macStr", T(""));/*Զ���豸mac��ַ*/
    if(strcmp(strMac, T(""))!=0) /*Ҫ������mac����Ϊ��*/
    {
	str = websGetVar(wp, "numPorts", T(""));/*Զ���豸�˿ڸ���*/
	portCt = (uint32)strtoul(str, NULL, 10);/*ת��Ϊ����*/

	if((portCt>0u)&&(portCt<=MAX_URCP_PORT_NUM))/*���ڴ˷�Χ���²��������ڴ����*/
	{
	    /*������*/
	    _unit = websGetVar(wp, T("rateunitReg"), T(""));
	    if ((strcmp(_unit, "") != 0) )
	    {
		/*��ȡҳ�����*/
		strIp = websGetVar(wp, "ipStr", T(""));/*Զ���豸ip*/
		strPasswd = websGetVar(wp, "passStr", T(""));/*Զ���豸����*/

		/*���汾�β�����ַ*/
		memcpy(gMac, strMac, sizeof(gMac)-1u);
		memcpy(gIp, strIp, sizeof(gIp)-1u);
		strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

		/*��ʼ��general msg*/
		urcp_general_msg = &(urcp_qos_rate_limit_msg.general_msg);
		/*ת��mac��ַ*/
		str2mac(strMac, urcp_general_msg->dst_mac);
		strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
		urcp_general_msg->user_ip = ip2int(wp->ipaddr);
		urcp_general_msg->port_ct = portCt;

		/*��ʼ�����ݲ���*/
		conf_buff = &(urcp_qos_rate_limit_msg.qos_rate_limit_conf);
		conf_buff->port_num = portCt;
		conf_buff->rate_unit = (uint32)(strtoul(_unit, 0, 10));
		/*ȡ��ҳ��ؼ��е�ֵ*/
		for (i=0u; i<portCt; i++)
		{
		    sprintf(web_name,"R%x",0x50u+i);/*����ģʽ*/
		    _shaper = websGetVar(wp,web_name, T(""));

		    sprintf(web_name,"R%x",0x20u+i);/*����ģʽ*/
		    _policer = websGetVar(wp, web_name, T(""));


		    conf_buff->qos_rate_limit_group[i].shaper = 0xffffffffu; /*������-1*/
		    if (strcmp(_shaper, "-1") != 0)/*����*/
		    {
			conf_buff->qos_rate_limit_group[i].shaper = strtoul(_shaper,0,10);
		    }

		    conf_buff->qos_rate_limit_group[i].policer = 0xffffffffu; /*������-1*/
		    if (strcmp(_policer, "-1") != 0)/*����*/
		    {
			conf_buff->qos_rate_limit_group[i].policer = strtoul(_policer,0,10);
		    }
		}
		/*����Ϣ,֪ͨ���������̷���*/
		sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_QOS_RATE_LIMIT, (char*)(&urcp_qos_rate_limit_msg), sizeof(struct _urcp_qos_rate_limit_msg_t));

	    }
	    else
		/*�����ȶ�ȡ����*/
	    {
		setTheErrorMsg(T("rate unit wrong"));
	    }
	}
	else
	{
	    /*�˿�������*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    websRedirect(wp, "ratelimit.asp?urcpSet=1");
    return;
}

/***************************************************************
 * ������formStormRatelimitUrcp
 * ���ܣ����÷籩��������
 * ��������
 * ���أ���
 * ���ߣ�bhou
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
	urcp_general_msg = &(urcp_storm_rate_limit_msg.general_msg);
	/*ת��mac��ַ*/
	str2mac(strMac, urcp_general_msg->dst_mac);
	strncpy(urcp_general_msg->password, strPasswd, MAX_PASSWORD_LEN);
	urcp_general_msg->user_ip = ip2int(wp->ipaddr);

	/*��ʼ�����ݲ���*/
	conf_buff = &(urcp_storm_rate_limit_msg.storm_rate_limit_conf);

	/*ȡ���籩���Ƹ��������*/
	_uc = websGetVar(wp, T("R10"), T(""));
	_mc = websGetVar(wp, T("R11"), T(""));
	_bc = websGetVar(wp, T("R12"), T(""));
	_icmp = websGetVar(wp, T("R13"), T(""));

	/*����*/
	conf_buff->uc_rate = 0xffffffffu;/*Ĭ��Ϊ�����ƣ���-1*/
	if (strcmp(_uc, "-1") != 0)
	{
	    conf_buff->uc_rate = (unsigned long)0x00000001u << (strtoul(_uc, 0, 10));
	}

	/*�鲥*/
	conf_buff->mc_rate = 0xffffffffu;/*Ĭ��Ϊ�����ƣ���-1*/
	if (strcmp(_mc, "-1") != 0)
	{
	    conf_buff->mc_rate = (unsigned long)0x00000001u << (strtoul(_mc, 0, 10));
	}

	/*�㲥*/
	conf_buff->bc_rate = 0xffffffffu;/*Ĭ��Ϊ�����ƣ���-1*/
	if (strcmp(_bc, "-1") != 0)
	{
	    conf_buff->bc_rate = (unsigned long)0x00000001u << (strtoul(_bc, 0, 10));
	}

	/*icmp*/
	conf_buff->icmp_rate = 0xffffffffu;/*Ĭ��Ϊ�����ƣ���-1*/
	if (strcmp(_icmp, "-1") != 0)
	{
	    conf_buff->icmp_rate = (unsigned long)0x00000001u << (strtoul(_icmp, 0, 10));
	}

	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, URCP_SET_STORM_RATE_LIMIT, (char*)(&urcp_storm_rate_limit_msg), sizeof(struct _urcp_storm_rate_limit_msg_t));

    }
    websRedirect(wp, "ratelimit.asp?urcpSet=2");/*urcpSetΪ2���ڱ�ʾ��һ��Ҫ��ʾ��ҳ����ת�Է籩�������ú�*/
    return;
}
/********************************************************************
 * ������ getStormRateLimitMemberUrcp
 * ���ܣ� ҳ����ʾ��������ӡ�����籩���ƣ�
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 ********************************************************************/ 
extern urcp_status_t getStormRateLimitMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_status_t pre_ret)
{
    urcp_storm_rate_limit_conf_t conf_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/
    misc_opt_assist misc_buff;

    memset(&conf_buff,0,sizeof(urcp_storm_rate_limit_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/
    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_STORM_RATE_LIMIT);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*��ȡ*/
    }
    if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
    {
	memset(&conf_buff,0,sizeof(urcp_storm_rate_limit_conf_t));/*ʧ�ܰ������ô���*/
    }
    /*ҳ���ӡ*/
    websWrite(wp, "var rates = new Array(%d, %d, %d, %d);\n",
	    test_32_bit(conf_buff.uc_rate) , /*����*/
	    test_32_bit(conf_buff.mc_rate),/*�鲥*/
	    test_32_bit(conf_buff.bc_rate),/*�㲥*/
	    test_32_bit(conf_buff.icmp_rate));  /*icmp*/
    return ret;
}
/********************************************************************
 * ������ getQosRateLimitMemberUrcp
 * ���ܣ� ҳ����ʾ��������ӡ������������)
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 ********************************************************************/ 
extern urcp_status_t getQosRateLimitMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{

    uint32 i = 0u;
    urcp_qos_rate_limit_conf_t conf_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/
    misc_opt_assist misc_buff;

    char strShaper[256];
    char tmpstr[16];

    memset(strShaper, 0, sizeof(strShaper));

    memset(&conf_buff,0,sizeof(urcp_qos_rate_limit_conf_t));
    memset(&misc_buff,0,sizeof(misc_buff));

    init_qos_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/
    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_QOS_RATE_LIMIT);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&conf_buff), &misc_buff);/*��ȡ*/
    }
    if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
    {
	memset(&conf_buff,0,sizeof(urcp_qos_rate_limit_conf_t));/*ʧ�ܰ������ô���*/
    }
    if(portNum != NULL)
    {
	*portNum = conf_buff.port_num;/*Զ���豸�˿ڸ���*/
    }

    sprintf(strShaper, "%s", "var shaper = new Array(");
    /*��ӡ��ҳ��*/
    /*������*/
    websWrite(wp, "var rateunit_reg = %lu;\n", conf_buff.rate_unit);
    websWrite(wp,"var policer = new Array(");

    for (i=0u; i<conf_buff.port_num;i++)
    {
	/*�ϴ�*/
	if (i == conf_buff.port_num - 1u)
	{
	    websWrite(wp,"%d", conf_buff.qos_rate_limit_group[i].policer);/*����*/
	    sprintf(tmpstr, "%d", conf_buff.qos_rate_limit_group[i].shaper);/*����*/
	}
	else
	{
	    websWrite(wp,"%d, ", conf_buff.qos_rate_limit_group[i].policer);/*����*/
	    sprintf(tmpstr, "%d, ", conf_buff.qos_rate_limit_group[i].shaper);/*����*/
	}
	strcat(strShaper, tmpstr);
    }
    websWrite(wp,");\n");
    websWrite(wp, "%s);\n", strShaper);
    return ret;
}
/***************************************************************
 * ������getQosRatelimitListUrcp
 * ���ܣ���ӡqos������Ϣ
 * ��������
 * ���أ���
 * ���ߣ�bhou
 * ���ڣ�2011-05-09 
 * **************************************************************/
static int getQosRatelimitListUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*ҳ���ӡ�Ķ˿ڸ���*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));

    /*�ж��Ƿ���formAggrUrcp()��ת������*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    
    if(strcmp(urcpSet, T("1"))==0)/*��ת�������������ú�*/
    {
	is_set = 1u;
	init_qos_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��Ϣ*/
    }
    else
    {
	if(strcmp(urcpSet, T("2"))==0)/*��ת�Է籩�������ú�*/
	{
	    is_set = 1u;
	    init_storm_rate_limit_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��Ϣ*/
	}
    }
    if(is_set==0u)
    {
	/*��һ�δ�*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    }
    else
    {
	/*ҳ�汣������ת*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
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
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_SET_GENERAL,(uchar*)NULL, &misc_buff) ;/*��ȡ���ý��*/ 
    }
    ret = getQosRateLimitMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�������Ʊ�ҳ���Ա*/
    ret = getStormRateLimitMemberUrcp(wp,strMac,strPasswd,ret);/*��ӡ�籩����*/
    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�����Ϣ*/
    if(ret==URCP_STATUS_OK)
    {
	/*�ɹ�*/
	websWrite(wp, "var urcpError = \"\";\n");
	webPortNum=portNum;/*����ȷ������£�ҳ���ӡ�˿ڸ���Ϊ0�������������ʾ����*/
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
    websWrite(wp, "var numPorts = %d;\n", webPortNum);
    /*vlan��Ϣ*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
