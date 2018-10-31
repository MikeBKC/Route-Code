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

static unsigned char gMac[STR_MAC_SIZE+1];/*ҳ�汣��ʱҪ������Ŀ��mac,��ҳ����ת��"get"ʹ��*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static uint32 isRefresh = 0u;
static void formMirrorUrcp(webs_t wp, char_t *path, char_t *query);
static int getMirrorUrcp(int eid, webs_t wp, int argc, char **argv);
/********************************************************************
 * ������ formDefineTopoyDiscovery
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� KeHuatao
 * ������ 2010-09-20
 *******************************************************************/
extern void formDefineMirrorUrcp(void)
{
    websAspDefine(T("aspOutPutMirrorUrcp"), getMirrorUrcp);
    websFormDefine(T("formMirrorUrcp"), formMirrorUrcp);
}

/*******************************************************************
 *������ formMirror
 *���ܣ� ҳ���ύ����
 *������ 
 *���أ� ��
 *���ߣ� KeHuatao
 *������ 2010-10-12
 ******************************************************************/
static void formMirrorUrcp(webs_t wp, char_t *path, char_t *query)
{
    uint32 i, portCt, port, ip;
    port_bit_mask_t pm; 
    char *str, *strMac, *strIp, *strPasswd;
    urcp_mirror_msg_t  urcp_mirror_msg;

    /*��ȡ����˿�*/
    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));
    isRefresh = 0u;/*ˢ�±�־��λ*/

    strMac = websGetVar(wp, "macStr", T(""));
    if(strcmp(strMac, T(""))!=0)
    {
	/*����Դ�˿�*/
	str = websGetVar(wp, "numPorts", T(""));
	portCt = (uint32)strtoul(str, NULL, 10);
	/*��ȡmask*/
	if((portCt>0u)&&(portCt<=PORT_COUNT))
	{
	    pm = 0u;
	    for(i=0u;i<portCt;i++)
	    {
		/*ȡҳ����ֵ*/
		str = websGetVar(wp, mirror_source[i], T(""));
		if(strtoul(str, NULL, 10) == 1u)
		{
		    pm |= 1u<<i;
		}
	    }

	    /*��ȡҳ�����*/
	    strIp = websGetVar(wp, "ipStr", T(""));
	    strPasswd = websGetVar(wp, "passStr", T(""));
	    str = websGetVar(wp, mirror_port, T(""));
	    port = (uint32)strtoul(str, NULL, 10);
	    ip=ip2int(strIp);

	    /*���汾�β�����ַ*/
	    i=(uint32)sizeof(gMac)-1u;
	    memcpy(gMac, strMac, i);
	    i=(uint32)sizeof(gIp)-1u;
	    memcpy(gIp, strIp, i);
	    i=(uint32)sizeof(gPasswd)-1u;
	    strncpy(gPasswd, strPasswd, i);

	    /*����Ϣ*/
	    memset(&urcp_mirror_msg, 0, sizeof(struct _urcp_mirror_msg_t));
	    /*ҳ�����mac��ַ��ʽת��*/
            str2mac((uchar*)strMac, urcp_mirror_msg.dst_mac);
	    strncpy(urcp_mirror_msg.password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_mirror_msg.user_ip = ip2int(wp->ipaddr);
	    urcp_mirror_msg.port_ct = portCt;
	    urcp_mirror_msg.mirror_port = port;
	    urcp_mirror_msg.mirror_source= pm;
	    /*����Ϣ,֪ͨ���������̷���*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, SET_MIRROR, (char*)(&urcp_mirror_msg), sizeof(struct _urcp_mirror_msg_t));
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
    /*��ת������urcpSet��ʾ�����ٷ�����ȡ������Ϣ��,ֱ�Ӷ����α��浽misc buffer�е���Ϣ*/
    websRedirect(wp, "switch/mirror.asp?urcpSet=1");

    return;
}

/********************************************************************
 * ������ getUrcpMirrorConf
 * ���ܣ� ��ȡ�����������ݽṹ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� kehuatao / edit by bhou
 *******************************************************************/
extern urcp_status_t getUrcpMirrorConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_mirror_conf_t *mirror_buff,uint32 misc_type)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000 *1000 ;/*ת��Ϊ΢��*/
    uint32 ip;

    urcp_mirror_msg_t  urcp_mirror_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    ip=ip2int(wp->ipaddr);
    memset(&urcp_mirror_msg, 0, sizeof(struct _urcp_mirror_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_mirror_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_mirror_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_mirror_msg.user_ip = ip;
    /*get���������»�ȡ������ֱ�Ӷ�ȡ����������Ϣ����*/
    if(misc_type == MISC_GET_GENERAL)
    {
	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, GET_MIRROR, (char*)(&urcp_mirror_msg), sizeof(struct _urcp_mirror_msg_t));
    }
    /*�ȴ�1��*/
    while(1)
    {	 
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ȴ�100ms��0.1s*/  
	wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC; /*��misc buffer�ж�ȡ��Ϣ*/	   
	memset(mirror_buff, 0, sizeof(struct _urcp_mirror_conf_t));     
	ret = urcp_read_mirror_conf(urcp_mirror_msg.dst_mac, ip, misc_type , mirror_buff);	 
	if((ret == URCP_STATUS_WAIT) || (ret==URCP_STATUS_FOUND_NONE))
	{
	    /*URCP_STATUS_FOUND_NONE ��ʾ����urcp����û���ü������ڵ���Ϣ�뷢����ϵͳ���ֵ���ִ��goahead������*/
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
 * ������ getMirror
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� KeHuatao
 * ������ 2010-10-12
 ********************************************************************/ 
static int getMirrorUrcp(int eid, webs_t wp, int argc, char **argv)
{
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 i,portNum;
    port_bit_mask_t  pm;
    urcp_mirror_conf_t mirror_buff;
    urcp_status_t ret = URCP_STATUS_OK ;

    memset(&mirror_buff, 0, sizeof(struct _urcp_mirror_conf_t));
    /*�ж��Ƿ���formMirrorUrcp()��ת������*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*��һ�δ�
	 *���˷�ʽ�򿪵�ҳ�档����ˢ���� ���ݱ�����post��ҳ��*/
	isRefresh = 0u;
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	i = MISC_GET_GENERAL;
    }
    else
    {
	/*ҳ�汣������ת*/
	strMac = gMac;
	strPasswd = gPasswd;
	strIp = gIp;
	if(isRefresh == 0u)
	{/*�������״���ת*/
	    i = MISC_SET_GENERAL;
	    isRefresh = 1u;
	}
	else
	{/*ҳ�汣������ˢ��
	  *��ʱҳ���Ǳ��ٴ����󡣵� Զ���豸���룬mac����Ϣδ������post*/
	    i = MISC_GET_GENERAL;
	}
    }
    /*���´�ӡ��ҳ�棬���´�"form"ʹ��*/
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    /*Ҫ������mac����Ϊ��*/
    if(strcmp(strMac, T(""))!=0)
    {
	/*��misc buffer�ж�ȡ������Ϣ*/
	ret = getUrcpMirrorConf(wp,strMac,strPasswd,&mirror_buff,i);
    }
    else/*����*/
    {
	ret=URCP_STATUS_FAIL;/*���ձ��β���Ϊ�Ƿ����������ֻ��ӡ������������*/
	websWrite(wp, "urcpError = \"get the destination host MAC error!\";\n");
	/*�����ӡ�ı����Ѿ�û�����壬Ϊ��ҳ�����ʾ�����ִ��󻹵�������*/
    }
    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�����Ϣ*/
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

    /*����Դ�˿�*/
    portNum = mirror_buff.port_num;
    if(portNum>PORT_COUNT)
    {
	/*Ŀǰҳ����ʾ������ô��*/
	portNum = PORT_COUNT;
    }
    websWrite(wp, "var numPorts = %d;\n", portNum);

    /*����˿�*/
    websWrite(wp, "var mirrorPort = %d;\n", mirror_buff.mirror_port);

    pm = mirror_buff.mirror_source;
    /*����Դ�˿�*/
    websWrite(wp, "var mirrorSource = new Array(");
    /*��������bit*/
    for(i=0u;i<portNum; i++)
    {
	if(i!=0u)
	{
	    websWrite(wp, ",");
	}

	if((pm & (uint32)(1u<<i))>0u)/*����*/
	{
	    websWrite(wp, "\"checked\"");
	}
	else/*����*/
	{
	    websWrite(wp, "\"\"");
	}
    }
    /*��β*/
    websWrite(wp, ");\n");
   getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif
