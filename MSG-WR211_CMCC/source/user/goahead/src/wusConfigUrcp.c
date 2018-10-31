
/*
 *批量获取配置或下方配置交互文件
 *
 *KeHuatao
 *
 */
#include    <stdio.h>
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <string.h>
#include    <dirent.h>
#include    "webs.h"
#include    "utils.h"
#include    "usb.h"
#include    "uttfunction.h"
#include    <linux/autoconf.h>
#include    <uttMachine.h>

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include    <urcp_ipc.h>
#include    <switch.h>
#include    <sw_cfg.h>
#include    "webmsg.h"
#endif

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
#define  LINUX_MAX_NAME 255
#define  DEBUG_CONFIG  1
#if DEBUG_CONFIG 
#define  CONFIG_DEBUG(argx)  printf argx
#else
#define  CONFIG_DEBUG(argx) 
#endif

#define CONF_SEND_ALL "1"
#define IMAGE_SIZE 5000000
#define CONFIG_SIZE 1000
 
typedef struct _config_item_t{
    const char *item_name; /*配置项名，和页面上名称一样*/   
    unsigned char item_id;        /*配置项ID*/
}config_item_t;
config_item_t config_items_tab[MAX_CONF_ITEM]={
    {"sysConf",         SYS_CONF},
    {"vlanConf",        VLAN_CONF},
    {"aggrConf",        AGGR_CONF},
    {"portConf",        PORT_CONF},
    {"rateLimitConf",   RATELIMIT_CONF},
    {"arpConf",         ARP_CONF},
};
typedef urcp_status_t (*urcp_fp)(unsigned char mac[6], unsigned int userIp, unsigned int op_type);
extern void getErrorArr(webs_t wp, urcp_fp fp, uint32 op_type);
extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);

int apConfInfo;
static enum apConfStatus {
		GETSUCCESS = 0,
		FILEFULL,
		UFULL,
		MEMFULL,
		OPERFAIL,
} apConfStatus_t;

static const char *webMsg[] = {
                               "请插入外部存储设备",
                               "外部存储设备空间不够",
			       "操作失败",
				   "内存空间不足",
                               };
