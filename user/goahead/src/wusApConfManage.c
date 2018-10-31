#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <string.h>
#include    <dirent.h>
#include    <time.h>
#include    <uttMachine.h>
#include    "webs.h"
#include    "utils.h"
#include    "usb.h"
#include    "uttfunction.h"
#include    <linux/autoconf.h>
#include    "translate.h"

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
#define  DEBUG_CONFIG  0
#if DEBUG_CONFIG
#define  CONFIG_DEBUG(argx)  printf argx
#else
#define  CONFIG_DEBUG(argx)
#endif

#define CONF_SEND_ALL "1"
#define RAM_CONFIG_PATH "/tmp/urcp/configs/"
#define USB_CONFIG_PATH     "configs/" /*u盘中联动获得配置文件的目录*/

#define RAM_UPDATE_CONFIG_PATH "/tmp/urcp/ap_configs/"
#define USB_UPDATE_CONFIG_PATH     "/ap_configs/" /*u盘中联动获得配置文件的目录*/
#define IMAGE_SIZE      4000000

static enum apConfStatus {
		GETSUCCESS = 0,
		FILEFULL,
		UFULL,
		MEMFULL,
		OPERFAIL,
} apConfStatus_t;

typedef struct _config_item_t{
    const char *item_name; /*配置项名，和页面上名称一样*/
    unsigned char item_id;        /*配置项ID*/
}config_item_t;
#if 0
config_item_t config_items_tab[MAX_CONF_ITEM]={
    {"sysConf",         SYS_CONF},
    {"vlanConf",        VLAN_CONF},
    {"aggrConf",        AGGR_CONF},
    {"portConf",        PORT_CONF},
    {"rateLimitConf",   RATELIMIT_CONF},
    {"arpConf",         ARP_CONF},
};
#endif
typedef urcp_status_t (*urcp_fp)(unsigned char mac[6], unsigned int userIp, unsigned int op_type);
extern void getErrorArr(webs_t wp, urcp_fp fp, uint32 op_type);
extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);
extern void aspOutGlobalVar(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd,unsigned char *szName,unsigned char *managed);

static const char *webMsg[] = {
                               "请插入外部存储设备",
                               "外部存储设备空间不够",
			       "操作失败",
                               };
static void aspOutDirConf(webs_t wp, const char *path, unsigned int *out_index);
static unsigned char gMac[13*MAX_URCP_BATCH_NUM+1];/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gSzName[13*MAX_URCP_BATCH_NUM+1];/*页面保存时要操作的目的SZ,供页面跳转后"get"使用*/
static unsigned char gManaged[2*MAX_URCP_BATCH_NUM+1];/*页面保存时要操作的目的managedEn,供页面跳转后"get"使用*/
static unsigned char gIp[16*MAX_URCP_BATCH_NUM+1];
static unsigned char gPasswd[(MAX_PASSWORD_LEN+1)*MAX_URCP_BATCH_NUM+1];
static int aspRemoteConfGet(int eid, webs_t wp, int argc, char **argv);
static int aspOutPutRemoteApConf(int eid, webs_t wp, int argc, char **argv);
static int aspOutPutApUpdateConf(int eid, webs_t wp, int argc, char **argv);
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
static int aspOutPutRemoteApConfinUSB(webs_t wp, uint32* i);
//static void formLinkageConfSave(webs_t wp, char_t *path, char_t *query);
//static void formLinkageConfSaveAll(webs_t wp, char_t *path, char_t *query);
#endif
static void formApConfFileDel(webs_t wp, char_t *path, char_t *query);
static void formApUpdateConfFileDel(webs_t wp, char_t *path, char_t *query);
static void formApConfFileDelAll(webs_t wp, char_t *path, char_t *query);
static void formApUpdateConfFileDelAll(webs_t wp, char_t *path, char_t *query);
static int aspRemoteConfSend(int eid, webs_t wp, int argc, char **argv);
static void formApConfFileReName(webs_t wp, char_t *path, char_t *query);
static void formApUpdateConfFileReName(webs_t wp, char_t *path, char_t *query);
static int aspOutPutUploadConf(int eid, webs_t wp, int argc, char_t **argv);
static void formUploadConf(webs_t wp, char * path, char * query);
extern void ramFree(long long *freeDisk);
/********************************************************************
 * 函数： formDefineTopoyDiscovery
 * 功能： 页面与交互转换函数
 / 参数： 无
 * 输出： 无
 * 返回： 无
 * 作者： KeHuatao
 * 创建： 2010-09-20
 *******************************************************************/
