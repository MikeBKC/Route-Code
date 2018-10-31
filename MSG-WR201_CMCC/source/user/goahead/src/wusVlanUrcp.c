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
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
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
static const char* errorMsg[]={"�Ҳ���ɾ�����飡"};
static uint32 isRefresh = 0u;

static void formPortVlanUrcp(webs_t wp, char_t *path, char_t *query);
static int getPortVlanUrcp(int eid, webs_t wp, int argc, char **argv);
static void formDelPortVlanUrcp(webs_t wp, char_t *path, char_t *query);
static void websOutPvlanUrcp(webs_t wp,urcp_vlan_conf_t *pvlan_conf);


extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
/********************************************************************
 * ������ formDefinePortVlanUrcp
 * ���ܣ� ҳ���뽻��ת������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern void formDefinePortVlanUrcp(void)
{
    websAspDefine(T("aspOutPutPortVlanUrcp"), getPortVlanUrcp);
    websFormDefine(T("formPortVlanUrcp"), formPortVlanUrcp);
    websFormDefine(T("formdelPortVlanUrcp"), formDelPortVlanUrcp);
}
/********************************************************************
 * ������ getPvlanUrcpSetRet
 * ���ܣ� ��ȡ����vlan�������
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
static urcp_status_t getPvlanUrcpSetRet(webs_t wp,uchar* strMac)
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
	ret = urcp_read_vlan_conf(std_mac, uip , MISC_SET_PVLAN, (urcp_vlan_conf_t*)NULL);
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
 * ������ getUrcpVlanConf
 * ���ܣ� ��ȡ����vlan�������ݽṹ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern urcp_status_t getUrcpVlanConf(webs_t wp,uchar* strMac,uchar* strPasswd,uint32 u_misc_type,uint16 u_start_vlan_no,urcp_vlan_conf_t *vlan_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;

    urcp_vlan_msg_t  urcp_vlan_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_vlan_msg, 0, sizeof(struct _urcp_vlan_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_vlan_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_vlan_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_vlan_msg.user_ip = uip;
    urcp_vlan_msg.opt_type = u_misc_type;/*��ȡ����*/
    urcp_vlan_msg.start_vlan_no = u_start_vlan_no;/*��ʼvlan no*/
    /*����Ϣ,֪ͨ���������̷���*/
    sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));
    /*�ȴ�1��*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ȴ�100ms��0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
    while(1)
    {
	/*��misc buffer�ж�ȡ��Ϣ*/
	memset(vlan_buff, 0, sizeof(struct _urcp_vlan_conf_t));
	ret = urcp_read_vlan_conf(urcp_vlan_msg.dst_mac, uip , u_misc_type , vlan_buff);
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

/**
 *ҳ�����������ȡ����port vlan�ṹ��Ϣ��ӡ���port vlan��Ϣ
 *bhou
 */
static void websOutPvlanUrcp(webs_t wp,urcp_vlan_conf_t *pvlan_conf)
{
    uint32 i, u_vlan_count, num;
    uint32 u_vlan_type=0u;
    char vlanGrpNos[256], vlanGrpNames[512], web_buf[128];
    port_bit_mask_t pm;

    websWrite(wp, "var vlanLists = new Array(");
    u_vlan_count = 0u;
    vlanGrpNos[0] = '\0';
    vlanGrpNames[0] = '\0';   
    web_buf[0] = '\0';
    if( pvlan_conf != NULL)/*��ȡ��ȷ*/
    {
	u_vlan_type=pvlan_conf->vlan_type;/*Զ���豸vlan����*/
	for(i=0u;i<pvlan_conf->port_num;i++)/*���port_num��portvlan*/
	{
	    pm = pvlan_conf->vlan_group[i].member;/*���˿ڳ�Ա*/
	    if(pm!=0u)/*��Ϊ0*/
	    {
		num = strlen(vlanGrpNos);
		sprintf(&vlanGrpNos[num], "\"%d\",", pvlan_conf->vlan_group[i].vlan_no);/*pvlan ���*/

		num = strlen(vlanGrpNames);
		sprintf(&vlanGrpNames[num], "\"%s\",", pvlan_conf->vlan_group[i].vlan_name);/*pvlan����*/
		web_print_port_mask(web_buf, pm);
		if(u_vlan_count>0u)
		{
		    websWrite(wp, ",");/*vlan��Ա*/
		}
		websWrite(wp, "%s", web_buf);/*vlan��Ա*/
		u_vlan_count = u_vlan_count+1u;/*����*/
	    }
	}

	/*��Ϊ��*/
	if(u_vlan_count > 0u)
	{
	    num = strlen(vlanGrpNos)-1u;
	    vlanGrpNos[num] = '\0';
	    num = strlen(vlanGrpNames)-1u;
	    vlanGrpNames[num] = '\0';
	}

    }/*end if ret == URCP_STATUS_OK*/
    websWrite(wp, ");\n");/*vlan��Ա*/
    websWrite(wp, "var vlanFlag = %u;\n",u_vlan_type);
    websWrite(wp, "var vlanGrpNos = new Array(%s);\n", vlanGrpNos);/*vlan��Ա*/
    websWrite(wp, "var vlanGrpNames = new Array(%s);\n", vlanGrpNames);/*vlan��Ա*/
    return;
}

/***
 *��ӡ����vlan��Ϣ��ҳ��
 *�����ض�ȡmisc buffer��״̬
 *portNum��ȡ�����豸�˿ڸ���
 *pre_ret�ṩǰһ�����������Ƿ�ɹ������򱾺���ֻ��ӡ������������
 *bhou
 */
extern urcp_status_t getPvlanMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    urcp_vlan_conf_t vlan_buff;
    urcp_vlan_conf_t* vlan_buff_ptr=NULL;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/

    memset(&vlan_buff,0,sizeof(urcp_vlan_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_PVLAN,1u, &vlan_buff);/*��ȡport vlan����*/
    }

    if( ret == URCP_STATUS_OK)/*��ȡ��ȷ*/
    {
	vlan_buff_ptr=&vlan_buff;
	if(portNum!=NULL)
	{
	    (*portNum) = vlan_buff.port_num;
	}
    }
    websOutPvlanUrcp(wp,vlan_buff_ptr);
    return ret;
}
/********************************************************************
 * ������ getQvlanPconfConf
 * ���ܣ� ��ȡ����tag vlan�˿��������ݽṹ
 * ������ ��
 * ����� ��
 * ���أ� ��
 * ���ߣ� bhou
 *******************************************************************/
extern urcp_status_t getUrcpQvlanPconfConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_qvlan_pconf_conf_t *qvlan_pconf_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*ת��Ϊ΢��*/
    uint32 uip;

    urcp_qvlan_pconf_msg_t  urcp_qvlan_pconf_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ĭ��Ϊ��ȡʧ��*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_qvlan_pconf_msg, 0, sizeof(urcp_qvlan_pconf_msg_t));
    /*ת��mac��ַ*/
    str2mac(strMac, urcp_qvlan_pconf_msg.dst_mac);
    /*Ҫ������mac����Ϊ��*/
    strncpy(urcp_qvlan_pconf_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_qvlan_pconf_msg.user_ip = uip;
    /*����Ϣ,֪ͨ���������̷���*/
    sendwebmsgbuffer(MSG_TYPE_URCP, QVLAN_PORT_CONFIG , (char*)(&urcp_qvlan_pconf_msg), sizeof(urcp_qvlan_pconf_msg_t));
    /*�ȴ�1��*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*�ȴ�100ms��0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;

    while(1)
    {
	/*��misc buffer�ж�ȡ��Ϣ*/
	memset(qvlan_pconf_buff, 0, sizeof(urcp_qvlan_pconf_conf_t));
	ret = urcp_read_qvlan_pconf_conf(urcp_qvlan_pconf_msg.dst_mac, uip , MISC_GET_GENERAL,qvlan_pconf_buff);
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
 *��ӡ����tag vlan�˿�������Ϣ��ҳ��
 *�����ض�ȡmisc buffer��״̬
 *portNum��ȡ�����豸�˿ڸ���
 *pre_ret�ṩǰһ�����������Ƿ�ɹ������򱾺���ֻ��ӡ������������
 *bhou
 */
extern urcp_status_t getQvlanPconfUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 i=0u;
    uint32 num=0u;

    urcp_qvlan_pconf_conf_t qvlan_pconf_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/
    char buff1[256], buff2[256],buff3[128];

    memset(&qvlan_pconf_buff,0,sizeof(urcp_qvlan_pconf_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	ret = getUrcpQvlanPconfConf(wp,strMac,strPasswd, &qvlan_pconf_buff);/*��ȡ����tag v;am�˿����ýṹ������*/
    }
    if(ret != URCP_STATUS_OK)/*not get right*/
    {
	memset(&qvlan_pconf_buff,0,sizeof(urcp_qvlan_pconf_conf_t));/*��գ���Ϊҳ����ʾ��ȷ������ʾ*/
    }
    if(portNum!=NULL)
    {
	(*portNum) = qvlan_pconf_buff.port_num;/*�˿ڸ���*/
    }
    buff1[0] = '\0';
    buff2[0] = '\0';
    buff3[0] = '\0';
    /*��ӡ�˿�������Ϣ*/
    websWrite(wp, "var vlanAware = new Array(");/*vlan�豸*/
    for(i=0u;i<qvlan_pconf_buff.port_num;i++)
    {
	if(i>0u)
	{
	    websWrite(wp, ",");/*vlan��Ա*/
	}
	if(qvlan_pconf_buff.pconf_group[i].vlan_aware>0u)
	{
	    websWrite(wp, "\"checked\"");/*vlan aware*/
	}
	else
	{
	    websWrite(wp, "\"\"");/*not aware*/
	}
	/*frame type*/
	num = strlen(buff1);
	sprintf(&buff1[num], "\"%d\",", qvlan_pconf_buff.pconf_group[i].frame_type);/*֡����*/
	/*pvid*/
	num = strlen(buff2);
	sprintf(&buff2[num], "\"%d\",", qvlan_pconf_buff.pconf_group[i].pvid);/*pvid*/

	num = strlen(buff3);
	sprintf(&buff3[num], "\"%s\",", "");/*ingr*/

    }
    websWrite(wp, ");\n"); 
    /*ȥ��ĩβ�ַ�*/
    num = strlen(buff1)-1u;
    buff1[num] = '\0';
    num = strlen(buff2)-1u;
    buff2[num] = '\0';

    num = strlen(buff3)-1u;
    buff3[num] = '\0';

    websWrite(wp, "var frameType = new Array(%s);\n", buff1);/*֡����*/
    websWrite(wp, "var pvid = new Array(%s);\n", buff2);/*pvid*/
    websWrite(wp, "var ingr = new Array(%s);\n", buff3);/*ingrԤ����*/
    return ret;
}
/**
 *������ȡʵ���豸���͵�vlan��Ϣʱ����ӡport vlan
 *bhou
 * **/
static urcp_status_t getVlanUrcpOutPvlan(webs_t wp,urcp_vlan_conf_t *pvlan_conf)
{
    /*pvlan����Ҫ�ٻ�ȡ��ֻ���ӡ����*/
    char web_buf[128];
    uint32 i=0u, u_vlan_count=0u;
    port_bit_mask_t pm=0u;
    websWrite(wp, "var vlanLists = new Array(");
    web_buf[0] = '\0';
    for(i=0u;i<pvlan_conf->port_num;i++)/*port vlan���˿ڸ�����vlan*/
    {
	pm = pvlan_conf->vlan_group[i].member;/*���˿ڳ�Ա*/
	if(pm!=0u)/*��Ϊ0*/
	{
	    web_print_port_mask(web_buf, pm);
	    if(u_vlan_count>0u)
	    {
		websWrite(wp, ",");/*vlan��Ա*/
	    }
	    websWrite(wp, "%s", web_buf);/*vlan��Ա*/
	    u_vlan_count = u_vlan_count+1u;/*����*/
	}
    }
    websWrite(wp, ");\n");/*vlan��Ա*/
    return URCP_STATUS_OK;
}

/******
 *������ȡԶ���豸ʵ�����͵�vlan��Ϣ��ʱ���ӡtag vlan
 *bhou
 */
static urcp_status_t getVlanUrcpOutTagVlan(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_vlan_conf_t *qvlan_conf)
{
    /*tag vlan���ܻ����ȡ���*/
    uint16 u_next_vlan_no = 0u;
    uint32 u_vlan_count=0u;
    uint32 urcp_receive_num=1u;/*�Ѿ���ȡ�˼���tag vlan��Ϣ*/
    uint16 i=0u;
    port_bit_mask_t pm=0u;
    char web_buf[128];
    urcp_status_t ret = URCP_STATUS_OK;
    web_buf[0] = '\0';
    websWrite(wp, "var vlantabs = new Array(");
    while(ret == URCP_STATUS_OK)/*��ȡ��ȷ*/
    {
	if(qvlan_conf->vlan_type!=VLAN_TYPE_8021Q)/*��ȡ�Ĺ����У�����vlan���ͱ��л���*/
	{
	    ret = URCP_STATUS_FAIL;/*������ȡ������ʧ�ܸ���*/
	}
	else
	{
	    for(i=0u;i<MAX_URCP_BUFF_VLAN_NUM;i++)/*һ�λ�ȡ�󣬴�ӡ*/
	    {
		pm = qvlan_conf->vlan_group[i].member;/*���˿ڳ�Ա*/
		if(pm>0u)
		{
		    if(u_vlan_count>0u)
		    {
			websWrite(wp, ",");/*vlan��Ա*/
		    }
		    /*��ʾvid*/
		    websWrite(wp, "new Array(\"%hu\",", qvlan_conf->vlan_group[i].vlan_no);/*vlan id*/
		    /*��ʾmember*/
		    web_print_port_mask(web_buf,pm);
		    web_buf[sizeof(web_buf)-1u]='\0';
		    websWrite(wp, "%s)", web_buf);/*vlan��Ա*/
		    u_vlan_count=1u ;
		}
	    }
	    u_next_vlan_no = qvlan_conf->next_vlan_no;
	    if(u_next_vlan_no>0u)/*֤����δ��ȡ��*/
	    {
		/*���½��л�ȡ�������ƣ��Է�ֹα�����ԭ����ɵĽ�����������ѭ����ȡ*/
		if( (urcp_receive_num*MAX_URCP_BUFF_VLAN_NUM)<MAX_URCP_VID)/*������ȷ��ȡ*/
		{
		    memset(qvlan_conf,0,sizeof(urcp_vlan_conf_t));
		    ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_VLAN,u_next_vlan_no, qvlan_conf);/*��ȡport vlan����*/
		    urcp_receive_num++;/*��ȡ������һ*/
		}
		else/*�����ڰ������ԭ����ϵͳ����Ϊ�����ȡʱ��Ӧ�����������ֻ�ȡ��Ϊ*/
		{
		    ret = URCP_STATUS_FAIL;
		}
	    }/*�Ѿ���ȡ���*/
	    else
	    {
		break;
	    }
	}

    }
    websWrite(wp, ");\n");/*vlan��Ա*/
    return ret;
}

/***
 *��ӡ����vlan��Ϣ��ҳ�棨��ҪΪ��۵�ҳ����ʹ�ã�
 *�����ض�ȡmisc buffer��״̬
 *portNum��ȡ�����豸�˿ڸ���
 *pre_ret�ṩǰһ�����������Ƿ�ɹ������򱾺���ֻ��ӡ������������
 *bhou
 */
extern urcp_status_t getVlanUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    urcp_vlan_conf_t vlan_buff;
    urcp_status_t ret=pre_ret;/*Ĭ��Ϊǰһ�β�����״̬*/
    uint32 u_vlan_type=0u;
    uint32 web_vlan_type=0u;
    memset(&vlan_buff,0,sizeof(urcp_vlan_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*��֮ǰ��һ�β��������򱾴ζ�ȡû��Ҫ*/
    {
	/*�Ȼ�ȡһ�Σ���ȷ��vlan����*/
	ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_VLAN,1u, &vlan_buff);/*�״ζ�ȡ��vlan noΪ1�Ŀ�ʼ*/
	if( ret == URCP_STATUS_OK)/*��ȡ��ȷ*/
	{
	    u_vlan_type = vlan_buff.vlan_type;
	    if(u_vlan_type==VLAN_TYPE_PORT)/*Զ���豸ʵ������Ϊport vlan*/
	    {
		ret = getVlanUrcpOutPvlan(wp,&vlan_buff);	/*��ӡport vlan�����Ϣ*/	
	    }
	    else
	    {
		if(u_vlan_type == VLAN_TYPE_8021Q)/*Զ���豸����Ϊtag vlan*/
		{
		    ret = getVlanUrcpOutTagVlan(wp,strMac,strPasswd,&vlan_buff);/*��ӡtag vlan�����Ϣ*/		
		    ret = getQvlanPconfUrcp(wp,strMac,strPasswd,(uint32*)NULL,ret);/*��ӡtag vlan�˿�����*/
		}
	    }
	}
    }
    if( ret == URCP_STATUS_OK)/*��ȡ��ȷ*/
    {
	if(portNum!=NULL)
	{
	    *portNum = vlan_buff.port_num;
	}
	web_vlan_type = u_vlan_type;
    }

    websWrite(wp,T("var vlanFlag=%u;"),web_vlan_type);/*����ȡ������ҳ����ʾ��vlan flagΪ0��Ϊ������vlan*/
    /*��ȡʧ��ʱvlan flagΪ0ֻ��Ϊҳ�治�������˴�ӡ��*/
    return ret;
}
/********************************************************************
 * ������ getPortVlanUrcp 
 * ���ܣ� ҳ����ʾ����
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� bhou
 ********************************************************************/ 
