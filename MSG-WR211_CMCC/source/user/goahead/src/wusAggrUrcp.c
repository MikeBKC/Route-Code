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


extern urcp_status_t getVlanUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
extern urcp_status_t getUrcpMirrorConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_mirror_conf_t *mirror_buff,uint32 misc_type);
extern urcp_status_t getPortConfMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
extern urcp_status_t getQosRateLimitMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);

static unsigned char gMac[STR_MAC_SIZE+1];/*ҳ�汣��ʱҪ������Ŀ��mac,��ҳ����ת��"get"ʹ��*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static const char* errorMsg[]={"�Ҳ���ɾ�����飡"};
static uint32 isRefresh = 0u;

static int  getAggrGroupListUrcp(int eid, webs_t wp, int argc, char **argv);
static void formAggrGroupUrcp(webs_t wp, char_t *path, char_t *query);
static void formAggrGroupDelUrcp(webs_t wp, char_t *path, char_t *query);

/********************************************************************
 * ������ formDefineAggrUrcp
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern void formDefineAggrUrcp(void)
{
    websAspDefine(T("aspOutPutAggrGroupListUrcp"), getAggrGroupListUrcp);   
    websFormDefine(T("formAggrGroupUrcp"), formAggrGroupUrcp);
    websFormDefine(T("formAggrGroupDelUrcp"), formAggrGroupDelUrcp);
}
/********************************************************************
 * ������ getUrcpRstpLacpConf
 * ���ܣ� ��ȡ����RSTP LACP�������ݽṹ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern urcp_status_t getUrcpRstpLacpEnableConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_rstp_lacp_conf_t *rstp_lacp_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;

    urcp_rstp_lacp_msg_t  urcp_rstp_lacp_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_rstp_lacp_msg, 0, sizeof(struct _urcp_rstp_lacp_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_rstp_lacp_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_rstp_lacp_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_rstp_lacp_msg.user_ip = uip;
    /*����Ϣ,֪ͨ���������̷���*/
    sendwebmsgbuffer(MSG_TYPE_URCP,GET_RSTP_LACP , (char*)(&urcp_rstp_lacp_msg), sizeof(struct _urcp_rstp_lacp_msg_t));
    /*�ȴ�1��*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ȴ�100ms��0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;

    while(1)
    {
	/*��misc buffer�ж�ȡ��Ϣ*/
	memset(rstp_lacp_buff, 0, sizeof(struct _urcp_rstp_lacp_conf_t));
	ret = urcp_read_rstp_lacp_conf(urcp_rstp_lacp_msg.dst_mac, uip , MISC_GET_GENERAL, rstp_lacp_buff);
	if(ret == URCP_STATUS_WAIT)
	{
	    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ٵȴ�*/
	    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
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
/***
 *��ӡ����rstp lacp��Ϣ��ҳ��
 *�����ض�ȡmisc buffer��״̬
 *portNum��ȡ�����豸�˿ڸ���
 *pre_ret�ṩǰһ�����������Ƿ�ɹ������򱾺���ֻ��ӡ������������
 *bhou
 */
extern urcp_status_t getRstpLacpEnableUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 uindex=0u;
    uint32 i=0u;
    char  str_buf[128];

    urcp_rstp_lacp_conf_t rstp_lacp_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/

    memset(str_buf,0,sizeof(str_buf));
    memset(&rstp_lacp_buff,0,sizeof(urcp_rstp_lacp_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	ret = getUrcpRstpLacpEnableConf(wp,strMac,strPasswd, &rstp_lacp_buff);/*��ȡ����rstp_lacp�ṹ������*/
    }
    websWrite(wp, "var lacp = new Array("); /*start print lacp enable*/
    if(ret != URCP_STATUS_OK)/*not get right*/
    {
	memset(&rstp_lacp_buff,0,sizeof(urcp_rstp_lacp_conf_t));/*��գ���Ϊҳ����ʾ��ȷ������ʾ*/
    }
    if(portNum!=NULL)
    {
	(*portNum) = rstp_lacp_buff.port_num;/*�˿ڸ���*/
    }
    for (uindex = 0u; uindex < rstp_lacp_buff.port_num; uindex++)
    {
	if(uindex > 0u)/*����λ ���ӡ�ָ���*/
	{
	    websWrite(wp,",");
	    sprintf(&str_buf[i],",");
	    i = i+1u;

	}
	if( (rstp_lacp_buff.lacp_enable &(uint32)(1u<<uindex)) >0u )/*prinf lacp enable to web*/
	{
	    websWrite(wp,"1");
	}
	else
	{
	    websWrite(wp,"0");
	}
	if( (rstp_lacp_buff.rstp_enable &(uint32)(1u<<uindex)) >0u )/*get rstp enable*/
	{
	    sprintf(&str_buf[i],"1");
	    i = i+1u;
	}
	else
	{
	    sprintf(&str_buf[i],"0");
	    i = i+1u;
	}
    }/*end for*/
    str_buf[sizeof(str_buf)-1u] = '\0';/*���һλ��0����ֹ�ڴ�й¶*/
    websWrite(wp, ");"); 
    websWrite(wp,"var rstp = new Array(%s);",str_buf);/*print rstp enable to web*/
    return ret;
}
/********************************************************************
 * ������ getAggrUrcpSetRet
 * ���ܣ� ��ȡ������۲������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
static urcp_status_t getAggrUrcpSetRet(webs_t wp,uchar* strMac)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;
    unsigned char std_mac[MAC_LEN];

    memset(std_mac,0,sizeof(std_mac));

    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(wp->ipaddr);
    /*ת��mac��ַ*/
    str2mac(strMac, std_mac);
    while(1)
    {
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ٵȴ�*/
	/*��misc buffer�ж�ȡ��Ϣ*/
	ret = urcp_read_aggr_conf(std_mac, uip , MISC_SET_GENERAL, (urcp_aggr_conf_t*)NULL);
	if((ret == URCP_STATUS_WAIT) || (ret==URCP_STATUS_FOUND_NONE))
	{
	    /*URCP_STATUS_FOUND_NONE ��ʾ����urcp����û���ü������ڵ���Ϣ�뷢����ϵͳ���ֵ���ִ��goahead������*/
	    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
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
 * ������ getUrcpAggrConf
 * ���ܣ� ��ȡ��������������ݽṹ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern urcp_status_t getUrcpAggrConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_aggr_conf_t *aggr_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;

    urcp_aggr_msg_t  urcp_aggr_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_aggr_msg, 0, sizeof(struct _urcp_aggr_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_aggr_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_aggr_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_aggr_msg.user_ip = uip;
    urcp_aggr_msg.opt_type = MISC_GET_GENERAL;/*��ȡ����*/
    /*����Ϣ,֪ͨ���������̷���*/
    sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));
    /*�ȴ�1��*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ȴ�100ms��0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
    while(1)
    {
	/*��misc buffer�ж�ȡ��Ϣ*/
	memset(aggr_buff, 0, sizeof(struct _urcp_aggr_conf_t));
	ret = urcp_read_aggr_conf(urcp_aggr_msg.dst_mac, uip , MISC_GET_GENERAL, aggr_buff);
	if(ret == URCP_STATUS_WAIT)
	{
	    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ٵȴ�*/
	    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
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

/***
 *��ӡ���������Ϣ��ҳ��
 *�����ض�ȡmisc buffer��״̬
 *portNum��ȡ�����豸�˿ڸ���
 *pre_ret�ṩǰһ�����������Ƿ�ɹ������򱾺���ֻ��ӡ������������
 *bhou
 */
extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 uindex=0u;
    uint32  umember,i;
    int icount = 0;
    int pcount = 0;
    uint16 ugroup = 0u;

    urcp_aggr_conf_t aggr_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/

    memset(&aggr_buff,0,sizeof(urcp_aggr_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	ret = getUrcpAggrConf(wp,strMac,strPasswd, &aggr_buff);
    }
    websWrite(wp, "var aggGrpNos = new Array();\n");/*���*/
    websWrite(wp, "var aggLists = new Array();\n");/*��Ա�б�*/
    websWrite(wp, "var aggGrpNames = new Array();\n");/*����*/
    if( ret == URCP_STATUS_OK)/*��ȡ��ȷ*/
    {
	if(portNum!=NULL)
	{
	    (*portNum) = aggr_buff.port_num;/*�˿ڸ���*/
	}
	for (uindex = 0u; uindex < aggr_buff.port_num; uindex++)
	{
	    /*���*/
	    ugroup = aggr_buff.aggr_group[uindex].index;
	    /*��Ա*/
	    umember = aggr_buff.aggr_group[uindex].member;
	    if ((ugroup>0u) && (umember>0u))/*���л�����Ա*/
	    {
		websWrite(wp, "aggGrpNos[%d] = \"%hu\";", icount, ugroup);	 
		websWrite(wp, "aggLists[%d] = \"", icount);
		for (i=0u; i < aggr_buff.port_num; i++)
		{
		    if ( (umember & (((uint32)1u<<i))) > 0u)/*���иó�Ա*/
		    {
			if (pcount == 0)
			{
			    websWrite(wp, "%d", (i+1u));
			    pcount++;
			}
			else
			{
			    websWrite(wp, " %d", (i+1u));
			    pcount++;
			}
		    }
		}
		websWrite(wp, "\";");
		/*remark*/
		websWrite(wp, "aggGrpNames[%d] = \"%s\";", icount, aggr_buff.aggr_group[uindex].remark);
		websWrite(wp, "\n");
		icount++;
	    }
	}/*end for*/
    }/*end if ret == URCP_STATUS_OK*/
    return ret;
}

/****
 *��ӡԶ�̾���˿ڵ�webҳ��
 *��Զ�̾���Դ�˿ڲ����ڣ����ӡ���Ϊ0
 *bhou
 */
static urcp_status_t getMirrorPortUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_status_t pre_ret)
{
    urcp_status_t ret = pre_ret;
    urcp_mirror_conf_t mirror_buff;
    uint32 u_mport = 0u;
    memset(&mirror_buff,0,sizeof(urcp_mirror_conf_t));
    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��������Ϣû�л�ȡ�ɹ����򱾴λ�ȡû�б�Ҫ*/
    {
	ret = getUrcpMirrorConf(wp,strMac,strPasswd,&mirror_buff,MISC_GET_GENERAL);/*get mirror conf*/
    }
    if(ret == URCP_STATUS_OK)/*���λ�ȡ�ɹ�*/
    {
	if(mirror_buff.mirror_source > 0u)
	{
	    u_mport = mirror_buff.mirror_port;
	}
    }
    websWrite(wp, "var mirrorPort = %d;\n", u_mport);
    return ret;

}
/********************************************************************
 * ������ getAggrGroupListUrcp 
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 * ���ڣg� 201-04-09
 ********************************************************************/ 
static int getAggrGroupListUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*ҳ���ӡ�Ķ˿ڸ���*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;

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
	    ret = getAggrUrcpSetRet(wp,strMac);/*��ȡ���ý��*/
	    isRefresh = 1u;/*�������ˢ���򲻶�ȡ���ý���������ý��������ܱ����*/
	}
    }

    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�����Ϣ*/
    ret = getRstpLacpEnableUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡrstp lacp enable��Ϣ*/
    ret = getMirrorPortUrcp(wp,strMac, strPasswd,ret); /*print mirror  port to web page*/
    ret = getPortConfMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡ�˿����ñ�ҳ���Ա*/
    ret = getQosRateLimitMemberUrcp(wp,strMac, strPasswd,&portNum,ret); /*��ӡ��������*/
    ret = getVlanUrcp(wp,strMac, strPasswd,&portNum,ret); /*print vlan to web page**/
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
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}