extern void formDefineApConfManage(void)
{
//  websAspDefine(T("aspRemoteConfGet"), aspRemoteConfGet);
//  websAspDefine(T("aspRemoteConfSend"), aspRemoteConfSend);
//#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
//    websFormDefine(T("formLinkageConfSave"), formLinkageConfSave);
//    websFormDefine(T("formLinkageConfSaveAll"), formLinkageConfSaveAll);
//#endif
//
    websAspDefine(T("aspOutPutRemoteApConf"), aspOutPutRemoteApConf);
    websAspDefine(T("aspOutPutApUpdateConf"), aspOutPutApUpdateConf);
    websFormDefine(T("formApConfFileDel"), formApConfFileDel);
    websFormDefine(T("formApUpdateConfFileDel"), formApUpdateConfFileDel);
    websFormDefine(T("formApConfFileDelAll"), formApConfFileDelAll);
    websFormDefine(T("formApUpdateConfFileDelAll"), formApUpdateConfFileDelAll);
    websFormDefine(T("formApConfFileReName"), formApConfFileReName);
    websFormDefine(T("formApUpdateConfFileReName"), formApUpdateConfFileReName);
    websAspDefine(T("aspOutPutUploadConf"), aspOutPutUploadConf);
    websFormDefine(T("formUploadConf"), formUploadConf);
}

static void formUploadConf(webs_t wp, char * path, char * query)
{
	char *     fn;
	char *     bn = NULL;
	unsigned char *postData=NULL;
	int fnlen, len, count = 0, ret = 0, isUsb = 0; 
	image_header_t *head;
	FILE *p = NULL;
	int fd;
	char tp[128];
	const char *ErrMsg = NULL;
	char tmp_path[256];
	long long freeDisk = 0;
	unsigned long crc = 0u;

	a_assert(websValid(wp));

	fnlen = wp->lenPostData;
	if(fnlen <= 4)
	{
	    ErrMsg = "非法文件！";
	    setTheErrorMsg((char *)ErrMsg);
	    goto OUT;
	}
	postData = wp->postData;
	wp->postData = NULL;
	wp->lenPostData = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		if (-1 != storageFree(tmp_path, NULL, &freeDisk)) {
			if (freeDisk > IMAGE_SIZE) {
				isUsb = 1;
			} else {
				ErrMsg = getTransDataForC(C_LANG_INDEX_FLASH_SPACE_NOT_ENOUGH);
				setTheErrorMsg((char *)ErrMsg);
				goto OUT;
			}
		}
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		isUsb = 0;
#else
		strcat(tmp_path, "/tmp/urcp/");
		isUsb = 0;
#endif
	}

	crc = *((unsigned long *)postData);
	//printf("crc = %u\n", crc);
	if (crc != crc32(0, postData + sizeof(crc), fnlen - sizeof(crc))) {
		ErrMsg=getTransDataForC(C_LANG_INDEX_CONFIG_FILE_FORMAT);
		setTheErrorMsg(ErrMsg);
		goto OUT;
	}

	ramFree(&freeDisk);
	if (freeDisk <  IMAGE_SIZE) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_MEMERY_SPACE_NOT_ENOUGH);
		setTheErrorMsg((char *)ErrMsg);
		goto OUT;
	}

	fn = websGetVar(wp, T("filename"), T(""));
	if (fn != NULL && *fn != '\0') {        
		//if ((int)(bn = gstrrchr(fn, '/') + 1) == 1) {
		if ((bn = gstrrchr(fn, '\\')) != NULL) {
			fn = bn+1;
		}
		//}
	}

	if (isUsb == 0) {
		// 没有U盘的情况
		memset(tp, 0, sizeof(tp));
		strcat(tp, tmp_path);
		strcat(tp, fn);
		p = fopen(tp, "wb");
		if(p != NULL)
		{
			if(fwrite(postData + sizeof(crc), fnlen - sizeof(crc), 1, p) != 1)
			{
				printf("write file error !!!\n");
			}
			fclose(p);

		}
		else
		{
			printf("open file error !!!");
			goto OUT;
		}
	} else {
		// 有U盘的情况
		memset(tp, 0, sizeof(tp));
		strcat(tp, tmp_path);
		strcat(tp, fn);
		fd = open(tp, O_WRONLY | O_CREAT);
		if (fd != -1) {
			len = write(fd, postData + sizeof(crc), fnlen - sizeof(crc));
			if (len != fnlen - sizeof(crc)) {
				close(fd);
				unlink(tmp_path);
			}

			close(fd);

		}
	}

			doSystem("gzip -d %s; tar xvf %s/*.tar -C %s; rm -rf %s/*.tar", tp, tmp_path, tmp_path, tmp_path);

