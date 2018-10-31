#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <uttMachine.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if (FEATURE_AC == FYES)
#include    <urcp_ipc.h>
#include    <switch.h>
#include    <sw_cfg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include    "mibUrcp.h"
#include    "webmsg.h"

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#define DBCKS_CLT 0
#if DBCKS_CLT
#define DBCK(fmt, args...) \
			{ \
				printf("%s, %d, %s(), "fmt, __FILE__, __LINE__, __func__, ##args); \
				printf("\n"); \
			}
#else
#define DBCK(fmt, args...)
#endif

#define FILE_NAME "/tmp/urcp/ap_client_info"
extern const char *wlModeToStr(unsigned int mode);
extern int converMac12To6Byte(char *macStr, MacAddr* macAddr);
extern char* getCurrentUserMacFromAc(webs_t wp, char *macAddr);
extern char* getNameByIPMac(char *ip, char *mac);
static int arp_table_lookup_ipaddr( char mac[], char* ip_addr, int ip_addr_size);
static int getApInfo(int eid, webs_t wp, int argc, char **argv);
static int getSsidInfo(int eid, webs_t wp, int argc, char **argv);
static int getHistroyInfo(int eid, webs_t wp, int argc, char **argv);
#if (HISTORY_FLOW_INFO == FYES)
static int getHistoryFlowInfo(int eid, webs_t wp, int argc, char **argv);
#endif
#if (AC_TOP_DIS == FYES)
static void formStatForTopDis(webs_t wp, char_t *path, char_t *query);
#endif
static void formExportCltInfo(webs_t wp, char_t * path, char_t * query);
#if (HISTORY_FLOW_INFO == FYES)
static void formExportCltFlowInfo(webs_t wp, char_t * path, char_t * query);
#endif
static int onoff_flag = 0;
static int date_flag = 0;
#if (URCP_DISCONNECT_STA==FYES)
static void formDisconnect(webs_t wp, char_t *path, char_t *query)
{
    int len, i;
    unsigned short ct=0u;
    char buffer[1024], *sta = NULL, *tmpMac = NULL, *ptr = NULL;
    urcp_sta_msg_t  *msg;

    sta = websGetVar(wp, "filterstr", T(""));
    memset(buffer, 0, sizeof(buffer));
    msg = (urcp_sta_msg_t*)buffer; 
    len = sizeof(msg->staNum);
    /*避免溢出*/
    i = sizeof(buffer) - MAC_LEN;
    tmpMac = strtok_r(sta, UTT_SPLIT_SIGN_STR, &ptr);
    while ((tmpMac != NULL) && (len<i)) {
	/*转换并复制MAC地址*/
	if(str2mac(tmpMac, msg->staMac[ct])==0) {
	    len += MAC_LEN;
	    ct++;
	}
	tmpMac = strtok_r((char *)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }
    /*send msg to urcp*/
    if(ct != 0)
    {
	msg->staNum=ct;
	sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_DISCONNECT_STA, buffer, len);
    }
    websRedirect(wp, "wlanCltList.asp");
}
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
static char* getSzBySSID(char* ssid)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    ServiceZoneProfile *prof;
    char *sz=NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(index=min; index<max; index++)
    {
	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE) && (prof->serviceEn == PROF_ENABLE)) {
	    if(strncmp(prof->ssid, ssid,UTT_SSID_LENTH) == 0)
	    {
                sz = prof->head.name;
	    }
	}
    }

    return sz;
}

char* getSSIDBySz(char* sz)
{
    int min, max, index;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    ServiceZoneProfile *prof;
    char *ssid=NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(index=min; index<max; index++)
    {
	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0) && (prof->head.active == TRUE) && (prof->serviceEn == PROF_ENABLE)) 
	{
	    if(strncmp(prof->head.name, sz, sizeof(prof->head.name)) == 0)
	    {
                ssid = prof->ssid;
	    }
	}
    }

    return ssid;
}

void change_ssid(char *ssid)
{
#if(SSID_ENCODE_TYPE == FYES)
    int min, max, index;
    MibProfileType mibType = MIB_PROF_SERVICE_ZONE;
    ServiceZoneProfile *prof;
    char *ssid_change = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(index=min; index<max; index++)
    {
	prof = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (prof->head.active == TRUE) && (strncmp(ssid, prof->ssid, sizeof(prof->ssid)) == 0)) 
	{
	    if(prof->ssid_encode_type == 1)	    //手机优先
	    {
		ssid_change = iconv_string(ssid,"GBK","UTF-8");
		strncpy(ssid, ssid_change, sizeof(prof->ssid));
		break;
	    }
	}
    }
#endif
}

static void mac2str(uchar mac[], char *str)
{
    if (str != NULL)
    {
	sprintf(str, "%02X%02X%02X%02X%02X%02X",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    return;
}

static int checkout_mac(unsigned char rel_mac[MAC_LEN])
{
    urcp_shm_t *mem_shadow = NULL;
    uint16 i, tb_index;
    sta_entry_t* tmp=NULL;
    char name[16] = "";

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
		    if (memcmp(tmp->hdr.mac_addr, rel_mac, MAC_LEN ) == 0)
		    {
			mac2str(rel_mac, name);
			tmp->hdr.aged = 2;
		    }
		}
		tb_index = tmp->hdr.tb_next;
	    }
	}
    }

    return 0;
}

static int black_info_cmp(char *sz, unsigned char *mac)
{
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    ApBlackWhiteListProfile *prof=NULL;
    int min = 0, max = 0, i = 0, j = 0;
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++)
    {
	prof = (ApBlackWhiteListProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((prof != NULL) && (prof->head.active == TRUE) && (prof->num != 0) && (strncmp(prof->sz, sz, sizeof(prof->sz)) == 0))
	{
	    for (j = 0; j < MAX_STA_PER_BLACKLIST; j++)
	    {
		if (memcmp(mac, &(prof->mac[j]), MAC_LEN) == 0 )
		{
//		    printf("%s,检测重复用户\n", __func__);
		    return 1;
		}
	    }
	}

    }
    return 0;
}


static int formAddToBlackList(webs_t wp, char_t *path, char_t *query)
{
    int len, i, max, min, index, j = 0, cmp_flag = 0;
    unsigned short ct=0u;
    char buffer[1500], mac[18], ssid[UTT_SSID_LENTH+1], *list = NULL, *tmp = NULL, *ptr = NULL;
    urcp_black_list_msg_t  *msg;
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    ApBlackWhiteListProfile *prof=NULL, *Blacklist=NULL;
    char str_mac[18] = "";
    char *sz=NULL;
    unsigned char rel_mac[MAC_LEN] = "";
    unsigned char mac_null[MAC_LEN] = "";
    int black_flag = 0;
    int black_repeat_user = 0;
   
#if 1
    list = websGetVar(wp, "filterstr", T(""));
    strcat(list, ",");
//    printf("%s,list:%s\n", __func__, list);
    memset(buffer, 0, sizeof(buffer));
    msg = (urcp_black_list_msg_t*)buffer; 
    len = sizeof(msg->staNum) + sizeof(msg->mode);
    /*避免溢出*/
    i = sizeof(buffer) - sizeof(urcp_black_list_t);
 //   printf("web str: %s\n", list);
    tmp = strtok_r(list, UTT_SPLIT_SIGN_STR, &ptr);
    while ((tmp != NULL) && (len<i)) 
    {
	memset(mac, 0, sizeof(mac));
	memset(ssid, 0, sizeof(ssid));
	if(sscanf(tmp, "%[^:]:%[^,]", mac, ssid)==2)
	{
	    sz = getSzBySSID(ssid);
	    cmp_flag = 0;
//	    printf("%s, %s, sz:%s\n", mac, ssid, sz);
	    if((sz!=NULL) && (str2mac(mac, rel_mac)==0) && ((cmp_flag = black_info_cmp(sz, rel_mac)) == 0)) 
	    {
		memcpy(msg->black_list[ct].staMac, rel_mac, sizeof(rel_mac));
		memset(str_mac, 0, sizeof(str_mac));
		mac2str(msg->black_list[ct].staMac, str_mac);
//		printf("now,mac:%s\n",str_mac);
		change_ssid(ssid);
		strncpy(msg->black_list[ct].ssid, ssid, sizeof(msg->black_list[0].ssid));
		len += sizeof(urcp_black_list_t);
		Blacklist = NULL;
		ct++;

		/*转换并复制MAC地址*/
		ProfInstLowHigh(mibType, &max, &min);
		/*搜索黑名单*/
		for(index = min; index < max; index++)
		{
		    prof = (ApBlackWhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
//		    printf("index: %d, type: %d, min %d, max %d, name: %s, sz: %s\n", index, mibType, min, max, prof->head.name, prof->sz);
		    if ((prof != NULL) && (ProfInstIsFree((Pointer)prof) == 0))
		    {
			if(prof->head.active == TRUE)
			{
//			    printf("find sz: %s, name: %s\n",prof->sz, prof->head.name);
			    if (prof->num == MAX_STA_PER_BLACKLIST)
			    {
				continue;
			    }
			    if (strncmp(prof->sz, sz, UTT_INST_NAME_LEN)==0)
			    {
//				printf("just it: %d\n", prof -> num);
				for(j = 0; j < MAX_STA_PER_BLACKLIST; j++)
				{
				    if(memcmp( &(prof->mac[j]), mac_null, MAC_LEN ) == 0)
				    {
					/*ssid已经保存过了*/
					memcpy(&(prof->mac[prof->num]), rel_mac, MAC_LEN);
					prof -> num += 1;
					break;
				    }
				}
				if (j != MAX_STA_PER_BLACKLIST)
				{
				    break;
				}
			    }
			}
			else 
			{
			    /*记录可用的空实例*/
			    Blacklist = prof;
//			    printf("free: %d\n", index);
			}
		    }
		}

		if(index>=max)
		{
		    if(Blacklist != NULL)
		    {
			/*ssid没有保存过，建立新黑名单实例*/
			Blacklist -> head.active = TRUE;
			Blacklist -> mode = AP_BLACK_LIST;
			Blacklist -> num = 1;
			memcpy(&(Blacklist->mac[0]), rel_mac, MAC_LEN);
			memcpy(Blacklist->sz, sz, UTT_INST_NAME_LEN);
//			printf("user new list\n");
		    }
		    else
		    {
//			printf("%s: have no more room!\n", __func__);
			len -= sizeof(urcp_black_list_t);
			ct--;
		    }
		}
	    }
	    else if (sz == NULL)
	    {
		black_flag = 1;
	    }
	    else if (cmp_flag)
	    {
		black_repeat_user = 1;
	    }
	}
	tmp = strtok_r((char *)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }
    /*send msg to urcp*/
    if(ct != 0)
    {
	msg->staNum=ct;
	msg->mode = 1;	    //表示添加
	sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_ADD_BLACK_LIST, buffer, len);
	nvramWriteCommit();
//	printf("send msg: %d\n", len);
    }

    if (black_flag && black_repeat_user)
    {
	setTheErrorMsg("黑名单功能只对加入服务区的AP生效并且不能重复加入黑名单");
    }
    else if (black_repeat_user)
    {
	setTheErrorMsg("不能重复加入黑名单");
    }
    else if (black_flag)
    {
	setTheErrorMsg("黑名单功能只对加入服务区的AP生效");
    }

#endif
    websRedirect(wp, "wlanCltList.asp");
}

/*显示黑名单*/
static void aspOutShowBlackList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    ApBlackWhiteListProfile *prof=NULL;
    int total = 0, max = 0, min = 0, index = 0, i = 0;
    char *ssid = NULL, mac[16] = "";
    uchar mac_null[MAC_LEN] = "";

    websWrite(wp, "var macarrays = new Array();\n");
    websWrite(wp, "var linkedSSIDs = new Array();\n");
    websWrite(wp, "var totalrecs = new Array();\n"); 
    websWrite(wp, "var max_totalrecs = new Array();\n");
    ProfInstLowHigh(mibType, &max, &min);
    websWrite(wp, "max_totalrecs = \"%d\";\n", (max - min)*MAX_STA_PER_BLACKLIST);
    for(index = min; index < max; index++)
    {
	prof = (ApBlackWhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (prof->head.active == TRUE) && (prof->num != 0))
	{
	    ssid = getSSIDBySz(prof->sz);
	    for (i = 0; i < MAX_STA_PER_BLACKLIST; i++)
	    {
		if (memcmp(&(prof->mac[i]), mac_null, MAC_LEN) == 0)
		{
		    continue;
		}
		mac2str(&(prof->mac[i]), mac);
		websWrite(wp, "macarrays[%d] = \"%s\";\n", total, mac);
		websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", total, ssid);
		total++;
	    }
	}
    }
    websWrite(wp, "totalrecs = \"%d\";\n", total);

    return;
}

/*根据MAC查找所在的黑名单实例，并删除*/
static int del_one_black_list(char *sz, char *str_mac)
{
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    ApBlackWhiteListProfile *prof=NULL;
    uchar mac[MAC_LEN] = "";
    int min = 0, max = 0,index = 0, i = 0;
    if (mac == NULL) 
    {
	return NULL;
    }
    else
    {
	str2mac(str_mac, mac);
    }
    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (ApBlackWhiteListProfile *)ProfGetInstPointByIndex(mibType, index);
	if ((prof != NULL) && (prof->head.active == TRUE) && (prof->num != 0) && (strncmp(sz, prof->sz, UTT_INST_NAME_LEN) == 0))
	{
	    for (i = 0; i < MAX_STA_PER_BLACKLIST; i++)
	    {
		if (memcmp(mac, &(prof->mac[i]), MAC_LEN) == 0 )
		{
		    memset(&(prof->mac[i]), 0, MAC_LEN);
		    prof->num -= 1;
		    if (0 == prof->num)
		    {
			prof->head.active  = FALSE;
		    }
//		    printf("%s,已删除:%s\n", __func__, str_mac);
		    return 1;
		}
	    }
	}
    }
    return 0;
}

#if 1
static void formDelBlackList(webs_t wp, char_t *path, char_t *query)
{
    int len = 0, i = 0, num = 0;
    char buffer[1500], mac[18], ssid[UTT_SSID_LENTH+1], *list = NULL, *tmp = NULL, *ptr = NULL;
    urcp_black_list_msg_t  *msg;
    char *sz=NULL;

    list = websGetVar(wp, "delstr", T(""));
    strcat(list, ",");
//    printf("%s,list:%s\n", __func__, list);
    memset(buffer, 0, sizeof(buffer));
    msg = (urcp_black_list_msg_t*)buffer; 
    len = sizeof(msg->staNum) + sizeof(msg->mode);
    /*避免溢出*/
    i = sizeof(buffer) - sizeof(urcp_black_list_t);
    tmp = strtok_r(list, UTT_SPLIT_SIGN_STR, &ptr);
    while ((tmp != NULL) && (len<i)) 
    {
	memset(mac, 0, sizeof(mac));
	memset(ssid, 0, sizeof(ssid));
	if(sscanf(tmp, "%[^:]:%[^,]", mac, ssid)==2)
	{
//	    printf("%s,mac:%s,ssid:%s\n", __func__, mac, ssid);
	    sz = getSzBySSID(ssid);
	    if ( (sz != NULL) && (del_one_black_list(sz, mac) != 0))
	    {
		change_ssid(ssid);
		strncpy(msg->black_list[num].ssid, ssid, UTT_SSID_LENTH);	
		str2mac(mac, msg->black_list[num].staMac);
		num++;
		len += sizeof(urcp_black_list_t);
	    }
	}

	tmp = strtok_r((char *)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }
    
    if (num != 0)
    {
	msg->mode = 2;
	msg->staNum = num;
	sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_ADD_BLACK_LIST, buffer, len);
//	printf("%s,num:%d,len:%d\n",__func__, num, len);
	nvramWriteCommit();
    }
//    else
//    {
//	printf("%s,未找到要删除的用户\n", __func__);
//    }
    websRedirect(wp, "wlanCltMacFilter.asp");
    return ;
}
#endif
#if 0
/*现在函数获取的数据有问题*/
static void formDelBlackList(webs_t wp, char_t *path, char_t *query)
{
    char *buf = NULL;
    char mac[MAC_LEN*2 + 1] = "";
    char buffer[1500] = "";
    int num = 0, len = 0;;
    urcp_black_list_msg_t  *msg;
    buf = websGetVar(wp, "delstr", T(""));
    printf("%s, %s\n", __func__, buf);
    msg = (urcp_black_list_msg_t *)buffer;
    msg->mode = 2;
    char *sz = NULL;
    char *ssid = NULL;
    char ssid_change[UTT_SSID_LENTH + 1] = "";
    len = sizeof(msg->mode) + sizeof(msg->staNum);
    if (buf != NULL)
    {
	num = 0;
	do
	{
	    memset(mac, 0, sizeof(mac));
	    memcpy(mac, buf, MAC_LEN*2);
	    sz = del_one_black_list(mac);
	    if (sz != NULL)
	    {
		ssid = getSSIDBySz(sz);
		strncpy(ssid_change, ssid, UTT_SSID_LENTH);
		change_ssid(ssid_change);
		if (ssid != NULL)
		{
		    strncpy(msg->black_list[num].ssid, ssid_change, UTT_SSID_LENTH);	
		    str2mac(mac, msg->black_list[num].staMac);
		    num++;
		    len += sizeof(urcp_black_list_t);
		}
	    }
	    buf = strchr(buf, ',');
	    if (buf != NULL)
	    {
		buf++;
		if (*buf == '\0')
		{
		    break;
		}
	    }
	    else
	    {
		break;
	    }
	}while(1);
    }
    msg->staNum = num;
    sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_ADD_BLACK_LIST, buffer, len);
    nvramWriteCommit();
    websRedirect(wp, "wlanCltMacFilter.asp");
    return ;
}
#endif

static void formDelAllBlackList(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType=MIB_PROF_AP_BLACK_WHITE_LIST;
    int min = 0, max = 0,index = 0, len = 0;
    char buffer[sizeof(urcp_black_list_msg_t)] = "";
    urcp_black_list_msg_t  *msg;

    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	ProfDelInstByIndex(mibType, index);
    }
    msg = (urcp_black_list_msg_t *)buffer;
    msg->mode = 3;
    msg->staNum  = 0;
    len = sizeof(msg->mode) + sizeof(msg->staNum);
    sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_ADD_BLACK_LIST, buffer, len);
    nvramWriteCommit();
    websRedirect(wp, "wlanCltMacFilter.asp");
    return;
}
#endif
#if (AC_TOP_DIS == FYES)
/***********************************************************************
 * 函 数 名：   formStatForTopDis
 * 创建日期：	2014-07-20
 * 修改日期：
 * 作者：       hec
 * 附加说明：	无
***********************************************************************/
static void formStatForTopDis(webs_t wp, char_t *path, char_t *query)
{
    int apOnline = 0,apOffline = 0,apOnlineUser = 0, tb_index,i;
    urcp_shm_t *mem_shadow=NULL;
    urcp_dev_t* tmp=NULL;
    mem_shadow = get_shm_ptr();
    char dataTmp[100] = {0},data[100] = {0};
    sprintf(dataTmp, "{\"%s\":\"%%u\",\"%s\":\"%%u\",\"%s\":\"%%u\"}","onlineap","offlineap","onlineuser");
    if(mem_shadow!=NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++) 
	{
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]);
		if(tmp->aged == 2)
		{
		    //离线
		    apOffline++;
		    tb_index = tmp->tb_next;
		    continue;
		}
		else
		{
		    apOnline++;    
		}
		apOnlineUser += tmp->wl_info.clients;
		apOnlineUser += tmp->wl_info_5G.clients;
		tb_index = tmp->tb_next;
	    }
	}
    }
    sprintf(data,dataTmp,apOnline,apOffline,apOnlineUser);
    websWrite(wp, "%s\n",data);
    websDone(wp, 200);
}
#endif
extern void mac_trans(unsigned char* mac, unsigned char* pmac){
	int i,j;
	int n = 18;
	int m = 12;
	memset(pmac,0,n);
	for(i=2;i<n;i+=3){
		pmac[i]=':';	
	}
	for(i=0,j=0;j<m;i++,j++){
		if(pmac[i] != ':'){
			pmac[i]=mac[j];
		}else{
			j--;
		}
	}
	pmac[n-1]=0;
}

extern char *aspOutAuthIP(webs_t wp, char *mac, int num)
{
        static unsigned char ip_a[16];
        char * ip = NULL;
	memset(ip_a,0,sizeof(ip_a));
	arp_table_lookup_ipaddr( mac, ip_a ,sizeof(ip_a) );
	ip = ip_a;
//	printf("%d,ip = %s\n",__LINE__,ip_a);
	return ip;
}
/***********************************************************************
 * 函 数 名：getCltFromOldPlace
 * 参    数：
 * 返 回 值：
 * 创建日期：2014-08-07
 * 修改日期：
 * 作    者：hec
 * 附加说明：该函数让新版本的AC软件"在线用户"页面兼容老版本
***********************************************************************/
static void getCltFromOldPlace(uint32 *clt_num,webs_t wp,char *prevMac)
{
    urcp_dev_t *dev_table = NULL;
    urcp_shm_t *mem_shadow = NULL;
    int tb_index,i,j,k;
    char *ip_tmp = NULL,sta_mac[13],mac[13],apName[50];
    char *arp_bind_name = NULL;
    unsigned char *tmp_sta_mac = NULL;
#if (MAC_COMPANY_NAME == FYES)
	char *company = NULL;
#endif

    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		dev_table = &(mem_shadow->dev_table[tb_index - 1]);
		sprintf(mac,"%02X%02X%02X%02X%02X%02X",dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
		if((strcmp(prevMac,"") != 0) && (strcmp(prevMac,mac) != 0))
		{
		    tb_index = (mem_shadow->dev_table[tb_index - 1]).tb_next;
		    continue;
		}
		for(j = 0;j < dev_table->wl_info.joined_sz_num;j++)
		{
		    for(k = 0;k < dev_table->wl_info.ssid_info[j].sta_num;k++)
		    {
			memset(apName,0,sizeof(apName));
			if(strcmp(dev_table->dev_name,"") != 0)
			{
			    strcpy(apName,dev_table->dev_name);
			}
			else
			{
			    sprintf(apName,"%02X%02X%02X%02X%02X%02X",dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
			}
			memset(sta_mac,0,sizeof(sta_mac));
			tmp_sta_mac = dev_table->wl_info.ssid_info[j].sta_info[k].sta_mac;
			sprintf(sta_mac,"%02X%02X%02X%02X%02X%02X",tmp_sta_mac[0],tmp_sta_mac[1],tmp_sta_mac[2],tmp_sta_mac[3],tmp_sta_mac[4],tmp_sta_mac[5]);
			ip_tmp = aspOutAuthIP(wp, sta_mac, *clt_num);
			websWrite(wp, "iparrays[%d] = \"%s\";\n", *clt_num, ip_tmp);
			websWrite(wp, "macarrays[%d] = \"%s\";\n", *clt_num, sta_mac);
			if((arp_bind_name = getNameByIPMac(ip_tmp, sta_mac)) != NULL)
			{
			    websWrite(wp, "users[%d]=\"%s\";\n", *clt_num, arp_bind_name);
			}
			else
			{
			    websWrite(wp, "users[%d]=\"\";\n", *clt_num);
			}
			websWrite(wp, "linkedaps[%d] = \"%s\";\n", *clt_num,apName);
			websWrite(wp, "wlanmodes[%d] = \"%s\";\n", *clt_num,wlModeToStr(dev_table->wl_info.ap_flag));
			//ssid
			websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", *clt_num, dev_table->wl_info.ssid_info[j].ssid);
			websWrite(wp, "signals [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "rates[%d] = \"-\";\n", *clt_num);
			//频宽
			websWrite(wp, "bindwidths [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "downloads [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "downRate [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "uploads [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "upRate [%d] = \"-\";\n", *clt_num);
			websWrite(wp, "time [%d] = \"-\";", *clt_num);
			websWrite(wp, "clientenables[%d] = 0;\n",*clt_num);
			websWrite(wp, "filterRules[%d] = 0;\n",*clt_num);
			websWrite(wp, "wlanFre[%d] = \"2.4GHz\";\n",*clt_num);
#if (MAC_COMPANY_NAME == FYES)
			    company = getManuByOUI(tmp_sta_mac); 
			    if (company != NULL)
			    {
				websWrite(wp, "companyName[%d] = \"%s\";\n", *clt_num,company);
			    }
			    else
			    { 
				websWrite(wp, "companyName[%d] = \"\";\n", *clt_num);
			    }
			    
			    websWrite(wp, "hostName[%d] = \"\";\n", *clt_num);
#endif
			(*clt_num)++;
		    }
		}
		tb_index = (mem_shadow->dev_table[tb_index - 1]).tb_next;
	    }
	}
    }
    return;
}
#define BW_20 0
#define BW_40 1
#define BW_80 2
#define BW_10 3
static char *phy_bw_5g_str[] = {"20M", "40M", "80M", "10M"};
static char *phy_bw_25g_str[] = {"20M", "40M", "N/A", "10M"};
char* GetBW(int phy, int flag)
{
    int BW;
    HTTRANSMIT_SETTING phy5g;
    MACHTTRANSMIT_SETTING phy24g;

    if((flag & RF_FLAG_5G) != 0) {
	phy5g.word = phy;
	BW = phy5g.field.BW;
	switch(BW)
	{
	    case 0:
		return "20M";
	    case 1:
		return "40M";
	    case 2:
		return "80M";
	    case 3:
		return "20/40M";
	    default:
		return "N/A";
	}

    }
    else {
	phy24g.word = phy;
	BW = phy24g.field.BW;
	switch(BW)
	{
	    case 0:
		return "20M";
	    case 1:
		return "40M";
	    case 2:
		return "20/40M";
	    default:
		return "N/A";
	}
    }

}
#if 1
static uint32 get_clts_info_auto(webs_t wp, char *prevMac)
{
	char  mac[13], checkMac[13];
	urcp_shm_t *mem_shadow = NULL;
	uint32 clts_num = 0;
	uint16 i, tb_index;
	char *arp_bind_name = NULL;
	char *ip_tmp = NULL;
	sta_entry_t* tmp=NULL;
	urcp_dev_t *pAp=NULL;
	strTime_t *time=NULL;
#if (MAC_COMPANY_NAME == FYES)
	char *company = NULL;
#endif
	struct in_addr addr;

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
			memset(checkMac,0,sizeof(checkMac));
			sprintf(checkMac,"%02X%02X%02X%02X%02X%02X",tmp->ap_mac[0],tmp->ap_mac[1],tmp->ap_mac[2],tmp->ap_mac[3],tmp->ap_mac[4],tmp->ap_mac[5]);
			/*"prevMac由跳转发起页面传递过来到本页面，如果prevMac不为空，那么"*/
			/*我们只查询prevMac这个设备，如果为空，那么查询所有的设备*/
			if((strcmp(prevMac,"") != 0) && (strcmp(prevMac,checkMac) != 0))
			{
			    tb_index = tmp->hdr.tb_next;
			    continue;
			}
			 pAp = urcp_dev_table_lookup(tmp->ap_mac, 0);
			 if((pAp != NULL) && (pAp->aged != 2))
			 {
			    memset(mac, 0, sizeof(mac));
			    sprintf(mac, "%02X%02X%02X%02X%02X%02X",tmp->hdr.mac_addr[0],tmp->hdr.mac_addr[1],
				    tmp->hdr.mac_addr[2],tmp->hdr.mac_addr[3],tmp->hdr.mac_addr[4],tmp->hdr.mac_addr[5]);
			    //mac_trans(mac,pmac);
			    ip_tmp = aspOutAuthIP(wp, mac, clts_num);
			    
			    if ((ip_tmp[0] == '\0') && (tmp->ip != 0))
			    {
				addr.s_addr = tmp->ip;
				websWrite(wp, "iparrays[%d] = \"%s\";\n", clts_num, inet_ntoa(addr) );
			    }
			    else
			    {
				websWrite(wp, "iparrays[%d] = \"%s\";\n", clts_num, ip_tmp);
			    }
			    websWrite(wp, "macarrays[%d] = \"%s\";\n", clts_num, mac);
			    if((arp_bind_name = getNameByIPMac(ip_tmp, mac)) != NULL) {
				  websWrite(wp, "users[%d]=\"%s\";\n", clts_num, arp_bind_name);
    //			      printf("%d,%s\n",__LINE__,arp_bind_name);
			    }else{
				  websWrite(wp, "users[%d]=\"\";\n", clts_num);
			    }
			    if(strcmp(pAp->dev_name, "") != 0){
				websWrite(wp, "linkedaps[%d] = \"%s\";\n", clts_num, pAp->dev_name);
			    }
			    else {
				websWrite(wp, "linkedaps[%d] = \"%02X%02X%02X%02X%02X%02X\";\n", clts_num, pAp->mac[0],
				   pAp->mac[1], pAp->mac[2], pAp->mac[3], pAp->mac[4], pAp->mac[5]);
			    }
			    websWrite(wp, "wlanmodes[%d] = \"%s\";\n", clts_num, wlModeToStr(pAp->wl_info.ap_flag));
#if 0
			    if(tmp->ssid_idx < UTT_SSID_NUM) {
				websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", clts_num, pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid);
			    }
			    else {
				websWrite(wp, "linkedSSIDs[%d] = \"\";\n", clts_num);
			    }
#endif
			    websWrite(wp, "signals [%d] = \"%ddBm\";\n", clts_num, tmp->rssi);
			    websWrite(wp, "rates[%d] = \"%dM\";\n", clts_num, (int)(tmp->rate));
			    websWrite(wp, "bindwidths [%d] = \"%s\";\n", clts_num, GetBW(tmp->phy.word, tmp->flag));
			    websWrite(wp, "downloads [%d] = \"%s\";\n", clts_num, convertB2KM(tmp->tx_bytes));
			    websWrite(wp, "downRate [%d] = \"%s\";\n", clts_num, convertB2KM((unsigned long long)(tmp->tx_rate * 8)));

			    websWrite(wp, "uploads [%d] = \"%s\";\n", clts_num, convertB2KM(tmp->rx_bytes));
			    websWrite(wp, "upRate [%d] = \"%s\";\n", clts_num, convertB2KM((unsigned long long)(tmp->rx_rate * 8)));
			    websWrite(wp, "time [%d] = \"", clts_num);

			    if(tmp->con_time > 0)
			    {
				time = convertS2HMS(tmp->con_time);
				if(time->day > 0){
				    websWrite(wp, "%d天", time->day);
				}
				if(time->hour > 0){
				    websWrite(wp, "%d时", time->hour);
				}
				if(time->min > 0){
				    websWrite(wp, "%d分", time->min);
				}
				if(time->sec > 0){
				    websWrite(wp, "%d秒", time->sec);
				}
				websWrite(wp, "\";\n");
			    }
			    else
			    {
				websWrite(wp, "0秒\";\n");
			    }
			    websWrite(wp, "clientenables[%d] = 0;\n",clts_num);
			    websWrite(wp, "filterRules[%d] = 0;\n",clts_num);
			    if(tmp->flag&RF_FLAG_5G)
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
#if (MAC_COMPANY_NAME == FYES)
			    company = getManuByOUI(tmp->hdr.mac_addr); 
			    if (company != NULL)
			    {
				websWrite(wp, "companyName[%d] = \"%s\";\n", clts_num,company);
			    }
			    else
			    { 
				websWrite(wp, "companyName[%d] = \"\";\n", clts_num);
			    }
			    
			    websWrite(wp, "hostName[%d] = \"%s\";\n", clts_num, tmp->host_name);
#endif
			    clts_num++;
			 }
		     }
		     tb_index = tmp->hdr.tb_next;
		}
	    }
	}
	/*读取老版本的AP的客户端数据*/
	getCltFromOldPlace(&clts_num,wp,prevMac);
	return clts_num;
}
#else
static uint32 get_clts_info_auto(webs_t wp)
{
	char  mac[13], pmac[18];
	urcp_shm_t *mem_shadow = NULL;
	urcp_dev_t *tmp = NULL;
	uint32 j,k, i = 0u,tb_index,val,clts_num = 0;
    char *arp_bind_name = NULL;
	char *ip_tmp = NULL,*tmpmac = NULL;

	MibProfileType mibTypeFilter = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *profFilter = NULL;
	int minFilter, maxFilter, indexFilter,t,m,filterFlag=0,filterRule = 0;

	mem_shadow = get_shm_ptr();
	if(mem_shadow!=NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]);

		val = tmp->dev_type;
		if(((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))&&(tmp->aged != 2))
		{
#if 0
{
printf("serial_no: %d\n",tmp->serial_no);
printf("clients: %d\n",tmp->wl_info.clients);
printf("ssid_num: %d\n",tmp->wl_info.ssid_num);
printf("joined_sz_num: %d\n",tmp->wl_info.joined_sz_num);
int l,m;
for(l=0;l<tmp->wl_info.joined_sz_num;l++){
printf("No. %d:\n",l);
printf("    ssid: %s\n",tmp->wl_info.ssid_info[l].ssid);
printf("    szone: %s\n",tmp->wl_info.ssid_info[l].szone);
printf("    mac: %s\n",str_macaddr(tmp->wl_info.ssid_info[l].mac));
printf("    sta_num: %d\n",tmp->wl_info.ssid_info[l].sta_num);
for(m=0;m<tmp->wl_info.ssid_info[l].sta_num;m++){
printf("    %s\n",str_macaddr(tmp->wl_info.ssid_info[l].sta_info[m].sta_mac));
}
}
}
#endif
//      printf("%d,keyipipei\n",__LINE__);
//	printf("%d,%d\n",__LINE__,tmp->wl_info.joined_sz_num);
        for(j=0;j<tmp->wl_info.joined_sz_num;j++){
//	    printf("%d,%d\n",__LINE__,tmp->wl_info.ssid_info[j].sta_num);
            for(k=0;k<tmp->wl_info.ssid_info[j].sta_num;k++){
			memset(mac, '\0', sizeof(mac));
			memset(pmac, '\0', sizeof(pmac));
			sprintf(mac, "%02X%02X%02X%02X%02X%02X", 
                    tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[0],tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[1],
					tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[2],tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[3],
					tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[4],tmp->wl_info.ssid_info[j].sta_info[k].sta_mac[5]);
			mac_trans(mac,pmac);
//			printf("%d,%s\n",__LINE__,mac);
			if(strcmp(mac,"000000000000") != 0){
//			printf("%d,buquanwei0\n",__LINE__);
			ip_tmp = aspOutAuthIP(wp, mac, clts_num);
//			printf("%d,%s\n",__LINE__,ip_tmp);
			if((arp_bind_name = getNameByIPMac(ip_tmp, mac)) != NULL)
			{
			      websWrite(wp, "users[%d]=\"%s\";\n", clts_num, arp_bind_name);
//			      printf("%d,%s\n",__LINE__,arp_bind_name);
			}else{
                              websWrite(wp, "users[%d]=\"\";\n", i);
			}
			websWrite(wp, "linkedaps[%d] = \"%s\";\n", clts_num, tmp->dev_name);
//			printf("%d,%s\n",__LINE__,tmp->dev_name);
			websWrite(wp, "macarrays[%d] = \"%s\";\n", clts_num, pmac);
//			printf("%d,%s\n",__LINE__,pmac);
			websWrite(wp, "wlanmodes[%d] = \"%s\";\n", clts_num, wlModeToStr(tmp->wl_info.ap_flag));
//			printf("%d,%s\n",__LINE__,wlModeToStr(tmp->wl_info.ap_flag));
			websWrite(wp, "linkedSSIDs[%d] = \"%s\";\n", clts_num, tmp->wl_info.ssid_info[j].ssid);
//			printf("%d,%s\n",__LINE__,tmp->wl_info.ssid_info[j].ssid);
//	        websWrite(wp, "clientenables[%d] = 0;\n",clts_num);
			aspOutAuth(wp, mac, clts_num);

            if((tmp->wl_info.ssid_info[j].szone[0] != '\0')&&(strlen(tmp->wl_info.ssid_info[j].szone) > 0))
			{
			filterFlag = 0;//置0每一个客户端都需判断,将标志置0
		    //根据客户端所连接的ssid在服务区遍历该服务区引用的MAC地址组
		    ProfInstLowHigh(mibTypeFilter, &maxFilter, &minFilter);
			for (indexFilter = minFilter; indexFilter < maxFilter; indexFilter++)
			{
			    profFilter = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeFilter, indexFilter);
			    if ((profFilter != NULL) && (ProfInstIsFree((Pointer)profFilter) == 0) && (profFilter->head.active == TRUE))
				{
					for(t = 0; t < MAX_AP_MAC_FILTER_NUM; t++)
					{
						if(strcmp(profFilter->sz[t],tmp->wl_info.ssid_info[j].szone) == 0)//过滤地址组是否引用服务区
						{
						    filterRule = profFilter->mode;
				            for(m = 0; m < profFilter->num; m++)
							{
				                tmpmac = converMac6To12Byte(profFilter->mac[m]);
//								printf("mac=%s,profmac=%s\n",mac,tmpmac);
				                if(strcasecmp(mac,tmpmac) == 0)//客户端MAC地址在MAC地址组中
							    {
				                     filterFlag = 1;//在地址组中找到用户的MAC地址
									 break;
							    }
						    }
							if(filterFlag == 1)
							{
							    break;
							}
					    }
			        }
					if(filterFlag == 1)
					{
					    break;
					}
				 }
			  }
		    if(filterFlag == 1)
			{
		        if(filterRule == 1)//允许规则
		        {
                    websWrite(wp, "clientenables[%d] = 0;\n",clts_num);
                    websWrite(wp, "filterRules[%d] = 1;\n",clts_num);
		        }else if(filterRule == 2){
                    websWrite(wp, "clientenables[%d] = 1;\n",clts_num);
                    websWrite(wp, "filterRules[%d] = 2;\n",clts_num);
			    }
			}else{
		        if(filterRule == 1)//允许规则
		        {
                    websWrite(wp, "clientenables[%d] = 1;\n",clts_num);
                    websWrite(wp, "filterRules[%d] = 1;\n",clts_num);
		        }else if(filterRule == 2){
                    websWrite(wp, "clientenables[%d] = 0;\n",clts_num);
                    websWrite(wp, "filterRules[%d] = 2;\n",clts_num);
			    }
			}

			}else{
			//ap没有下发服务区，已有客户端连接上
                websWrite(wp, "clientenables[%d] = 0;\n",clts_num);
                websWrite(wp, "filterRules[%d] = 0;\n",clts_num);
			}
			clts_num++;
            }
            }
        }

		}

		tb_index = tmp->tb_next;
		}
	}
	}
	return clts_num;
}
#endif

#if 0
static int aspOutCltList(webs_t wp, char *buf, unsigned int *num,char **apName)
{
	int  n = 0;
	char linked[32], wlanmode[10], ssid[32], mac[13], pmac[18], nstr[5];
	char *p = buf;
	char tmp[32];

	memset(tmp, '\0', sizeof(tmp));
	memset(linked, '\0', sizeof(linked));
	memset(wlanmode, '\0', sizeof(wlanmode));
	memset(ssid, '\0', sizeof(ssid));
	memset(mac, '\0', sizeof(mac));

	if (*p == ',') {
	// AP没有用户名
		p += 1;
		sscanf(p, "%[^,],%[^,],%d", ssid, wlanmode, &n);
		sprintf(nstr, "%d", n);
		//four commas and a number
		p += (strlen(linked) + strlen(ssid) + strlen(wlanmode) + strlen(nstr) + 3);

		DBCK("buf: %s", buf);
		DBCK("ssid = %s, wlanmode =  %s, n = %d", ssid, wlanmode, n);
		DBCK("p: %s", p);

		if(is_ap_ssid_exit(ssid,apName) == 0){
		while (n > 0) {
			memset(mac, '\0', sizeof(mac));
			memset(pmac, '\0', sizeof(pmac));
			sscanf(p, "%[^,]", mac);
			mac_trans(mac,pmac);
			websWrite(wp, "macarray[%d] = \"%s\";\n", *num, pmac);
			websWrite(wp, "linked[%d] = \"\";\n", *num);
			websWrite(wp, "wlanmode[%d] = \"%s\";\n", *num, wlanmode);
			websWrite(wp, "ssid[%d] = \"%s\";\n", *num, ssid);
			aspOutAuth(wp, mac, *num);

			p += sizeof(mac);
			n--;
			(*num)++;
		}
		}
	} else {
	//AP 有用户名的情况
		sscanf(p, "%[^,],%[^,],%[^,],%d", linked, ssid, wlanmode, &n);
		sprintf(nstr, "%d", n);
		//four commas and a number
		p += (strlen(linked) + strlen(ssid) + strlen(wlanmode) + strlen(nstr) + 4);
		DBCK("buf: %s", buf);
		DBCK("linked = %s, ssid = %s, wlanmode =  %s, n = %d", linked, ssid, wlanmode, n);
		DBCK("p: %s", p);

		if(is_ap_ssid_exit(ssid,apName) == 0){
		while (n > 0) {
			memset(mac, '\0', sizeof(mac));
			memset(pmac, '\0', sizeof(pmac));
			sscanf(p, "%[^,]", mac);
			mac_trans(mac,pmac);
			websWrite(wp, "macarray[%d] = \"%s\";\n", *num, pmac);
			websWrite(wp, "linked[%d] = \"%s\";\n", *num, linked);
			websWrite(wp, "wlanmode[%d] = \"%s\";\n", *num, wlanmode);
			websWrite(wp, "ssid[%d] = \"%s\";\n", *num, ssid);
			aspOutAuth(wp, mac, *num);

			p += sizeof(mac);
			DBCK("mac: %s", mac);
			n--;
			(*num)++;
		}
		}
	}


	return 0;
}
int is_ap_ssid_exit(char *buf,char apName[MAX_AP_MANAGED][UTT_SSID_LENTH])
{
	int i;
	int ret = 0;
DBCK("buf=%s apName=%s\n",buf,apName);
	for(i = 0; strlen(apName[i]) > 0; i++)
	{
		if(strcmp(buf,apName[i]) == 0)
		{
			ret = 1;
			break;
		}
	}
	if(ret == 0)
	{
		strcpy(apName[i],buf);
	}
DBCK("buf=%s apName=%s ret=%d\n",buf,apName,ret);

	return ret;
}
#endif
static int get_usb_sta_log_file_name(char *path)
{
    int ret = -1;
    int size = 0;
    struct dirent* ent = NULL;
    DIR *pDir = NULL;
    char buff[256] = "";
    FILE *log_fp = NULL;
    log_file_head_t head;
    memset(&head, 0, sizeof(head));
    /*遍历指定路径下的所有文件，查找到符合条件文件*/
    pDir=opendir(path);
    if (pDir == NULL)
    {
	ret = -1;
    }
    else
    {
	while ((ent = readdir(pDir)) != NULL)
	{ 
	    if ((strcmp(ent->d_name,".") != 0) && (strcmp(ent->d_name, "..") != 0))
	    {
		memset(buff,0, sizeof(buff));
		strncpy(buff, path, sizeof(buff));
		strcat(buff, ent->d_name);
		log_fp = fopen(buff, "rb");
		if (log_fp != NULL)
		{
		    /*读取包头*/
		    size = fread(&(head), sizeof(head), 1, log_fp);
		    if ( size == 1)
		    {
			if ((ntohl(head.head) == 0x4C4F4746) && (((ntohs(head.flag))&0x0F) == STA_LOG_FLAG))
			{
			    /*满足条件的文件*/
			    strcat(path, ent->d_name);
			    fclose(log_fp);
			    ret = 0;
			    break;
			}
		    }
		    fclose(log_fp);
		    log_fp = NULL;
		}
	    }
	}
    }
    if (pDir != NULL)
    {
	closedir(pDir);
    }
    return ret;
}

static int get_usb_log_file(char *file_path)
{
    int ret = -1;
    /*检测是否有U盘*/
    long long total_size = 0, free_size = 0;

#if (UTT_WX_2000 == FYES)
    /*2000内置TF卡*/
    strcpy(file_path, "/ff/");
    strcat(file_path, USB_STA_LOG_PATH);
    ret = get_usb_sta_log_file_name(file_path);
#else
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    getOuterFlashStat(&total_size, &free_size);
#endif
    if (total_size != 0)
    {
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	isUSBStorageExist(file_path);
#endif
	strcat(file_path, USB_STA_LOG_PATH);
	/*确定U盘中的文件路径*/
	ret = get_usb_sta_log_file_name(file_path);
    }
#endif

    return ret;
}

static uint32 get_sta_log_size(char *path)
{
    uint32 size = 0;
    struct stat file_info;
    if (stat(path, &file_info) == 0)
    {
	size = file_info.st_size;
    }
    return size;
}

static int get_sta_log_info_to_buff(char *path, uint32 size, uchar *buff)
{
    int ret = -1;
    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (fp != NULL)
    {
	if (fread(buff, size, 1, fp) == 1)
	{
	    ret = 0;
	}
	fclose(fp);
    }
    return ret;
}

/*获取当天的零点时刻*/
static uint32 get_date_flag_zero(void)
{
    struct tm *p;
    uint32 time_flag = time(NULL);
    p = gmtime((time_t *)&time_flag);
    if (p != NULL)
    {
	/*当天*/
	time_flag -= (p->tm_hour * 3600 + p->tm_min * 60 + p->tm_sec );
    }
   
    if (date_flag != 0)
    {
	if (1 == date_flag)
	{
	    /*3天内*/
	    time_flag -= LOG_1_DAY * 2;
	}
	else if (2 == date_flag)
	{
	    time_flag -= LOG_1_DAY * 6;
	}
	else if (3 == date_flag)
	{
	    /*一个月之内*/
	    time_flag -= LOG_1_DAY * 30;	
	}
	else
	{
	    /*所有*/
	    time_flag = 0;
	}
    }

    return time_flag;
}

static unsigned long long get_updown_data(uint16 data)
{
#if 0
    static char buff[32];

    if ((((data&0xC000) >> 14)&0x03) == 0)
    {
	sprintf(buff, "%huB", data&0x3fff); 
    }
    else if ((((data&0xC000) >> 14)&0x03)== 1)
    {
	sprintf(buff, "%huK",data&0x3fff); 
    }
    else if ((((data&0xC000) >> 14)&0x03)== 2)
    {
	sprintf(buff, "%huM",data&0x3fff); 
    }
    else
    {
	sprintf(buff, "%huG",data&0x3fff); 
    }
    return buff;
#else
    unsigned long long big_num = 0ull;
    big_num = data&0x3fff; 
    if ((((data&0xC000) >> 14)&0x03) == 0)
    {
	big_num = big_num; 
    }
    else if ((((data&0xC000) >> 14)&0x03)== 1)
    {
	big_num = big_num * 1000; 
    }
    else if ((((data&0xC000) >> 14)&0x03)== 2)
    {
	big_num = big_num * 1000000; 
    }
    else
    {
	big_num = big_num * 1000000000; 
    }
    
    return big_num;
#endif
}

static char *get_sta_online_date(uint32 timep)
{
    static char buff[32];
    struct tm *p;
    p = localtime((time_t *)&timep); /*取得当地时间*/
    sprintf(buff, "%d-%02d-%02d %02d:%02d:%02d", (1900+p->tm_year),( 1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return buff;
}

/*获取AP名称*/
static char* get_ap_name(char *mac)
{
    urcp_shm_t *mem_shadow = NULL;
    uint32 key;
    int32 index;
    urcp_dev_t *tmp = NULL;
    char *name = NULL;
    
    mem_shadow = get_shm_ptr();
    if ( (mac != NULL) && (mem_shadow != NULL))
    {
	key = urcp_dev_table_hash(mac, 0u);
	index = mem_shadow->dev_tb_hash[key];
	if (index != NONE_NODE) 
	{
	    while (index != NONE_NODE) 
	    {
		tmp = &(mem_shadow->dev_table[index - 1]);
		if (mac_cmp(tmp->mac, mac) == 0) 
		{
		    if (tmp->dev_name[0] != '\0')
		    {
			name = tmp->dev_name;
		    }
		    break;
		}

		index = tmp->tb_next;
	    }
	}
    }

    return name;
}

/*筛选重复sta的mac，每个sta只保留最新的那个节点*/
static void change_sta( log_file_info_t *sta , uint32 sta_num, uint32 time_flag)
{
    int i = 0, j = 0, k = 0;
    char mac_null[MAC_LEN] = "";
    uint32 timep;

    for (i = sta_num - 1; i > 0; i--)
    {
	if ((memcmp(sta[i].sta_mac, mac_null, sizeof(mac_null)) != 0) && ((timep = ntohl(sta[i].up_time)) > time_flag))
	{
	    for (j = i - 1; j >= 0; j--)
	    {
		if (  ntohl(sta[j].up_time) > time_flag )
		{
		    if (memcmp(sta[i].sta_mac, sta[j].sta_mac, MAC_LEN) == 0)
		    {
			memset(sta[j].sta_mac, 0, MAC_LEN );
			k++;
		    }
		}
		else
		{
		    break;
		}
	    }
	}
	else if ( timep <= time_flag )
	{
	    break;   
	}
    }
}

static uint32 put_sta_log_file_to_web(log_file_info_t *sta, uint32 sta_num, char *sta_mac, webs_t wp)
{
    int i = 0;
    uint32 num = 0;
    uint32 time_flag = 0;
    uint32 timep;
    strTime_t *time = NULL;
    char *company = NULL;
    char check_mac[13] = "";
    char mac_null[MAC_LEN] = "";
    char *dev_name = NULL;

    if (NULL == sta_mac)
    {
	time_flag = get_date_flag_zero();
	/*删除日志中重复节点*/
	change_sta(sta, sta_num, time_flag);
    }
    for(i = sta_num - 1; i >=0; i--)
    {
	if (memcmp(sta[i].sta_mac, mac_null, sizeof(mac_null)) == 0)
	{
	    continue;
	}
	sprintf(check_mac, "%02X%02X%02X%02X%02X%02X", sta[i].sta_mac[0], sta[i].sta_mac[1], sta[i].sta_mac[2],\
		sta[i].sta_mac[3], sta[i].sta_mac[4], sta[i].sta_mac[5]);
	if ( ( (timep = ntohl(sta[i].up_time)) > time_flag) && ( (sta_mac == NULL) || ( (sta_mac != NULL) && (strncmp(check_mac, sta_mac, sizeof(check_mac)-1) == 0)) ) )
	{
	    websWrite(wp, "linkedDate[%d] = \"%s\";\n",num, get_sta_online_date(timep));
	    websWrite(wp, "macarrays[%d] =\"%s\";\n", num, check_mac );
	    if ( (dev_name = get_ap_name(sta[i].ap_mac)) != NULL)
	    {
		websWrite(wp, "linkedaps[%d] =\"%s\";\n",num, dev_name);
	    }
	    else
	    {
		websWrite(wp, "linkedaps[%d] =\"%02X%02X%02X%02X%02X%02X\";\n",num, sta[i].ap_mac[0], sta[i].ap_mac[1], sta[i].ap_mac[2],\
									    sta[i].ap_mac[3], sta[i].ap_mac[4], sta[i].ap_mac[5]);
	    }
	    websWrite(wp, "time[%d] = \"", num);
	    if(ntohl(sta[i].duration) > 0)
	    {
		time = convertS2HMS(ntohl(sta[i].duration));
		if((time->day > 0) && (time->day < 1447)){
		    websWrite(wp, "%d天", time->day);
		}
		if((time->hour > 0) && (time->day < 1447)){
		    websWrite(wp, "%d时", time->hour);
		}
		if(time->min > 0){
		    websWrite(wp, "%d分", time->min);
		}
		if(time->sec > 0){
		    websWrite(wp, "%d秒", time->sec);
		}
		websWrite(wp, "\";\n");
		
	    }
	    else
	    {
		websWrite(wp, "0秒\";\n");
	    }

	    company = getManuByOUI(sta[i].sta_mac); 
	    if (company != NULL)
	    {
		websWrite(wp, "companyName[%d] = \"%s\";\n", num, company);
	    }
	    else
	    { 
		websWrite(wp, "companyName[%d] = \"\";\n", num);
	    }

	    websWrite(wp, "downloads[%d] =\"%s\" ;\n", num,convertB2KM( get_updown_data(ntohs(sta[i].download))) );
	    websWrite(wp, "uploads[%d] =\"%s\" ;\n",num,convertB2KM( get_updown_data(ntohs(sta[i].upload))) );
	    
	    websWrite(wp, "users[%d] = \"\";\n",num);
	    websWrite(wp, "iparrays[%d] = \"\";\n",num);
	    websWrite(wp, "hostName[%d] =\"\" ;\n",num);
	    websWrite(wp, "clientenables[%d] = 0;\n", num);

	    num++;
	}
	else if ( timep <= time_flag )
	{
	    break;
	}
    }
    return num;
}

static uint32 get_offline_clts_info(webs_t wp, char *cli_mac)
{
    urcp_shm_t *mem_shadow = NULL;
    sta_log_t *sta_log = NULL;
    log_pool_t *log_pool = NULL;
    log_info_t *log = NULL;
    uint32 num = 0, i = 0, file_size = 0, sta_num;
    char path[64] = "";
    uchar *file_buff = NULL;
    log_file_info_t *sta_file = NULL;
    uint32 node = 0, node_mark = 0;
    uchar *sta_mac = NULL;
    strTime_t *time=NULL;
    char check_mac[13] = "";
    uint32 time_flag = 0u;
    char *dev_name = NULL;
#if (MAC_COMPANY_NAME == FYES)
    char *company = NULL;
#endif

    if ( get_usb_log_file(path) == 0)
    {
	/*有U盘有文件*/
	file_size = get_sta_log_size(path);
	if ((file_size > sizeof(log_file_head_t)) && ((file_size - sizeof(log_file_head_t))%sizeof(log_file_info_t) == 0 ) )
	{
	    if ((file_buff = malloc(file_size)) != NULL )
	    {
		if (get_sta_log_info_to_buff(path, file_size, file_buff) == 0)
		{
		    /*日志数据的起始部分*/
		    sta_file = (log_file_info_t *)(file_buff + sizeof(log_file_head_t));
		    num =  put_sta_log_file_to_web( sta_file , (file_size - sizeof(log_file_head_t))/sizeof(log_file_info_t), cli_mac, wp);
		}
		free(file_buff);
		file_buff = NULL;
	    }
	}
    }
    else
    {
	/*无文件,读内存*/
	mem_shadow = get_shm_ptr();
	if (mem_shadow != NULL)
	{
	    log_pool = &(mem_shadow->logPool);
	    node = log_pool->lastestIdx;
	    num = 0;
	    if (cli_mac == NULL)
	    {
		time_flag = get_date_flag_zero();
	    }
#if 1
	    sta_log = mem_shadow->sta_log;
	    if (cli_mac == NULL)
	    {
		for (i = 1; i < MAX_STA_LOG_TB_SIZE; i++)
		{
		    if (sta_log[i].hdr.used != 0)
		    {
			if (sta_log[i].hdr.used == 1)
			{
			    log =  &(log_pool->logBuffer[sta_log[i].logPoolIdx]);
			    if (log->next != 0 )
			    {
				log = &(log_pool->logBuffer[log->next]);	
			    }
			    else
			    {
				log = NULL;
			    }
			}
			else
			{
			    log =  &(log_pool->logBuffer[sta_log[i].logPoolIdx]);
			}

			sta_mac = sta_log[i].hdr.mac_addr;
			sprintf(check_mac,"%02X%02X%02X%02X%02X%02X",sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5]);
			if ( (log != NULL) && (log->upTime > time_flag))
			{
			    websWrite(wp, "macarrays[%d] =\"%s\";\n", num, check_mac);
			    if ((dev_name = get_ap_name(log->apMac)) != NULL)
			    {
				websWrite(wp, "linkedaps[%d] =\"%s\";\n",num, dev_name);
			    }
			    else
			    {
				websWrite(wp, "linkedaps[%d] =\"%02X%02X%02X%02X%02X%02X\";\n",num, log->apMac[0], log->apMac[1], log->apMac[2],\
				    log->apMac[3], log->apMac[4], log->apMac[5]);
			    }
			    websWrite(wp, "linkedDate[%d] = \"%s\";\n",num, get_sta_online_date(log->upTime));
			    websWrite(wp, "time[%d] = \"", num);
			    if(log->duration> 0)
			    {
				time = convertS2HMS(log->duration);
				if( (time->day > 0) && (time->day < 1447)){
				    websWrite(wp, "%d天", time->day);
				}
				if((time->hour > 0) && (time->day < 1447)){
				    websWrite(wp, "%d时", time->hour);
				}
				if(time->min > 0){
				    websWrite(wp, "%d分", time->min);
				}
				if(time->sec > 0){
				    websWrite(wp, "%d秒", time->sec);
				}
				websWrite(wp, "\";\n");

			    }
			    else
			    {
				websWrite(wp, "0秒\";\n");
			    }

			    company = getManuByOUI(sta_mac); 
			    if (company != NULL)
			    {
				websWrite(wp, "companyName[%d] = \"%s\";\n", num, company);
			    }
			    else
			    { 
				websWrite(wp, "companyName[%d] = \"\";\n", num);
			    }

			    websWrite(wp, "downloads[%d] =\"%s\" ;\n", num,convertB2KM( get_updown_data(log->download)) );
			    websWrite(wp, "uploads[%d] =\"%s\" ;\n",num,convertB2KM( get_updown_data(log->upload)) );
			    websWrite(wp, "users[%d] = \"\";\n",num);
			    websWrite(wp, "iparrays[%d] = \"\";\n",num);
			    websWrite(wp, "hostName[%d] =\"\" ;\n",num);

			    num++;
			}
		    }
		}
	    }
	    else
	    {
		for (i = 1; i < MAX_STA_LOG_TB_SIZE; i++)
		{
		    if (sta_log[i].hdr.used != 0)
		    {
			sta_mac = sta_log[i].hdr.mac_addr;
			sprintf(check_mac,"%02X%02X%02X%02X%02X%02X",sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5]);
			if (strncmp(check_mac, cli_mac, sizeof(check_mac) - 1) != 0)
			{
			    continue;
			}
			if (sta_log[i].hdr.used == 1)
			{
			    log =  &(log_pool->logBuffer[sta_log[i].logPoolIdx]);
			    if (log->next != 0 )
			    {
				log = &(log_pool->logBuffer[log->next]);	
			    }
			    else
			    {
				log = NULL;
			    }
			}
			else
			{
			    log =  &(log_pool->logBuffer[sta_log[i].logPoolIdx]);
			}

			while ( log != NULL)
			{
			    websWrite(wp, "macarrays[%d] =\"%s\";\n", num, check_mac);
			    if ((dev_name = get_ap_name(log->apMac)) != NULL)
			    {
				websWrite(wp, "linkedaps[%d] =\"%s\";\n",num, dev_name);
			    }
			    else
			    {
				websWrite(wp, "linkedaps[%d] =\"%02X%02X%02X%02X%02X%02X\";\n",num, log->apMac[0], log->apMac[1], log->apMac[2],\
					log->apMac[3], log->apMac[4], log->apMac[5]);
			    }
			    websWrite(wp, "linkedDate[%d] = \"%s\";\n",num, get_sta_online_date(log->upTime));
			    websWrite(wp, "time[%d] = \"", num);
			    if(log->duration> 0)
			    {
				time = convertS2HMS(log->duration);
				if( (time->day > 0) && (time->day < 1447)){
				    websWrite(wp, "%d天", time->day);
				}
				if((time->hour > 0) && (time->day < 1447)){
				    websWrite(wp, "%d时", time->hour);
				}
				if(time->min > 0){
				    websWrite(wp, "%d分", time->min);
				}
				if(time->sec > 0){
				    websWrite(wp, "%d秒", time->sec);
				}
				websWrite(wp, "\";\n");

			    }
			    else
			    {
				websWrite(wp, "0秒\";\n");
			    }

			    company = getManuByOUI(sta_mac); 
			    if (company != NULL)
			    {
				websWrite(wp, "companyName[%d] = \"%s\";\n", num, company);
			    }
			    else
			    { 
				websWrite(wp, "companyName[%d] = \"\";\n", num);
			    }

			    websWrite(wp, "downloads[%d] =\"%s\" ;\n", num,convertB2KM( get_updown_data(log->download)) );
			    websWrite(wp, "uploads[%d] =\"%s\" ;\n",num,convertB2KM( get_updown_data(log->upload)) );
			    websWrite(wp, "users[%d] = \"\";\n",num);
			    websWrite(wp, "iparrays[%d] = \"\";\n",num);
			    websWrite(wp, "hostName[%d] =\"\" ;\n",num);

			    num++;

			    if (log->next != 0)
			    {
				log = &(log_pool->logBuffer[log->next]);		
			    }
			    else
			    {
				log = NULL;
			    }
			}
			break;
		    }
		}

	    }
#else
	    while(node)
	    {
		log = &(log_pool->logBuffer[node]);
		sta_mac = mem_shadow->sta_log[log->staIdx].hdr.mac_addr;
		sprintf(check_mac,"%02X%02X%02X%02X%02X%02X",sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5]);
		if ((log->upTime > time_flag) && ((cli_mac == NULL) || ((cli_mac != NULL) && (strncmp(check_mac, cli_mac, sizeof(check_mac) -1) == 0)) ) )
		{
		    websWrite(wp, "macarrays[%d] =\"%s\";\n", num, check_mac);
		    websWrite(wp, "linkedaps[%d] =\"%02X%02X%02X%02X%02X%02X\";\n",num, log->apMac[0], log->apMac[1], log->apMac[2],\
			    log->apMac[3], log->apMac[4], log->apMac[5]);
		    websWrite(wp, "linkedDate[%d] = \"%s\";\n",num, get_sta_online_date(log->upTime));
		    websWrite(wp, "time[%d] = \"", num);
		    if(log->duration> 0)
		    {
			time = convertS2HMS(log->duration);
			if( (time->day > 0) && (time->day < 1447)){
			    websWrite(wp, "%d天", time->day);
			}
			if((time->hour > 0) && (time->day < 1447)){
			    websWrite(wp, "%d时", time->hour);
			}
			if(time->min > 0){
			    websWrite(wp, "%d分", time->min);
			}
			if(time->sec > 0){
			    websWrite(wp, "%d秒", time->sec);
			}
			websWrite(wp, "\";\n");

		    }
		    else
		    {
			websWrite(wp, "0秒\";\n");
		    }

		    company = getManuByOUI(sta_mac); 
		    if (company != NULL)
		    {
			websWrite(wp, "companyName[%d] = \"%s\";\n", num, company);
		    }
		    else
		    { 
			websWrite(wp, "companyName[%d] = \"\";\n", num);
		    }

		    websWrite(wp, "downloads[%d] =\"%s\" ;\n", num,convertB2KM( get_updown_data(log->download)) );
		    websWrite(wp, "uploads[%d] =\"%s\" ;\n",num,convertB2KM( get_updown_data(log->upload)) );
		    websWrite(wp, "users[%d] = \"\";\n",num);
		    websWrite(wp, "iparrays[%d] = \"\";\n",num);
		    websWrite(wp, "hostName[%d] =\"\" ;\n",num);

		    num++;
		}
		
		node = log->earlierIdx;
	    }	    
#endif

	    /*
	    websWrite(wp, "users[0] = \"%s\";\n","test");
	    websWrite(wp, "iparrays[0] = \"%s\";\n","192.168.1.199");
	    websWrite(wp, "macarrays[0] =\"%s\" ;\n","D4970B8A16ED");
	    websWrite(wp, "linkedaps[0] =\"%s\" ;\n", "0022AA335244");
	    websWrite(wp, "linkedDate[0] = \"%s\";\n", "2014-12-12 5:30");
	    websWrite(wp, "time[0] =\"%s\" ;\n", "9分9秒");
	    websWrite(wp, "downloads[0] =\"%s\" ;\n", "100K");
	    websWrite(wp, "uploads[0] =\"%s\" ;\n", "100K");
	    websWrite(wp, "hostName[0] =\"%s\" ;\n", "test_name");
	    websWrite(wp, "companyName[0] =\"%s\" ;\n", "test_xiaomi");
	    */
	}
    }

    return num;
}

static int aspOutStaHistory(int eid, webs_t wp, int argc, char_t **argv)
{
    char *mac = NULL;
    unsigned int num = 0;
    mac = websGetVar(wp,"staMac",T(""));
    websWrite(wp, "var users= new Array();\n");
    websWrite(wp, "var iparrays = new Array();\n");
    websWrite(wp, "var macarrays = new Array();\n");
    websWrite(wp, "var linkedaps = new Array();\n");
    websWrite(wp, "var clientenables = new Array();\n");
    websWrite(wp, "var linkedDate = new Array();\n");
    websWrite(wp, "var time = new Array();\n");
    websWrite(wp, "var downloads = new Array();\n");
    websWrite(wp, "var uploads = new Array();\n");
    websWrite(wp, "var hostName = new Array();\n");
    websWrite(wp, "var companyName = new Array();\n");

    if (mac != NULL)
    {
	num = get_offline_clts_info(wp , mac);
    }

    websWrite(wp, "var totalrecs = %d;\n", num);
    websWrite(wp, "var max_totalrecs = %d;\n", num);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

static int aspOutPutWlanCltList(int eid, webs_t wp, int argc, char_t **argv)
{
    unsigned int num = 0u;
    char *mac = NULL;
    char apName[MAX_AP_MANAGED][UTT_SSID_LENTH];
    memset(apName,0,MAX_AP_MANAGED*UTT_SSID_LENTH);

    mac = websGetVar(wp,"mac",T(""));
    websWrite(wp, "var onoff_flag=%d;\n", onoff_flag);
    if (onoff_flag == 0)
    {
	websWrite(wp, "var users= new Array();\n");
	websWrite(wp, "var iparrays = new Array();\n");
	websWrite(wp, "var macarrays = new Array();\n");
	websWrite(wp, "var linkedaps = new Array();\n");
	websWrite(wp, "var linkedSSIDs = new Array();\n");
	websWrite(wp, "var wlanmodes = new Array();\n");
	websWrite(wp, "var clientenables = new Array();\n");
	websWrite(wp, "var filterRules = new Array();\n");
	websWrite(wp, "var signals = new Array();\n");
	websWrite(wp, "var rates = new Array();\n");
	websWrite(wp, "var bindwidths = new Array();\n");
	websWrite(wp, "var downloads = new Array();\n");
	websWrite(wp, "var downRate = new Array();\n");
	websWrite(wp, "var uploads = new Array();\n");
	websWrite(wp, "var upRate = new Array();\n");
	websWrite(wp, "var time = new Array();\n");
	websWrite(wp, "var wlanFre = new Array();\n");
	websWrite(wp, "var hostName = new Array();\n");
	websWrite(wp, "var companyName = new Array();\n");
	num = get_clts_info_auto(wp,mac); 
    }
    else
    {
	websWrite(wp, "var users= new Array();\n");
    	websWrite(wp, "var iparrays = new Array();\n");
	websWrite(wp, "var macarrays = new Array();\n");
	websWrite(wp, "var linkedaps = new Array();\n");
	websWrite(wp, "var linkedDate = new Array();\n");
	websWrite(wp, "var clientenables = new Array();\n");
	websWrite(wp, "var time = new Array();\n");
	websWrite(wp, "var downloads = new Array();\n");
	websWrite(wp, "var uploads = new Array();\n");
	websWrite(wp, "var hostName = new Array();\n");
	websWrite(wp, "var companyName = new Array();\n");
	websWrite(wp, "var date = \"%d\";\n", date_flag);

	/*
	websWrite(wp, "users[0] = \"%s\";\n","test");
    	websWrite(wp, "iparrays[0] = \"%s\";\n","192.168.1.199");
	websWrite(wp, "macarrays[0] =\"%s\" ;\n","D4970B8A16ED");
	websWrite(wp, "linkedaps[0] =\"%s\" ;\n", "0022AA335244");
	websWrite(wp, "linkedDate[0] = \"%s\";\n", "2014-12-12 5:30");
	websWrite(wp, "time[0] =\"%s\" ;\n", "9分9秒");
	websWrite(wp, "downloads[0] =\"%s\" ;\n", "100K");
	websWrite(wp, "uploads[0] =\"%s\" ;\n", "100K");
	websWrite(wp, "hostName[0] =\"%s\" ;\n", "test_name");
	websWrite(wp, "companyName[0] =\"%s\" ;\n", "test_xiaomi");
	*/
	num = get_offline_clts_info(wp , NULL);
	onoff_flag = 0;
    }

    websWrite(wp, "var totalrecs = %d;\n", num);
    websWrite(wp, "var max_totalrecs = %d;\n", num);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

static void formDelAllLog(webs_t wp, char_t *path, char_t *query)
{
    int len = 0;
    char buffer[sizeof(urcp_del_log_msg_t)] = "";
    urcp_del_log_msg_t  *msg;

    msg = (urcp_del_log_msg_t *)buffer;
    msg->mode = 0; 
    msg->staNum  = 0;
    len = sizeof(msg->mode) + sizeof(msg->staNum);
    sendwebmsgbuffer(MSG_TYPE_GET, URCP_MSG_DEL_LOG, buffer, len);
    websRedirect(wp, "wlanCltList.asp");
    return;
}

static void formGetOnlineState(webs_t wp, char_t *path, char_t *query)
{
    char *flag = NULL;
    char *date = NULL;
    flag = websGetVar(wp, "userState", T(""));
    if (flag != NULL)
    {
	if (strcmp(flag, "ONLINE") == 0)
	{
	    /*在线*/
	    onoff_flag = 0;
	    date_flag = 0;
	}
	else
	{
	    /*离线*/
	    onoff_flag = 1;
	    date = websGetVar(wp, "date_select", T(""));
	    date_flag = atoi(date);
//	    printf("%s, date_flag:%d\n", __func__, date_flag);
	}
    }
//    printf("%s, onoff_flag:%d\n", __func__, onoff_flag);
    websRedirect(wp, "wlanCltList.asp");
    return;
}

static void formApMacFilterChange(webs_t wp, char_t *path, char_t *query)
{
	char *ssid=NULL,*mac=NULL,*rule=NULL,*checked=NULL;
	MibProfileType mibTypeFilter = MIB_PROF_AP_MAC_FILTER;
	ApMacFilterProfile *profFilter = NULL;
	int minFilter, maxFilter, indexFilter,macTotal = 0,i,j,m;
	int change = 0;
    MacAddr macCurrent;
	const char *ErrMsg = NULL;

	MibProfileType mibTypeSZ = MIB_PROF_SERVICE_ZONE;
	ServiceZoneProfile *profSZ = NULL;
	int minSZ, maxSZ, indexSZ;

	MacAddr macAddr;
	memset(&macAddr, 0, sizeof(macAddr));

	mac = websGetVar(wp, "clientenableID", T(""));
	ssid = websGetVar(wp, "filterSsid", T(""));
	rule = websGetVar(wp, "macRule", T(""));
	checked = websGetVar(wp, "clientenable", T(""));

	converMac12To6Byte(mac, &macAddr);/*转化为6字节类型*/
    getCurrentUserMacFromAc(wp, (char *)(macCurrent.addr));/*根据操作者ip得到mac地址*/
	//根据ssid获取服务区
	ProfInstLowHigh(mibTypeSZ, &maxSZ, &minSZ);
	for(indexSZ = minSZ; indexSZ < maxSZ; indexSZ++)
	{
		profSZ = (ServiceZoneProfile *)ProfGetInstPointByIndex(mibTypeSZ, indexSZ);
		if ((profSZ != NULL) && (ProfInstIsFree((Pointer)profSZ) == 0) && (profSZ->head.active == TRUE))
		{
            if(strcmp(profSZ->ssid,ssid) == 0)
			{
			    break;
			}
		}

	}
	ProfInstLowHigh(mibTypeFilter, &maxFilter, &minFilter);
	//根据服务区名字查找所包含的地址组，做mac地址添加，判断添加是否导致数目超限
	if(((strcmp(rule,"1")==0)&&(strcmp(checked,"0")==0))||((strcmp(rule,"2")==0)&&(strcmp(checked,"1")==0)))
	{
		for(indexFilter = minFilter; indexFilter < maxFilter; indexFilter++)
		{
			profFilter = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeFilter, indexFilter);
			if ((profFilter != NULL) && (ProfInstIsFree((Pointer)profFilter) == 0) && (profFilter->head.active == TRUE))
			{
		        for(i = 0;i < MAX_AP_MAC_FILTER_NUM; i++)
				{
				    if(strcmp(profFilter->sz[i],profSZ->head.name) == 0)
					{
					    macTotal += (profFilter->num + 1);//加1表示即将要在该组加入该MAC
					}
				}
			}
		}
		if(macTotal > MAX_MAC_FILTER_SIGNAL_AP)
		{
		    setTheErrorMsg("添加该MAC地址导致过滤地址条目超过最大条目，请先删除该服务区其他MAC地址");
			websRedirect(wp, "wlanCltList.asp");
		    return;
		}
		//添加动作
		for(indexFilter = minFilter; indexFilter < maxFilter; indexFilter++)
		{
			profFilter = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeFilter, indexFilter);
			if ((profFilter != NULL) && (ProfInstIsFree((Pointer)profFilter) == 0) && (profFilter->head.active == TRUE))
			{
		        for(i = 0;i < MAX_AP_MAC_FILTER_NUM; i++)
				{
				    if(strcmp(profFilter->sz[i],profSZ->head.name) == 0)
					{
						if(memcmp(&macCurrent,&macAddr,sizeof(MacAddr))==0)//与当前登陆MAC地址相同
						{
						    if(profFilter->mode == 2)//prof规则为禁止，则操作失败
							{
                               ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
                               setTheErrorMsgOnFree(ErrMsg);
			                   websRedirect(wp, "wlanCltList.asp");
		                       return;
							}
						}
		                memcpy(profFilter->mac[profFilter->num].addr,macAddr.addr,sizeof(macAddr.addr));
						profFilter->num++;
						change = 1;
					}
				}
			}
		}

    }else{
	    //删除动作
		for(indexFilter = minFilter; indexFilter < maxFilter; indexFilter++)
		{
			profFilter = (ApMacFilterProfile *)ProfGetInstPointByIndex(mibTypeFilter, indexFilter);
			if ((profFilter != NULL) && (ProfInstIsFree((Pointer)profFilter) == 0) && (profFilter->head.active == TRUE))
			{
		        for(i = 0;i < MAX_AP_MAC_FILTER_NUM; i++)
				{
				    if(strcmp(profFilter->sz[i],profSZ->head.name) == 0)
					{
					    for(j = 0; j < profFilter->num; j++)
						{
						    if(memcmp(&profFilter->mac[j],&macAddr,sizeof(MacAddr)) == 0)
							{
						        if(memcmp(&macCurrent,&macAddr,sizeof(MacAddr))==0)//与当前登陆MAC地址相同
						        {
						            if(profFilter->mode == 1)//prof规则为允许，则操作失败
							        {
                                        ErrMsg = getTransDataForC(C_LANG_INDEX_SUBMIT_FOR_NO_LOGIN);
                                        setTheErrorMsgOnFree(ErrMsg);
			                            websRedirect(wp, "wlanCltList.asp");
		                                return;
							         }
						        }
							    for(m = j; m < profFilter->num - 1; m++)
								{
								    strcpy(profFilter->mac[m].addr,profFilter->mac[m+1].addr);
								}
								change = 1;
						        profFilter->num--;
								break;
							}
						}
						break;
					}
				}
			}
		}
	}

	if(change == 1)
	{
		nvramWriteCommit();
	}
	websRedirect(wp, "wlanCltList.asp");
	return;
}
void formDefineApWlanCltList(void)
{
    websAspDefine(T("aspOutPutWlanCltList"), aspOutPutWlanCltList);
    websFormDefine(T("formGetOnlineState"), formGetOnlineState);
    websFormDefine(T("formDelAllLog"), formDelAllLog);
    websAspDefine(T("aspOutStaHistory"), aspOutStaHistory);
#if (URCP_DISCONNECT_STA==FYES)
    websFormDefine(T("formDisconnect"), formDisconnect);
#endif
#if (AP_BLACK_WHITE_LIST == FYES)
    websFormDefine(T("formAddToBlackList"), formAddToBlackList);
    websAspDefine(T("aspOutShowBlackList"), aspOutShowBlackList);
    websFormDefine(T("formDelBlackList"), formDelBlackList);
    websFormDefine(T("formDelAllBlackList"), formDelAllBlackList);
#endif
#if (AC_TOP_DIS == FYES)
    websFormDefine(T("formStatForTopDis"), formStatForTopDis);
#endif
    websFormDefine(T("formApMacFilterChange"), formApMacFilterChange);
    websFormDefine(T("formExportCltInfo"), formExportCltInfo);
    websAspDefine(T("aspOutGetApInfo"), getApInfo);
    websAspDefine(T("aspOutGetSsidInfo"), getSsidInfo);
    websAspDefine(T("aspOutGetHistroyInfo"), getHistroyInfo);
#if (HISTORY_FLOW_INFO == FYES)
    websAspDefine(T("aspOutGetHistoryFLowInfo"), getHistoryFlowInfo);
    websFormDefine(T("formExportCltFlowInfo"), formExportCltFlowInfo);
#endif
}
#define ATF_COM 0x02
/*
 *  根据mac地址找ip地址
 */
extern int arp_table_lookup_ipaddr( char mac[], char* ip_addr, int ip_addr_size)
{
    unsigned char buffer[256], ip[32], tmp[128], pmac[18];
    unsigned int arp_flags;
    FILE* fp;

	memset(buffer,0,sizeof(buffer));
    /*打开文件*/
    fp = fopen("/proc/net/arp", "r");
    if(fp == NULL)
    {
	return -1;
    }
    fgets(buffer, sizeof(buffer), fp);
    /*读取文件*/
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
	/*ip*/
	if(sscanf(buffer, "%s %*s 0x%x %s",  ip,&arp_flags, tmp)<2)
	{
	    /*出错*/
	    fclose(fp);
	    return -1;
	}
	mac_trans(mac,pmac);
#if ((UTT_WX_2000 == FYES) || (UTT_WX_3000 == FYES) || (UTT_WX_5000 == FYES) || (UTT_NV_D908W == FYES))
	if( !strcasecmp( pmac,tmp ) && (arp_flags & ATF_COM) ){
#else
	if( !memcmp( pmac,tmp,17 ) && (arp_flags & ATF_COM) ){
#endif
		if( strlen(ip) > ip_addr_size ){
			break;
		}
		memcpy(ip_addr,ip,16);
		fclose(fp);
		return 0;
	}
	memset(buffer,0,sizeof(buffer));
    }
    /*找到了*/
    fclose(fp);
    return -1;
}

/******************************************************************
 * 函数：getApInfo
 * 功能：统计AP信息并输出至页面
 * 创建：2014-03-18
 * 参数：略
 * 输出：略
 * 返回：略
 * 作者：cli
 * 维护：cli
 *****************************************************************/
static int getApInfo(int eid, webs_t wp, int argc, char **argv)
{
    int i, j, k, max, tb_index, swap, swap2, index=0;
    int ap_index[MAX_AP_MANAGED];
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t *tmp = NULL; 
    uint32 clientsNum[MAX_AP_MANAGED];

    websWrite(wp, "var ApClients1 = new Array();\n");

    memset(ap_index,0,sizeof(ap_index));
    mem_shadow = get_shm_ptr();
    /*遍历hash*/
    for(i=0; i < DEV_HASH_SIZE; i++)
    {
        tb_index = mem_shadow->dev_tb_hash[i];

        while(tb_index!=NONE_NODE)
        {
            tmp = &mem_shadow->dev_table[tb_index-1]; 

            /*读取客户端数量*/
//            clientsNum[index] = tmp->wl_info.clients; 
            clientsNum[index] = tmp->wl_info.clients + tmp->wl_info_5G.clients; 
            ap_index[index] = tb_index - 1;

            index++;
            tb_index = tmp->tb_next;
        }
    }
    websWrite(wp, "var totalrecs = %d;\n", index);
    websWrite(wp, "var max_totalrecs=%d;\n", MAX_AP_MANAGED);

    /*按照客户端数目排序*/
    for(j = 0;j < index;j++)
    {
        max = j;
        for(k=j+1;k < index;k++){
            if(clientsNum[max] < clientsNum[k]){
                max = k;
            }
        }

        if(max != j)
        {
            swap = clientsNum[max];
            swap2 = ap_index[max];
            clientsNum[max] = clientsNum[j];
            ap_index[max] = ap_index[j];
            clientsNum[j] = swap;
            ap_index[j] = swap2;
        }
    }

    /*按照从大到小的顺序输出到页面*/
    websWrite(wp, "var ApClients1 = new Array(");
    for(i = 0;i < index;i++){
        if(i != 0){
            websWrite(wp, ",");
        }
        if(mem_shadow->dev_table[ap_index[i]].dev_name[0]){
            websWrite(wp, "new Array(\"%s\",%d)", mem_shadow->dev_table[ap_index[i]].dev_name, mem_shadow->dev_table[ap_index[i]].wl_info.clients \
											    + mem_shadow->dev_table[ap_index[i]].wl_info_5G.clients);
        }else{
            websWrite(wp, "new Array(\"%u\",%d)", mem_shadow->dev_table[ap_index[i]].serial_no, mem_shadow->dev_table[ap_index[i]].wl_info.clients \
											    + mem_shadow->dev_table[ap_index[i]].wl_info_5G.clients);
        }
    }

    websWrite(wp, ");\n");
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}
 /******************************************************************
 * 函数：getSsidInfoFromNewSavePlace
 * 功能：由于AC更改了ssid存储位置，所以需要遍历新位置兼容新版本的AC软件
 * 创建：2014-08-07
 * 作者：hec
 * ****************************************************************/
static void getSsidInfoFromNewSavePlace(int *index,int *array_index, char array_ssid[MAX_SERVICE_ZONE_PROFILES][UTT_SSID_LENTH+1])
{
    urcp_shm_t *mem_shadow = NULL;
    sta_entry_t *tmp = NULL;
    urcp_dev_t *pAp = NULL;
    int i,j,tb_index;
    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0;i < STA_TB_HASH_SIZE;i++) 
	{
	   tb_index = mem_shadow->sta_tb_hash[i].idx;
	   while(tb_index != NONE_NODE)
	   {
	       tmp = &(mem_shadow->sta_tb[tb_index - 1]);
               if((tmp->hdr.aged) <= 1)
	       {
		   pAp = urcp_dev_table_lookup(tmp->ap_mac, 0);
		   /*只统计在线AP的SSID及用户，剃除离线AP*/
		   if((pAp != NULL) && (pAp->aged != 2))
		   {
		       for(j = 0;j < *index;j++)
		       {
			   if(tmp->ssid_idx < UTT_SSID_NUM)
			   {
			       if(strcmp(array_ssid[j],pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid) == 0)
			       {
				   (*(array_index + j))++;
				   break;
			       }
			   }
		       }
		       if((j == *index) && (strcmp(pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid,"") != 0))
		       {
			   if( (*index) < MAX_SERVICE_ZONE_PROFILES )
			   {
			       strcpy(array_ssid[j],pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid);
			       (*(array_index + j))++;
			       (*index)++;
			   }
		       }
		   }
	       }
	       tb_index = tmp->hdr.tb_next;
	   }
	}
    }
    return;
}

 /******************************************************************
 * 函数：getSsidInfo
 * 功能：统计SSID信息并输出至页面
 * 创建：2014-03-18
 * 参数：略
 * 输出：略
 * 返回：略
 * 作者：cli
 * 维护：cli
 * ****************************************************************/
static int getSsidInfo(int eid, webs_t wp, int argc, char **argv)
{
    char m_buffer[MAX_SERVICE_ZONE_PROFILES][UTT_SSID_LENTH+1];
    int i, k, tb_index, index = 0;
    int ssid_index[MAX_SERVICE_ZONE_PROFILES];
    urcp_shm_t *mem_shadow = NULL;
    urcp_dev_t *tmp = NULL;
    sta_entry_t *sta_tmp=NULL;
    memset(m_buffer,0,sizeof(m_buffer));
    memset(ssid_index,0,sizeof(ssid_index));
    mem_shadow = get_shm_ptr();
#if 0
    /*先读取sta表，这个表是新用于存放用户状态的表*/
    getSsidInfoFromNewSavePlace(&index, ssid_index, m_buffer);
    /*遍历hash*/
    for(i=0; i < DEV_HASH_SIZE; i++)
    {
        tb_index = mem_shadow->dev_tb_hash[i];

        while(tb_index!=NONE_NODE)
        {
            tmp = &mem_shadow->dev_table[tb_index-1]; 

            /*读取SSID*/
            for(j = 0;j < tmp->wl_info.joined_sz_num;j++)
            {
                /*SSID统计*/
                for(k=0;k < index;k++)
                {
                    /*判断该SSID是否已存在*/
                    if(strcmp(tmp->wl_info.ssid_info[j].ssid, m_buffer[k]) == 0)
                    {
                        ssid_index[k] += tmp->wl_info.ssid_info[j].sta_num;
                        //repeated_ssid++;
                        break;
                    }
                }
                /*读取的SSID不存在，则添加至m_buffer存储*/
                if(k == index)
                {
		    if( index<MAX_SERVICE_ZONE_PROFILES ){
			sprintf(m_buffer[index], "%s", tmp->wl_info.ssid_info[j].ssid);
			ssid_index[index] = tmp->wl_info.ssid_info[j].sta_num;
                        index++;
		    }
                }
            }

            tb_index = tmp->tb_next;
        }
    }
#endif
    for(i = 0u; i < STA_TB_HASH_SIZE; i++)
    {
	tb_index = mem_shadow->sta_tb_hash[i].idx;
	while(tb_index!=NONE_NODE)
	{
	    sta_tmp = &(mem_shadow->sta_tb[tb_index-1]);
	    if(((sta_tmp->hdr.aged) <= 1) && ((sta_tmp->flag&RF_FLAG_5G) == 0))
	    {
		tmp = urcp_dev_table_lookup(sta_tmp->ap_mac, 0);
		if ((tmp != NULL) && (tmp->aged != 2))
		{
		    for(k=0;k < index;k++)
		    {
			/*判断该SSID是否已存在*/
			if(strcmp(tmp->wl_info.ssid_info[(int32)(sta_tmp->ssid_idx)].ssid, m_buffer[k]) == 0)
			{
			    ssid_index[k] += 1;
			    break;
			}
		    }
		    /*读取的SSID不存在，则添加至m_buffer存储*/
		    if(k == index)
		    {
			if( index<MAX_SERVICE_ZONE_PROFILES )
			{
			    sprintf(m_buffer[index], "%s", tmp->wl_info.ssid_info[(int32)(sta_tmp->ssid_idx)].ssid);
			    ssid_index[index] = 1;
			    index++;
			}
		    }
		}
	    }
	    tb_index = sta_tmp->hdr.tb_next;
	}
    }

    /*统计5g的ssid用户数*/
    for(i = 0u; i < STA_TB_HASH_SIZE; i++)
    {
	tb_index = mem_shadow->sta_tb_hash[i].idx;
	while(tb_index!=NONE_NODE)
	{
	    sta_tmp = &(mem_shadow->sta_tb[tb_index-1]);
	    if(((sta_tmp->hdr.aged) <= 1) && (sta_tmp->flag&RF_FLAG_5G))
	    {
		tmp = urcp_dev_table_lookup(sta_tmp->ap_mac, 0);
		if ((tmp != NULL) && (tmp->aged != 2))
		{
		    for(k=0;k < index;k++)
		    {
			/*判断该SSID是否已存在*/
			if(strcmp(tmp->wl_info_5G.ssid_info[(int32)(sta_tmp->ssid_idx)].ssid, m_buffer[k]) == 0)
			{
			    ssid_index[k] += 1;
			    break;
			}
		    }
		    /*读取的SSID不存在，则添加至m_buffer存储*/
		    if(k == index)
		    {
			if( index<MAX_SERVICE_ZONE_PROFILES )
			{
			    sprintf(m_buffer[index], "%s", tmp->wl_info_5G.ssid_info[(int32)(sta_tmp->ssid_idx)].ssid);
			    ssid_index[index] = 1;
			    index++;
			}
		    }
		}
	    }
	    tb_index = sta_tmp->hdr.tb_next;
	}
    }
//    websWrite(wp, "var SSIDNum = %d\n", index+repeated_ssid);
    websWrite(wp, "var SSIDNum = %d;\n", index);

    /*输出到页面*/
    websWrite(wp, "var SSIDClients = {");
    for(i = 0;i < index;i++){
        if(i != 0){
        websWrite(wp, ",");
        }
        websWrite(wp, "\"%s\":%d", m_buffer[i], ssid_index[i]);
    }
    websWrite(wp, "};\n");

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}
/*
 *功能：读取2.4版本以前的AP客户端信息并粘贴到buf尾部
 *作者：hec
 *日期：2014-08-21
 *说明：这个函数在新版本软件广泛使用后会被丢弃不用
 * *************************************************************************************/
static void creatStrOfCltFromOldVersion(char_t *buf,long int *len,urcp_dev_t *dev_table)
{
    char_t temp[1024],mac[13] = {0},apName[32] = {0},ssid[33] = {0},date[25] = {0};
    char_t buffer[1024];
    unsigned char *tmp_sta_mac = NULL;
    time_t nowtime;
    struct tm* timeInfo;
    strTime_t *timeTemp=NULL;
    int ret = 0,j,k;

    (*len) -= sizeof(buffer);
    if((*len) > 0)
    {
	for(j = 0;j < dev_table->wl_info.joined_sz_num;j++)
	{
	    for(k = 0;k < dev_table->wl_info.ssid_info[j].sta_num;k++)
	    {
		/*get current time*/
		time(&nowtime);
		timeInfo = localtime(&nowtime);
		sprintf(date,"%04d%02d%02d %02d:%02d:%02d",1900+timeInfo->tm_year,1+timeInfo->tm_mon,timeInfo->tm_mday,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec);
		/*get AP's dev_name*/
		memset(apName,0,sizeof(apName));
		if(strcmp(dev_table->dev_name,"") != 0)
		{
		    strcpy(apName,dev_table->dev_name);
		}
		else
		{
		    sprintf(apName,"%02X%02X%02X%02X%02X%02X",dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
		}
		/*get sta's MAC*/
		memset(mac,0,sizeof(mac));
		tmp_sta_mac = dev_table->wl_info.ssid_info[j].sta_info[k].sta_mac;
		sprintf(mac,"%02X%02X%02X%02X%02X%02X",tmp_sta_mac[0],tmp_sta_mac[1],tmp_sta_mac[2],tmp_sta_mac[3],tmp_sta_mac[4],tmp_sta_mac[5]);
		/*get SSID:dev_table->wl_info.ssid_info[j].ssid*/
		/*2.4以前的版本，不保存wireless rate、dowload、upload、connected time*/
		memset(temp,0,sizeof(temp));
		memset(buffer,0,sizeof(buffer));
		sprintf(temp,"%%s,%%s,%%s,%%s,-,-,-,-\r\n");
		sprintf(buffer,temp,date,mac,apName,dev_table->wl_info.ssid_info[j].ssid);
		strcat(buf,buffer);
	    }
	}
    }
    return;
}
/*
 *功能：读取2.4版本以前的AP客户端信息
 *作者：hec
 *日期：2014-08-21
 *说明：这个函数在新版本软件广泛使用后会被丢弃不用
 * *************************************************************************************/
static void getInfofromOldVersion(char_t *buf,long int *len)
{
    urcp_dev_t *dev_table = NULL;
    urcp_shm_t *mem_shadow = NULL;
    int tb_index,i;

    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u; i < STA_TB_HASH_SIZE; i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		dev_table = &(mem_shadow->dev_table[tb_index - 1]);
		if((*len) > 1024)
		{
		    creatStrOfCltFromOldVersion(buf,len,dev_table);
		}
		else
		{
		    break;
		}
		tb_index = (mem_shadow->dev_table[tb_index - 1]).tb_next;
	    }
	}
    }
    return;
}

/*
 *功能：将在线AP客户端信息转化为字符串并粘贴到字符串buf的尾部
 *作者：hec
 *日期：2014-07-30
 *说明：无
 *************************************************************************************/
static int creatStrOfClt(char_t *buf, long int *len,sta_entry_t* tmp)
{
    char_t temp[1024],mac[13] = {0},apName[32] = {0},ssid[33] = {0},timeCon[50] = {0},date[25] = {0};
    char_t buffer[1024],xtime[15] = {0},down[20] = {0};
    time_t nowtime;
    struct tm* timeInfo;
    urcp_dev_t *pAp=NULL;
    strTime_t *timeTemp=NULL;
    int ret = 0;
    const char *textDay = NULL,*textHour = NULL,*textMin = NULL,*textSec = NULL;

    (*len) -= sizeof(buffer);
    if((*len) < 0)
    {
	goto OUT;
    }
    textDay = getTransDataForC(C_LANG_INDEX_DAY);
    textHour = getTransDataForC(C_LANG_INDEX_HOUR);
    textMin = getTransDataForC(C_LANG_INDEX_MINUTE);
    textSec = getTransDataForC(C_LANG_INDEX_SECOND);
    pAp = urcp_dev_table_lookup(tmp->ap_mac, 0);
    if((pAp != NULL) && (pAp->aged != 2))
    {
	ret = 1;
	sprintf(mac, "%02X%02X%02X%02X%02X%02X",tmp->hdr.mac_addr[0],tmp->hdr.mac_addr[1],tmp->hdr.mac_addr[2],tmp->hdr.mac_addr[3],tmp->hdr.mac_addr[4],tmp->hdr.mac_addr[5]);
	if(strcmp(pAp->dev_name, "") != 0)
	{
	    sprintf(apName,"%s",pAp->dev_name);
	}
	else
	{
	    sprintf(apName,"%02X%02X%02X%02X%02X%02X",pAp->mac[0],pAp->mac[1], pAp->mac[2], pAp->mac[3], pAp->mac[4], pAp->mac[5]);
	}
	if(tmp->ssid_idx < UTT_SSID_NUM)
	{
	    sprintf(ssid,"%s",pAp->wl_info.ssid_info[(int32)(tmp->ssid_idx)].ssid);
	}
	else
	{
	    sprintf(ssid,"%s","");
	}
	if(tmp->con_time > 0)
	{
	    timeTemp = convertS2HMS(tmp->con_time);
	    if(timeTemp->day > 0)
	    {
		memset(xtime,0,sizeof(xtime));
		sprintf(xtime,"%d%s",timeTemp->day,textDay);
		strcat(timeCon,xtime);
	    }
	    if(timeTemp->hour > 0)
	    {
		memset(xtime,0,sizeof(xtime));
		sprintf(xtime,"%d%s",timeTemp->hour,textHour);
		strcat(timeCon,xtime);
	    }
	    if(timeTemp->min > 0)
	    {
		memset(xtime,0,sizeof(xtime));
		sprintf(xtime,"%d%s",timeTemp->min,textMin);
		strcat(timeCon,xtime);
	    }
	    if(timeTemp->sec > 0)
	    {
		memset(xtime,0,sizeof(xtime));
		sprintf(xtime,"%d%s",timeTemp->sec,textSec);
		strcat(timeCon,xtime);
	    }
	}
	else
	{
	    memset(xtime,0,sizeof(xtime));
	    sprintf(xtime,"0%s",textSec);
	    strcpy(timeCon,xtime);
	}
	time(&nowtime);
	timeInfo = localtime(&nowtime);
	sprintf(date,"%04d%02d%02d %02d:%02d:%02d",1900+timeInfo->tm_year,1+timeInfo->tm_mon,timeInfo->tm_mday,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec);
	memset(temp,0,sizeof(temp));
	memset(buffer,0,sizeof(buffer));
	sprintf(temp,"%%s,%%s,%%s,%%s,%%dM,%%s,%%s,%%s\r\n");
	strcpy(down,convertB2KM(tmp->tx_bytes));
	sprintf(buffer,temp,date,mac,apName,ssid,(int)(tmp->rate),down,convertB2KM(tmp->rx_bytes),timeCon);
	/*paste on the tail*/
	strcat(buf,buffer);
    }
OUT:
    return ret;
}
/*
 *功能：保存在线AP信息到文件
 *作者：hec
 *日期：2014-07-30
 *说明：无
 *************************************************************************************/
static void formExportCltInfo(webs_t wp, char_t * path, char_t * query)
{
    char *buf = NULL;
    long int len = STA_TB_SIZE*1024;
    urcp_shm_t *mem_shadow = NULL;
    uint16 i, tb_index;
    sta_entry_t* tmp=NULL;
    char fileName[64];
    time_t now;
    struct tm *timenow;
    buf = (char *)malloc(len);
    if(!buf)
    {
	goto OUT;
    }
    memset(buf,0,len);
    time(&now);
    timenow = localtime(&now);
    sprintf(fileName,"Clients_detail_%d%02d%02d%02d%02d.csv",(1900+timenow->tm_year),(1+timenow->tm_mon),timenow->tm_mday,timenow->tm_hour,timenow->tm_min);
    sprintf(buf,"time,MAC,AP,SSID,wireless rate,dowload,upload,connected time\r\n");
    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0u;i < STA_TB_HASH_SIZE;i++)
	{
	    tb_index = mem_shadow->sta_tb_hash[i].idx;
	    while(tb_index != NONE_NODE)
	    {
		tmp = &(mem_shadow->sta_tb[tb_index-1]);
		if(len > 1024)
		{
		    creatStrOfClt(buf, &len, tmp);
		}
		else
		{
		    break;
		}
		tb_index = tmp->hdr.tb_next;
	    }
	}
    }
    /*读取2.4以前的版本的AP的客户端信息*/
    getInfofromOldVersion(buf,&len);
    wimDownloadFile(wp,fileName,buf,strlen(buf)); 
    free(buf);
OUT:
    return;
}
#if (HISTORY_FLOW_INFO == FYES)
/*
 *功能：构造保存流量信息的字符串，并将字符串粘贴到buf尾部
 *作者：hec
 *日期：2014-07-30
 *说明：无
 * **************************************************************************************/
static int creatStrOfCltFlowInfo(char *buf, int *len, urcp_dev_t *dev_table)
{
    char mac[13], date[25], flow[8192], oneDay[600], *buffer = NULL;
    char all24[24][20], ip[20] = {0},ipTemp[20] = {0};
    char channel[15] = {0}, up[15] ={0}, down[15] = {0}, whichDay[10] = {0};
    time_t nowtime;
    struct tm* timeInfo;
    int i = 0, j = 0, dayPos = 0,cntDay = 0;
    unsigned long long tx = 0,rx = 0;
    wl_info_stat_t *acStat = NULL;
    if(!dev_table)
    {
	return -1;
    }
    acStat = &(dev_table->acStat);

    buffer = (char *)malloc(200*sizeof(wl_his_stat_t) + 8192);
    if(!buffer)
    {
	return -1;
    }
    time(&nowtime);
    timeInfo = localtime(&nowtime);
    sprintf(date,"%04d%02d%02d %02d:%02d:%02d",1900+timeInfo->tm_year,1+timeInfo->tm_mon,timeInfo->tm_mday,timeInfo->tm_hour,timeInfo->tm_min,timeInfo->tm_sec);

    /*从共享内存中读取该AP七天的流量信息*/
    memset(flow,0,sizeof(flow));
    for(i = 1;i <= 7;i++)
    {
	if((8192 - strlen(flow)) < 500)
	{
	    break;
	}
	dayPos = (acStat->wl_his_stat.day_pos + i)%7;
	if(acStat->wl_his_stat.stat_day[dayPos].year)
	{
	    cntDay++;
	    sprintf(whichDay,"day %d",cntDay);
	    strcat(flow,whichDay);
	    strcat(flow,",0:00:00,1:00:00,2:00:00,3:00:00,4:00:00,5:00:00,6:00:00,7:00:00,8:00:00,9:00:00,10:00:00,11:00:00,12:00:00,13:00:00,14:00:00,15:00:00,16:00:00,17:00:00,18:00:00,19:00:00,20:00:00,21:00:00,22:00:00,23:00:00\r\n");
	    memset(oneDay,0,sizeof(oneDay));
	    memset(all24,0,sizeof(all24));
	    for(j = 0;j < 24;j++)
	    {
		if((acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].rx+acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].tx))
		{
		    strcpy(all24[j],convertB2KM(acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].rx+acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].tx));
		    rx += (unsigned long long)acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].rx;
		    tx += (unsigned long long)acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].tx;
		}
		else
		{
		    strcpy(all24[j],"0");
		}
	    }
	    sprintf(oneDay,"%02d%02d%02d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n",acStat->wl_his_stat.stat_day[dayPos].year,acStat->wl_his_stat.stat_day[dayPos].mon,acStat->wl_his_stat.stat_day[dayPos].day,all24[0],all24[1],all24[2],all24[3],all24[4],all24[5],all24[6],all24[7],all24[8],all24[9],all24[10],all24[11],all24[12],all24[13],all24[14],all24[15],all24[16],all24[17],all24[18],all24[19],all24[20],all24[21],all24[22],all24[23]);
	    strcat(flow,oneDay);
	}
    }
    /*mac*/
    sprintf(mac, "%02X%02X%02X%02X%02X%02X",dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
    /*IP*/
    if(dev_table->ip)
    {
	strcpy(ip,int2ip(dev_table->ip,ipTemp));
    }
    else
    {
	strcpy(ip,"");
    }
    /*channel*/
    if((dev_table->wl_info.channel_head != 0) || (dev_table->wl_info.channel_end != 0))
    {
	if(dev_table->wl_info.channel_head > 0)
	{
	    sprintf(channel,"%d",dev_table->wl_info.channel_head);
	}
	else
	{
	    sprintf(channel,"自动(%d)",dev_table->wl_info.channel_end);
	}
    }
    else
    {
	strcpy(channel,"自动");
    }
    /*up down*/
    if(acStat->tx_byte)
    {
	strcpy(up,convertB2KM(tx));
    }
    else
    {
	strcpy(up,"0");
    }
    if(acStat->rx_byte)
    {
	strcpy(down,convertB2KM(rx));
    }
    else
    {
	strcpy(down,"0");
    }
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"%s,%s,%s,%s,%d,%s,%s,%s,%s\r\n%s\r\n",date,mac,ip,dev_table->dev_name,dev_table->wl_info.clients,wlModeToStr(dev_table->wl_info.ap_flag),channel,up,down,flow);
    (*len) = (*len) - strlen(buffer);
    if((*len) > 0)
    {
	strcat(buf,buffer);
    }
    else
    {
	free(buffer);
	return -1;
    }
    free(buffer);
    return 0;
}
/*
 *功能：保存所有有记录的AP流量信息到文件中
 *作者：hec
 *日期：2014-07-30
 *说明：无
 * *************************************************************************************/
static void formExportCltFlowInfo(webs_t wp, char_t * path, char_t * query)
{
    urcp_shm_t *mem_shadow = NULL;
    uint32 tb_index;
    int i = 0;
    long int len;
    char *buf = NULL,fileName[30];
    time_t now;
    struct tm *timenow;

    len = MAX_AP_MANAGED*(10*sizeof(wl_his_stat_t) + 8192);
    buf = (char *)malloc(len);
    if(!buf)
    {
	return;
    }
    memset(buf,0,len);
    time(&now);
    timenow = localtime(&now);
    sprintf(fileName,"AP_detail_%d%02d%02d%02d%02d.csv",(1900+timenow->tm_year),(1+timenow->tm_mon),timenow->tm_mday,timenow->tm_hour,timenow->tm_min);
    sprintf(buf,"time,MAC,IP,name,numuber of clients,wireless mode,channel,upload,download\r\n");
    len -= strlen(fileName);
    len -= strlen(buf);

    mem_shadow = get_shm_ptr();
    if(mem_shadow != NULL)
    {
	for(i = 0;i < DEV_HASH_SIZE;i++)
	{
	    tb_index = mem_shadow->dev_tb_hash[i];
	    while(tb_index!=NONE_NODE)
	    {
		if(len > 0)
		{
		creatStrOfCltFlowInfo(buf,&len,&(mem_shadow->dev_table[tb_index - 1]));	
		}
		else
		{
		    break;
		}
		tb_index = (mem_shadow->dev_table[tb_index - 1]).tb_next;
	    }
	}
    }
    wimDownloadFile(wp,fileName,buf,strlen(buf));
    free(buf);
    return;
}
/*
 *功能：向asp输出所有AP名称以及请求的两个AP的统计时间
 *作者：hec
 *日期：2014-07-29
 *说明：无
 *************************************************************************************/
