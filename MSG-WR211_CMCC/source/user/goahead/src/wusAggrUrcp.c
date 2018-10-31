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

static unsigned char gMac[STR_MAC_SIZE+1];/*Ò³Ãæ±£´æÊ±Òª²Ù×÷µÄÄ¿µÄmac,¹©Ò³ÃæÌø×ªºó"get"Ê¹ÓÃ*/
static unsigned char gIp[STR_IP_SIZE+1];
static unsigned char gPasswd[MAX_PASSWORD_LEN+1];
static const char* errorMsg[]={"ÕÒ²»µ½É¾³ıµÄ×é£¡"};
static uint32 isRefresh = 0u;

static int  getAggrGroupListUrcp(int eid, webs_t wp, int argc, char **argv);
static void formAggrGroupUrcp(webs_t wp, char_t *path, char_t *query);
static void formAggrGroupDelUrcp(webs_t wp, char_t *path, char_t *query);

/********************************************************************
 * º¯Êı£º formDefineAggrUrcp
 * ¹¦ÄÜ£º Ò³ÃæÓë½»»¥×ª»»º¯Êı
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern void formDefineAggrUrcp(void)
{
    websAspDefine(T("aspOutPutAggrGroupListUrcp"), getAggrGroupListUrcp);   
    websFormDefine(T("formAggrGroupUrcp"), formAggrGroupUrcp);
    websFormDefine(T("formAggrGroupDelUrcp"), formAggrGroupDelUrcp);
}
/********************************************************************
 * º¯Êı£º getUrcpRstpLacpConf
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯RSTP LACPÅäÖÃÊı¾İ½á¹¹
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern urcp_status_t getUrcpRstpLacpEnableConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_rstp_lacp_conf_t *rstp_lacp_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*×ª»¯ÎªÎ¢Ãî*/
    uint32 uip;

    urcp_rstp_lacp_msg_t  urcp_rstp_lacp_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ä¬ÈÏÎª¶ÁÈ¡Ê§°Ü*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_rstp_lacp_msg, 0, sizeof(struct _urcp_rstp_lacp_msg_t));
    /*×ª»»macµØÖ·*/
    str2mac(strMac, urcp_rstp_lacp_msg.dst_mac);
    /*Òª²Ù×÷µÄmac²»ÄÜÎª¿Õ*/
    strncpy(urcp_rstp_lacp_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_rstp_lacp_msg.user_ip = uip;
    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
    sendwebmsgbuffer(MSG_TYPE_URCP,GET_RSTP_LACP , (char*)(&urcp_rstp_lacp_msg), sizeof(struct _urcp_rstp_lacp_msg_t));
    /*µÈ´ı1»á*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*µÈ´ı100ms¼´0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;

    while(1)
    {
	/*´Ómisc bufferÖĞ¶ÁÈ¡ĞÅÏ¢*/
	memset(rstp_lacp_buff, 0, sizeof(struct _urcp_rstp_lacp_conf_t));
	ret = urcp_read_rstp_lacp_conf(urcp_rstp_lacp_msg.dst_mac, uip , MISC_GET_GENERAL, rstp_lacp_buff);
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
 *´òÓ¡Áª¶¯rstp lacpĞÅÏ¢µ½Ò³Ãæ
 *²¢·µ»Ø¶ÁÈ¡misc bufferµÄ×´Ì¬
 *portNum»ñÈ¡Áª¶¯Éè±¸¶Ë¿Ú¸öÊı
 *pre_retÌá¹©Ç°Ò»´ÎÁª¶¯²Ù×÷ÊÇ·ñ³É¹¦£¬·ñÔò±¾º¯ÊıÖ»´òÓ¡£¬²»·¢Áª¶¯°ü
 *bhou
 */
extern urcp_status_t getRstpLacpEnableUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,uint32* portNum,urcp_status_t pre_ret)
{
    uint32 uindex=0u;
    uint32 i=0u;
    char  str_buf[128];

    urcp_rstp_lacp_conf_t rstp_lacp_buff;
    urcp_status_t ret=pre_ret;/*Ä¬ÈÏÎªÇ°Ò»´Î²Ù×÷µÄ×´Ì¬*/

    memset(str_buf,0,sizeof(str_buf));
    memset(&rstp_lacp_buff,0,sizeof(urcp_rstp_lacp_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°ÓĞÒ»´Î²Ù×÷´íÎó£¬Ôò±¾´Î¶ÁÈ¡Ã»±ØÒª*/
    {
	ret = getUrcpRstpLacpEnableConf(wp,strMac,strPasswd, &rstp_lacp_buff);/*»ñÈ¡Áª¶¯rstp_lacp½á¹¹ÌåÄÚÈİ*/
    }
    websWrite(wp, "var lacp = new Array("); /*start print lacp enable*/
    if(ret != URCP_STATUS_OK)/*not get right*/
    {
	memset(&rstp_lacp_buff,0,sizeof(urcp_rstp_lacp_conf_t));/*Çå¿Õ£¬µ«ÎªÒ³ÃæÏÔÊ¾ÕıÈ·»¹µÃÏÔÊ¾*/
    }
    if(portNum!=NULL)
    {
	(*portNum) = rstp_lacp_buff.port_num;/*¶Ë¿Ú¸öÊı*/
    }
    for (uindex = 0u; uindex < rstp_lacp_buff.port_num; uindex++)
    {
	if(uindex > 0u)/*·ÇÊ×Î» Ôò´òÓ¡·Ö¸ô·û*/
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
    str_buf[sizeof(str_buf)-1u] = '\0';/*×îºóÒ»Î»ÖÃ0£¬·ÀÖ¹ÄÚ´æĞ¹Â¶*/
    websWrite(wp, ");"); 
    websWrite(wp,"var rstp = new Array(%s);",str_buf);/*print rstp enable to web*/
    return ret;
}
/********************************************************************
 * º¯Êı£º getAggrUrcpSetRet
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯»ã¾Û²Ù×÷½á¹û
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
static urcp_status_t getAggrUrcpSetRet(webs_t wp,uchar* strMac)
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
	ret = urcp_read_aggr_conf(std_mac, uip , MISC_SET_GENERAL, (urcp_aggr_conf_t*)NULL);
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
 * º¯Êı£º getUrcpAggrConf
 * ¹¦ÄÜ£º »ñÈ¡Áª¶¯»ã¾ÛÅäÖÃÊı¾İ½á¹¹
 * ²ÎÊı£º ÎŞ
 * Êä³ö£º ÎŞ
 * ·µ»Ø£º ÎŞ
 * ×÷Õß£º bhou
 *******************************************************************/
extern urcp_status_t getUrcpAggrConf(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_aggr_conf_t *aggr_buff)
{
    uint32 wait_time_us=0u;
    uint32 max_wait_time=MAX_URCP_WEB_TIME_OUT_SEC;
    max_wait_time = max_wait_time * 1000u *1000u ;/*×ª»¯ÎªÎ¢Ãî*/
    uint32 uip;

    urcp_aggr_msg_t  urcp_aggr_msg;
    urcp_status_t ret=URCP_STATUS_FAIL;/*Ä¬ÈÏÎª¶ÁÈ¡Ê§°Ü*/

    uip=ip2int(wp->ipaddr);
    memset(&urcp_aggr_msg, 0, sizeof(struct _urcp_aggr_msg_t));
    /*×ª»»macµØÖ·*/
    str2mac(strMac, urcp_aggr_msg.dst_mac);
    /*Òª²Ù×÷µÄmac²»ÄÜÎª¿Õ*/
    strncpy(urcp_aggr_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_aggr_msg.user_ip = uip;
    urcp_aggr_msg.opt_type = MISC_GET_GENERAL;/*¶ÁÈ¡²Ù×÷*/
    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
    sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));
    /*µÈ´ı1»á*/
    usleep(URCP_WEB_WAIT_INTERVAL_USEC);/*µÈ´ı100ms¼´0.1s*/
    wait_time_us += URCP_WEB_WAIT_INTERVAL_USEC;
    while(1)
    {
	/*´Ómisc bufferÖĞ¶ÁÈ¡ĞÅÏ¢*/
	memset(aggr_buff, 0, sizeof(struct _urcp_aggr_conf_t));
	ret = urcp_read_aggr_conf(urcp_aggr_msg.dst_mac, uip , MISC_GET_GENERAL, aggr_buff);
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
 *´òÓ¡Áª¶¯»ã¾ÛĞÅÏ¢µ½Ò³Ãæ
 *²¢·µ»Ø¶ÁÈ¡misc bufferµÄ×´Ì¬
 *portNum»ñÈ¡Áª¶¯Éè±¸¶Ë¿Ú¸öÊı
 *pre_retÌá¹©Ç°Ò»´ÎÁª¶¯²Ù×÷ÊÇ·ñ³É¹¦£¬·ñÔò±¾º¯ÊıÖ»´òÓ¡£¬²»·¢Áª¶¯°ü
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
    urcp_status_t ret=pre_ret;/*Ä¬ÈÏÎªÇ°Ò»´Î²Ù×÷µÄ×´Ì¬*/

    memset(&aggr_buff,0,sizeof(urcp_aggr_conf_t));

    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°ÓĞÒ»´Î²Ù×÷´íÎó£¬Ôò±¾´Î¶ÁÈ¡Ã»±ØÒª*/
    {
	ret = getUrcpAggrConf(wp,strMac,strPasswd, &aggr_buff);
    }
    websWrite(wp, "var aggGrpNos = new Array();\n");/*×éºÅ*/
    websWrite(wp, "var aggLists = new Array();\n");/*³ÉÔ±ÁĞ±í*/
    websWrite(wp, "var aggGrpNames = new Array();\n");/*×éÃû*/
    if( ret == URCP_STATUS_OK)/*¶ÁÈ¡ÕıÈ·*/
    {
	if(portNum!=NULL)
	{
	    (*portNum) = aggr_buff.port_num;/*¶Ë¿Ú¸öÊı*/
	}
	for (uindex = 0u; uindex < aggr_buff.port_num; uindex++)
	{
	    /*×éºÅ*/
	    ugroup = aggr_buff.aggr_group[uindex].index;
	    /*³ÉÔ±*/
	    umember = aggr_buff.aggr_group[uindex].member;
	    if ((ugroup>0u) && (umember>0u))/*º¬ÓĞ»ã¾Û×é³ÉÔ±*/
	    {
		websWrite(wp, "aggGrpNos[%d] = \"%hu\";", icount, ugroup);	 
		websWrite(wp, "aggLists[%d] = \"", icount);
		for (i=0u; i < aggr_buff.port_num; i++)
		{
		    if ( (umember & (((uint32)1u<<i))) > 0u)/*º¬ÓĞ¸Ã³ÉÔ±*/
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
 *´òÓ¡Ô¶³Ì¾µÏñ¶Ë¿Úµ½webÒ³Ãæ
 *ÈôÔ¶³Ì¾µÏñÔ´¶Ë¿Ú²»´æÔÚ£¬Ôò´òÓ¡½á¹ûÎª0
 *bhou
 */
static urcp_status_t getMirrorPortUrcp(webs_t wp,uchar* strMac,uchar* strPasswd,urcp_status_t pre_ret)
{
    urcp_status_t ret = pre_ret;
    urcp_mirror_conf_t mirror_buff;
    uint32 u_mport = 0u;
    memset(&mirror_buff,0,sizeof(urcp_mirror_conf_t));
    if(pre_ret == URCP_STATUS_OK)/*ÈôÖ®Ç°µÄÁª¶¯ĞÅÏ¢Ã»ÓĞ»ñÈ¡³É¹¦£¬Ôò±¾´Î»ñÈ¡Ã»ÓĞ±ØÒª*/
    {
	ret = getUrcpMirrorConf(wp,strMac,strPasswd,&mirror_buff,MISC_GET_GENERAL);/*get mirror conf*/
    }
    if(ret == URCP_STATUS_OK)/*±¾´Î»ñÈ¡³É¹¦*/
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
 * º¯Êı£º getAggrGroupListUrcp 
 * ¹¦ÄÜ£º Ò³ÃæÏÔÊ¾º¯Êı
 * ²ÎÊı£º ÂÔ
 * ·µ»Ø£º ÂÔ
 * Êä³ö£º ÂÔ
 * ×÷Õß£º bhou
 * ÈÕÆÚ£gº 201-04-09
 ********************************************************************/ 
static int getAggrGroupListUrcp(int eid, webs_t wp, int argc, char **argv)
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
	isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/
	is_set = 0u;
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
	    ret = getAggrUrcpSetRet(wp,strMac);/*¶ÁÈ¡ÉèÖÃ½á¹û*/
	    isRefresh = 1u;/*±£´æºó¡£ÔÙË¢ĞÂÔò²»¶ÁÈ¡ÉèÖÃ½á¹û¡£ÒòÉèÖÃ½á¹û»º´æ¿ÉÄÜ±»Çå¿Õ*/
	}
    }

    ret = getAggrMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*´òÓ¡»ã¾ÛĞÅÏ¢*/
    ret = getRstpLacpEnableUrcp(wp,strMac,strPasswd,&portNum,ret);/*´òÓ¡rstp lacp enableĞÅÏ¢*/
    ret = getMirrorPortUrcp(wp,strMac, strPasswd,ret); /*print mirror  port to web page*/
    ret = getPortConfMemberUrcp(wp,strMac,strPasswd,&portNum,ret);/*´òÓ¡¶Ë¿ÚÉèÖÃ±¾Ò³Ãæ³ÉÔ±*/
    ret = getQosRateLimitMemberUrcp(wp,strMac, strPasswd,&portNum,ret); /*´òÓ¡ËÙÂÊÏŞÖÆ*/
    ret = getVlanUrcp(wp,strMac, strPasswd,&portNum,ret); /*print vlan to web page**/
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
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}

/**
 *»ã¾ÛÁª¶¯²Ù×÷£¬form¹«ÓÃº¯Êı
 *±£´æÒ³Ãæip£¬mac£¬passwordµÄÖµ¡£²¢³õÊ¼»¯msg½á¹¹Ìå
 *·µ»Ø £º1u ³É¹¦ ,0uÊ§°Ü
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

	    memset(urcp_aggr_msg, 0, sizeof(struct _urcp_aggr_msg_t));/*³õÊ¼»¯ÏûÏ¢*/
	    /*Ò³Ãæ¶ÁµÄmacµØÖ·¸ñÊ½×ª»»*/
	    str2mac(strMac, urcp_aggr_msg->dst_mac);
	    strncpy(urcp_aggr_msg->password, strPasswd, MAX_PASSWORD_LEN);
	    urcp_aggr_msg->user_ip = ip2int(wp->ipaddr);
	    urcp_aggr_msg->port_ct = portCt;
	    urcp_aggr_msg->opt_type = MISC_SET_GENERAL;/*ÉèÖÃ*/
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
 *º¯Êı£º formAggrGroupUrcp
 *¹¦ÄÜ£º Ò³ÃæÌá½»º¯Êı£¨±à¼­»ã¾Û×é£¬Ìí¼Ó»òĞŞ¸Ä£©
 *²ÎÊı£º 
 *·µ»Ø£º ÎŞ
 *×÷Õß£º bhou
 *´´½¨£º 2011-04-12
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
    isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/

    if(formAggrProcessForUrcp(wp,&urcp_aggr_msg) > 0u)
    {

	/*get value from web*/
	action = websGetVar(wp, "actions", T(""));/*²Ù×÷ÀàĞÍ Ìí¼Ó»òĞŞ¸Ä £¬Áª¶¯²Ù×÷Ä¿Ç°²»ĞèÒª*/
	cremark = websGetVar(wp, "aggName", T(""));/*×éÃû³Æ*/
	group = websGetVar(wp, "aggNo", T(""));/*×éºÅ*/


	/*set member to index 0*/
	for (i = 0u; i < urcp_aggr_msg.port_ct; i++)
	{
	    sprintf(chk_name,"chkPort%u",i);/*html¿Ø¼şÃû*/
	    cmember = websGetVar(wp,chk_name, T(""));
	    if (strcmp(cmember, "on") == 0)
	    {
		umember  |= (uint32)(1u << i);
	    }
	}

	/*³õÊ¼»¯aggr_group½á¹¹*/
	urcp_aggr_msg.aggr_group.index = (uint16)strtoul(group, NULL, 10);
	urcp_aggr_msg.aggr_group.member = umember;
	memcpy(urcp_aggr_msg.aggr_group.remark,cremark,MAX_REMARK_LEN);


	/*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
	sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));
    }

    /*urcpSet±íÊ¾¼ÓÔØµÄÒ³ÃæÊÇÉèÖÃºóÌø×ªµ½µÄ£¬¹ÊĞè¶ÁÈ¡ÊÇ·ñÉèÖÃ³É¹¦µÄ±êÖ¾*/
    websRedirect(wp, "switch/aggregation.asp?urcpSet=1");
    return;
}

/*******************************************************************
 *º¯Êı£º formAggrGroupDelUrcp
 *¹¦ÄÜ£º É¾³ı»ã¾Û×é
 *²ÎÊı£º 
 *·µ»Ø£º ÎŞ
 *×÷Õß£º bhou
 *´´½¨£º 2011-04-12
 ******************************************************************/
static void formAggrGroupDelUrcp(webs_t wp, char_t *path, char_t *query)
{

    char *del_str = NULL;
    unsigned short uindex;
    char *cid;
    char *idnext;
    uint32 umember=0u;
    urcp_aggr_msg_t  urcp_aggr_msg;

    isRefresh = 0u;/*Ë¢ĞÂ±êÖ¾Î»¸´Î»*/
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
	    /*·Ö¸î*/
	    while(idnext != NULL)
	    {
		cid = idnext;
		idnext = strtok(NULL, DELSEPARATE);
		uindex = (unsigned short)strtoul(cid, NULL, 10);
		if( (uindex > 0u)&&(uindex<=urcp_aggr_msg.port_ct) )/*ºÏ·¨»ã¾Û×éºÅ*/
		{
		    umember |= ((uint32)1u<<(uindex-1u));/*ÖÃ1±íÊ¾É¾³ı»ã¾Ûindex*/
		}
	    }

	    /*ÓÃÕı³£Çé¿öÏÂÓÃ²»µ½µÄ»ã¾Û×éºÅ0±íÊ¾É¾³ı²Ù×÷£¬ÇÒÆä³ÉÔ±±íÊ¾ÒªÉ¾³ıµÄ»ã¾Û×éÑÚÂë*/
	    urcp_aggr_msg.aggr_group.index =  0u;/*×éºÅ£¨Îª0±íÊ¾É¾³ı£©*/
	    urcp_aggr_msg.aggr_group.member = umember;/*³ÉÔ±£¨ÓûÉ¾³ıµÄ×éºÅÑÚÂë£©*/

	    /*·¢ÏûÏ¢,Í¨Öª½»»»»ú½ø³Ì·¢°ü*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, AGGR_CONFIG, (char*)(&urcp_aggr_msg), sizeof(struct _urcp_aggr_msg_t));

	}
    }
    /*urcpSet±íÊ¾¼ÓÔØµÄÒ³ÃæÊÇÉèÖÃºóÌø×ªµ½µÄ£¬¹ÊĞè¶ÁÈ¡ÊÇ·ñÉèÖÃ³É¹¦µÄ±êÖ¾*/
    websRedirect(wp, "switch/aggregation.asp?urcpSet=1");
    return;
}
#endif