OUT:
	if(fnlen > 4)
	{
	    sync();
	    doSystem("echo 3 > /proc/sys/vm/drop_caches");
	}
	if(postData) {
		bfree(B_L, postData);
	}

#if (UTT_NV_D908W == FYES)
    /*保存所有AP配置到FALSH中*/
    sleep(2); /*等待2是，解压完后再保存AP配置*/
    apConfigWriteToFlash();
#endif
	websRedirect(wp, "apUploadConf.asp");
	return;
}

static int aspOutPutUploadConf(int eid, webs_t wp, int argc, char_t **argv)
{
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/*
 *页面提交后，需保存页面上的macStr,ipStr,passStr等信息
 *供页面跳转时重新读取到页面上
 *KeHuatao
 *
 */
void saveGlobalVar(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd,unsigned char *szName)
{
    char *strMac, *strIp, *strPasswd,*strSzName;
    uint32 i;

    /*初始化*/
    memset(mac, 0, sizeof(gMac));
    memset(szName, 0, sizeof(gSzName));
    memset(ip, 0, sizeof(gIp));
    memset(passwd, 0, sizeof(gPasswd));
    /*读取*/
    strMac = websGetVar(wp, "macStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    strSzName = websGetVar(wp, "szName", T(""));
    /*保存*/
    i=(uint32)sizeof(gMac)-1u;
    strncpy(mac, strMac, i);
    i=(uint32)sizeof(gIp)-1u;
    strncpy(ip, strIp, i);
    i=(uint32)sizeof(gPasswd)-1u;
    strncpy(passwd, strPasswd, i);
    i=(uint32)sizeof(gSzName)-1u;
    strncpy(szName, strSzName, i);

    return;
}
/*
 *将之前保存的mac等信息打印到页面
 *KeHuatao
 *
 */
void aspOutGlobalVar(webs_t wp, unsigned char *mac, unsigned char *ip, unsigned char *passwd,unsigned char *szName,unsigned char *managed)
{
    char *strMac, *strIp, *strPasswd, *urcpSet,*strSzName,*strManaged;

    /*判断是页面跳转方式*/
    urcpSet = websGetVar(wp, "urcpSet", T(""));
    if(strcmp(urcpSet, T(""))==0)
    {
	/*非页面保存后的跳转*/
	strMac = websGetVar(wp, "macStr", T(""));
	strPasswd = websGetVar(wp, "passStr", T(""));
	strIp = websGetVar(wp, "ipStr", T(""));
	strSzName = websGetVar(wp, "szName", T(""));
	strManaged = websGetVar(wp, "managedEn", T(""));
    }
    else
    {
	/*页面保存后的跳转*/
	strMac = mac;
	strPasswd = passwd;
	strIp = ip;
	strSzName = szName;
	strManaged = managed;
    }

    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", strSzName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);

    return;
}

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
/*
 *打印u盘内的的配置文件
 *KeHuatao
 */
static int aspOutPutRemoteApConfinUSB(webs_t wp, uint32 *i)
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
/*
 *输出指定目录下的配置文件
 *keHuatao
 */
static int aspOutVersion(webs_t wp, const char *firstLine, unsigned int out_index, const char *file_name)
{
	char str1[128];
	char *p;
	unsigned int serial = 0;

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

	if (NULL != (p = strstr(firstLine, "serial"))) {
		p = p + sizeof("serial");

		sscanf(p, "%[^<]", str1);

		websWrite(wp, "serialarray[%d] = \"%u\";\n", out_index, strtoul(str1, NULL, 10));
	} else {
		sscanf(file_name, "%u.xml", &serial);
		websWrite(wp, "serialarray[%d] = \"%u\";\n", out_index, serial);
	}


	if ('\0' != *str1) {
		return 1;
	}

	return 0;
}

static void aspOutDirConf(webs_t wp, const char *path, unsigned int *out_index)
{
	unsigned char tmp[256], file_name[256];
	unsigned char firstLine[256];
	off_t size1;
	DIR *dir_info = NULL;
	FILE *fp = NULL;
	struct dirent *dir_entry = NULL;
	struct stat buf1;
	struct tm *t = NULL;

	dir_info = opendir(path);
	if (NULL != dir_info) {
		while (1) {
			dir_entry = readdir(dir_info);
			if (NULL != dir_entry) {
				if ((0 != strcmp(dir_entry->d_name, ".")) && (0 != strcmp(dir_entry->d_name, ".."))) {
					memset(tmp, 0, sizeof(tmp));
					memset(file_name, 0, sizeof(file_name));
					strncpy(file_name, dir_entry->d_name, sizeof(dir_entry->d_name));
					strcat(tmp, path);
					strcat(tmp, dir_entry->d_name);

					if ((0 == stat(tmp, &buf1)) && (S_ISREG(buf1.st_mode))) {
						websWrite(wp, "namearray[%d] = \"%s\";\n", *out_index, dir_entry->d_name);
						websWrite(wp, "statuarray[%d] = %d;\n", *out_index, 0);
						size1 = buf1.st_size;

						if (size1 >= 1024) {
							size1 = size1 >> 10;
							websWrite(wp, "filesizearray[%d] = \"%dKB\";\n", *out_index, size1);
						} else {
							websWrite(wp, "filesizearray[%d] = \"%dB\";\n", *out_index, size1);
						}

						t = gmtime(&buf1.st_mtime);
						if (t != NULL) {
							sprintf(firstLine, "%d-%02d-%02d %02d:%02d:%02d",
								(1900 + t->tm_year), (1 + t->tm_mon), t->tm_mday,
								t->tm_hour, t->tm_min, t->tm_sec);
							websWrite(wp, "uptimearray[%d] = \"%s\";\n", *out_index, firstLine);
						}

						fp = fopen(tmp, "r");
						if (!fp) {
							perror(__func__);
							return;
						}

						memset(firstLine, 0, sizeof(firstLine));
						while (fgets(firstLine, sizeof(tmp), fp)) {
							if (1 == aspOutVersion(wp, firstLine, *out_index, file_name)) {
								break;
							}
						}

						fclose(fp);

						*out_index = *out_index + 1u;
						if (*out_index >= MAX_AP_MANAGED * 2) {
							break;
						}
					}
				}
			}
			else {
				break;
			}
		}

		closedir(dir_info);
	}

	websWrite(wp, "var count = \"%d\";\n", *out_index);
	websWrite(wp, "var totalrecs = \"%d\";\n", MAX_AP_MANAGED * 2);

	return;
}

/********************************************************************
 * 函数： aspOutPutApUpdateConf
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspOutPutApUpdateConf(int eid, webs_t wp, int argc, char **argv)
{
    const char *ErrMsg = NULL;
    char *waitOver=NULL, *optType=NULL;
    uint32 i=0u;
    long long outer_flash_total_space=0;
    char dat[32]={'\0'};
	char tmp_path[256];
	extern int apConfInfo;

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    getOuterFlashStat(&outer_flash_total_space,NULL);
    sprintf(dat, "%lld", outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", dat);/*总空间*/
#endif

    websWrite(wp, "var timearray = new Array();\n");
    websWrite(wp, "var serialarray = new Array();\n");
    websWrite(wp, "var namearray = new Array();\n");
    websWrite(wp, "var types = new Array();\n");
    websWrite(wp, "var softVersion = new Array();\n");
    websWrite(wp, "var firmVersion = new Array();\n");
    websWrite(wp, "var filesizearray= new Array();\n");
    websWrite(wp, "var statuarray = new Array();\n");
    websWrite(wp, "var uptimearray = new Array();\n");
    websWrite(wp, "var urcpSet = \"1\";\n");

    /*mac,ip,passwd*/
	aspOutGlobalVar(wp, gMac, gIp, gPasswd,gSzName,gManaged);

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_UPDATE_CONFIG_PATH);
#endif
	}
    /*读取内存文件*/
    aspOutDirConf(wp, tmp_path, &i);
    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "getApConf")==0)
	{
	    /*读取配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_GET_GENERAL);
	}
	else if(strcmp(optType, "sendApTemp")==0)
	{
	    /*下发配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_SET_GENERAL);
	}
    }

	if (apConfInfo != 0) {
		if (apConfInfo == FILEFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_FILE_NUM_IS_MAX);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else if (apConfInfo == UFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_FLASH_SPACE_NOT_ENOUGH);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else if (apConfInfo == MEMFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MEMERY_SPACE_NOT_ENOUGH);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATE_FAILED);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		}
	}

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/********************************************************************
 * 函数： aspOutPutRemoteApConf
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspOutPutRemoteApConf(int eid, webs_t wp, int argc, char **argv)
{
    const char *ErrMsg = NULL;
    char *waitOver=NULL, *optType=NULL;
    uint32 i=0u;
    long long outer_flash_total_space=0;
    char dat[32]={'\0'};
	char tmp_path[256];
	extern int apConfInfo;

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
    getOuterFlashStat(&outer_flash_total_space,NULL);
    sprintf(dat, "%lld", outer_flash_total_space);
    websWrite(wp, "var outerFlashTotalSpace = \"%s\";\n", dat);/*总空间*/
#endif

    websWrite(wp, "var timearray = new Array();\n");
    websWrite(wp, "var namearray = new Array();\n");
    websWrite(wp, "var types = new Array();\n");
    websWrite(wp, "var softVersion = new Array();\n");
    websWrite(wp, "var firmVersion = new Array();\n");
    websWrite(wp, "var filesizearray= new Array();\n");
    websWrite(wp, "var statuarray = new Array();\n");
    websWrite(wp, "var urcpSet = \"1\";\n");

    /*mac,ip,passwd*/
	aspOutGlobalVar(wp, gMac, gIp, gPasswd,gSzName,gManaged);

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
    /*读取内存文件*/
    aspOutDirConf(wp, tmp_path, &i);
    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "getApConf")==0)
	{
	    /*读取配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_GET_GENERAL);
	}
	else if(strcmp(optType, "sendApTemp")==0)
	{
	    /*下发配置*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_SET_GENERAL);
	}
    }

	if (apConfInfo != 0) {
		if (apConfInfo == FILEFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_FILE_NUM_IS_MAX);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else if (apConfInfo == UFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_FLASH_SPACE_NOT_ENOUGH);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else if (apConfInfo == MEMFULL) {
			ErrMsg = getTransDataForC(C_LANG_INDEX_MEMERY_SPACE_NOT_ENOUGH);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		} else {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATE_FAILED);
			websWrite(wp, "var errorArr = new Array(\"%s\");", ErrMsg);
		}
	}

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

#if 0
/********************************************************************
 * 函数： aspRemoteConfSend
 * 功能： 页面显示函数
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： KeHuatao
 * 创建： 2011-04-26
 ********************************************************************/ 
static int aspRemoteConfSend(int eid, webs_t wp, int argc, char **argv)
{
    int i=0;
    uint32 j;
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
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
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
    aspRemoteFileGet(wp, GET_CONFIG); 
    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}
#endif

#if 0
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
	error_index = WEB_MSG_NO_DISK;
    }

    /*出错信息*/
    if(error_index!=-1)
    {
	setTheErrorMsg(webMsg[(error_index)]);
    }

    /*保存mac信息*/
    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);

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
	error_index = WEB_MSG_NO_DISK;
    }

    /*出错信息*/
    if(error_index!=-1)
    {
	setTheErrorMsg(webMsg[(error_index)]);
    }
    /*保存mac信息*/
    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);

    websRedirectUrcp(wp, "switch/linkageConf.asp");

    return;
}
#endif
#endif
/*******************************************************************
 *函数： formApConfFileDel
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formApConfFileDel(webs_t wp, char_t *path, char_t *query)
{
    char tp[256];
    char *filename, *isInUSB=tp;
    char *tmp, *ptr;
    unsigned int i=0u, len, out=0u;
    const char *ErrMsg = NULL;
	char tmp_path[256];

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

    filename = websGetVar(wp, "delstr", T("")); /*要删除的文件名*/
    isInUSB = websGetVar(wp, "statustr", T("")); /*是否存在u盘中的文件*/
    tmp = strtok_r(filename, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
    while(tmp!=NULL)
    {
	    memset(tp, 0, sizeof(tp));
		len = sizeof(tp) - 1u - (unsigned int)strlen(RAM_CONFIG_PATH);
		strcat(tp, tmp_path);
	    strncat(tp, tmp, len);
	    CONFIG_DEBUG(("remove file:%s\n", tp));
	    if(unlink(tp)!=0)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("操作失败"));
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

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);

    websRedirect(wp, "getApConf.asp?urcpSet=1");

    return;
}