static void putAllFlowTimeAddApnames(webs_t wp, char_t *mac1, char_t *mac2)
{
    int i = 0, k = 0, dayPos = 0, dayPosX = 0, ti = 1, aps = 0;
    int l = 0, flag = 0;
    unsigned char isDate[50],allDate[14][20];
    char_t *mac[50] = {0}, apName[50] = {0};
    uint32 tb_index;
    urcp_shm_t *mem_shadow = NULL;
    wl_info_stat_t *acStat = NULL;
    urcp_dev_t *dev_table = NULL;
    mem_shadow = get_shm_ptr();
    if(mem_shadow == NULL)
    {
	return;
    }
    /*遍历哈希表*/
    for(i = 0u; i < DEV_HASH_SIZE; i++) 
    {
	tb_index = mem_shadow->dev_tb_hash[i];
	while(tb_index!=NONE_NODE) 
	{
	    dev_table = &(mem_shadow->dev_table[tb_index - 1]);
	    /*向页面写Ap名称和Ap的mac*/
	    memset(mac,0,sizeof(mac));
	    memset(apName,0,sizeof(apName));
	    sprintf(mac,"%02X%02X%02X%02X%02X%02X", dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
	    if(strcmp(dev_table->dev_name,"") != 0)
	    {
		sprintf(apName,"%s",dev_table->dev_name);
	    }
	    else
	    {
		sprintf(apName,"%s",mac);
	    }
	    websWrite(wp,"aps[%d] = \"%s\";\n",aps++,apName);
	    websWrite(wp,"apMac[%d] = \"%s\";\n",aps-1,mac);
	    /*--------------------------------------------*/
	    /*向页面写Ap的统计时间*/
	    if((strcmp(mac1, mac) != 0) && (strcmp(mac2, mac) != 0))
	    {
		tb_index = dev_table->tb_next;
		continue;
	    }
	    acStat = &(dev_table->acStat);
	    dayPos = acStat->wl_his_stat.day_pos;
	    for(k = 0;k < 7;k++)
	    {
		dayPosX = (dayPos + k +1)%7;
		memset(isDate,0,sizeof(isDate));
		sprintf(isDate,"%04d-%02d-%02d",acStat->wl_his_stat.stat_day[dayPosX].year,acStat->wl_his_stat.stat_day[dayPosX].mon,acStat->wl_his_stat.stat_day[dayPosX].day);
		if(acStat->wl_his_stat.stat_day[dayPosX].year != 0)
		{
		    /*统计时间有效*/
		    for(l = 1;l < ti;l++)/*allDate[0]默认为"最近1小时"*/
		    {
			if(strcmp(allDate[l],isDate) == 0)
			{
			    flag = 1;
			    break;
			}
		    }
		    if(flag == 0)
		    {
			websWrite(wp, "time[%d] = \"%s\";\n",ti++,isDate);
			strcpy(allDate[ti - 1],isDate);
		    }
		    else
		    {
			flag = 0;
		    }
		}
	    }
	    /*--------------------------------------*/
	    tb_index = dev_table->tb_next;
	}
    }
    return;
}
/*输出AP当前查询的流量信息*/
static int getFlowInfoRequest(webs_t wp,char_t *selectTime,char_t *mac,int inx,char *isUnitM,int *maxNum)
{
    char macTmp[15] = {0}, isDate[15];
    const char *lastHour = NULL;
    uint32 tb_index;
    int i = 0, dataNum = 0, j,minPos,tmpMinPos,dayPos = -1;
    int flowII,flowIIDot,state = 0;
    uint64 flow = 0ull;
    urcp_shm_t *mem_shadow = NULL;
    wl_info_stat_t *acStat = NULL;
    urcp_dev_t *dev_table = NULL;

    lastHour = getTransDataForC(C_LANG_INDEX_AP_FLOW_LAST_HOUR);
    if((lastHour != NULL) && (strcmp(lastHour,selectTime) == 0))
    {
	dataNum = 60;
    }
    else
    {
	dataNum = 24;
    }
    mem_shadow = get_shm_ptr();
    if(mem_shadow == NULL)
    {
	return -1;
    }
    /*遍历哈希表*/
    for(i = 0u; i < DEV_HASH_SIZE; i++) 
    {
	tb_index = mem_shadow->dev_tb_hash[i];
	while(tb_index!=NONE_NODE) 
	{
	    dev_table = &(mem_shadow->dev_table[tb_index - 1]);
	    acStat = &(dev_table->acStat);	   
	    sprintf(macTmp,"%02X%02X%02X%02X%02X%02X", dev_table->mac[0],dev_table->mac[1],dev_table->mac[2],dev_table->mac[3],dev_table->mac[4],dev_table->mac[5]);
	    if(strcmp(macTmp,mac) != 0)
	    {
		tb_index = dev_table->tb_next;
		continue;
	    }
	    state = 1;
	    /*最近一小时*/
	    if(dataNum == 60)
	    {
		if(dev_table->aged != 2)
		{
		    minPos = acStat->wl_stat_current.min_pos + 1; 
		    for(j = 0;j < dataNum;j++)
		    {
			tmpMinPos = (minPos + j)%60;
			flow = (acStat->wl_stat_current.stat_min[tmpMinPos].rx+acStat->wl_stat_current.stat_min[tmpMinPos].tx);
			if(flow == 0ull)
			{
			    flowII = 0;
			    flowIIDot = 0;
			}
			else if(((*isUnitM) == 'G') || (((*isUnitM) != 'M' && (*isUnitM) != 'K') && (flow >= 1000000000ull)))/*G*/
			{
			    *isUnitM = 'G';
			    flowII = (int)(flow/1000000000ull);
			    flowIIDot = (int)((flow%1000000000ull)/10000000ull);
			}else if(((*isUnitM) == 'M') || (((*isUnitM) != 'K') && (flow >= 1000000ull)))
			{
			    *isUnitM = 'M';
			    flowII = (int)(flow/1000000ull);
			    flowIIDot = (int)((flow%1000000ull)/10000ull);
			}else/*K*/
			{
			    *isUnitM = 'K';
			    flowII = (int)(flow/1000ull);
			    flowIIDot = (int)(flow%1000ull);
			}
			if((*maxNum) < flowII)
			{
			    *maxNum = flowII + 10;
			}
			websWrite(wp, "data%d[%d] = %d.%d;",inx,j,flowII,flowIIDot);
		    }
		    websWrite(wp, "\n");
		    break;
		}
		else
		{
		    state = 0;
		    break;
		}
	    }
	    else if(dataNum == 24)
	    {
		for(j = 0;j < 7;j++)
		{
		    memset(isDate,0,sizeof(isDate));
		    sprintf(isDate,"%04d-%02d-%02d",acStat->wl_his_stat.stat_day[j].year,acStat->wl_his_stat.stat_day[j].mon,acStat->wl_his_stat.stat_day[j].day);
		    if(strcmp(selectTime,isDate) == 0)
		    {
			dayPos = j;
			break;
		    }

		}
		if(dayPos == -1)
		{
		    return -1;
		}
		for(j = 0;j < dataNum;j++)
		{
		    flow = acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].rx + acStat->wl_his_stat.stat_day[dayPos].stat_hour[j].tx;
		    if(flow == 0ull)
		    {
			flowII = 0;
			flowIIDot = 0;
		    }
		    else if(((*isUnitM) == 'G') || (((*isUnitM) != 'M' && (*isUnitM) != 'K') && (flow >= 1000000000ull)))
		    {
			*isUnitM = 'G';
			flowII = (int)(flow/1000000000ull);
			flowIIDot = (int)((flow%1000000000ull)/10000000ull);
		    }else if(((*isUnitM) == 'M') || ((*isUnitM) != 'K' && flow >= 1000000ull))
		    {
			*isUnitM = 'M';
			flowII = (int)(flow/1000000ull);
			flowIIDot = (int)((flow%1000000ull)/10000ull);
		    }else
		    {
			*isUnitM = 'K';
			flowII = (int)(flow/1000ull);
			flowIIDot = (int)(flow%1000ull);
		    }
		    if((*maxNum) < flowII)
		    {
			*maxNum = flowII + 10;
		    }
		    websWrite(wp, "data%d[%d] = %u.%u;",inx,j,flowII,flowIIDot);
		}
		websWrite(wp,"\n");
		break;
	    }
	    else
	    {
		return -1;
	    }
	    tb_index = dev_table->tb_next;
	}
	if(state == 1)
	{
	    break;
	}
    }
    if(!state)
    {
	return -1;
    }
    return 0;
}
/******************************************************************
 * 函数：getHistoryFlowInfo
 * 功能：统计AP历史流量并输出至页面
 * 创建：2014-07-10
 * 参数：略
 * 输出：略
 * 返回：略
 * 作者：hec
 * 维护：hec
 *****************************************************************/
static int getHistoryFlowInfo(int eid, webs_t wp, int argc, char **argv)
{
    /*临时变量*/
    int ret = 0,maxNum = 0;
    char buffer[128],isUintM = 'x';
    const char *lastHour = NULL;
    lastHour = getTransDataForC(C_LANG_INDEX_AP_FLOW_LAST_HOUR);
    time_t nowtime;
    struct tm* timeInfo;
    //需要从网页取回来的变量
    char_t *ap1 = NULL;
    char_t *ap2 = NULL;
    char_t *mac1 = NULL;
    char_t *mac2 = NULL;
    char_t *currenttimeT = NULL;
    /*向web写变量*/
    websWrite(wp, "\nvar aps = new Array();\n");
    websWrite(wp, "var apMac = new Array();\n");
    websWrite(wp, "var time = new Array();\n");
    websWrite(wp, "var data1 = new Array();\n");
    websWrite(wp, "var data2 = new Array();\n");
    websWrite(wp, "var currAP1 = \"\";\n");
    websWrite(wp, "var currAP2 = \"\";\n"); 
    websWrite(wp, "var currenttime = \"%s\";\n",lastHour); 
    websWrite(wp, "var unit = \"M\";\n");
    websWrite(wp, "var maxNum = 100;\n");
    /*获取并显示系统当前时间*/
    memset(buffer,0,sizeof(buffer));
    time(&nowtime);
    timeInfo = localtime(&nowtime);
    sprintf(buffer,"%04d-%02d-%02d %02d:%02d",
	    1900+timeInfo->tm_year,1+timeInfo->tm_mon,timeInfo->tm_mday,
	    timeInfo->tm_hour,timeInfo->tm_min);
    websWrite(wp, "var currentsystime = \"%s\";\n", buffer);
    /*获取web页面的变量*/   
    ap1 = websGetVar(wp, T("apNames1"), T(""));//AP 1
    ap2 = websGetVar(wp, T("apNames2"), T(""));//AP 2
    mac1 = websGetVar(wp,T("mac1"),T(""));
    mac2 = websGetVar(wp,T("mac2"),T(""));
    currenttimeT = websGetVar(wp, T("statsticsTime"), T(""));//统计时间
    /*初始化web页面图表*/
    if(strcmp(currenttimeT,"") != 0)
    {
	websWrite(wp, "currenttime = \"%s\";\n", currenttimeT);
    }
    if(strcmp(ap1,"") != 0)
    {
	websWrite(wp, "currAP1 = \"%s\";",ap1);
    }
    if(strcmp(ap2,"") != 0)
    {
	websWrite(wp, "currAP2 = \"%s\";",ap2);
    }
    websWrite(wp, "\ntime[0] = \"%s\";\n",lastHour);
    putAllFlowTimeAddApnames(wp,mac1,mac2);
    ret = getFlowInfoRequest(wp,currenttimeT,mac1,1,&isUintM,&maxNum);
    if(ret == -1)
    {
	websWrite(wp,"var data1 = new Array(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);\n");
    }
    ret = getFlowInfoRequest(wp,currenttimeT,mac2,2,&isUintM,&maxNum);
    if(ret == -1)
    {
	websWrite(wp,"var data2 = new Array(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);\n");
    
    }
    if(maxNum)
    {
	websWrite(wp,"\nmaxNum = %d;\n",maxNum);
    }
    if(isUintM != 'x')
    {
	websWrite(wp,"unit=\"%c\";\n",isUintM);
    }
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
#endif
/******************************************************************
 * 函数：getHistroyInfo
 * 功能：统计历史记录并输出至页面
 * 创建：2014-03-19
 * 参数：略
 * 输出：略
 * 返回：略
 * 作者：cli
 * 维护：cli
 *****************************************************************/
static int getHistroyInfo(int eid, webs_t wp, int argc, char **argv)
{
    int i,j,k;
    unsigned int a;
    unsigned long data[MAX_WL_STATIS_DAY][MAX_WL_STATIS_DATA];
    unsigned char buffer[128];
    urcp_shm_t *mem_shadow = NULL;
    wl_statis_t *wl_statis = NULL;
    time_t rawtime;
    struct tm* timeInfo;

    websWrite(wp, "var HistoryClients = new Array();\n");
    websWrite(wp, "var dateGroup= new Array();\n");

    memset(data,0,sizeof(data));
    memset(buffer,0,sizeof(buffer));
    mem_shadow = get_shm_ptr();
    wl_statis = &(mem_shadow->wl_statis);

    a = wl_statis->current_index;

    /*读取历史信息*/
    for(i=0;i<MAX_WL_STATIS_DAY;i++)
    {
        k = (a>=i) ? (a-i) : (a-i+MAX_WL_STATIS_DAY);
        if(wl_statis->wl_histroy[k].year != 0){
        websWrite(wp, "dateGroup[%d] = \"%04d-%02d-%02d\";\n",
                i,wl_statis->wl_histroy[k].year,wl_statis->wl_histroy[k].mon,wl_statis->wl_histroy[k].day);
        for(j = 0;j < MAX_WL_STATIS_DATA;j++){
            data[i][j] = mem_shadow->wl_statis.wl_histroy[k].wl_data[j].client_num;
        }
        }
    }

    websWrite(wp, "var HistroyNum = %d;\n", MAX_WL_STATIS_DAY*MAX_WL_STATIS_DATA);

    /*获取并显示系统当前时间*/
    time(&rawtime);
    timeInfo = localtime(&rawtime);

    sprintf(buffer,"%04d-%02d-%02d %02d:%02d",
            1900+timeInfo->tm_year,1+timeInfo->tm_mon,timeInfo->tm_mday,
            timeInfo->tm_hour,timeInfo->tm_min);
    websWrite(wp, "var SystemTime = \"%s\";\n", buffer);


    /*输出到页面*/
    for(i = 0;i < MAX_WL_STATIS_DAY;i++)
    {
        for(j = 0;j<MAX_WL_STATIS_DATA;j++){
            websWrite(wp, "HistoryClients[%d] = %d;\n",i*MAX_WL_STATIS_DATA+j, data[i][j]);
        }
    }

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;

}


#endif