static unsigned char gMac[13*MAX_URCP_BATCH_NUM+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gIp[16*MAX_URCP_BATCH_NUM+1];
static unsigned char gPasswd[(MAX_PASSWORD_LEN+1)*MAX_URCP_BATCH_NUM+1];
static int aspRemoteConfGet(int eid, webs_t wp, int argc, char **argv);
static int aspRemoteSoftGet(int eid, webs_t wp, int argc, char **argv);
static int aspOutRemoteAPConf(int eid, webs_t wp, int argc, char **argv);
static int aspOutRemoteConf(int eid, webs_t wp, int argc, char **argv);
static void aspOutDirConf(webs_t wp, const char *path, uint32 *out_index);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
static int aspOutRemoteConfinUSB(webs_t wp, uint32* i);
static void formLinkageConfSave(webs_t wp, char_t *path, char_t *query);
static void formLinkageConfSaveAll(webs_t wp, char_t *path, char_t *query);
#endif
static void formLinkageConfDel(webs_t wp, char_t *path, char_t *query);
static void formLinkageConfDelAll(webs_t wp, char_t *path, char_t *query);
static int aspRemoteAPConfSend(int eid, webs_t wp, int argc, char **argv);
static void formLinkageConfReName(webs_t wp, char_t *path, char_t *query);
void aspOut_global_var(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd);

/********************************************************************
 * 函数： formDefineTopoyDiscovery
 * 功能： 页面与交互转换函数
 * 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
extern void formDefineConfUrcp(void)
{
    websAspDefine(T("aspRemoteConfGet"), aspRemoteConfGet);
    websAspDefine(T("aspRemoteSoftGet"), aspRemoteSoftGet);
    websAspDefine(T("aspOutRemoteConf"), aspOutRemoteConf);
    websAspDefine(T("aspRemoteAPConfSend"), aspRemoteAPConfSend);
    websAspDefine(T("aspOutRemoteAPConf"), aspOutRemoteAPConf);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    websFormDefine(T("formLinkageConfSave"), formLinkageConfSave);
    websFormDefine(T("formLinkageConfSaveAll"), formLinkageConfSaveAll);
#endif
    websFormDefine(T("formLinkageConfDel"), formLinkageConfDel);
    websFormDefine(T("formLinkageConfDelAll"), formLinkageConfDelAll);
    websFormDefine(T("formLinkageConfReName"), formLinkageConfReName);
}

/********************************************************************
 * 函数： aspOutRemoteConf
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspOutRemoteConf(int eid, webs_t wp, int argc, char **argv)
{
    char *waitOver=NULL, *optType=NULL;
    uint32 i=0u;
    long long outer_flash_total_space=0;
    char dat[32]={'\0'};

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    getOuterFlashStat(&outer_flash_total_space,NULL);
    sprintf(dat, "%lld", outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", dat);/*总空间*/
#endif
 
    /*文件名*/
    websWrite(wp, "var namearray = new Array();\n");
    /*文件创建时间*/
    websWrite(wp, "var timearray = new Array();\n");
    /*是否已经保到u盘中*/
    websWrite(wp, "var statuarray = new Array();\n");
    websWrite(wp, "var filesizearray = new Array();\n");
    /*mac,ip,passwd*/
    aspOut_global_var(wp, gMac, gIp, gPasswd);
    /*读取内存文件*/
    aspOutDirConf(wp, RAM_CONFIG_PATH, &i);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    aspOutRemoteConfinUSB(wp, &i);
#endif
    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "getConf")==0)
	{
	    /*读取配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_GET_GENERAL);
	}
	else if(strcmp(optType, "sendConf")==0)
	{
	    /*下发配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_SET_GENERAL);
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/*
 *页面提交后，需保存页面上的macStr,ipStr,passStr等信息
 *供页面跳转时重新读取到页面上
 *KeHuatao
 *
 */
void save_global_var(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd)
{
    char *strMac, *strIp, *strPasswd;
    uint32 i;

    /*初始化*/
    memset(mac, 0, sizeof(gMac));
    memset(ip, 0, sizeof(gIp));
    memset(passwd, 0, sizeof(gPasswd));
    /*读取*/
    strMac = websGetVar(wp, "macStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    /*保存*/
    i=(uint32)sizeof(gMac)-1u;
    strncpy(mac, strMac, i);
    i=(uint32)sizeof(gIp)-1u;
    strncpy(ip, strIp, i);
    i=(uint32)sizeof(gPasswd)-1u;
    strncpy(passwd, strPasswd, i);

    return;
}
/*
 *将之前保存的mac等信息打印到页面
 *KeHuatao
 *
 */
void aspOut_global_var(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd)
{
    char *strMac, *strIp, *strPasswd, *urcpSet;

    /*判断是页面跳转方式*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*非页面保存后的跳转*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
    }
    else
    {
	/*页面保存后的跳转*/
	strMac = mac;
	strPasswd = passwd;
	strIp = ip;
    }
    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);

    return;
}

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
/*
 *打印u盘内的的配置文件
 *KeHuatao
 */