/**
 *�������������form���ú���
 *����ҳ��ip��mac��password��ֵ������ʼ��msg�ṹ��
 *���� ��1u �ɹ� ,0uʧ��
 *bhou
 */
static uint32 formAggrProcessForUrcp(webs_t wp,urcp_aggr_msg_t *urcp_aggr_msg)
{

    uint32 ret = 1u;
    uint32 portCt;
    uchar *str, *strMac, *strIp, *strPasswd;

    memset(gMac, 0, sizeof(gMac));
    memset(gIp, 0, sizeof(gIp));
    memset(gPasswd, 0, sizeof(gPasswd));

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

	    memset(urcp_aggr_msg, 0, sizeof(struct _urcp_aggr_msg_t));/*��ʼ����Ϣ*/
	    /*ҳ�����mac��ַ��ʽת��*/
	    str2mac(strMac, urcp_aggr_msg->dst_mac);
	    strncpy(urcp_aggr_msg->password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_aggr_msg->user_ip = ip2int(wp->ipaddr);
	    urcp_aggr_msg->port_ct = portCt;
	    urcp_aggr_msg->opt_type = MISC_SET_GENERAL;/*����*/
	}
	else
	{
	    ret = 0u;
	    /*�˿�������*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    else
    {
	ret = 0u;
	/*û��Ŀ�Ľ�����mac������*/
	setTheErrorMsg("get the destination host MAC error!");
    }
    return ret;
}