static int getPortVlanUrcp(int eid, webs_t wp, int argc, char **argv)
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
	    ret = getPvlanUrcpSetRet(wp,strMac);/*��ȡ���ý��*/
	    isRefresh = 1u;/*�������ˢ���򲻶�ȡ���ý���������ý��������ܱ����*/
	}
    }

    ret = getPvlanMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*��ӡport vlan���Ϣ*/
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

/**
 *vlan����������form���ú���
 *����ҳ��ip��mac��password��ֵ������ʼ��msg�ṹ��
 *���� ��1u �ɹ� ,0uʧ��
 *bhou
 */
static uint32 formVlanProcessForUrcp(webs_t wp,urcp_vlan_msg_t *urcp_vlan_msg)
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

	    memset(urcp_vlan_msg, 0, sizeof(struct _urcp_vlan_msg_t));/*��ʼ����Ϣ*/
	    /*ҳ�����mac��ַ��ʽת��*/
	    str2mac(strMac, urcp_vlan_msg->dst_mac);
	    strncpy(urcp_vlan_msg->password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_vlan_msg->user_ip = ip2int(wp->ipaddr);
	    urcp_vlan_msg->port_ct = portCt;
	    urcp_vlan_msg->opt_type = MISC_SET_PVLAN;/*����*/
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
 *������ formPortVlanUrcp
 *���ܣ� ҳ���ύ�������༭port vlan����ӻ��޸ģ�
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-04-21
 ******************************************************************/
static void formPortVlanUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *c_vlan_member = NULL;
    char *c_vlan_name = NULL;
    char *c_vlan_no = NULL;

    urcp_vlan_msg_t  urcp_vlan_msg;

    memset(&urcp_vlan_msg,0,sizeof(urcp_vlan_msg_t));
    isRefresh = 0u;/*ˢ�±�־λ��λ*/

    if(formVlanProcessForUrcp(wp,&urcp_vlan_msg) > 0u)/*��ʼ��*/
    {

	/*get value from web*/
	c_vlan_name = websGetVar(wp, "vlanName", T(""));/*������*/
	c_vlan_no = websGetVar(wp, "vlanNo", T(""));/*���*/
	c_vlan_member = websGetVar(wp, "chkstr", T(""));/*��Ա*/

	/*��ʼ��vlan_group�ṹ*/
	urcp_vlan_msg.vlan_group.vlan_no = (uint16)strtoul(c_vlan_no, NULL, 10);
	urcp_vlan_msg.vlan_group.member = (uint32)strtoul(c_vlan_member, NULL, 10);
	memcpy(urcp_vlan_msg.vlan_group.vlan_name,c_vlan_name,MAX_VLAN_NAME_LEN);


	/*����Ϣ,֪ͨ���������̷���*/
	sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));
    }

    /*urcpSet��ʾ���ص�ҳ�������ú���ת���ģ������ȡ�Ƿ����óɹ��ı�־*/
    websRedirect(wp, "switch/port_vlan.asp?urcpSet=1");
    return;
}

