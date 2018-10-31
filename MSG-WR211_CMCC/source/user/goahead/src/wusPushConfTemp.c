
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include <mibUrcp.h>

#if(FEATURE_AP_CONF_TEMP == FYES)
#include    <urcp_ipc.h>
#include    "webmsg.h"
#include    <sw_cfg.h>

#define DEBUG_TEMP 0
#if DEBUG_TEMP
#define TEMP_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define TEMP_DEBUG(fmt, argx...)
#endif

extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);
#if 0
/***********************************************************
 *函数名：getWirelessWlFre
 *功能：取出标记wlFre(这个参数用于标记AP无线 5G和2.4G 的不同模式)
 *创建日期：2014-09-04
 *作者：hec
 *说明：
 * *********************************************************/
Uint32 getWirelessWlFre(unsigned char *mac)
{
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t *tmp = NULL;
    uint32 i,tb_index;
    unsigned char apMac[15];
    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u;i < DEV_HASH_SIZE;i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index != NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index - 1]);
		memset(apMac,0,sizeof(apMac));
		sprintf(apMac,"%02X%02X%02X%02X%02X%02X",tmp->mac[0],tmp->mac[1],tmp->mac[2],tmp->mac[3],tmp->mac[4],tmp->mac[5]);
		tb_index = tmp->tb_next;
	    }
	}
    }
    return 0;
}
#endif
/***********************************************************************
 * 函 数 名：   aspOutRemoteAPTemp
 * 创建日期：	2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
***********************************************************************/
static int aspOutRemoteAPTemp(int eid, webs_t wp, int argc, char_t **argv)
{
    TEMP_DEBUG("\n---> aspOutRemoteAPTemp() begin <---\n");
    int min, max, index, count = 0, num = 0, i;
    char tempName[320], wlFre[320], wlMode[320],wlRate[320];
    ApConfTempProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;
    unsigned char *strMac = NULL, *strIp = NULL, *strPasswd = NULL, *web_type = NULL,*szName=NULL,*strManaged=NULL;
    unsigned char *waitOver = NULL, *optType = NULL;
    urcp_dev_t *dev_table = NULL;
    uint32 ip = 0u, devNot5G = 0u;
    unsigned char stdMac[MAC_LEN];
    char tpMac[400], tpIp[800], *tmpMac = tpMac, *tmpIp = tpIp, *ptrMac, *ptrIp; 
    
    strMac = websGetVar(wp, "macStr", T(""));
    szName = websGetVar(wp, "szName", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    web_type = websGetVar(wp, "devType", T(""));
    strManaged = websGetVar(wp, "managedEn", T(""));

    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", szName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);
    
    memset(tempName, '\0', sizeof(tempName));
	memset(wlFre, '\0', sizeof(wlFre));
	memset(wlMode, '\0', sizeof(wlMode));
	memset(wlRate, '\0', sizeof(wlRate));

    ProfInstLowHigh(mibType, &max, &min);
    /*找到AP设备,列出适合该设备的配置模板(检查设备是否支持5G)*/
    i = sizeof(tpMac) - 1;
    memset(tpMac, 0, sizeof(tpMac));
    strncpy(tpMac, strMac, i);
    i = sizeof(tpIp) - 1;
    memset(tpIp, 0, sizeof(tpIp));
    strncpy(tpIp, strIp, i);
    tmpMac = strtok_r(tpMac,",",&ptrMac);
    tmpIp = strtok_r(tpIp,",",&ptrIp);
    while((NULL != tmpMac) && (NULL != tmpIp))
    {
	dev_table = NULL;
	memset(stdMac, 0, MAC_LEN);
	ip = ip2int((char *)tmpIp);
	str2mac((unsigned char*)tmpMac, stdMac);
	dev_table = urcp_dev_table_lookup(stdMac, ip);
	if((NULL != dev_table) && (((dev_table->dev_type) & DEV_TYPE_WIRELESS_5G) == 0))
	{
	    devNot5G = 1u;
	    break;
	}
	tmpMac = strtok_r((char *)NULL, ",", &ptrMac);
	tmpIp = strtok_r((char *)NULL, ",", &ptrIp);
    }
    for (index = min; index < max; index++)
    {
	prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
		{
			if((prof->wlFre == RF_FLAG_5G) && (1u == devNot5G))
			{
			    continue;
			}
			num = strlen(tempName);
			sprintf(&tempName[num], "\"%s\",", prof->tempName);
			num = strlen(wlFre);
			if(prof->wlFre == RF_FLAG_24G)
			{
				sprintf(&wlFre[num], "\"%s\",", "2.4GHz");
			}else if(prof->wlFre == RF_FLAG_5G)
			{
			    sprintf(&wlFre[num], "\"%s\",", "5GHz");
			}

			num = strlen(wlMode);
			if(prof->wlFre == RF_FLAG_24G)
			{
			    if(prof->mode == 1)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11g");
			    }else if(prof->mode == 2)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11n");
			    }else if(prof->mode == 3)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11b/g/n");
			    }
			}else if(prof->wlFre == RF_FLAG_5G)
			{
			    if(prof->mode == 3)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11a");
			    }else if(prof->mode == 4)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11a/n");
			    }else if(prof->mode == 5)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11vht AC/AN/A");
			    }else if(prof->mode == 6)
			    {
				sprintf(&wlMode[num], "\"%s\",", "11vht AC/AN");
			    }
			}   
			num = strlen(wlRate);
			if (prof->rate == M11)
			{
				sprintf(&wlRate[num], "\"%s\",", "11M");
			}
			else if(prof->rate == M54)
			{
				sprintf(&wlRate[num], "\"%s\",", "54M");
			}
			else if(prof->rate == M150)
			{
				sprintf(&wlRate[num], "\"%s\",", "150M");
			}
			else if(prof->rate == M300)
			{
				sprintf(&wlRate[num], "\"%s\",", "300M");
			}
			else if(prof->rate == AUTO_RATE)
			{
				sprintf(&wlRate[num], "\"%s\",", "自动");
			}
			count++;
		}
    }
	/*不为空*/
	if(count > 0u)
	{
		num = strlen(tempName)-1u;
		tempName[num] = '\0';
		num = strlen(wlFre)-1u;
		wlFre[num] = '\0';
		num = strlen(wlMode)-1u;
		wlMode[num] = '\0';
		num = strlen(wlRate)-1u;
		wlRate[num] = '\0';
	}

    websWrite(wp, "var tmpname = new Array(%s);\n", tempName);/*  */
	websWrite(wp, "var wlFres = new Array(%s);\n", wlFre);/*  */
	websWrite(wp, "var wlModes = new Array(%s);\n", wlMode);/*  */
	websWrite(wp, "var wlRates = new Array(%s);\n", wlRate);/*  */

    websWrite(wp,"var totalrecs = %d;", count);
    
    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "sendAPTemp")==0)
	{
	    /*下发配置*/
	    getErrorArr(wp, urcp_read_ap_conf_temp, MISC_SET_GENERAL);
	}
    }


    getTheErrorMsg(eid, wp, argc, argv);

    TEMP_DEBUG("\n---> aspOutRemoteAPTemp() end <---\n");
    return 0;
}


