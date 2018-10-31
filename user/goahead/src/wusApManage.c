#include <stdio.h>
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"
#include <uttMachine.h>

#if (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "mibUrcp.h"
#define AP_TYPE_OTHER  0
#define AP_TYPE_FAT    1
#define AP_TYPE_FIT    2


/***********************************************************************
 * 函 数 名：   aspOutPutScanMode
 * 创建日期：	2012-6-18
 * 修改日期：
 * 作者：       KeHuatao
 * 附加说明：	无
***********************************************************************/
static int aspOutPutScanMode(int eid, webs_t wp, int argc, char_t **argv)
{
    int val=1;
    ApScanProfile *prof;

    prof = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
    {
	if(prof->autoScan==FALSE)
	{
            val = 0;
	}
    }
    websWrite(wp,"var scanMode = %d;\n", val);
    
    return 0;
}
/***********************************************************************
 * 函 数 名：   aspOutPutScanInfo
 * 创建日期：	2012-6-18
 * 修改日期：
 * 作者：       KeHuatao
 * 附加说明：	无
***********************************************************************/
static int aspOutPutScanInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    int mode=1, scanAll=0, num=5;
    ApScanProfile *prof;

    prof = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
    {
	if(prof->autoScan==FALSE)
	{
            mode = 0;
	}
	if(prof->scanAll==PROF_ENABLE)
	{
	    scanAll = 1;
	}
        num = prof->maxAp;
    }
    websWrite(wp,"var maxAp = %d;\n", num);
    websWrite(wp,"var autoScan = %d;\n", mode);
    websWrite(wp,"var allDev = %d;\n", scanAll);
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/*
 * Function: get_dev_authed()
 *
 * Description: device authed
 *	    return value 1: authed	0:unauthed
 *
 * Histroy: added by Shiyu.Chen, 2012.06.25
 */
uint32 get_dev_authed(uint32 serial_no)
{
    uint32 ret = 0u;
#if (AP_PERMISSION == FYES)

    MibProfileType mibType = MIB_PROF_AP_PERMISSION;
    ApPermissionProfile *prof = NULL;
    int min = 0, max =0, index = 0;
    uint32 in_range = 0u;
    
    ProfInstLowHigh(mibType, &max, &min);
    for(index = min+1; index < max; index++)
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0)&& (prof->head.active == TRUE))
	{
	    if((serial_no >= prof->num_start) && (serial_no <= prof->num_end))
	    {
		in_range = 1u;
		break;
	    }
	}
    }
    /* The serial no is not in the range of authorized. */
    if(in_range == 0u)  /* get default value.*/
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, 0);
    }

    if(prof->action == PROF_ENABLE)
    {
	ret = 1u;
    }