/*******************************************************************
 *������ formAggrGroupUrcp
 *���ܣ� ҳ���ύ�������༭����飬��ӻ��޸ģ�
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-04-12
 ******************************************************************/
static void formAggrGroupUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *action = NULL;
    char *cmember = NULL;
    char *cremark = NULL;
    char *group = NULL;

    uint32 i;
    uint32 umember=0u;
    char chk_name[32];
    urcp_aggr_msg_t  urcp_aggr_msg;

    memset(&urcp_aggr_msg,0,sizeof(urcp_aggr_msg_t));
    memset(chk_name,0,sizeof(chk_name));
    isRefresh = 0u;/*ˢ�±�־λ��λ*/

    if(formAggrProcessForUrcp(wp,&urcp_aggr_msg) > 0u)
    {

	/*get value from web*/
	action = websGetVar(wp, "actions", T(""));/*�������� ��ӻ��޸� ����������Ŀǰ����Ҫ*/
	cremark = websGetVar(wp, "aggName", T(""));/*������*/
	group = websGetVar(wp, "aggNo", T(""));/*���*/


	/*set member to index 0*/
	for (i = 0u; i < urcp_aggr_msg.port_ct; i++)
	{
	    sprintf(chk_name,"chkPort%u",i);/*html�ؼ���*/
	    cmember = websGetVar(wp,chk_name, T(""));
	    if (strcmp(cmember, "on") == 0)
	    {
		umember  |= (uint32)(1u << i);
	    }
	}

	/*��ʼ��aggr_group�ṹ*/
	urcp_aggr_msg.aggr_group.index = (uint16)strtoul(group, NULL, 10);
	urcp_aggr_msg.aggr_group.member = umember;
	memcpy(urcp_aggr_msg.aggr_group.remark,cremark,MAX_REMARK_LEN);


	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));
    }

    /*urcpSet��ʾ���ص�ҳ�������ú���ת���ģ������ȡ�Ƿ����óɹ��ı�־*/
    websRedirect(wp, "switch/aggregation.asp?urcpSet=1");
    return;
}