static int aspOutRemoteConfinUSB(webs_t wp, uint32 *i)
{
    char tp[256];

    CONFIG_DEBUG(("%s\n", __func__));
    /*u 盘是否插上*/
    if(isUSBStorageExist(tp)==1)
    {
        CONFIG_DEBUG(("usb disk found\n"));
	strcat(tp, USB_CONFIG_PATH);
	aspOutDirConf(wp, tp, i);
    }
#if DEBUG_CONFIG
    else
    {
        CONFIG_DEBUG(("no usb disk found\n"));
    }
#endif
    return 0;
}
#endif
static int aspOutVersion(webs_t wp, const char *firstLine, unsigned int out_index)
{
	char str1[128];
	char *p;

	memset(str1, '\0', sizeof(str1));

	if (NULL != (p = strstr(firstLine, "apTypes"))) {
		p = p + sizeof("apTypes");

		sscanf(p, "%[^<]", str1);

		websWrite(wp, "types[%d] = \"%s\";\n", out_index, str1);
	}


	if (NULL != (p = strstr(firstLine, "softVersion"))) {
		p = p + sizeof("softVersion");

		sscanf(p, "%[^<]", str1);

		websWrite(wp, "softVersion[%d] = \"%s\";\n", out_index, str1);
	}

	if (NULL != (p = strstr(firstLine, "firmVersion"))) {
		p = p + sizeof("firmVersion");

		sscanf(p, "%[^<]", str1);

		websWrite(wp, "firmVersion[%d] = \"%s\";\n", out_index, str1);
	}

	if ('\0' != *str1) {
		return 1;
	}

	return 0;
}
/*
 *输出指定目录下的配置文件
 *keHuatao
 */
static void aspOutDirConf(webs_t wp, const char *path, uint32 *out_index)
{
    unsigned char tmp[256];
    off_t size1;
    DIR *dir_info;
    FILE *fp = NULL;
    struct dirent *dir_entry;
    struct stat buf1;

    dir_info = opendir(path);
    if (NULL != dir_info) 
    {
	while (1) 
	{
	    dir_entry = readdir(dir_info);
	    if (NULL != dir_entry) 
	    {
		if ((0 != strcmp(dir_entry->d_name, ".")) 
			&& (0 != strcmp(dir_entry->d_name, ".."))) 
		{
		    memset(tmp, 0, sizeof(tmp));
		    strcat(tmp, path);
		    strcat(tmp, dir_entry->d_name);

		    if ((0 == stat(tmp, &buf1)) && (S_ISREG(buf1.st_mode))) 
		    {
			websWrite(wp, "namearray[%d] = \"%s\";\n", *out_index, dir_entry->d_name);
			websWrite(wp, "statuarray[%d] = %d;\n", *out_index, 0);
			size1 = buf1.st_size;

			if (size1 >= 1024) 
			{
			    size1 = size1 >> 10;
			    websWrite(wp, "filesizearray[%d] = \"%dKB\";\n", *out_index, size1);
			}
			else 
			{
			    websWrite(wp, "filesizearray[%d] = \"%dB\";\n", *out_index, size1);
			}

			fp = fopen(tmp, "r");
			if (!fp) 
			{
			    perror(__func__);
			    return;
			}
#if 1
    unsigned char firstLine[256];

			while (fgets(firstLine, sizeof(tmp), fp)) 
			{
			    if (1 == aspOutVersion(wp, firstLine, *out_index)) 
			    {
				break;
			    }
			}
#endif
			*out_index = *out_index + 1u;
		    }
		}
	    }
	    else 
	    {
		break;
	    }
	}

	closedir(dir_info);
    }

    websWrite(wp, "var totalrecs = \"%d\";\n", *out_index);

    return;
}
/********************************************************************
 * 函数： aspOutRemoteAPConf
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspOutRemoteAPConf(int eid, webs_t wp, int argc, char **argv)
{
    char *waitOver=NULL, *optType=NULL;
    uint32 i=0u;
	char tmp_path[256];

#if 0
    long long outer_flash_total_space=0;
    char dat[32]={'\0'};

    getOuterFlashStat(&outer_flash_total_space,NULL);
    sprintf(dat, "%lld", outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", dat);/*总空间*/
#else
    websWrite(wp, "var outerFlashTotalSpace = \"\";\n");/*总空间*/