static void formApUpdateConfFileDel(webs_t wp, char_t *path, char_t *query)
{
    char tp[256];
    char *filename, *isInUSB=tp;
    char *tmp, *ptr;
    unsigned int i=0u, len, out=0u;
    const char *ErrMsg = NULL;
	char tmp_path[256];

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_UPDATE_CONFIG_PATH);
#endif
	}

    filename = websGetVar(wp, "delstr", T("")); /*要删除的文件名*/
    isInUSB = websGetVar(wp, "statustr", T("")); /*是否存在u盘中的文件*/
    tmp = strtok_r(filename, UTT_SPLIT_SIGN_STR, &ptr);/*可能删除多条映射*/
    while(tmp!=NULL)
    {
	    memset(tp, 0, sizeof(tp));
		len = sizeof(tp) - 1u - (unsigned int)strlen(RAM_UPDATE_CONFIG_PATH);
		strcat(tp, tmp_path);
	    strncat(tp, tmp, len);
	    CONFIG_DEBUG(("remove file:%s\n", tp));
	    if(unlink(tp)!=0)
	    {
			ErrMsg = getTransDataForC(C_LANG_INDEX_OPERATION_FAIL);
			setTheErrorMsg((char *)ErrMsg);
			//setTheErrorMsg(T("操作失败"));
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
#if (UTT_NV_D908W == FYES)
    /*保存所有AP配置到FALSH中*/
    apConfigWriteToFlash();
#endif

    saveGlobalVar(wp, gMac, gIp, gPasswd, gSzName);

    websRedirect(wp, "apUpdateConf.asp?urcpSet=1");

    return;
}
/*******************************************************************
 *函数： formApConfFileDelAll
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formApConfFileDelAll(webs_t wp, char_t *path, char_t *query)
{
	char tmp[512];
	char tmp_path[256];

	memset(tmp, '\0', sizeof(tmp));
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

	sprintf(tmp, "rm -rf %s./*", tmp_path);

	system(tmp);
	wait(NULL);

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);
    websRedirect(wp, "getApConf.asp?urcpSet=1");

    return;
}

static void formApUpdateConfFileDelAll(webs_t wp, char_t *path, char_t *query)
{
	char tmp[512];
	char tmp_path[256];

	memset(tmp, '\0', sizeof(tmp));
	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_UPDATE_CONFIG_PATH);
#endif
	}

	sprintf(tmp, "rm -rf %s./*", tmp_path);

	system(tmp);
	wait(NULL);

#if (UTT_NV_D908W == FYES)
	/*保存所有AP配置到FALSH中*/
	apConfigWriteToFlash();
#endif
    saveGlobalVar(wp, gMac, gIp, gPasswd, gSzName);
    websRedirect(wp, "apUpdateConf.asp?urcpSet=1");

    return;
}
/*******************************************************************
 *函数： formApConfFileReName
 *功能： 页面提交函数
 *参数： 
 *返回： 无
 *作者： KeHuatao
 *创建： 2011-04-28
 ******************************************************************/