static int aspRemoteApConfTempSend(int eid, webs_t wp, int argc, char_t **argv)
{
    TEMP_DEBUG("\n---> %s() start <---\n", __func__);
    uint32 i = 0u, found = 0u, j = 0u;
    char *strMac = NULL, *strPasswd = NULL, *strValue = NULL,*devType = NULL;
    urcp_ap_conf_temp_msg_t urcp_conf_temp_msg;
    urcp_batch_msg_t batch_info;

    int min, max, index, count = 0, num = 0;
    ApConfTempProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_AP_CONF_TEMP;

    memset(&urcp_conf_temp_msg, 0, sizeof(urcp_ap_conf_temp_msg_t));
    /*读页面数据*/
    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    devType = websGetVar(wp, "devType", T(""));
    /*文件名*/
    strValue = websGetVar(wp, "remoteSrcTemp", T(""));

//    str2mac(strMac, urcp_conf_temp_msg.general_msg.dst_mac);
//    strncpy(urcp_conf_temp_msg.general_msg.password, strPasswd, MAX_PASSWORD_LEN);
    urcp_conf_temp_msg.general_msg.user_ip = ip2int(wp->ipaddr);

    ProfInstLowHigh(mibType, &max, &min);

    for (index = min; index < max; index++)
    {
	// TEMP_DEBUG("\n---> index:%d <---\n", index);
	prof = (ApConfTempProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
	{
	    if(strcmp(strValue, prof->tempName) == 0)
	    {
		found = 1u;
		break;
	    }
	}
    }
    memset(&batch_info, 0, sizeof(batch_info));
    j = getBatchInfo(wp, strMac, strPasswd, &batch_info);
    if((found == 1u) && (j > 0))
    {
	i = sizeof(urcp_conf_temp_msg.conf_temp_conf.tempName);
	strncpy(urcp_conf_temp_msg.conf_temp_conf.tempName, strValue, i);
	strcpy(urcp_conf_temp_msg.conf_temp_conf.suitType, devType);
	urcp_conf_temp_msg.conf_temp_conf.wireless = prof->wireless;
	urcp_conf_temp_msg.conf_temp_conf.sgi = prof->sgi;
	urcp_conf_temp_msg.conf_temp_conf.preamble = prof->preamble;
	urcp_conf_temp_msg.conf_temp_conf.wmm = prof->wmm;
	urcp_conf_temp_msg.conf_temp_conf.rate = prof->rate;
	urcp_conf_temp_msg.conf_temp_conf.bw = prof->bw;
	urcp_conf_temp_msg.conf_temp_conf.wlFre = prof->wlFre;
	urcp_conf_temp_msg.conf_temp_conf.VChanWidth = prof->VChanWidth;
	urcp_conf_temp_msg.conf_temp_conf.channel = prof->channel;
	urcp_conf_temp_msg.conf_temp_conf.power = prof->power;
	urcp_conf_temp_msg.conf_temp_conf.manPower = prof->manPower;
	urcp_conf_temp_msg.conf_temp_conf.mode = prof->mode;
	urcp_conf_temp_msg.conf_temp_conf.beaconPeriod = prof->BeaconPeriod;

	/*发消息,通知交换机进程发包*/
	for (j = 0u; j < batch_info.number; j++)
	{
	    memcpy(urcp_conf_temp_msg.general_msg.dst_mac, &(batch_info.dst_mac[j*MAC_LEN]), MAC_LEN);
	    strncpy(urcp_conf_temp_msg.general_msg.password, batch_info.password[j], MAX_PASSWORD_LEN);
	    sendwebmsgbuffer(MSG_TYPE_URCP, SET_CONFIG_TEMPLATE, (char*)(&urcp_conf_temp_msg), sizeof(urcp_ap_conf_temp_msg_t));
	}

    }

    getTheErrorMsg(eid, wp, argc, argv);

    TEMP_DEBUG("\n---> %s() end <---\n", __func__);

    return 0;
}

/***********************************************************************
 * 函 数 名：   formDefinePushConfTemp
 * 创建日期：	2012-07-18
 * 修改日期：
 * 作者：       Shiyu.Chen
 * 附加说明：	无
***********************************************************************/
void formDefinePushConfTemp(void)
{
    websAspDefine(T("aspOutRemoteAPTemp"), aspOutRemoteAPTemp);
    websAspDefine(T("aspRemoteApConfTempSend"), aspRemoteApConfTempSend);

    return;
}
#endif