#endif

    return ret;

}
extern const char *wlModeToStr(unsigned int mode)
{
    const char *tmp="";

    if(mode & AP_MODE_11BGN)
    {
	tmp = "11b/g/n";
    }
    else if(mode & AP_MODE_11B)
    {
	tmp = "11b";
    }
    else if(mode & AP_MODE_11G)
    {
	tmp = "11g";
    }
    else if(mode & AP_MODE_11N)
    {
	tmp = "11n";
    }
    else if(mode & AP_MODE_11A)
    {
	tmp = "11a";
    }
    else if(mode & AP_MODE_11BG)
    {
	tmp = "11b/g";
    }
    else if(mode & AP_MODE_11AN)
    {
	tmp = "11a/n";
    }
    else if(mode & AP_MODE_11GN)
    {
	tmp = "11g/n";
    }
    else if(mode & AP_MODE_11AC_AN_A)
    {    
	tmp = "11vht AC/AN/A";  
    }    
    else if(mode & AP_MODE_11AC_AN)
    {    
	tmp = "11vht AC/AN";    
    }
    return tmp;
}
/***********************************************************************
 * 函 数 名：   aspOutPutFitApList
 * 创建日期：	2012-6-13
 * 修改日期：
 * 作者：       KeHuatao
 * 附加说明：	无
***********************************************************************/
static int aspOutPutApList(int eid, webs_t wp, int argc, char_t **argv)
{
    unsigned int i=0u, j=0u, k=0u;
    int val, tb_index;
    char buf[48];
    urcp_shm_t *mem_shadow=NULL;
    urcp_dev_t* tmp=NULL;
    ApScanProfile *prof;

    mem_shadow = get_shm_ptr();
    websWrite(wp,"var namearray=new Array();\n");
    websWrite(wp,"var types = new Array();\n");
    websWrite(wp,"var serialno = new Array();\n");
    websWrite(wp,"var apStatus = new Array();\n");
    websWrite(wp,"var tmpSerialno = new Array();\n");
    websWrite(wp,"var iparray = new Array();\n");
    websWrite(wp,"var macarray = new Array();\n");
    websWrite(wp,"var vidarray = new Array();\n");
    websWrite(wp,"var fitOrFat = new Array();\n");
    websWrite(wp,"var channel = new Array();\n");
    websWrite(wp,"var clienCt = new Array();\n");
    websWrite(wp,"var downloads = new Array();\n");
    websWrite(wp,"var uploads = new Array();\n");
    websWrite(wp,"var ssid = new Array();\n");
    websWrite(wp,"var zones = new Array();\n");
    websWrite(wp,"var permission = new Array();\n");
    websWrite(wp,"var managed_enable = new Array();\n");
    websWrite(wp,"var wlMode = new Array();\n");
    websWrite(wp,"var dhcp_en = new Array();\n");
    websWrite(wp,"var mfNames = new Array();\n");

#if 1
    prof = (ApScanProfile *)ProfGetInstPointByIndex(MIB_PROF_AP_SCAN, 0);
    if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active==TRUE))
    {
	if(prof->autoScan==TRUE)
	{
	    /*if the device table was reset befor scaning, just wait a moment*/
	    if(prof->scanAll==TRUE) {
	    /*if scaning all wifi sigal, wait for more time*/
		sleep(2);
	    }
#if 0
	    else{
		sleep(1);
	    }
#endif
	}
    }