static void formApConfFileReName(webs_t wp, char_t *path, char_t *query)
{
    char *old_name, *new_name;
    char buf1[256], buf2[256];
    const char *ErrMsg = NULL;
	char tmp_path[256];

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)){
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
    CONFIG_DEBUG(("%s\n", __func__));
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
//	tmp = websGetVar(wp, "statustr", T(""));
    old_name = websGetVar(wp, "oldName", T(""));
    new_name = websGetVar(wp, "newName", T(""));

	if (NULL != tmp_path) {
		strcat(buf1, tmp_path);
		strcat(buf1, old_name);
		strcat(buf2, tmp_path);
		strcat(buf2, new_name);
	}

	if (0 != rename(buf1, buf2)) {
		CONFIG_DEBUG(("%S: old name:%s, new name:%s\n", __func__, buf1, buf2));
		ErrMsg = getTransDataForC(C_LANG_INDEX_RENAME_FAILURE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("rename failure"));
	}

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);
    websRedirect(wp, "getApConf.asp?urcpSet=1");

    return;
}

static void formApUpdateConfFileReName(webs_t wp, char_t *path, char_t *query)
{
    char *old_name, *new_name;
    char buf1[256], buf2[256];
    const char *ErrMsg = NULL;
	char tmp_path[256];

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)){
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
	} else
