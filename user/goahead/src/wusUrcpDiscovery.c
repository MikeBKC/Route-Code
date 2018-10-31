#include    <stdio.h>
#include    <sys/ioctl.h>
#include    <string.h>
#include    <dirent.h>
#include    <linux/autoconf.h>
#include    <uttMachine.h>
#include    "webs.h"
#include    "webmsg.h"
#include    "utils.h"
#include    "uttfunction.h"

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    <mibUrcp.h>
#endif

#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
static void formRrcpDiscover(webs_t wp, char_t *path, char_t *query);
static int getRrcpDiscoveryList(int eid, webs_t wp, int argc, char **argv);
static int aspOutSysInfoRemote(int eid, webs_t wp, int argc, char **argv);

/********************************************************************
 * 函数： fromDefineRrcpDiscovery
 * 功能： 页面与交互转换函数
 * 创建： 2010-09-09
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： jfgu
 * 维护： jfgu
 *******************************************************************/
extern void formDefineRrcpDiscovery(void)
{
	websAspDefine(T("aspOutPutgetRrcpDiscoveryList"), getRrcpDiscoveryList);
	websAspDefine(T("aspOutSysInfoRemote"), aspOutSysInfoRemote);
	websFormDefine(T("formRrcpDiscover"), formRrcpDiscover);
}

/*******************************************************************
 *函数： formRrcpDiscover 
 *功能： 页面提交函数
 *参数： 略
 *返回： 无
 *作者： jfgu
 *日期： 2010-8-24
 ******************************************************************/
static void formRrcpDiscover(webs_t wp, char_t *path, char_t *query)
{
    /*发消息*/
    sendwebmsg(MSG_TYPE_GET, GET_URCP_DISCOVERY);

    websRedirect(wp, "switch/wait_rrcp.asp");
}


/********************************************************************
 * 函数： getRrcpDiscoveryList 
 * 功能： 页面显示函数
 * 创建： 2010-08-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： jfgu
 * 维护： jfgu
 * 日期： 2010-09-09
 ********************************************************************/ 
static int getRrcpDiscoveryList(int eid, webs_t wp, int argc, char **argv)
{
    char m_buffer[64] = {0};
    unsigned int i, val;
    urcp_dev_t *tmp = NULL; 
    int tb_index, index = 0;
    urcp_shm_t *mem_shadow=NULL;


    websWrite(wp, "var iparray = new Array();\n");
    websWrite(wp, "var macarray = new Array();\n");
    websWrite(wp, "var portarray = new Array();\n");
    websWrite(wp, "var namearray = new Array();\n");
    websWrite(wp, "var serialno = new Array();\n");
    websWrite(wp, "var types = new Array();\n");
    websWrite(wp, "var aliveports = new Array();\n");
    websWrite(wp, "var managed_enable = new Array();\n");
    websWrite(wp, "var events = new Array();\n");
    websWrite(wp, "var verarray = new Array();\n");

    mem_shadow = get_shm_ptr();
    /*遍历hash*/
    for(i = 0u; i < DEV_HASH_SIZE; i++) 
    {
	tb_index = mem_shadow->dev_tb_hash[i];

	while(tb_index!=NONE_NODE)
	{
	    tmp = &mem_shadow->dev_table[tb_index-1]; 
	    /*mac*/
	    sprintf(m_buffer, "%02x-%02x-%02x-%02x-%02x-%02x", 
		    tmp->mac[0], tmp->mac[1], tmp->mac[2], tmp->mac[3], tmp->mac[4], tmp->mac[5]);
	    websWrite(wp, "macarray[%d] = '%s';\n", index, m_buffer);

	    /*ip*/
	    sprintf(m_buffer, "%u.%u.%u.%u", 
		    (tmp->ip >> 24) & 0x000000ff, 
		    (tmp->ip >> 16) & 0x000000ff, 
		    (tmp->ip >> 8) & 0x000000ff, 
		    (tmp->ip) & 0x000000ff);
	    websWrite(wp, "iparray[%d] = '%s';\n", index, m_buffer);
	    /*port*/
	    websWrite(wp, "portarray[%d] = '%d';\n", index, tmp->link_port);
	    /*hostname */
	    websWrite(wp, "namearray[%d] = '%s';\n", index, tmp->dev_name);
	    /*serialno*/
	    websWrite(wp, "serialno[%d] = '0x%x';\n", index, tmp->serial_no);
	    /*managed enable*/
	    val = tmp->dev_type;
	    val = val & DEV_FLAG_MANAGE;
	    if(val>0){
		websWrite(wp, "managed_enable[%d] = '0x1';\n", index);
	    }
	    else{
		websWrite(wp, "managed_enable[%d] = '0x0';\n", index);
	    }
	    /*type*/
	    websWrite(wp, "types[%d] = '%s';\n", index, tmp->dev_model);
	    /*alive port*/
	    websWrite(wp, "aliveports[%d] = '%d';\n", index, tmp->up_ports);
	    /*alarm level*/
	    websWrite(wp, "events[%d] = '%d';\n", index, tmp->alarm_level);
	    /*version*/
	    websWrite(wp, "verarray[%d] = '%s';\n", index, tmp->sw_version);

	    index++;
	    tb_index = tmp->tb_next;
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}

/********************************************************************
 * 函数： aspOutSysInfoRemote
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 日期： 2011-03-16
 ********************************************************************/ 
static int aspOutSysInfoRemote(int eid, webs_t wp, int argc, char **argv)
{
    urcp_dev_t* tmp=NULL;
    uint32 ip;
    unsigned char *strMac=NULL, *strIp=NULL, stdMac[MAC_LEN], *passWord = NULL;

    /*读取mac*/
    strMac = websGetVar(wp, "macStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    passWord = websGetVar(wp, "passStr", T(""));
    /*转换格式*/
    ip = ip2int(strIp);
    str2mac(strMac, stdMac);
    /*查表*/
    tmp = urcp_dev_table_lookup(stdMac, ip);
    if(tmp!=NULL)
    {
	websWrite(wp, "var sysSerial = \"%u\";\n", tmp->serial_no);
	websWrite(wp, "var sysVersion = \"%s\";\n", tmp->sw_version);
	websWrite(wp, "var deviceName = \"%s\";\n", tmp->dev_name);
	websWrite(wp, "var ProductID = \"%s\";\n", tmp->dev_model);
	websWrite(wp, "var devType = \"%s\";\n", tmp->dev_model);
	if ( ((tmp->dev_type)&DEV_TYPE_WIRELESS_5G)!=0 )
	{
	    websWrite(wp, "var flag_5G = \"%d\";\n", 1);
	}
	else
	{	
	    websWrite(wp, "var flag_5G = \"%d\";\n", 0);
	}

    }
    else
    {
	websWrite(wp, "var sysSerial = \"\";\n");
	websWrite(wp, "var sysVersion = \"\";\n");
	websWrite(wp, "var deviceName = \"\";\n");
	websWrite(wp, "var ProductID = \"\";\n");
	websWrite(wp, "var flag_5G = \"\";\n");
    }
    
    sendGeneralUrcpMsg(strIp, strMac, passWord,PORT_COUNT,MISC_GET_GENERAL, GET_AP_BASIC_CONFIG);/*发消息*/
    return 0;
}
#endif