#endif
    if(mem_shadow!=NULL)
    {
	for(i = 0u; i < DEV_HASH_SIZE; i++) 
	{
	    tb_index = mem_shadow->dev_tb_hash[i];

	    while(tb_index!=NONE_NODE)
	    {
		tmp = &(mem_shadow->dev_table[tb_index-1]); 

		val = tmp->dev_type;
		if((val&DEV_TYPE_FIT_AP) || (val&DEV_TYPE_FAT_AP) || (val&DEV_TYPE_WIRELESS))
		{
			sprintf(buf, "%d", tmp->dhcp_en);
			websWrite(wp, "dhcp_en[%d] = \"%s\";\n", j, buf);
		    websWrite(wp, "namearray[%d] = \"%s\";\n", j, tmp->dev_name);
		    websWrite(wp, "types[%d] = \"%s\";\n", j, tmp->dev_model);
		    if(tmp->serial_no==0) {
			websWrite(wp, "serialno[%d] = \"\";\n", j);
			websWrite(wp, "tmpSerialno[%d] = \"\";\n", j);
		    }
		    else {
			sprintf(buf, "%u", tmp->serial_no);
			websWrite(wp, "serialno[%d] = \"%s\";\n", j, buf);
			websWrite(wp, "tmpSerialno[%d] = \"%s\";\n", j, buf);
		    }
			if (tmp->aged == 0) {
				websWrite(wp, "apStatus[%d] = \"1\";\n", j);
			} else if (tmp->aged == 2) {
				websWrite(wp, "apStatus[%d] = \"0\";\n", j);
			} else {
				websWrite(wp, "apStatus[%d] = \"1\";\n", j);
			}
		    memset(buf, 0, sizeof(buf));
		    if(tmp->ip!=0)
		    {
			websWrite(wp, "iparray[%d] = \"%s\";\n", j, int2ip(tmp->ip, buf));
		    }
		    else
		    {
			websWrite(wp, "iparray[%d] = \"\";\n", j);
		    }
		    sprintf(buf, "%02X%02X%02X%02X%02X%02X", tmp->mac[0],tmp->mac[1],tmp->mac[2],
							     tmp->mac[3],tmp->mac[4],tmp->mac[5]);
		    websWrite(wp, "macarray[%d] = \"%s\";\n", j, buf);

		    websWrite(wp, "vidarray[%d] = \"%d\";\n", j, tmp->vid);
#if 1
		    if(val & DEV_TYPE_FIT_AP) {
			websWrite(wp, "fitOrFat[%d] = \"2\";\n", j);
		    }
		    else if(val & DEV_TYPE_FAT_AP) {
			websWrite(wp, "fitOrFat[%d] = \"1\";\n", j);
		    }
		    else{
			websWrite(wp, "fitOrFat[%d] = \"0\";\n", j);
		    }
#endif

		    /*是否已授权*/
		    if(val&DEV_FLAG_AUTHED) {
			if(get_dev_authed(tmp->serial_no) == 1u) {

			    websWrite(wp, "permission[%d] = \"1\";\n", j);
			}
			else{
			    websWrite(wp, "permission[%d] = \"0\";\n", j);
			}
		    }
		    else
		    {
			websWrite(wp, "permission[%d] = \"0\";\n", j);
		    }

		    /*是否可管理, 不支持管理协议的设备，未授权的设备*/
		    if(val&DEV_FLAG_MANAGE) {
			websWrite(wp, "managed_enable[%d] = \"1\";\n", j);
		    }
		    else{
			websWrite(wp, "managed_enable[%d] = \"0\";\n", j);
		    }

		    websWrite(wp, "wlMode[%d] = \"%s\";\n", j, wlModeToStr(tmp->wl_info.ap_flag));
		    if ((tmp->wl_info.channel_head != 0) || (tmp->wl_info.channel_end != 0) )
		    {
			if (tmp->wl_info.channel_head > 0)
			{
			     websWrite(wp, "channel[%d] = \"%d\";\n", j, tmp->wl_info.channel_head);
			}
			else
			{
			     websWrite(wp, "channel[%d] = \"自动(%d)\";\n", j, tmp->wl_info.channel_end);
			}
		    }
		    else
		    {
			websWrite(wp, "channel[%d] = \"自动\";\n", j);
		    }
		    websWrite(wp, "clienCt[%d] = \"%d\";\n", j, tmp->wl_info.clients + tmp->wl_info_5G.clients);
#if (HISTORY_FLOW_INFO == FYES)
		    if(strstr(tmp->sw_version,"v1") != NULL)
		    {
			websWrite(wp, "downloads[%d] = \"-\";\n", j);
			websWrite(wp, "uploads[%d] = \"-\";\n", j);
		    }
		    else if( (tmp->aged != 2) && (tmp->wl_info.ap_flag != 0xFFFFFFFFu))
		    {
#if 1
			websWrite(wp, "downloads[%d] = \"%s\";\n", j, convertB2KM((unsigned long long)(tmp->acStat.rx_rate*8)));
			websWrite(wp, "uploads[%d] = \"%s\";\n", j, convertB2KM((unsigned long long)(tmp->acStat.tx_rate*8)));
#else
			websWrite(wp, "downloads[%d] = \"%s\";\n", j, convertB2KM((unsigned long long)tmp->acStat.rx_rate));
			websWrite(wp, "uploads[%d] = \"%s\";\n", j, convertB2KM((unsigned long long)tmp->acStat.tx_rate));
#endif
		    }
		    else
		    {
			websWrite(wp, "downloads[%d] = \"0\";\n", j);
			websWrite(wp, "uploads[%d] = \"0\";\n", j);
		    }
#endif
		    if ( tmp->wl_info.ap_flag != 0xFFFFFFFFu)
		    {
			websWrite(wp, "ssid[%d] = \"", j);
			if(val&DEV_FLAG_MANAGE) {
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
			/*ssid*/

			for(k=0u;k<val;k++)
			{
			    if(k!=0){
				websWrite(wp, ",");
			    }
			    websWrite(wp, "%s", tmp->wl_info.ssid_info[k].ssid);
			}
			websWrite(wp, "\";\n");
			websWrite(wp, "zones[%d] = \"", j);
			/*service zone*/
			for(k=0u;k<val;k++)
			{
			    if(k!=0){
				websWrite(wp, ",");
			    }
			    websWrite(wp, "%s", tmp->wl_info.ssid_info[k].szone);
			}
			websWrite(wp, "\";\n");

			websWrite(wp, "mfNames[%d] = \"", j);
			/*mac filter name*/
			for(k=0u;k<tmp->wl_info.ap_mac_filter_num;k++)
			{
			    if(k!=0){
				websWrite(wp, ",");
			    }
			    websWrite(wp, "%s", tmp->wl_info.mfNames[k]);
			}
			websWrite(wp, "\";\n");
		    }
		    else
		    {
			websWrite(wp, "ssid[%d] = \"\";\n", j);
			websWrite(wp, "zones[%d] = \"\";\n", j);
			websWrite(wp, "mfNames[%d] = \"\";\n", j);
		    }


		    j++;
		}
		tb_index = tmp->tb_next;
	    } 
	}
    }
    websWrite(wp,"var totalrecs = %d;\n", j);
    websWrite(wp,"var max_totalrecs = %d;\n",MAX_AP_MANAGED);

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/*******************************************************************
 *函数： formScanMode
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-11-22
 ******************************************************************/
static void formScanMode(webs_t wp, char_t *path, char_t *query)
{
    char* web_str;
    int web_value, instIndex = 0, changed=0, i;
    ApScanProfile *prof;
    MibProfileType mibType = MIB_PROF_AP_SCAN;

    /*backup*/
    prof = (ApScanProfile *)ProfGetInstPointByIndex(mibType, instIndex);
    if(prof!=NULL)
    {
	/*scan mode*/
	web_str = websGetVar(wp, "mode", T(""));
	if(strcmp(web_str, "auto")==0) {
	    i = TRUE;
	}
	else {
	    i = FALSE;
	}

	if(prof->autoScan!=i)
	{
	    prof->autoScan=i;
	    changed =1;
	}
	
	/*allDevice*/
	web_str = websGetVar(wp, "allDevice", T(""));
	if(strcmp(web_str, T("1"))==0) {
	    i = TRUE;
	}
	else {
	    i = FALSE;
	}
	if(prof->scanAll!=i)
	{
	    prof->scanAll=i;
	    changed =1;
	}

	/*max ap*/
	web_str = websGetVar(wp, "maxMng", T(""));
	web_value = strtoul(web_str, NULL, 10);
	if(web_value>MAX_AP_MANAGED) {
	    i = MAX_AP_MANAGED;
	}
	else {
	    i = web_value;
	}
	if(prof->maxAp!=i)
	{
	    prof->maxAp=i;
	    changed = 1;
	}

	if(changed==1){ 
	    nvramWriteCommit();
            sendwebmsg(MSG_TYPE_GET, SET_AP_SCAN_MODE);
	}
    }

    websRedirect(wp, "apDiscover.asp");

    return;
}
/*******************************************************************
 *函数： formScan
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-11-22
 ******************************************************************/
static void formApDiscover(webs_t wp, char_t *path, char_t *query)
{
    /*发消息*/
    sendwebmsg(MSG_TYPE_GET, START_AP_SCAN);

    websRedirect(wp, "ap_wait_rrcp.asp");
}

#if (AP_PERMISSION == FYES)
/*
 * Function: addPermissionProfile()
 *
 * Description: add one permission profile.
 *
 * History: Shiyu.Chen, 2012.06.25
 */
static void addOnePermissionProfile(uint32 start, uint32 end, ProfileEnableEnum action, uint32 keep)
{
    char newName[32] = {0};
	const char *ErrMsg = NULL;
    MibProfileType mibType = MIB_PROF_AP_PERMISSION;
    ApPermissionProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    uint32 ischanged = 0u;
    int index = -1;

    sprintf(newName, "P");
    index = ProfNewInst(mibType, newName, FALSE);
    if(index == PROFFAIL)
    {
	ErrMsg=getTransDataForC(C_LANG_INDEX_APMANAGE_TIP1);
 
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
	if(prof != NULL)
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, index, &profhead);
	    sprintf(newName, "P%d", index);
	    strcpy(prof->head.name, newName);
	    prof->head.active = TRUE;
	    prof->num_start = start;
	    prof->num_end = end;
	    if(keep == 1u)
	    {
		prof->action = action;
	    }
	    else
	    {
		if(action == PROF_ENABLE)
		{
		    prof->action = PROF_DISABLE;
		}
		else
		{
		    prof->action = PROF_ENABLE;
		}
	    }
	    ischanged = 1u;
	}

	if(ischanged == 1u)
	{
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }

    return;
}

