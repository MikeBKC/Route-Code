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
#include    "wsIntrn.h"
#include    "um.h"
#include    "utils.h"
#include    "internet.h"
#include    "mib.h"
#include    "profacce.h"
#include    "translate.h"

#if (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    <mibUrcp.h>


#if (AC_AP_SYS_INFO == FYES)
#define DATA_1K 1024.0
#define DATA_1M 1048576.0
#define DATA_1G 1073741824.0

static void mac2str(uchar mac[], char *str)
{
    if (str != NULL)
    {
	sprintf(str, "%02X%02X%02X%02X%02X%02X",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    return;
}

static void time2str(uint32 sec, char *timeStr, int strLen)
{
    int updays=0,uphours=0,upminutes=0,upsecs=0;
    const char *com_day = NULL, *com_hour = NULL, *com_minute = NULL, *com_second = NULL;
    int ret = 0;
    updays = sec / (60*60*24);
    upminutes = sec / 60;
    uphours = (upminutes / 60) % 24;
    upminutes %= 60;
    upsecs = sec % 60;
    com_day = getTransDataForC(C_LANG_INDEX_DAYS);
    com_hour = getTransDataForC(C_LANG_INDEX_HOURS);
    com_minute = getTransDataForC(C_LANG_INDEX_MINUTES);
    com_second = getTransDataForC(C_LANG_INDEX_SECONDS);
    if ( com_day != NULL &&
	     com_hour != NULL &&
	     com_minute != NULL &&
	     com_second != NULL ) 
    {
	snprintf(timeStr,strLen,"%d%s%d%s%d%s%d%s",updays, com_day, uphours, com_hour, upminutes, com_minute, upsecs, com_second);
	ret = 1;
    }
    return ret;
}

static int aspOutApSysInfo(int eid, webs_t wp, int argc, char **argv)
{
    urcp_dev_t* tmp=NULL;
    uint32 ip = 0u, k = 0u;
    char str[64] = "", *open_str = NULL, *close_str = NULL;

    unsigned char *strMac=NULL, *strIp=NULL, stdMac[MAC_LEN];

    websWrite(wp, "var mfNames = new Array();\n");
    websWrite(wp, "var dhcp_en = new Array();\n");
    websWrite(wp, "var flag_5G = new Array();\n");
    /*读取mac*/
    strMac = websGetVar(wp, "macStr", T(""));
//    strIp = websGetVar(wp, "ipStr", T(""));
    /*转换格式*/
//    ip = ip2int(strIp);
    str2mac(strMac, stdMac);
    /*查表*/
    tmp = urcp_dev_table_lookup(stdMac, 0u);
    if((tmp!=NULL) && (tmp->aged != 2))
    {
	int2ip(tmp->ip, str);
	websWrite(wp, "var ipStr = \"%s\";\n", str);         /*IP*/
	websWrite(wp, "var sysSerial = \"%u\";\n", tmp->serial_no);	    /*序列号*/
	websWrite(wp, "var sysHwVersion = \"%s\";\n", tmp->hw_version);	    /*硬件版本*/
	websWrite(wp, "var sysSwVersion = \"%s\";\n", tmp->sw_version);	    /*软件版本*/
	websWrite(wp, "var deviceName = \"%s\";\n", tmp->dev_name);	    /*设备名*/
	websWrite(wp, "var devType = \"%s\";\n", tmp->dev_model);	    /*设备型号*/
	websWrite(wp, "var cpuRate = \"%d%%\";\n", tmp->wl_info.cpu_rate);  /*cpu*/
	websWrite(wp, "var memRate = \"%d%%\";\n",tmp->wl_info.mem_rate);  /*mem*/
	websWrite(wp, "var clients = \"%d\";\n", tmp->wl_info.clients + tmp->wl_info_5G.clients);
	if (tmp->dhcp_en)
	{
#if (UTT_TOMAX == FYES)
	    websWrite(wp, "dhcp_en = \"开启\";\n");
#else
	    open_str = getTransDataForC(C_LANG_INDEX_ENABLED);
	    if (open_str != NULL)
	    {
		websWrite(wp, "dhcp_en = \"%s\";\n", open_str);
	    }
#endif
	}
	else
	{
#if (UTT_TOMAX == FYES)
	    websWrite(wp, "dhcp_en = \"关闭\";\n");
#else
	    open_str = getTransDataForC(C_LANG_INDEX_DISABLED);
	    if (open_str != NULL)
	    {
		websWrite(wp, "dhcp_en = \"%s\";\n", open_str);
	    }
#endif
	}
	memset(str, 0, sizeof(str));
	time2str(tmp->wl_info.up_time, str, sizeof(str));
	websWrite(wp, "var upTime = \"%s\";\n", str);		    /*运行时间*/

	websWrite(wp, "mfNames = \"");
	/*mac filter name*/
	for(k=0u; k<tmp->wl_info.ap_mac_filter_num; k++)
	{
	    if(k!=0)
	    {
		websWrite(wp, ",");
	    }
	    websWrite(wp, "%s", tmp->wl_info.mfNames[k]);
	}
	websWrite(wp, "\";\n");

	if (((tmp->dev_type)&DEV_TYPE_WIRELESS_5G)!=0)
	{
	    websWrite(wp, "flag_5G = \"%d\";\n", 1);
	}
	else
	{	
	    websWrite(wp, "flag_5G = \"%d\";\n", 0);
	}
    }
    else
    {
	websWrite(wp, "var ipStr = \"\";\n");         /*IP*/
    	websWrite(wp, "var sysSerial = \"\";\n");	    
	websWrite(wp, "var sysHwVersion = \"\";\n");
	websWrite(wp, "var sysSwVersion = \"\";\n");
	websWrite(wp, "var deviceName = \"\";\n"); 
	websWrite(wp, "var devType = \"\";\n");
	websWrite(wp, "var cpuRate = \"\";\n"); 
	websWrite(wp, "var memRate = \"\";\n"); 
	websWrite(wp, "var upTime = \"\";\n");
	websWrite(wp, "var clients = \"\";\n");
	websWrite(wp, "var dhcp_en = \"\";\n");
    	if ((tmp!=NULL) && (((tmp->dev_type)&DEV_TYPE_WIRELESS_5G)!=0) ) {
	    websWrite(wp, "flag_5G = \"%d\";\n", 1);
	}
	else {
	    websWrite(wp, "flag_5G = \"%d\";\n", 0);
	}	 
    }

    return 0;	
}

static void data2str(uint32 packets, uint32 bytes, char* str)
{
 
    if (packets <= DATA_1K)
    {
	sprintf(str, "%d",packets);
    }
    else if ( (packets > DATA_1K) && (packets <= DATA_1M))
    {
	sprintf(str,"%.3fK", packets / DATA_1K);
    }
    else if ( (packets > DATA_1M) && (packets <= DATA_1G) )
    {
	sprintf(str,"%.3fM", packets / DATA_1M);
    }
    else
    {
	sprintf(str, "%.3fG", packets / DATA_1G);
    }
    strcat(str, "/");
    str += strlen(str);
    if (bytes <= DATA_1K)
    {
	sprintf(str, "%dB", bytes);
    }
    else if ( (bytes > DATA_1K) && (bytes <= DATA_1M))
    {
	sprintf(str,"%.3fK", bytes / DATA_1K);
    }
    else if ( (bytes > DATA_1M) && (bytes <= DATA_1G) )
    {
	sprintf(str,"%.3fM", bytes / DATA_1M);
    }
    else
    {
	sprintf(str, "%.3fG", bytes / DATA_1G);
    }

    return ;
}

static int aspOutApRfInfo(int eid, webs_t wp, int argc, char **argv)
{
    urcp_dev_t* tmp=NULL;
    uint32 ip = 0u, k = 0u;
    char str[64] = "";
    int i = 0, val = 0;
    unsigned char *strMac=NULL, *strIp=NULL, stdMac[MAC_LEN];
    
//    websWrite(wp, "var radio_on = new Array();\n");
    websWrite(wp, "var ssid = new Array();\n");
    websWrite(wp, "var Bssid = new Array();\n");
    websWrite(wp, "var userNum = new Array();\n");
    websWrite(wp, "var flag_5G = new Array();\n");
    /*读取mac*/
    strMac = websGetVar(wp, "macStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    /*转换格式*/
    ip = ip2int(strIp);
    str2mac(strMac, stdMac);
       /*查表*/
    tmp = urcp_dev_table_lookup(stdMac, ip);
    
    if((tmp != NULL) && (tmp->aged != 2))
    {
	if(tmp->wl_info.ap_flag == 0xFFFFFFFFu)
	{
	    websWrite(wp, "var radio_on = \"已关闭\";\n");
	    goto RADIO_OFF;
	}
	else
	{
	    websWrite(wp, "var radio_on = \"已开启\";\n");
	}
	int2ip(tmp->ip, str);
	websWrite(wp, "var ipStr = \"%s\";\n", str);         /*IP*/
	websWrite(wp, "var bitRate = \"%d Mbps\";\n", tmp->wl_info.bit_rate);
	websWrite(wp, "var power = \"%d%%\";\n", tmp->wl_info.power_new);
	websWrite(wp, "var beacon = \"%d ms\";\n", tmp->wl_info.beacon);
	websWrite(wp, "var wlMode = \"%s\";\n", wlModeToStr(tmp->wl_info.ap_flag));
	websWrite(wp, "userNum = \"%d\";\n", tmp->wl_info.clients);
	if (0 == tmp->dbm)
	{
	    websWrite(wp, "var dbm = \"-%d dBm\";\n", 88);
	}
	else
	{
	    websWrite(wp, "var dbm = \"-%d dBm\";\n", tmp->dbm);
	}
	if (tmp->wl_info.chan_width == 1)
	{
	    websWrite(wp, "var chanWidth = \"%s\";\n", "20M/40M");
	}
	else
	{
	    websWrite(wp, "var chanWidth = \"%s\";\n", "20M");
	}
	if ((0 == tmp->wl_info.channel_head) && (tmp->wl_info.channel_end != 0))
	{
	    websWrite(wp, "var channel = \"自动(%d)\";\n", tmp->wl_info.channel_end);
	}
	else if (tmp->wl_info.channel_head != 0)
	{
	    websWrite(wp, "var channel = \"%d\";\n", tmp->wl_info.channel_head);
	}
	else
	{
	    websWrite(wp, "var channel = \"自动\";\n");
	}

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
	websWrite(wp, "ssid = \"");
	for(i = 0; i < val; i++)
	{
	    if(i != 0)
	    {
		websWrite(wp, ",");
	    }
	    websWrite(wp, "%s", tmp->wl_info.ssid_info[i].ssid);
	}
	websWrite(wp, "\";\n");
    	websWrite(wp, "Bssid = \"");
	for(i = 0; i < val; i++)
	{
	    if(i != 0)
	    {
		websWrite(wp, ",");
	    }
	    mac2str(tmp->wl_info.ssid_info[i].mac, str);
	    websWrite(wp, "%s", str);
	}
	websWrite(wp, "\";\n");

	memset(str, 0, sizeof(str));
	data2str(tmp->wl_info.tx_packets, tmp->wl_info.tx_bytes, str);
	websWrite(wp, "var TX = \"%s\";\n", str);

	memset(str, 0, sizeof(str));
	data2str(tmp->wl_info.rx_packets, tmp->wl_info.rx_bytes, str);
	websWrite(wp, "var RX = \"%s\";\n", str);

	if (((tmp->dev_type)&DEV_TYPE_WIRELESS_5G)!=0)
	{
	    websWrite(wp, "flag_5G = \"%d\";\n", 1);
	}
	else
	{	
	    websWrite(wp, "flag_5G = \"%d\";\n", 0);
	}
    }
    else
    {
RADIO_OFF:
	websWrite(wp, "var ipStr = \"\";\n");         /*IP*/
    	websWrite(wp, "var bitRate = \"\";\n");
	websWrite(wp, "var chanWidth = \"\";\n");
	websWrite(wp, "var power = \"\";\n");
	websWrite(wp, "var beacon = \"\";\n");
	websWrite(wp, "var wlMode = \"\";\n");
	websWrite(wp, "var dbm = \"\";\n");
	websWrite(wp, "var channel = \"\";\n");
	websWrite(wp, "var TX = \"\";\n");
	websWrite(wp, "var RX = \"\";\n");
	if( (tmp != NULL) && (((tmp->dev_type) & DEV_TYPE_WIRELESS_5G) != 0) ) {
		websWrite(wp, "flag_5G = \"1\";\n");
	}
	else{
	    websWrite(wp, "flag_5G = \"0\";\n");
	}
    }

    return 0;	
}

static int aspOutApRf5GInfo(int eid, webs_t wp, int argc, char **argv)
{
    urcp_dev_t* tmp=NULL;
    uint32 ip = 0u, k = 0u;
    char str[64] = "";
    int i = 0, val = 0;
    unsigned char *strMac=NULL, *strIp=NULL, stdMac[MAC_LEN];
    const char *ErrMsg = NULL;
    
    websWrite(wp, "var ssid = new Array();\n");
    websWrite(wp, "var Bssid = new Array();\n");
    websWrite(wp, "var userNum = new Array();\n");
    /*读取mac*/
    strMac = websGetVar(wp, "macStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    /*转换格式*/
    ip = ip2int(strIp);
    str2mac(strMac, stdMac);
       /*查表*/
    tmp = urcp_dev_table_lookup(stdMac, ip);
    if((tmp != NULL) && (tmp->aged != 2) && ( ((tmp->dev_type)&DEV_TYPE_WIRELESS_5G) != 0))
    {
	if(0xFFFFFFFFu == tmp->wl_info_5G.ap_flag)
	{
	    websWrite(wp, "var radio_on = \"已关闭\";\n");
	    goto RADIO_OFF;
	}
	else
	{
	    websWrite(wp, "var radio_on = \"已开启\";\n");
	}
	int2ip(tmp->ip, str);
	websWrite(wp, "var ipStr = \"%s\";\n", str);         /*IP*/
	websWrite(wp, "var bitRate = \"%d Mbps\";\n", tmp->wl_info_5G.bit_rate);
	websWrite(wp, "var power = \"%d%%\";\n", tmp->wl_info_5G.power_new);
	websWrite(wp, "var beacon = \"%d ms\";\n", tmp->wl_info_5G.beacon);
	websWrite(wp, "var wlMode = \"%s\";\n", wlModeToStr(tmp->wl_info_5G.ap_flag));
	if(tmp->wl_info_5G.ap_flag & AP_MODE_11A) {/*ap读的速率不准，暂时这么处理*/
	    websWrite(wp, "var bitRate = \"54 Mbps\";\n");
	}
	else {
	    websWrite(wp, "var bitRate = \"%d Mbps\";\n", tmp->wl_info_5G.bit_rate);
	}
	websWrite(wp, "userNum = \"%d\";\n", tmp->wl_info_5G.clients);
	if (0 == tmp->dbm_5g) {
	    websWrite(wp, "var dbm = \"-%d dBm\";\n", 88);
	}
	else {
	    websWrite(wp, "var dbm = \"-%d dBm\";\n", tmp->dbm_5g);
	}
	if (tmp->wl_info_5G.chan_width == 1)
	{
	    websWrite(wp, "var chanWidth = \"%s\";\n", "20M/40M");
	}
	else if(tmp->wl_info_5G.chan_width == 2)
	{
	    websWrite(wp, "var chanWidth = \"80M\";\n");
	}
	else {
	    websWrite(wp, "var chanWidth = \"20M\";\n");
	}
	if ((0 == tmp->wl_info_5G.channel_head) && (tmp->wl_info_5G.channel_end != 0))
	{
	    websWrite(wp, "var channel = \"自动(%d)\";\n", tmp->wl_info_5G.channel_end);
	}
	else if (tmp->wl_info_5G.channel_head != 0)
	{
	    websWrite(wp, "var channel = \"%d\";\n", tmp->wl_info_5G.channel_head);
	}
	else
	{
	    websWrite(wp, "var channel = \"自动\";\n");
	}

	val = tmp->dev_type;
	if(val) 
	{
	    val = tmp->wl_info_5G.joined_sz_num;
	    if(val>MAX_SSID_NUM)
	    {
		 val = MAX_SSID_NUM;
	    }
	}
	else
	{
	    val = 1;
	}
	websWrite(wp, "ssid = \"");
	for(i = 0; i < val; i++)
	{
	    if(i != 0)
	    {
		websWrite(wp, ",");
	    }
	    websWrite(wp, "%s", tmp->wl_info_5G.ssid_info[i].ssid);
	}
	websWrite(wp, "\";\n");
    	websWrite(wp, "Bssid = \"");
	for(i = 0; i < val; i++)
	{
	    if(i != 0)
	    {
		websWrite(wp, ",");
	    }
	    mac2str(tmp->wl_info_5G.ssid_info[i].mac, str);
	    websWrite(wp, "%s", str);
	}
	websWrite(wp, "\";\n");

	memset(str, 0, sizeof(str));
	data2str(tmp->wl_info_5G.tx_packets, tmp->wl_info_5G.tx_bytes, str);
	websWrite(wp, "var TX = \"%s\";\n", str);

	memset(str, 0, sizeof(str));
	data2str(tmp->wl_info_5G.rx_packets, tmp->wl_info_5G.rx_bytes, str);
	websWrite(wp, "var RX = \"%s\";\n", str);
    }
    else
    {
RADIO_OFF:
	websWrite(wp, "var ipStr = \"\";\n");         /*IP*/
    	websWrite(wp, "var bitRate = \"\";\n");
	websWrite(wp, "var chanWidth = \"\";\n");
	websWrite(wp, "var power = \"\";\n");
	websWrite(wp, "var beacon = \"\";\n");
	websWrite(wp, "var wlMode = \"\";\n");
	websWrite(wp, "var dbm = \"\";\n");
	websWrite(wp, "var channel = \"\";\n");
	websWrite(wp, "var TX = \"\";\n");
	websWrite(wp, "var RX = \"\";\n");
//	ErrMsg = getTransDataForC(C_LANG_INDEX_AP_WIRELESS_NOT_SUPPORT_5G);
//	setTheErrorMsgOnFree(ErrMsg);	
    }
    getTheErrorMsg(eid, wp, argc, argv);    
    return 0;	
}

static int aspOutApUserInfo(int eid, webs_t wp, int argc, char **argv)
{
    char  mac[13], pmac[18];
    urcp_shm_t *mem_shadow = NULL;
    uint32 clts_num = 0;
    uint16 i, tb_index;
    char *arp_bind_name = NULL;
    char *ip_tmp = NULL;
    sta_entry_t* tmp=NULL;
    urcp_dev_t *pAp=NULL;
    strTime_t *time=NULL;
    uchar *strMac=NULL,stdMac[MAC_LEN];
    websWrite(wp, "var flag_5G = new Array();\n");
    /*读取mac*/
    strMac = websGetVar(wp, "macStr", T(""));
    /*转换格式*/
    str2mac(strMac, stdMac);


    websWrite(wp, "var iparrays = new Array();\n");
    websWrite(wp, "var macarrays = new Array();\n");
    websWrite(wp, "var linkedSSIDs = new Array();\n");
    websWrite(wp, "var wlanFre = new Array();\n");
    websWrite(wp, "var signals = new Array();\n");
    websWrite(wp, "var rates = new Array();\n");
    websWrite(wp, "var flag_5G = new Array();\n");

    pAp = urcp_dev_table_lookup(stdMac, 0u);
    if ((pAp != NULL) && (pAp->aged != 2) )
    {
    	if (((pAp->dev_type)&DEV_TYPE_WIRELESS_5G)!=0) {
	    websWrite(wp, "flag_5G = \"%d\";\n", 1);
	}
	else {
	    websWrite(wp, "flag_5G = \"%d\";\n", 0);
	}	 
	mem_shadow = get_shm_ptr();
	if(mem_shadow!=NULL)
	{
	    for(i = 0u; i < STA_TB_HASH_SIZE; i++)
	    {
		tb_index = mem_shadow->sta_tb_hash[i].idx;
		while(tb_index!=NONE_NODE)
		{
		    tmp = &(mem_shadow->sta_tb[tb_index-1]);
		    if((tmp->hdr.aged) <= 1)
		    {
			pAp = urcp_dev_table_lookup(tmp->ap_mac, 0);
			if((pAp != NULL) && (pAp->aged != 2) && (!memcmp(pAp->mac, stdMac, MAC_LEN)))
			{
			    memset(mac, 0, sizeof(mac));
			    sprintf(mac, "%02X%02X%02X%02X%02X%02X",tmp->hdr.mac_addr[0],tmp->hdr.mac_addr[1],
				    tmp->hdr.mac_addr[2],tmp->hdr.mac_addr[3],tmp->hdr.mac_addr[4],tmp->hdr.mac_addr[5]);
			    mac_trans(mac,pmac);
			    ip_tmp = aspOutAuthIP(wp, mac, clts_num);

			    websWrite(wp, "iparrays[%d] = \"%s\";\n", clts_num, ip_tmp);
			    websWrite(wp, "macarrays[%d] = \"%s\";\n", clts_num, pmac);
			    
			    if(tmp->flag & RF_FLAG_5G)
			    {
				websWrite(wp, "wlanFre[%d] = \"5GHz\";\n",clts_num);
				if(tmp->ssid_idx < UTT_SSID_NUM) 
				{
				    websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", clts_num, pAp->wl_info_5G.ssid_info[(int32)(tmp->ssid_idx)].ssid);
				}
				else 
				{
				    websWrite(wp, "linkedSSIDs[%d] = \"\";\n", clts_num);
				}
			    }
			    else
			    {
				websWrite(wp, "wlanFre[%d] = \"2.4GHz\";\n",clts_num);
				if(tmp->ssid_idx < UTT_SSID_NUM) 
				{
				    websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", clts_num, pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid);
				}
				else 
				{
				    websWrite(wp, "linkedSSIDs[%d] = \"\";\n", clts_num);
				}
			    }
			    websWrite(wp, "signals [%d] = \"%ddBm\";\n", clts_num, tmp->rssi);
			    websWrite(wp, "rates[%d] = \"%dM\";\n", clts_num, (int)(tmp->rate));

			    clts_num++;
			}
		    }
		    tb_index = tmp->hdr.tb_next;
		}
	    }
	}
    }
    else {
	websWrite(wp, "flag_5G = \"%d\";\n", 0);
    }
    
    websWrite(wp, "var totalrecs = \"%d\";\n", clts_num);
    return clts_num;
}


extern void formDefineApSysInfo(void)
{
    websAspDefine(T("aspOutApSysInfo"), aspOutApSysInfo);
    websAspDefine(T("aspOutApRfInfo"), aspOutApRfInfo);
    websAspDefine(T("aspOutApRf5GInfo"), aspOutApRf5GInfo);
    websAspDefine(T("aspOutApUserInfo"),aspOutApUserInfo);
}
#endif

#endif