#endif

    /*文件名*/
    websWrite(wp, "var namearray = new Array();\n");
    /*文件创建时间*/
    websWrite(wp, "var timearray = new Array();\n");
    /*是否已经保到u盘中*/
    websWrite(wp, "var statuarray = new Array();\n");
    websWrite(wp, "var filesizearray = new Array();\n");
    websWrite(wp, "var types = new Array();\n");
    websWrite(wp, "var softVersion = new Array();\n");
    websWrite(wp, "var firmVersion = new Array();\n");
    /*mac,ip,passwd*/
    aspOut_global_var(wp, gMac, gIp, gPasswd);

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_CONFIG_PATH);
#endif
	}

    /*读取文件*/
    aspOutDirConf(wp, tmp_path, &i);
#if 0
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    aspOutRemoteConfinUSB(wp, &i);
#endif
#endif

    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "getAPConf")==0)
	{
	    /*读取配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_GET_GENERAL);
	}
	else if(strcmp(optType, "sendAPConf")==0)
	{
	    /*下发配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_SET_GENERAL);
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/********************************************************************
 * 函数： aspRemoteAPConfSend
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspRemoteAPConfSend(int eid, webs_t wp, int argc, char **argv)
{
    int i=0, j = 0;
    char *strMac, *strPasswd, *strValue=NULL;
    urcp_send_conf_msg_t urcp_send_conf_msg;

    memset(&urcp_send_conf_msg, 0, sizeof(urcp_send_conf_msg_t));
    /*读页面数据*/
    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    /*取mac地址*/
    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_send_conf_msg.batch_info));
    if(i>0)
    {
	/*文件名*/
	strValue = websGetVar(wp, "remoteSrcConf", T(""));
	i = sizeof(urcp_send_conf_msg.src_file);
	strncpy(urcp_send_conf_msg.src_file, strValue, i);
	strValue = websGetVar(wp, "serialNo", T(""));
	urcp_send_conf_msg.serial_no = (uint32)strtoul(strValue, NULL, 10);
#if 0
	/*待下发文件是否位于u盘中*/
	strValue = websGetVar(wp, "statustr", T(""));
	urcp_send_conf_msg.inUSB = (uint32)strtoul(strValue, NULL, 10);

	/*覆盖方式*/
        strValue = websGetVar(wp, "sendMode", T(""));
	if(strcmp(strValue, CONF_SEND_ALL)==0)
	{/*全部覆盖*/
	    i = 1;
	}
	else
	{/*部分覆盖*/
	    i = 0;
	    /*读取配置项*/
	    for(j=0;j<MAX_CONF_ITEM;j++)
	    {
		strValue= websGetVar(wp, config_items_tab[j].item_name, T(""));
		if(strcmp(strValue, T(""))!=0)
		{
		    /*需下发*/
		    urcp_send_conf_msg.item_id[j]=config_items_tab[j].item_id;
                    i++;
		    CONFIG_DEBUG(("%s: %d\n", __func__, config_items_tab[j].item_id));
		}
	    }
	}

	if(i>0)/*需发送*/
	{
	    /*发消息,通知交换机进程发包*/
	    sendwebmsgbuffer(MSG_TYPE_URCP, SET_CONFIG, (char*)(&urcp_send_conf_msg), sizeof(urcp_send_conf_msg_t));
	}
#else
	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, SET_CONFIG, (char*)(&urcp_send_conf_msg), sizeof(urcp_send_conf_msg_t));
#endif
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

void ramFree(long long *freeDisk)
{
	FILE *fp = NULL;
	char *p;
	unsigned char buffer[512];
	char tmp[20];

	memset(tmp, '\0', sizeof(tmp));
	doSystem("free > /tmp/free_space");
	fp = fopen("/tmp/free_space", "r");
	if (NULL != fp) {
		fgets(buffer, sizeof(buffer), fp);
		memset(buffer, '\0', sizeof(buffer));
		fgets(buffer, sizeof(buffer), fp);
		fclose(fp);
	}
	p = buffer;
	while (*p == ' ') {
		p++;
	}
	CONFIG_DEBUG(("p = %s\n", p));
	sscanf(p, "%*s %*s %*s %s", tmp);

	*freeDisk = atoll(tmp);
	*freeDisk = *freeDisk * 1024;
	CONFIG_DEBUG(("freeDisk = %lld\n", *freeDisk));

	return;
}