/*******************************************************************
 *������ formAggrGroupDelUrcp
 *���ܣ� ɾ�������
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-04-12
 ******************************************************************/
static void formAggrGroupDelUrcp(webs_t wp, char_t *path, char_t *query)
{

    char *del_str = NULL;
    unsigned short uindex;
    char *cid;
    char *idnext;
    uint32 umember=0u;
    urcp_aggr_msg_t  urcp_aggr_msg;

    isRefresh = 0u;/*ˢ�±�־λ��λ*/
    memset(&urcp_aggr_msg,0,sizeof(urcp_aggr_msg_t));
    if(formAggrProcessForUrcp(wp,&urcp_aggr_msg) > 0u)
    {
	/*get value from web*/
	del_str = websGetVar(wp, T("delstr"), T(""));

	/*get delstr from web*/
	if ((strcmp(del_str, "") == 0) || (del_str == NULL))
	{
	    setTheErrorMsg(T(errorMsg[0]));
	}
	else
	{
	    /*delstr is "group 1, group 2, ....."*/
	    idnext = strtok(del_str, DELSEPARATE);
	    /*�ָ�*/
	    while(idnext != NULL)
	    {
		cid = idnext;
		idnext = strtok(NULL, DELSEPARATE);
		uindex = (unsigned short)strtoul(cid, NULL, 10);
		if( (uindex > 0u)&&(uindex<=urcp_aggr_msg.port_ct) )/*�Ϸ�������*/
		{
		    umember |= ((uint32)1u<<(uindex-1u));/*��1��ʾɾ�����index*/
		}
	    }

	    /*������������ò����Ļ�����0��ʾɾ�������������Ա��ʾҪɾ���Ļ��������*/
	    urcp_aggr_msg.aggr_group.index =  0u;/*��ţ�Ϊ0��ʾɾ����*/
	    urcp_aggr_msg.aggr_group.member = umember;/*��Ա����ɾ����������룩*/

	    /*����Ϣ,֪ͨ���������̷���*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));

	}
    }
    /*urcpSet��ʾ���ص�ҳ�������ú���ת���ģ������ȡ�Ƿ����óɹ��ı�־*/
    websRedirect(wp, "switch/aggregation.asp?urcpSet=1");
    return;
}
#endif