/*******************************************************************
 *������ formDelPortVlan
 *���ܣ� ɾ��port vlan��
 *������ 
 *���أ� ��
 *���ߣ� bhou
 *������ 2011-04-21
 ******************************************************************/
static void formDelPortVlanUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *del_str = NULL;
    unsigned short uindex;
    char *cid;
    char *idnext;
    uint32 umember=0u;
    urcp_vlan_msg_t  urcp_vlan_msg;

    memset(&urcp_vlan_msg,0,sizeof(urcp_vlan_msg_t));
    isRefresh = 0u;/*ˢ�±�־λ��λ*/
    if(formVlanProcessForUrcp(wp,&urcp_vlan_msg) > 0u)
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
	    idnext = strtok(del_str, UTT_SPLIT_SIGN_STR);
	    /*�ָ�*/
	    while(idnext != NULL)
	    {
		cid = idnext;
		idnext = strtok(NULL, UTT_SPLIT_SIGN_STR);
		uindex = (unsigned short)strtoul(cid, NULL, 10);
		if( (uindex > 0u)&&(uindex<=urcp_vlan_msg.port_ct) )/*�Ϸ�������*/
		{
		    umember |= ((uint32)1u<<(uindex-1u));/*��1��ʾɾ�����index*/
		}
	    }

	    /*������������ò����Ļ�����0��ʾɾ�������������Ա��ʾҪɾ���Ļ��������*/
	    urcp_vlan_msg.vlan_group.vlan_no =  0u;/*��ţ�Ϊ0��ʾɾ����*/
	    urcp_vlan_msg.vlan_group.member = umember;/*��Ա����ɾ����������룩*/

	    /*����Ϣ,֪ͨ���������̷���*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));

	}
    }
    /*urcpSet��ʾ���ص�ҳ�������ú���ת���ģ������ȡ�Ƿ����óɹ��ı�־*/
    websRedirect(wp, "switch/port_vlan.asp?urcpSet=1");
    return;
}
#endif