static int num_of_file_in_dir(char *path, int *count)
{
	int ret = 0;
	DIR *dir_info = NULL;
	struct dirent *dir_entry;

	if (NULL == path) {
		ret = -1;
		printf("The config path is wrong...\n");
	} else {
		dir_info = opendir(path);
		if (NULL == dir_info) {
			ret = -1;
			printf("Open the config dir failed...");
		} else {
			while (1) {
				dir_entry = readdir(dir_info);
				if (NULL != dir_entry) {
					if ((0 != strcmp(dir_entry->d_name, ".")) &&
						(0 != strcmp(dir_entry->d_name, ".."))) {
						*count = *count + 1;
					}
				} else {
					closedir(dir_info);
					break;
				}
			}
		}
	}

	return ret;
}

/********************************************************************
 * 函数： aspRemoteConfGet
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspRemoteConfGet(int eid, webs_t wp, int argc, char **argv)
{
	char tmp_name[256], tmp_path[256];
	int ret = 0, count = 0;
	long long freeDisk = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_CONFIG_PATH);
#endif
	}

	CONFIG_DEBUG(("tmp_path = %s\n", tmp_path));
	ret = num_of_file_in_dir(tmp_path, &count);
	CONFIG_DEBUG(("count = %d\n", count));
	if ((ret == 0) && (count == 24)) {
		apConfInfo = FILEFULL;
	} else {
		memset(tmp_name, '\0', sizeof(tmp_name));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
		if (1 == isUSBStorageExist(tmp_name)) {
			if (-1 != storageFree(tmp_name, NULL, &freeDisk)) {
				if (freeDisk > CONFIG_SIZE) {
					CONFIG_DEBUG(("freeDisk = %lld\n", freeDisk));
					aspRemoteFileGet(wp, GET_CONFIG);
					apConfInfo = GETSUCCESS;
				} else {
					apConfInfo = UFULL;
				}
			}
		} else
#endif
		{
				ramFree(&freeDisk);
				if (freeDisk > CONFIG_SIZE) {
					aspRemoteFileGet(wp, GET_CONFIG);
					apConfInfo = GETSUCCESS;
				} else {
					apConfInfo = MEMFULL;
				}
		}
	}

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/********************************************************************
 * 函数： aspRemoteSoftGet
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/

static int aspRemoteSoftGet(int eid, webs_t wp, int argc, char **argv)
{
	char tmp_name[256], tmp_path[256];
	int ret = 0, count = 0;
	long long freeDisk = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_IMAGE_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_IMAGE_PATH);
#else
		strcat(tmp_path, RAM_IMAGE_PATH);
#endif
	}

	CONFIG_DEBUG(("tmp_path = %s\n", tmp_path));
	ret = num_of_file_in_dir(tmp_path, &count);
	CONFIG_DEBUG(("count = %d\n", count));
	if ((ret == 0) && (count == 24)) {
		apConfInfo = FILEFULL;
	} else {
		memset(tmp_name, '\0', sizeof(tmp_name));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
		if (1 == isUSBStorageExist(tmp_name)) {
			if (-1 != storageFree(tmp_name, NULL, &freeDisk)) {
				if (freeDisk > IMAGE_SIZE) {
					CONFIG_DEBUG(("freeDisk = %lld\n", freeDisk));
					aspRemoteFileGet(wp, GET_IMAGE);
					apConfInfo = GETSUCCESS;
				} else {
					apConfInfo = UFULL;
				}
			}
		} else
#endif
		{
				ramFree(&freeDisk);
				if (freeDisk > IMAGE_SIZE) {
					aspRemoteFileGet(wp, GET_IMAGE);
					apConfInfo = GETSUCCESS;
				} else {
					apConfInfo = MEMFULL;
				}
		}
	}

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
/*******************************************************************
 *函数： formMirror
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2010-10-12
 ******************************************************************/
