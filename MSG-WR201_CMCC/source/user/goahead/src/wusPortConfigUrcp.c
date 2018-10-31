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

static void formPortConfigureUrcp(webs_t wp, char_t *path, char_t *query);
int getPortConfigureUrcp(int eid, webs_t wp, int argc, char **argv);

extern urcp_status_t getGeneralUrcpConf(char *strIp,uchar* strMac,uint32 u_misc_type,uchar* conf_buff, misc_opt_assist* misc_buff);
extern void sendGeneralUrcpMsg(char* strIp, uchar* strMac, uchar* strPasswd,uint32 u_port_num,uint32 u_misc_type, uint32 u_msg_type);
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);

/********************************************************************
 * ������ fromDefinePortStatics
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2010-08-16
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� jfgu
 * ά���� jfgu
 * ���ڣ� 2010-09-01
 *******************************************************************/
void formDefinePortConfigureUrcp(void)
{
	websAspDefine(T("aspOutPortConfigureUrcp"), getPortConfigureUrcp);
	websFormDefine(T("formPortConfigureUrcp"), formPortConfigureUrcp);	
}

/********************************************************************
 * ������ getportconfmemberurcp
 * ���ܣ� ҳ����ʾ��������ӡ�����˿ڹ�����Ϣ��
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
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
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/
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

    init_port_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ����Ϣ*/
    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,PORT_COUNT,MISC_GET_GENERAL, URCP_GET_PORT_CONF);/*����Ϣ*/
	ret = getGeneralUrcpConf(wp->ipaddr,strMac,MISC_GET_GENERAL,(uchar*)(&port_conf_buff), &misc_buff);/*��ȡ*/
    }
    if(ret != URCP_STATUS_OK)/*����ʧ�������ӡ������ҳ����ܳ���*/
    {
	memset(&port_conf_buff,0,sizeof(urcp_port_conf_conf_t));/*ʧ�ܰ������ô���*/
    }
    if(portNum != NULL)
    {
	*portNum = port_conf_buff.port_num;/*Զ���豸�˿ڸ���*/
    }

    for (i=0u; i<port_conf_buff.port_num ; i++)
    {
	if (i > 0u)
	{
	    start_index = 1u;
	    m_buffer[0] = ',';
	}
	/*ģʽ*/
	sprintf(&m_buffer[start_index], "\"%d\"", port_conf_buff.port_conf_group[i].selected_mode);
	strcat(speed_mode, m_buffer);

	/*���֡*/
	sprintf(&m_buffer[start_index], "\"%d\"", port_conf_buff.port_conf_group[i].max_frame);
	strcat(_max_frame, m_buffer);

	/*����*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].rate_limit);
	strcat(str_fc, m_buffer);

	/*�˿ڱ���*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].protect);
	strcat(str_protect, m_buffer);

	/*����*/
	sprintf(&m_buffer[start_index], "%d", port_conf_buff.port_conf_group[i].flooding);
	strcat(str_flood, m_buffer);

	/*�˿�״̬*/
	sprintf(&m_buffer[start_index], "\"%s\"", port_conf_buff.port_conf_group[i].link_status);
	strcat(cur_speed, m_buffer);

	/*�˿�����*/
	sprintf(&m_buffer[start_index], "\"%s\"", port_conf_buff.port_conf_group[i].remark);
	strcat(port_info, m_buffer);
    }

    /*��ӡ��ҳ��*/
    websWrite(wp, "var curSpeed = new Array(%s);\n", cur_speed);/*����״̬*/
    websWrite(wp, "var speedMode = new Array(%s);\n", speed_mode);/*ģʽ*/
    websWrite(wp, "var fc = new Array(%s);\n", str_fc);/*����*/
    websWrite(wp, "var maxFrame = new Array(%s);\n", _max_frame);/*���֡*/
    websWrite(wp, "var portprotect = new Array(%s);\n", str_protect);/*�˿ڱ���*/
    websWrite(wp, "var portInfo = new Array(%s); \n",  port_info);/*�˿�����*/
    websWrite(wp, "var floodings = new Array(%s); \n", str_flood); /*����*/

    return ret;
}
/********************************************************************
 * ������ getPortConfigureUrcp
 * ���ܣ� ҳ����ʾ��������ʾ�����˿ڹ���ҳ��������Ϣ��
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou 
 ********************************************************************/ 
int getPortConfigureUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*ҳ���ӡ�Ķ˿ڸ���*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;
    misc_opt_assist misc_buff;

    memset(&misc_buff,0,sizeof(misc_buff));
    init_port_conf_misc_opt_assist(&misc_buff);/*��ʼ�������ṹ��Ϣ*/

    
    /*�ж��Ƿ���formAggrUrcp()��ת������*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*��һ�δ�*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	isRefresh = 0u;/*ˢ�±�־λ��λ*/
	is_set = 0u;
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
	    if(ret == URCP_STATUS_OK)
	    {
		sleep(4);/*����3�롣���漰���˿ڵĲ��������ܸö˿�����������*/
	    }
	    isRefresh = 1u;/*�������ˢ���򲻶�ȡ���ý���������ý��������ܱ����*/
	}
    }
    ret = getPortConfMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�˿����ñ�ҳ���Ա*/
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

/*******************************************************************
 *������ formPortConfigureUrcp
 *���ܣ� ҳ���ύ����
 *������ ��
 *���أ� ��
 *���ߣ� bhou 
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
    isRefresh = 0u;/*ˢ�±�־λ��λ*/

    strMac = websGetVar(wp, "macStr", T(""));/*Զ���豸mac��ַ*/
    if(strcmp(strMac, T(""))!=0)
    {
	str = websGetVar(wp, "numPorts", T(""));/*Զ���豸�˿ڸ���*/
	portCt = (uint32)strtoul(str, NULL, 10);/*ת��Ϊ����*/

	if((portCt>0u)&&(portCt<=MAX_URCP_PORT_NUM))/*���ڴ˷�Χ���²��������ڴ����*/
	{

	    /*��ȡҳ�����*/
	    strIp = websGetVar(wp, "ipStr", T(""));/*Զ���豸ip*/
	    strPasswd = websGetVar(wp, "passStr", T(""));/*Զ���豸����*/

	    /*���汾�β�����ַ*/
	    memcpy(gMac, strMac, sizeof(gMac)-1u);
	    memcpy(gIp, strIp, sizeof(gIp)-1u);
	    strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

	    str = websGetVar(wp, "hiddenvalue", T(""));/*�˿ڱ���*/
	    u_protect = (uint32)(strtoul(str, 0, 10));
	    str = websGetVar(wp, "hiddenvalue2", T(""));/*����*/
	    u_flooding = (uint32)(strtoul(str, 0, 10));

	    _port_config.port_num = portCt;
	    /*ȡ��ҳ��ؼ��е�ֵ*/
	    for (i=0u; i<portCt; i++)
	    {
		sprintf(web_name,"R%x",0x50u+i);/*����ģʽ*/
		_port_config.port_conf_group[i].selected_mode = (int)(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0));	

		sprintf(web_name,"R%x",0x11u+i);/*�������֡*/
		_port_config.port_conf_group[i].max_frame = (int)(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0));	

		sprintf(web_name,"R%x",0xd0u+i);/*����*/

		if(strtoul(websGetVar(wp,web_name,T("")),  NULL, 0)>0u)
		{
		    _port_config.port_conf_group[i].rate_limit = 1;	
		}
		if((u_protect & ((uint32)1u << i))>0u)/*�˿ڱ���*/
		{
		    _port_config.port_conf_group[i].protect = 1;
		}
		else/*������˿ڱ�������ͬʱ����*/
		{
		    /*����*/
		    if((u_flooding & ((uint32)1u << i))>0u)
		    {
			_port_config.port_conf_group[i].flooding = 1;
		    }
		}
		/*�˿ڱ�ע*/
		sprintf(web_name,"portInfo%u",i+1u);
		m_portInfo = websGetVar(wp, web_name, T(""));
		strncpy(_port_config.port_conf_group[i].remark,m_portInfo,MAX_PORT_REMARK_LEN);
	    }
	    cfg_write_urcp_swap_conf((char*)(&_port_config),sizeof(urcp_port_conf_conf_t));/*д�빲���ڴ�*/
	    sendGeneralUrcpMsg(wp->ipaddr, strMac, strPasswd,portCt,MISC_SET_GENERAL, URCP_SET_PORT_CONF);/*����Ϣ*/
	    pend_sem(0); /*�ȴ����������̴�������Ϣ*/
	}
	else
	{
	    /*�˿�������*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    else
    {
	/*û��Ŀ�Ľ�����mac������*/
	setTheErrorMsg("get the destination host MAC error!");
    }
    websRedirect(wp, "switch/ports.asp?urcpSet=1");
    return;
}
#endif
