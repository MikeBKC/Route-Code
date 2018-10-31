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

static unsigned char gMac[STR_MAC_SIZE+1];/*Ò³Ãæ±£´æÊ±Òª²Ù×÷µÄÄ¿µÄmac,¹©Ò³ÃæÌø×ªºó"get"Ê¹ÓÃ*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static const char* errorMsg[]={"ÕÒ²»µ½É¾³ıµÄ×é£¡"};
static uint32 isRefresh = 0u;

static void formPortVlanUrcp(webs_t wp, char_t *path, char_t *query);
static int getPortVlanUrcp(int eid, webs_t wp, int argc, char **argv);
static void formDelPortVlanUrcp(webs_t wp, char_t *path, char_t *query);
static void websOutPvlanUrcp(webs_t wp,urcp_vlan_conf_t *pvlan_conf);


extern urcp_status_t getAggrMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret);
/********************************************************************
 * º¯Êı£º formDefinePortVlanUrcp
 * ¹¦ÄÜ£º Ò³ÃæÓë½»»¥×ª»»º¯Êı
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern void formDefinePortVlanUrcp(void)
{
    websAspDefine(T("aspOutPutPortVlanUrcp"), getPortVlanUrcp);
    websFormDefine(T("formPortVlanUrcp"), formPortVlanUrcp);
    websFormDefine(T("formdelPortVlanUrcp"), formDelPortVlanUrcp);
}
/********************************************************************
 * º¯Êı£º getPvlanUrcpSetRet
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯vlan²Ù×÷½á¹û
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
static urcp_status_t getPvlanUrcpSetRet(webs_t wp,uchar* strMac)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*×ª»¯ÎªÎ¢Ãî*/
    uint32 uip;
    unsigned char std_mac[MAC_LEN];

    memset(std_mac,0,sizeof(std_mac));

    urcp_status_t ret=URCP_STATUS_FAIL;/*Ä¬ÈÏÎª¶ÁÈ¡Ê§°Ü*/

    uip=ip2int(wp->ipaddr);
    /*×ª»»macµØÖ·*/
    str2mac(strMac, std_mac);
    while(1)
    {
	usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*ÔÙµÈ´ı*/
	/*´Ómisc bufferÖĞ¶ÁÈ¡ĞÅÏ¢*/
	ret = urcp_read_vlan_conf(std_mac, uip , MISC_SET_PVLAN, (urcp_vlan_conf_t*)NULL);
	if((ret == URCP_STATUS_WAIT) || (ret==URCP_STATUS_FOUND_NONE))
	{
	    /*URCP_STATUS_FOUND_NONE ±íÊ¾¿ÉÄÜurcp³ÌĞò»¹Ã»À´µÃ¼°´´½¨½ÚµãĞÅÏ¢Óë·¢°ü£¬ÏµÍ³¾ÍÓÖµ÷¶ÈÖ´ĞĞgoahead³ÌĞòÁË*/
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
 * º¯Êı£º getUrcpVlanConf
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯vlanÅäÖÃÊı¾İ½á¹¹
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern urcp_status_t getUrcpVlanConf(webs_t wp,uchar* strMac,uchar* strPasswd,uint32 u_misc_type,uint16 u_start_vlan_no,urcp_vlan_conf_t *vlan_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*×ª»¯ÎªÎ¢Ãî*/
    uint32 uip;

    urcp_vlan_msg_t  urcp_vlan_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ä¬ÈÏÎª¶ÁÈ¡Ê§°Ü*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_vlan_msg, 0, sizeof(struct _urcp_vlan_msg_t));
    /*×ª»»macµØÖ·*/
    str2mac(strMac, urcp_vlan_msg.dst_mac);
    /*Òª²Ù×÷µÄmac²»ÄÜÎª¿Õ*/
    strncpy(urcp_vlan_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_vlan_msg.user_ip = uip;
    urcp_vlan_msg.opt_type = u_misc_type;/*¶ÁÈ¡²Ù×÷*/
    urcp_vlan_msg.start_vlan_no = u_start_vlan_no;/*ÆğÊ¼vlan no*/
    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
    sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));
    /*µÈ´ı1»á*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*µÈ´ı100ms¼´0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
    while(1)
    {
	/*´Ómisc bufferÖĞ¶ÁÈ¡ĞÅÏ¢*/
	memset(vlan_buff, 0, sizeof(struct _urcp_vlan_conf_t));
	ret = urcp_read_vlan_conf(urcp_vlan_msg.dst_mac, uip , u_misc_type , vlan_buff);
	if(ret == URCP_STATUS_WAIT)
	{
	    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*ÔÙµÈ´ı*/
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
 *Ò³Ãæ¸ù¾İÁª¶¯»ñÈ¡µ½µÄport vlan½á¹¹ĞÅÏ¢´òÓ¡Ïà¹Øport vlanĞÅÏ¢
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
    if( pvlan_conf != NULL)/*¶ÁÈ¡ÕıÈ·*/
    {
	u_vlan_type=pvlan_conf->vlan_type;/*Ô¶³ÌÉè±¸vlanÀàĞÍ*/
	for(i=0u;i<pvlan_conf->port_num;i++)/*×î¶àport_num¸öportvlan*/
	{
	    pm = pvlan_conf->vlan_group[i].member;/*¶Á¶Ë¿Ú³ÉÔ±*/
	    if(pm!=0u)/*²»Îª0*/
	    {
		num = strlen(vlanGrpNos);
		sprintf(&vlanGrpNos[num], "\"%d\",", pvlan_conf->vlan_group[i].vlan_no);/*pvlan ×éºÅ*/

		num = strlen(vlanGrpNames);
		sprintf(&vlanGrpNames[num], "\"%s\",", pvlan_conf->vlan_group[i].vlan_name);/*pvlanÃû³Æ*/
		web_print_port_mask(web_buf, pm);
		if(u_vlan_count>0u)
		{
		    websWrite(wp, ",");/*vlan³ÉÔ±*/
		}
		websWrite(wp, "%s", web_buf);/*vlan³ÉÔ±*/
		u_vlan_count = u_vlan_count+1u;/*¼ÆÊı*/
	    }
	}

	/*²»Îª¿Õ*/
	if(u_vlan_count > 0u)
	{
	    num = strlen(vlanGrpNos)-1u;
	    vlanGrpNos[num] = '\0';
	    num = strlen(vlanGrpNames)-1u;
	    vlanGrpNames[num] = '\0';
	}

    }/*end if ret == URCP_STATUS_OK*/
    websWrite(wp, ");\n");/*vlan³ÉÔ±*/
    websWrite(wp, "var vlanFlag = %u;\n",u_vlan_type);
    websWrite(wp, "var vlanGrpNos = new Array(%s);\n", vlanGrpNos);/*vlan³ÉÔ±*/
    websWrite(wp, "var vlanGrpNames = new Array(%s);\n", vlanGrpNames);/*vlan³ÉÔ±*/
    return;
}

/***
 *´òÓ¡Áª¶¯vlanĞÅÏ¢µ½Ò³Ãæ
 *²¢·µ»Ø¶ÁÈ¡misc bufferµÄ×´Ì¬
 *portNum»ñÈ¡Áª¶¯Éè±¸¶Ë¿Ú¸öÊı
 *pre_retÌá¹©Ç°Ò»´ÎÁª¶¯²Ù×÷ÊÇ·ñ³É¹¦£¬·ñÔò±¾º¯ÊıÖ»´òÓ¡£¬²»·¢Áª¶¯°ü
 *bhou
 */
extern urcp_status_t getPvlanMemberUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    urcp_vlan_conf_t vlan_buff;
    urcp_vlan_conf_t* vlan_buff_ptr=NULL;
    urcp_status_t ret=pre_ret;/*Ä¬ÈÏÎªÇ°Ò»´Î²Ù×÷µÄ×´Ì¬*/

    memset(&vlan_buff,0,sizeof(urcp_vlan_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°ÓĞÒ»´Î²Ù×÷´íÎó£¬Ôò±¾´Î¶ÁÈ¡Ã»±ØÒª*/
    {
	ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_PVLAN,1u, &vlan_buff);/*¶ÁÈ¡port vlanÅäÖÃ*/
    }

    if( ret == URCP_STATUS_OK)/*¶ÁÈ¡ÕıÈ·*/
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
 * º¯Êı£º getQvlanPconfConf
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯tag vlan¶Ë¿ÚÅäÖÃÊı¾İ½á¹¹
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern urcp_status_t getUrcpQvlanPconfConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_qvlan_pconf_conf_t *qvlan_pconf_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*×ª»¯ÎªÎ¢Ãî*/
    uint32 uip;

    urcp_qvlan_pconf_msg_t  urcp_qvlan_pconf_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ä¬ÈÏÎª¶ÁÈ¡Ê§°Ü*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_qvlan_pconf_msg, 0, sizeof(urcp_qvlan_pconf_msg_t));
    /*×ª»»macµØÖ·*/
    str2mac(strMac, urcp_qvlan_pconf_msg.dst_mac);
    /*Òª²Ù×÷µÄmac²»ÄÜÎª¿Õ*/
    strncpy(urcp_qvlan_pconf_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_qvlan_pconf_msg.user_ip = uip;
    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
    sendwebmsgbuffer(MSG_TYPE_URCP, QVLAN_PORT_CONFIG , (char*)(&urcp_qvlan_pconf_msg), sizeof(urcp_qvlan_pconf_msg_t));
    /*µÈ´ı1»á*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*µÈ´ı100ms¼´0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;

    while(1)
    {
	/*´Ómisc bufferÖĞ¶ÁÈ¡ĞÅÏ¢*/
	memset(qvlan_pconf_buff, 0, sizeof(urcp_qvlan_pconf_conf_t));
	ret = urcp_read_qvlan_pconf_conf(urcp_qvlan_pconf_msg.dst_mac, uip , MISC_GET_GENERAL,qvlan_pconf_buff);
	if(ret == URCP_STATUS_WAIT)
	{
	    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*ÔÙµÈ´ı*/
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
 *´òÓ¡Áª¶¯tag vlan¶Ë¿ÚÉèÖÃĞÅÏ¢µ½Ò³Ãæ
 *²¢·µ»Ø¶ÁÈ¡misc bufferµÄ×´Ì¬
 *portNum»ñÈ¡Áª¶¯Éè±¸¶Ë¿Ú¸öÊı
 *pre_retÌá¹©Ç°Ò»´ÎÁª¶¯²Ù×÷ÊÇ·ñ³É¹¦£¬·ñÔò±¾º¯ÊıÖ»´òÓ¡£¬²»·¢Áª¶¯°ü
 *bhou
 */
extern urcp_status_t getQvlanPconfUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 i=0u;
    uint32 num=0u;

    urcp_qvlan_pconf_conf_t qvlan_pconf_buff;
    urcp_status_t ret=pre_ret;/*Ä¬ÈÏÎªÇ°Ò»´Î²Ù×÷µÄ×´Ì¬*/
    char buff1[256], buff2[256],buff3[128];

    memset(&qvlan_pconf_buff,0,sizeof(urcp_qvlan_pconf_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°ÓĞÒ»´Î²Ù×÷´íÎó£¬Ôò±¾´Î¶ÁÈ¡Ã»±ØÒª*/
    {
	ret = getUrcpQvlanPconfConf(wp,strMac,strPasswd, &qvlan_pconf_buff);/*»ñÈ¡Áª¶¯tag v;am¶Ë¿ÚÉèÖÃ½á¹¹ÌåÄÚÈİ*/
    }
    if(ret != URCP_STATUS_OK)/*not get right*/
    {
	memset(&qvlan_pconf_buff,0,sizeof(urcp_qvlan_pconf_conf_t));/*Çå¿Õ£¬µ«ÎªÒ³ÃæÏÔÊ¾ÕıÈ·»¹µÃÏÔÊ¾*/
    }
    if(portNum!=NULL)
    {
	(*portNum) = qvlan_pconf_buff.port_num;/*¶Ë¿Ú¸öÊı*/
    }
    buff1[0] = '\0';
    buff2[0] = '\0';
    buff3[0] = '\0';
    /*´òÓ¡¶Ë¿ÚÅäÖÃĞÅÏ¢*/
    websWrite(wp, "var vlanAware = new Array(");/*vlanÉè±¸*/
    for(i=0u;i<qvlan_pconf_buff.port_num;i++)
    {
	if(i>0u)
	{
	    websWrite(wp, ",");/*vlan³ÉÔ±*/
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
	sprintf(&buff1[num], "\"%d\",", qvlan_pconf_buff.pconf_group[i].frame_type);/*Ö¡ÀàĞÍ*/
	/*pvid*/
	num = strlen(buff2);
	sprintf(&buff2[num], "\"%d\",", qvlan_pconf_buff.pconf_group[i].pvid);/*pvid*/

	num = strlen(buff3);
	sprintf(&buff3[num], "\"%s\",", "");/*ingr*/

    }
    websWrite(wp, ");\n"); 
    /*È¥µôÄ©Î²×Ö·û*/
    num = strlen(buff1)-1u;
    buff1[num] = '\0';
    num = strlen(buff2)-1u;
    buff2[num] = '\0';

    num = strlen(buff3)-1u;
    buff3[num] = '\0';

    websWrite(wp, "var frameType = new Array(%s);\n", buff1);/*Ö¡ÀàĞÍ*/
    websWrite(wp, "var pvid = new Array(%s);\n", buff2);/*pvid*/
    websWrite(wp, "var ingr = new Array(%s);\n", buff3);/*ingrÔ¤ÁôÏî*/
    return ret;
}
/**
 *Áª¶¯»ñÈ¡Êµ¼ÊÉè±¸ÀàĞÍµÄvlanĞÅÏ¢Ê±£¬´òÓ¡port vlan
 *bhou
 * **/
static urcp_status_t getVlanUrcpOutPvlan(webs_t wp,urcp_vlan_conf_t *pvlan_conf)
{
    /*pvlan²»ĞèÒªÔÙ»ñÈ¡£¬Ö»Ğè´òÓ¡¼´¿É*/
    char web_buf[128];
    uint32 i=0u, u_vlan_count=0u;
    port_bit_mask_t pm=0u;
    websWrite(wp, "var vlanLists = new Array(");
    web_buf[0] = '\0';
    for(i=0u;i<pvlan_conf->port_num;i++)/*port vlan×î¶à¶Ë¿Ú¸öÊı¸övlan*/
    {
	pm = pvlan_conf->vlan_group[i].member;/*¶Á¶Ë¿Ú³ÉÔ±*/
	if(pm!=0u)/*²»Îª0*/
	{
	    web_print_port_mask(web_buf, pm);
	    if(u_vlan_count>0u)
	    {
		websWrite(wp, ",");/*vlan³ÉÔ±*/
	    }
	    websWrite(wp, "%s", web_buf);/*vlan³ÉÔ±*/
	    u_vlan_count = u_vlan_count+1u;/*¼ÆÊı*/
	}
    }
    websWrite(wp, ");\n");/*vlan³ÉÔ±*/
    return URCP_STATUS_OK;
}

/******
 *Áª¶¯»ñÈ¡Ô¶³ÌÉè±¸Êµ¼ÊÀàĞÍµÄvlanĞÅÏ¢µÄÊ±ºò´òÓ¡tag vlan
 *bhou
 */
static urcp_status_t getVlanUrcpOutTagVlan(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_vlan_conf_t *qvlan_conf)
{
    /*tag vlan¿ÉÄÜ»¹Ğè»ñÈ¡¶à´Î*/
    uint16 u_next_vlan_no = 0u;
    uint32 u_vlan_count=0u;
    uint32 urcp_receive_num=1u;/*ÒÑ¾­»ñÈ¡ÁË¼¸´Îtag vlanĞÅÏ¢*/
    uint16 i=0u;
    port_bit_mask_t pm=0u;
    char web_buf[128];
    urcp_status_t ret = URCP_STATUS_OK;
    web_buf[0] = '\0';
    websWrite(wp, "var vlantabs = new Array(");
    while(ret == URCP_STATUS_OK)/*»ñÈ¡ÕıÈ·*/
    {
	if(qvlan_conf->vlan_type!=VLAN_TYPE_8021Q)/*»ñÈ¡µÄ¹ı³ÌÖĞ£¬·¢ÏÖvlanÀàĞÍ±»ÇĞ»»ÁË*/
	{
	    ret = URCP_STATUS_FAIL;/*Õû¸ö»ñÈ¡¹ı³ÌÒÔÊ§°Ü¸æÖÕ*/
	}
	else
	{
	    for(i=0u;i<MAX_URCP_BUFF_VLAN_NUM;i++)/*Ò»´Î»ñÈ¡ºó£¬´òÓ¡*/
	    {
		pm = qvlan_conf->vlan_group[i].member;/*¶Á¶Ë¿Ú³ÉÔ±*/
		if(pm>0u)
		{
		    if(u_vlan_count>0u)
		    {
			websWrite(wp, ",");/*vlan³ÉÔ±*/
		    }
		    /*ÏÔÊ¾vid*/
		    websWrite(wp, "new Array(\"%hu\",", qvlan_conf->vlan_group[i].vlan_no);/*vlan id*/
		    /*ÏÔÊ¾member*/
		    web_print_port_mask(web_buf,pm);
		    web_buf[sizeof(web_buf)-1u]='\0';
		    websWrite(wp, "%s)", web_buf);/*vlan³ÉÔ±*/
		    u_vlan_count=1u ;
		}
	    }
	    u_next_vlan_no = qvlan_conf->next_vlan_no;
	    if(u_next_vlan_no>0u)/*Ö¤Ã÷»¹Î´»ñÈ¡Íê*/
	    {
		/*ÒÔÏÂ½øĞĞ»ñÈ¡´ÎÊıÏŞÖÆ£¬ÒÔ·ÀÖ¹Î±Ôì°üµÈÔ­ÒòÔì³ÉµÄ½»»»»úÎŞÏŞÖÆÑ­»·»ñÈ¡*/
		if( (urcp_receive_num*MAX_URCP_BUFF_VLAN_NUM)<MAX_URCP_VID)/*»¹¿ÉÕıÈ·»ñÈ¡*/
		{
		    memset(qvlan_conf,0,sizeof(urcp_vlan_conf_t));
		    ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_VLAN,u_next_vlan_no, qvlan_conf);/*¶ÁÈ¡port vlanÅäÖÃ*/
		    urcp_receive_num++;/*»ñÈ¡´ÎÊı¼ÓÒ»*/
		}
		else/*µ±ÓÉÓÚ°ü´íÎóµÈÔ­Òòµ¼ÖÂÏµÍ³ÎóÈÏÎª»¹Ğè»ñÈ¡Ê±£¬Ó¦Á¢¼´½áÊøÕâÖÖ»ñÈ¡ĞĞÎª*/
		{
		    ret = URCP_STATUS_FAIL;
		}
	    }/*ÒÑ¾­»ñÈ¡Íê±Ï*/
	    else
	    {
		break;
	    }
	}

    }
    websWrite(wp, ");\n");/*vlan³ÉÔ±*/
    return ret;
}

/***
 *´òÓ¡Áª¶¯vlanĞÅÏ¢µ½Ò³Ãæ£¨Ö÷ÒªÎª»ã¾ÛµÈÒ³ÃæËùÊ¹ÓÃ£©
 *²¢·µ»Ø¶ÁÈ¡misc bufferµÄ×´Ì¬
 *portNum»ñÈ¡Áª¶¯Éè±¸¶Ë¿Ú¸öÊı
 *pre_retÌá¹©Ç°Ò»´ÎÁª¶¯²Ù×÷ÊÇ·ñ³É¹¦£¬·ñÔò±¾º¯ÊıÖ»´òÓ¡£¬²»·¢Áª¶¯°ü
 *bhou
 */
extern urcp_status_t getVlanUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    urcp_vlan_conf_t vlan_buff;
    urcp_status_t ret=pre_ret;/*Ä¬ÈÏÎªÇ°Ò»´Î²Ù×÷µÄ×´Ì¬*/
    uint32 u_vlan_type=0u;
    uint32 web_vlan_type=0u;
    memset(&vlan_buff,0,sizeof(urcp_vlan_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°ÓĞÒ»´Î²Ù×÷´íÎó£¬Ôò±¾´Î¶ÁÈ¡Ã»±ØÒª*/
    {
	/*ÏÈ»ñÈ¡Ò»´Î£¬ÒÔÈ·¶¨vlanÀàĞÍ*/
	ret = getUrcpVlanConf(wp,strMac,strPasswd,MISC_GET_VLAN,1u, &vlan_buff);/*Ê×´Î¶ÁÈ¡´Óvlan noÎª1µÄ¿ªÊ¼*/
	if( ret == URCP_STATUS_OK)/*¶ÁÈ¡ÕıÈ·*/
	{
	    u_vlan_type = vlan_buff.vlan_type;
	    if(u_vlan_type==VLAN_TYPE_PORT)/*Ô¶³ÌÉè±¸Êµ¼ÊÀàĞÍÎªport vlan*/
	    {
		ret = getVlanUrcpOutPvlan(wp,&vlan_buff);	/*´òÓ¡port vlanÏà¹ØĞÅÏ¢*/	
	    }
	    else
	    {
		if(u_vlan_type == VLAN_TYPE_8021Q)/*Ô¶³ÌÉè±¸ÀàĞÍÎªtag vlan*/
		{
		    ret = getVlanUrcpOutTagVlan(wp,strMac,strPasswd,&vlan_buff);/*´òÓ¡tag vlanÏà¹ØĞÅÏ¢*/		
		    ret = getQvlanPconfUrcp(wp,strMac,strPasswd,(uint32*)NULL,ret);/*´òÓ¡tag vlan¶Ë¿ÚÉèÖÃ*/
		}
	    }
	}
    }
    if( ret == URCP_STATUS_OK)/*¶ÁÈ¡ÕıÈ·*/
    {
	if(portNum!=NULL)
	{
	    *portNum = vlan_buff.port_num;
	}
	web_vlan_type = u_vlan_type;
    }

    websWrite(wp,T("var vlanFlag=%u;"),web_vlan_type);/*Èô¶ÁÈ¡´íÎóÔòÒ³ÃæÏÔÊ¾µÄvlan flagÎª0¼´Îª²»ÉèÖÃvlan*/
    /*¶ÁÈ¡Ê§°ÜÊ±vlan flagÎª0Ö»ÊÇÎªÒ³Ãæ²»³ö´í¶øÈç´Ë´òÓ¡¡£*/
    return ret;
}
/********************************************************************
 * º¯Êı£º getPortVlanUrcp 
 * ¹¦ÄÜ£º Ò³ÃæÏÔÊ¾º¯Êı
 * ²ÎÊı£º ÂÔ
 * ·µ»Ø£º ÂÔ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º bhou
 ********************************************************************/ 
static int getPortVlanUrcp(int eid, webs_t wp, int argc, char **argv)
{
    uint32 portNum=PORT_COUNT;
    uint32 webPortNum = 0u;/*Ò³Ãæ´òÓ¡µÄ¶Ë¿Ú¸öÊı*/
    urcp_status_t ret = URCP_STATUS_OK;
    unsigned char *strMac, *strIp, *strPasswd, *urcpSet;
    uint32 is_set=0u;

    /*ÅĞ¶ÏÊÇ·ñÓÉformAggrUrcp()Ìø×ª¹ıÀ´µÄ*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*µÚÒ»´Î´ò¿ª*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	is_set = 0u;
	isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/
    }
    else
    {
	/*Ò³Ãæ±£´æºóµÄÌø×ª*/
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
	is_set = 0u;/*´ËÊ±²»ÓÃÔÙ¶ÁÈ¡ÊÇ·ñÉèÖÃ³É¹¦*/
	ret=URCP_STATUS_FAIL;/*°´ÕÕ±¾´Î²Ù×÷Îª·Ç·¨´¦Àí£¬ÔòºóÃæÖ»´òÓ¡£¬²»·¢Áª¶¯°ü*/
	/*ºóÃæ´òÓ¡µÄ±äÁ¿ÒÑ¾­Ã»ÓĞÒâÒå£¬ÎªÁËÒ³ÃæµÄÏÔÊ¾²»³öÏÖ´íÎó»¹µÃÍùÏÂ×ß*/
    }

    if(is_set == 1u)
    {
	if(isRefresh == 0u)
	{/*ÉèÖÃºóµÄÊ×´ÎÌø×ª*/
	    ret = getPvlanUrcpSetRet(wp,strMac);/*¶ÁÈ¡ÉèÖÃ½á¹û*/
	    isRefresh = 1u;/*±£´æºó¡£ÔÙË¢ĞÂÔò²»¶ÁÈ¡ÉèÖÃ½á¹û¡£ÒòÉèÖÃ½á¹û»º´æ¿ÉÄÜ±»Çå¿Õ*/
	}
    }

    ret = getPvlanMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*´òÓ¡port vlanÛĞÅÏ¢*/
    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*´òÓ¡»ã¾ÛĞÅÏ¢*/
    if(ret==URCP_STATUS_OK)
    {
	/*³É¹¦*/
	websWrite(wp, "var urcpError = \"\";\n");
	webPortNum=portNum;/*²»ÕıÈ·µÄÇé¿öÏÂ£¬Ò³Ãæ´òÓ¡¶Ë¿Ú¸öÊıÎª0£¨·ñÔòÔò¿ÉÄÜÏÔÊ¾´íÎó£©*/
    }
    else if(ret<=MAX_ERROR_TYPE)
    {
	/*Ê§°Ü*/
        websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[ret]);
    }
    else
    {
	/*Ê§°Ü*/
	websWrite(wp, "var urcpError = \"%s\";\n", urcpErrorMsg[0]);
    }
    websWrite(wp, "var numPorts = %d;\n", webPortNum);
 
    /*vlanĞÅÏ¢*/
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}

/**
 *vlanÁª¶¯²Ù×÷£¬form¹«ÓÃº¯Êı
 *±£´æÒ³Ãæip£¬mac£¬passwordµÄÖµ¡£²¢³õÊ¼»¯msg½á¹¹Ìå
 *·µ»Ø £º1u ³É¹¦ ,0uÊ§°Ü
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

    strMac = websGetVar(wp, "macStr", T(""));/*Ô¶³ÌÉè±¸macµØÖ·*/
    if(strcmp(strMac, T(""))!=0)
    {
	str = websGetVar(wp, "numPorts", T(""));/*Ô¶³ÌÉè±¸¶Ë¿Ú¸öÊı*/
	portCt = (uint32)strtoul(str, NULL, 10);/*×ª»¯ÎªÊı×Ö*/

	if((portCt>0u)&&(portCt<=MAX_URCP_PORT_NUM))/*²»ÔÚ´Ë·¶Î§ÒÔÏÂ²Ù×÷¿ÉÄÜÄÚ´æÒç³ö*/
	{

	    /*¶ÁÈ¡Ò³Ãæ±äÁ¿*/
	    strIp = websGetVar(wp, "ipStr", T(""));/*Ô¶³ÌÉè±¸ip*/
	    strPasswd = websGetVar(wp, "passStr", T(""));/*Ô¶³ÌÉè±¸ÃÜÂë*/

	    /*±£´æ±¾´Î²Ù×÷µØÖ·*/
	    memcpy(gMac, strMac, sizeof(gMac)-1u);
	    memcpy(gIp, strIp, sizeof(gIp)-1u);
	    strncpy(gPasswd, strPasswd, sizeof(gPasswd)-1u);

	    memset(urcp_vlan_msg, 0, sizeof(struct _urcp_vlan_msg_t));/*³õÊ¼»¯ÏûÏ¢*/
	    /*Ò³Ãæ¶ÁµÄmacµØÖ·¸ñÊ½×ª»»*/
	    str2mac(strMac, urcp_vlan_msg->dst_mac);
	    strncpy(urcp_vlan_msg->password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_vlan_msg->user_ip = ip2int(wp->ipaddr);
	    urcp_vlan_msg->port_ct = portCt;
	    urcp_vlan_msg->opt_type = MISC_SET_PVLAN;/*ÉèÖÃ*/
	}
	else
	{
	    ret = 0u;
	    /*¶Ë¿ÚÊı²»¶Ô*/
	    setTheErrorMsg("get the port_num error!");
	}
    }
    else
    {
	ret = 0u;
	/*Ã»ÓĞÄ¿µÄ½»»»»úmac£¬³ö´í*/
	setTheErrorMsg("get the destination host MAC error!");
    }
    return ret;
}

/*******************************************************************
 *º¯Êı£º formPortVlanUrcp
 *¹¦ÄÜ£º Ò³ÃæÌá½»º¯Êı£¨±à¼­port vlan£¬Ìí¼Ó»òĞŞ¸Ä£©
 *²ÎÊı£º 
 *·µ»Ø£º ÎŞ
 *×÷Õß£º bhou
 *´´½¨£º 2011-04-21
 ******************************************************************/
static void formPortVlanUrcp(webs_t wp, char_t *path, char_t *query)
{
    char *c_vlan_member = NULL;
    char *c_vlan_name = NULL;
    char *c_vlan_no = NULL;

    urcp_vlan_msg_t  urcp_vlan_msg;

    memset(&urcp_vlan_msg,0,sizeof(urcp_vlan_msg_t));
    isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/

    if(formVlanProcessForUrcp(wp,&urcp_vlan_msg) > 0u)/*³õÊ¼»¯*/
    {

	/*get value from web*/
	c_vlan_name = websGetVar(wp, "vlanName", T(""));/*×éÃû³Æ*/
	c_vlan_no = websGetVar(wp, "vlanNo", T(""));/*×éºÅ*/
	c_vlan_member = websGetVar(wp, "chkstr", T(""));/*³ÉÔ±*/

	/*³õÊ¼»¯vlan_group½á¹¹*/
	urcp_vlan_msg.vlan_group.vlan_no = (uint16)strtoul(c_vlan_no, NULL, 10);
	urcp_vlan_msg.vlan_group.member = (uint32)strtoul(c_vlan_member, NULL, 10);
	memcpy(urcp_vlan_msg.vlan_group.vlan_name,c_vlan_name,MAX_VLAN_NAME_LEN);


	/*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
	sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));
    }

    /*urcpSet±íÊ¾¼ÓÔØµÄÒ³ÃæÊÇÉèÖÃºóÌø×ªµ½µÄ£¬¹ÊĞè¶ÁÈ¡ÊÇ·ñÉèÖÃ³É¹¦µÄ±êÖ¾*/
    websRedirect(wp, "switch/port_vlan.asp?urcpSet=1");
    return;
}

/*******************************************************************
 *º¯Êı£º formDelPortVlan
 *¹¦ÄÜ£º É¾³ıport vlan×é
 *²ÎÊı£º 
 *·µ»Ø£º ÎŞ
 *×÷Õß£º bhou
 *´´½¨£º 2011-04-21
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
    isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/
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
	    /*·Ö¸î*/
	    while(idnext != NULL)
	    {
		cid = idnext;
		idnext = strtok(NULL, UTT_SPLIT_SIGN_STR);
		uindex = (unsigned short)strtoul(cid, NULL, 10);
		if( (uindex > 0u)&&(uindex<=urcp_vlan_msg.port_ct) )/*ºÏ·¨»ã¾Û×éºÅ*/
		{
		    umember |= ((uint32)1u<<(uindex-1u));/*ÖÃ1±íÊ¾É¾³ı»ã¾Ûindex*/
		}
	    }

	    /*ÓÃÕı³£Çé¿öÏÂÓÃ²»µ½µÄ»ã¾Û×éºÅ0±íÊ¾É¾³ı²Ù×÷£¬ÇÒÆä³ÉÔ±±íÊ¾ÒªÉ¾³ıµÄ»ã¾Û×éÑÚÂë*/
	    urcp_vlan_msg.vlan_group.vlan_no =  0u;/*×éºÅ£¨Îª0±íÊ¾É¾³ı£©*/
	    urcp_vlan_msg.vlan_group.member = umember;/*³ÉÔ±£¨ÓûÉ¾³ıµÄ×éºÅÑÚÂë£©*/

	    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, VLAN_CONFIG, (char*)(&urcp_vlan_msg), sizeof(struct _urcp_vlan_msg_t));

	}
    }
    /*urcpSet±íÊ¾¼ÓÔØµÄÒ³ÃæÊÇÉèÖÃºóÌø×ªµ½µÄ£¬¹ÊĞè¶ÁÈ¡ÊÇ·ñÉèÖÃ³É¹¦µÄ±êÖ¾*/
    websRedirect(wp, "switch/port_vlan.asp?urcpSet=1");
    return;
}
#endif