static void formLinkageConfSave(webs_t wp, char_t *path, char_t *query)
{
    char *del, *tmp, *ptr;
    int32 error_index=-1;

    del = websGetVar(wp, "delstr", T(""));
    /*U盘是否插入*/
#if (CONFIG_USER_USB_DISK == 1)
    if(isUSBStorageExist(NULL)==1)
    {
	tmp = strtok_r(del, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
	while(tmp!=NULL)
	{
	    error_index = saveFile2USB(tmp, RAM_CONFIG_PATH, USB_CONFIG_PATH);
	    if(error_index!=-1)
	    {
		break;
	    }
	    tmp = strtok_r((char*)NULL, UTT_SPLIT_SIGN_STR, &ptr);
	}
	sync();
    }
    else
    {
#endif
	error_index = WEB_MSG_NO_DISK;
#if (CONFIG_USER_USB_DISK == 1)
    }
#endif
    /*出错信息*/
    if(error_index!=-1)
    {
	setTheErrorMsg(webMsg[(error_index)]);
    }

    /*保存mac信息*/
    save_global_var(wp, gMac, gIp, gPasswd);

    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}

/*******************************************************************
 *函数： formLinkageConfSaveAll
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formLinkageConfSaveAll(webs_t wp, char_t *path, char_t *query)
{
    int32 error_index=-1, out=0;
    DIR* dir_info;
    struct dirent* dir_entry;

#if (CONFIG_USER_USB_DISK == 1)
    /*是否插u盘*/
    if(isUSBStorageExist(NULL)==1)
    {
	dir_info = opendir(RAM_CONFIG_PATH);
	if(dir_info!=NULL)/*读取成功*/
	{
	    /*读取文件*/
	    while(1)
	    {
		/*读取文件*/
		dir_entry = readdir(dir_info);
		if(dir_entry!=NULL)/*成功*/
		{
		    if((strcmp(dir_entry->d_name, "..")!=0) && (strcmp(dir_entry->d_name, ".")!=0))
		    {
			/*保存*/
                        error_index = saveFile2USB(dir_entry->d_name, RAM_CONFIG_PATH, USB_CONFIG_PATH);
			if(error_index!=-1)
			{
			    /*退出*/
			    out=1;
			}
		    }
		}
		else
		{
		    /*退出*/
		    out=1;
		}
		if(out==1)
		{
		    /*退出*/
		    break;
		}
	    }
	    /*关闭*/
	    closedir(dir_info);
	}
	else
	{
	    CONFIG_DEBUG(("%s: open dir\n", __func__));
	    error_index = WEB_MSG_FAIL;
	}

	sync();
    }
    else
    {
#endif
	error_index = WEB_MSG_NO_DISK;
#if (CONFIG_USER_USB_DISK == 1)
    }
#endif

    /*出错信息*/
    if(error_index!=-1)
    {
	setTheErrorMsg(webMsg[(error_index)]);
    }
    /*保存mac信息*/
    save_global_var(wp, gMac, gIp, gPasswd);

    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}