#endif
	{
#if (UTT_WX_2000 == FYES)
		strcat(tmp_path, "/ff/");
		strcat(tmp_path, USB_UPDATE_CONFIG_PATH);
#else
		strcat(tmp_path, RAM_UPDATE_CONFIG_PATH);
#endif
	}

    CONFIG_DEBUG(("%s\n", __func__));
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
//	tmp = websGetVar(wp, "statustr", T(""));
    old_name = websGetVar(wp, "oldName", T(""));
    new_name = websGetVar(wp, "newName", T(""));

	if (NULL != tmp_path) {
		strcat(buf1, tmp_path);
		strcat(buf1, old_name);
		strcat(buf2, tmp_path);
		strcat(buf2, new_name);
	}

	if (0 != rename(buf1, buf2)) {
		CONFIG_DEBUG(("%S: old name:%s, new name:%s\n", __func__, buf1, buf2));
		ErrMsg = getTransDataForC(C_LANG_INDEX_RENAME_FAILURE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("rename failure"));
	}

#if (UTT_NV_D908W == FYES)
	/*保存所有AP配置到FALSH中*/
	apConfigWriteToFlash();
#endif
    saveGlobalVar(wp, gMac, gIp, gPasswd, gSzName);
    websRedirect(wp, "apUpdateConf.asp?urcpSet=1");

    return;
}
#endif