/*
 * Function: webPermissionChange()
 *
 * Description: change AP permission on AP manage page.
 *
 * History: Shiyu.Chen, 2012.06.25
 */
static void webPermissionChange(uint32 serial_no)
{
    MibProfileType mibType = MIB_PROF_AP_PERMISSION;
    ApPermissionProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
    int min = 0, max = 0, index = 0;
    uint32 in_range = 0u, start = 0u, end = 0u;
    ProfileEnableEnum action = PROF_ENABLE;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = (min + 1); index < max; index++) 
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, index);
	if (prof != NULL) 
	{
	    if((serial_no >= prof->num_start) && (serial_no <= prof->num_end))
	    {
		in_range = 1u;
		start = prof->num_start;
		end = prof->num_end;
		action = prof->action;
		break;
	    }
	}
    }
    if(in_range == 0u)
    {
	prof = (ApPermissionProfile *)ProfGetInstPointByIndex(mibType, 0);
	if(serial_no > 0u)
	{
	    addOnePermissionProfile(serial_no, serial_no, prof->action, 0u);
	}
    }
    else
    {
	if(start != end)
	{
	    /*delete found profile by index*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_DEL  , index, &profhead);
	    ProfDelInstByIndex(mibType, index);
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();

	    /*create new profile*/
	    addOnePermissionProfile(serial_no, serial_no, action, 0u);
	    if((start <= (serial_no-1u)) && (serial_no-1u > 0u))
	    {
		addOnePermissionProfile(start, serial_no-1u, action, 1u);
	    }
	    if((serial_no+1u) <= end)
	    {
		addOnePermissionProfile(serial_no+1u, end, action, 1u);
	    }
	}
	else
	{
	    /*edit*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, index, &profhead);
	    if(prof->action == PROF_ENABLE)
	    {
		prof->action = PROF_DISABLE;
	    }
	    else
	    {
		prof->action = PROF_ENABLE;
	    }
	    ProfUpdate(profhead);
	    ProfFreeAllocList(profhead);
	    nvramWriteCommit();
	}
    }

    return;
}

/*
 * Function: formSinglePermissionChange()
 *
 * Description: change AP permission.
 *
 * History: Shiyu.Chen, 2012.06.25
 */
static void formSinglePermissionChange(webs_t wp, char_t *path, char_t *query)
{
    char *tmpstr = NULL;
    uint32 serial_no = 0u;
    const char *ErrMsg = NULL;

    tmpstr = websGetVar(wp,T("serialNo"),T(""));
    if ((strcmp(tmpstr, "") == 0) || (tmpstr == NULL))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_MODIFY_AP);
	setTheErrorMsg(ErrMsg);
	//setTheErrorMsg(T("找不到需要修改权限的AP"));
    }
    else
    {
	serial_no = (uint32)strtoul(tmpstr, NULL, 10);
	if(serial_no > 0)
	{
	    webPermissionChange(serial_no);
	}
    }
    
    websRedirect(wp, "apManage.asp");
    return;
}

/*
 * Function: formMultiPermissionChange()
 *
 * Description: change AP permission.
 *
 * History: Shiyu.Chen, 2012.06.25
 */
static void formMultiPermissionChange(webs_t wp, char_t *path, char_t *query)
{
    char *tmp_str = NULL, *tmp_next = NULL, *serial_str = NULL;
    uint32 serial_no = 0u;
    const char *ErrMsg = NULL;

    tmp_str = websGetVar(wp,T("serialNo"),T(""));
    if ((strcmp(tmp_str, "") == 0) || (tmp_str == NULL))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_CANT_FIND_MODIFY_AP);
	setTheErrorMsg(ErrMsg);
	//setTheErrorMsg(T("找不到需要修改权限的AP"));
    }
    else
    {
	tmp_next = strtok(tmp_str, UTT_SPLIT_SIGN_STR); 
	while(tmp_next != NULL)
	{
	    serial_str = tmp_next;
	    tmp_next = strtok(NULL, UTT_SPLIT_SIGN_STR);
	    serial_no = (unsigned int)strtoul(serial_str, NULL, 10);
	    webPermissionChange(serial_no);
	}
    }
    
    websRedirect(wp, "apManage.asp");

    return;
}

static void formApManageDel(webs_t wp, char_t *path, char_t *query)
{
	char *strMac = NULL, *tmpMac = NULL, *ptr = NULL;
	uchar mac[MAC_LEN];
	urcp_shm_t *mem_shadow = NULL;
	uint32 key;
	int32 index;
	urcp_dev_t *tmp = NULL, *prev = NULL;

	strMac = websGetVar(wp, "delstr", T(""));
    mem_shadow = get_shm_ptr();

	tmpMac = strtok_r(strMac, UTT_SPLIT_SIGN_STR, &ptr);
	while (tmpMac != NULL && mem_shadow != NULL) {
		str2mac(tmpMac, mac);

		key = urcp_dev_table_hash(mac, 0u);
		index = mem_shadow->dev_tb_hash[key];
		if (index != NONE_NODE) {

			while (index != NONE_NODE) {
				tmp = &(mem_shadow->dev_table[index - 1]);
				if (mac_cmp(tmp->mac, mac) == 0) {
					tmp->used = 0;
					tmp->aged = 0;

					if (prev == NULL) {
						mem_shadow->dev_tb_hash[key] = tmp->tb_next;
					} else {
						prev->tb_next = tmp->tb_next;
					}

					break;
				}

				index = tmp->tb_next;
				prev = tmp;
			}
		} else {
			printf("not find the mac: %s\n", tmpMac);
		}

		tmpMac = strtok_r((char *)NULL, UTT_SPLIT_SIGN_STR, &ptr);
	}

    websRedirect(wp, "apManage.asp");

	return 0;
}

static void formApManageDelAll(webs_t wp, char_t *path, char_t *query)
{
	char *strMac = NULL, *mac = NULL, *ptr = NULL;
	urcp_shm_t *mem_shadow = NULL;
	uint32 key, i = 0u;
	int32 index = 0;
	urcp_dev_t *tmp = NULL, *prev = NULL;

	mem_shadow = get_shm_ptr();

	for(i = 0u; i < DEV_HASH_SIZE; i++) {
		prev = NULL;
		index = mem_shadow->dev_tb_hash[i];

		while (index != NONE_NODE) {
			tmp = &mem_shadow->dev_table[index - 1];

			if (tmp->aged == 2) {
				tmp->used = 0;
				tmp->aged = 0;

				if (prev != NULL) {
					prev->tb_next = tmp->tb_next;
				} else {
					mem_shadow->dev_tb_hash[i] = tmp->tb_next;
				}
			} else {
				prev = tmp;
			}

			index = tmp->tb_next;
		}
	}

	websRedirect(wp, "apManage.asp");

	return 0;
}

static void formWlanCltList(webs_t wp, char_t *path, char_t *query)
{
    char *mac = NULL;
    char obj_wp[50] = {0};
    mac = websGetVar(wp,"apMac",T(""));
    sprintf(obj_wp,"wlanCltList.asp?mac=%s",mac);
    websRedirect(wp, obj_wp);
}
#endif /*End AP_PERMISSION*/
#if (FEATURE_AP_MAIL == FYES)

/*******************************************************************
 *函数：aspOutPutApMailConfigInfo
 *功能： 页面输出函数
 *参数：
 *返回： 无
 *创建： 2013-11-01
 ******************************************************************/
static int aspOutPutApMailConfigInfo(int eid, webs_t wp, int argc, char_t **argv)
{
		ApMailProfile *prof;
		MibProfileType mibType = MIB_PROF_AP_MAIL;
		int instIndex = 0;
		char_t *enable;

		websWrite(wp,"var enables = \"\";");
		websWrite(wp,"var senderEmails = \"\";");
		websWrite(wp,"var senderEmailPasswds = \"\";");
		websWrite(wp,"var receiverEmails = \"\";");
		websWrite(wp,"var smtpServers = \"\";");
		websWrite(wp,"var emailTitles = \"\";");
		websWrite(wp,"var senders = \"\";");
		websWrite(wp,"var space_time = \"\";");
		prof = (ApMailProfile *)ProfGetInstPointByIndex(mibType, instIndex);

		uttSemP(SEM_AP_MAIL,0);
		if((prof != NULL) && (ProfInstIsFree(prof) == 0))
		{
				enable = prof->head.active == TRUE ? "on" : "off";
				websWrite(wp,"enables = \"%s\";\n",enable);
				websWrite(wp,"senderEmails = \"%s\";\n",prof->senderEmail);
				websWrite(wp,"senderEmailPasswds = \"%s\";\n",prof->senderEmailPasswd);
				websWrite(wp,"receiverEmails = \"%s\";\n",prof->receiverEmail);
				websWrite(wp,"smtpServers = \"%s\";\n",prof->smtpServer);
				websWrite(wp,"emailTitles = \"%s\";\n",prof->emailTitle);
				websWrite(wp,"space_time = \"%u\";\n",prof->spaceTime);
		}
		uttSemV(SEM_AP_MAIL,0);

}

/*******************************************************************
 *函数：formApMail
 *功能： 页面交互函数
 *参数：
 *返回： 无
 *创建： 2013-11-01
 ******************************************************************/
static void formApMail(webs_t wp, char_t *path, char_t *query)
{
	int instIndex = 0, changed=0;
	char_t *senderEmail = NULL;
	char_t *senderEmailPasswd = NULL;
	char_t *receiverEmail = NULL;
	char_t *smtpServer = NULL;
	char_t *emailTitle = NULL;
	char_t *enable = NULL, *enableOld = NULL;
	char_t *time = NULL;
	Uint32 space_time;

	ApMailProfile *prof = NULL;
	MibProfileType mibType = MIB_PROF_AP_MAIL;
	struProfAlloc *profhead = NULL;
	const char *ErrMsg = NULL;
	enable = websGetVar(wp,"enable",T(""));
	senderEmail = websGetVar(wp,"senderEmail",T(""));
	senderEmailPasswd = websGetVar(wp,"senderEmailPasswd",T(""));
	receiverEmail = websGetVar(wp,"receiverEmail",T(""));
	smtpServer = websGetVar(wp,"smtpServer",T(""));
	emailTitle = websGetVar(wp,"emailTitle",T(""));
	time = websGetVar(wp,"sendSpaceTime",T(""));
	space_time = (Uint32)strtoul(time,NULL,10);

	prof = (ApMailProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	uttSemP(SEM_AP_MAIL,0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) {
		ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, instIndex, &profhead);
		enableOld = prof->head.active == TRUE ? "on" : "off";
		if (strncmp(enable,enableOld,3) != 0) {
			prof->head.active = strncmp(enable,"on",2) == 0 ? TRUE : FALSE;
			changed = 1;
		}
		if (strcmp(senderEmail,prof->senderEmail) != 0) {
			strcpy(prof->senderEmail,senderEmail);
			changed = 1;
		}
		if (strcmp(senderEmailPasswd,prof->senderEmailPasswd) != 0) {
			strcpy(prof->senderEmailPasswd,senderEmailPasswd);
			changed = 1;
		}

		if (strcmp(receiverEmail,prof->receiverEmail) != 0) {
			strcpy(prof->receiverEmail,receiverEmail);
			changed = 1;
		}

		if (strcmp(smtpServer,prof->smtpServer) != 0) {
			strcpy(prof->smtpServer,smtpServer);
			changed = 1;
		}
		if (strcmp(emailTitle,prof->emailTitle) != 0) {
			strcpy(prof->emailTitle,emailTitle);
			changed = 1;
		}
		if (prof->spaceTime != space_time) {
			prof->spaceTime = space_time;
			changed = 1;
		}

		if(changed == 1){
			ProfUpdate(profhead);
			ProfFreeAllocList(profhead);
			nvramWriteCommit();
		}
	}
	uttSemV(SEM_AP_MAIL,0);
	websRedirect(wp, "apMailConfig.asp");
}
#endif

void formDefineApManage(void)
{
    websAspDefine(T("aspOutPutApList"), aspOutPutApList);
    websAspDefine(T("aspOutPutScanMode"), aspOutPutScanMode);
    websAspDefine(T("aspOutPutScanInfo"), aspOutPutScanInfo);
    websFormDefine(T("formScanMode"), formScanMode);
    websFormDefine(T("formApDiscover"), formApDiscover);
    websFormDefine(T("formApManageDel"), formApManageDel);
    websFormDefine(T("formApManageDelAll"), formApManageDelAll);
#if (AP_PERMISSION == FYES)
	websFormDefine(T("formWlanCltList"), formWlanCltList);
    websFormDefine(T("formSinglePermissionChange"), formSinglePermissionChange);
    websFormDefine(T("formMultiPermissionChange"), formMultiPermissionChange);
#endif
#if (FEATURE_AP_MAIL == FYES)
	websFormDefine(T("formApMail"), formApMail);
    websAspDefine(T("aspOutPutApMailConfigInfo"), aspOutPutApMailConfigInfo);
#endif
}
#endif