#endif
/*******************************************************************
 *函数： formLinkageConfDel
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formLinkageConfDel(webs_t wp, char_t *path, char_t *query)
{
    char tp[256];
    char *filename, *isInUSB=tp;
    char *tmp, *ptr;
    uint32 i=0u, len, out=0u;

    filename = websGetVar(wp, "delstr", T("")); /*要删除的文件名*/
    isInUSB = websGetVar(wp, "statustr", T("")); /*是否存在u盘中的文件*/
    CONFIG_DEBUG(("filename: %s, isInUSB:%s\n", filename, isInUSB));
    tmp = strtok_r(filename, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
    while(tmp!=NULL)
    {
	if((isInUSB[i])!='\0')
	{
	    memset(tp, 0, sizeof(tp));
	    CONFIG_DEBUG(("USB:%c\n", isInUSB[i]));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	    if(isInUSB[i]=='1')/*u盘*/
	    {
		isUSBStorageExist(tp);
		len = sizeof(tp) - 1u - (uint32)strlen(tp);
		strcat(tp, USB_CONFIG_PATH);
	    }
	    else
#endif
	    {
		len = sizeof(tp) - 1u - (uint32)strlen(RAM_CONFIG_PATH);
		strcat(tp, RAM_CONFIG_PATH);
	    }
	    strncat(tp, tmp, len);
	    CONFIG_DEBUG(("remove file:%s\n", tp));
	    if(unlink(tp)!=0)
	    {
		setTheErrorMsg(webMsg[WEB_MSG_FAIL]);
		out=1u;
	    }
	}
	else/*个数不对*/
	{
	    setTheErrorMsg(webMsg[WEB_MSG_FAIL]);
	    out=1u;
	}
	if(out==1u)
	{
	    break;
	}
	i++;
	/*继续*/
	tmp = strtok_r((char*)NULL, UTT_SPLIT_SIGN_STR, &ptr);
    }

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    sync();
#endif
    /*保存mac信息*/
    save_global_var(wp, gMac, gIp, gPasswd);

    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}
/*******************************************************************
 *函数： formLinkageConfDelAll
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formLinkageConfDelAll(webs_t wp, char_t *path, char_t *query)
{
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    char tp[256];
#endif
    CONFIG_DEBUG(("%s\n", __func__));
    /*删除内存目录*/
    deldir(RAM_CONFIG_PATH);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    /*删除u盘目录*/
    if(isUSBStorageExist(tp)==1)
    {
	strcat(tp, USB_CONFIG_PATH);
	deldir(tp);
	sync();
    }
#endif
    /*保存mac信息*/
    save_global_var(wp, gMac, gIp, gPasswd);

    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}
/*******************************************************************
 *函数： formLinkageConfReName
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formLinkageConfReName(webs_t wp, char_t *path, char_t *query)
{
    char *tmp, *old_name, *new_name;
    char buf1[256], buf2[256];
    const char* file_path=NULL;
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    int32 need_sync=0;
#endif
    CONFIG_DEBUG(("%s\n", __func__));
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
    tmp = websGetVar(wp, "statustr", T(""));
    old_name = websGetVar(wp, "oldName", T(""));
    new_name = websGetVar(wp, "newName", T(""));
    /*是否相同*/
    if(strcmp(old_name, new_name)!=0)
    {
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if(strcmp(tmp, "1")==0)/*u盘文件*/ 
	{
	    /*u盘*/
	    if(isUSBStorageExist(buf1)==1)
	    {
		/*复制u盘挂载点*/
		strcpy(buf2, buf1);
		file_path = USB_CONFIG_PATH;
                need_sync = 1;
	    }
	    else
	    {
		setTheErrorMsg(webMsg[WEB_MSG_NO_DISK]);
	    }
	}
	else/*内存文件*/
#endif
	{
	    file_path = RAM_CONFIG_PATH;
	}

	if(file_path!=NULL)
	{
	    /*补充路径*/
	    strcat(buf1, file_path); 
	    strcat(buf1, old_name); 
	    strcat(buf2, file_path); 
	    strcat(buf2, new_name); 

	    /*重命名*/
	    if(rename(buf1, buf2)!=0)
	    {
		CONFIG_DEBUG(("%s: old name:%s, new name:%s\n", __func__,buf1, buf2));
		setTheErrorMsg(webMsg[WEB_MSG_FAIL]);
	    }
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	    else if(need_sync==1)
	    {
		/*同步*/
		sync();
	    }
#endif
	    else
	    {
	    }
	}
    }

    /*保存mac信息*/
    save_global_var(wp, gMac, gIp, gPasswd);
    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}
#endif
