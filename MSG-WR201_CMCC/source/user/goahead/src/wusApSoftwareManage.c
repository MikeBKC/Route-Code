
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "flash_api.h"
#include "uttfunction.h"
#include "webmsg.h"
#include <linux/autoconf.h>
#include <sys/utsname.h>
#include <string.h>
#include <dirent.h>
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if (FEATURE_AP_SW_MNG == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "webmsg.h"

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#define DEBUG_SOFT 0
#if DEBUG_SOFT
#define SOFT_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define SOFT_DEBUG(fmt, argx...)
#endif

extern int apConfInfo;
static enum apConfStatus {
		GETSUCCESS = 0,
		FILEFULL,
		UFULL,
		MEMFULL,
		OPERFAIL,
} apConfStatus_t;

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);
extern void ramFree(long long *freeDisk);

#define TFTP_IMAGE_ROOT "/tmp/urcp/images/"  /*save firmware*/
#define AP_MANAGE_RECORD_FILE "ap_manage_record"
#if 0
#define MODEL_1800N	    "WA1800N"
#define MODEL_2000N	    "WA2000N"
#define MODEL_2200N	    "WA2200N"
#define MODEL_510W	    "WA1000N"
#define MODEL_500N      "WA500N"
#define MODEL_1500N     "WA1500N"
#endif
#define IMAGE_SIZE      4000000

#define SWVER_1 8
#define SWVER_2 8
#define SWVER_3 8
#define HWVER_1 4
#define HWVER_2 4



static unsigned char gMac[13*MAX_URCP_BATCH_NUM+1]={'\0'};/*页面保存时要操作的目的mac,供页面跳转后"get"使用*/
static unsigned char gSzName[13*MAX_URCP_BATCH_NUM+1]={'\0'};/*页面保存时要操作的目的SZ,供页面跳转后"get"使用*/
static unsigned char gManaged[2*MAX_URCP_BATCH_NUM+1]={'\0'};/*页面保存时要操作的目的SZ,供页面跳转后"get"使用*/
static unsigned char gIp[16*MAX_URCP_BATCH_NUM+1]={'\0'};
static unsigned char gPasswd[(MAX_PASSWORD_LEN+1)*MAX_URCP_BATCH_NUM+1]={'\0'};
/*
 * Function: aspOutPutRemoteApSoft()
 *
 * Description: output vars to web page.
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */

/********************************************************
 * 把整型硬件版本号转换成可读的字符串形式 如 1.1
 * 参数 ：hwVer 硬件整型版本号
 *		  hwver_buf 转换存储到的字符串地址
 * 返回值 成功  返回 存储在hwver_buf中的版本号字节数
 *		  失败  返回 0
 *******************************************************/
int hwverinfo_to_str(unsigned int hwver, char* hwver_buf){
	int ret = 0;
	unsigned int major,minor;
	major = (hwver>>HWVER_2);
	minor = ((hwver & (0xFFFFFFFFu>>(HWVER_1+SWVER_1+SWVER_2+SWVER_3))) );
	ret = sprintf(hwver_buf,"V%d.%d",major,minor);  
	return ret;
}

/********************************************************
 * 把整型软件版本号转换成可读的字符串形式 如 1.1.1
 * 参数 ：hwVer 软件整型版本号
 *		  hwver_buf 转换存储到的字符串地址
 * 返回值 成功  返回 存储在hwver_buf中的版本号字节数
 *		  失败  返回 0
 *******************************************************/
int swverinfo_to_str(unsigned int swver, char* swver_buf){
	int ret = 0;
	unsigned int major,minor,minor2;
	major = (swver>>(SWVER_2+SWVER_3));
	minor = (swver>>SWVER_3) & (0xFFFFFFFFu>>(HWVER_1+HWVER_2+SWVER_1+SWVER_3));
	minor2 = swver & (0xFFFFFFFFu>>(HWVER_1+HWVER_2+SWVER_1+SWVER_2));
	ret = sprintf(swver_buf,"V%d.%d.%d",major,minor,minor2);
	return ret;
}
#if 0
int hwverinfo_to_str(int hwver, char* hwver_buf){
	int ret = 0;
	int major,minor;
	major = (hwver>>HWVER_2);
	minor = ((hwver & (0xFFFF>>HWVER_1+SWVER_1+SWVER_2+SWVER_3)) );
	ret = sprintf(hwver_buf,"V%d.%d",major,minor);	
	return ret;
}

int swverinfo_to_str(int swver, char* swver_buf){
	int ret = 0;
	int major,minor,minor2;
	major = (swver>>SWVER_2+SWVER_3);
	minor = (swver>>SWVER_3) & (0xFFFF>>HWVER_1+HWVER_2+SWVER_1+SWVER_3);
	minor2 = swver & (0xFFFF>>HWVER_1+HWVER_2+SWVER_1+SWVER_2);
	ret = sprintf(swver_buf,"V%d.%d.%d",major,minor,minor2);
	return ret;
}
#endif

static int num_of_file_in_dir(char *path, int *count)
{
	SOFT_DEBUG("path = %s, count = %d\n", path, *count);
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

static int aspOutPutRemoteApSoft(int eid, webs_t wp, int argc, char_t **argv)
{
    DIR *dir = NULL;
    const char *ErrMsg = NULL;
    struct dirent *ptr;
    struct stat stat_buf;
    off_t size1 = 0;
	char tmp_path[256];
    char product_model[1024], hw_version[512], sw_version[512];
    char file_name[2048], file_size[512], defaults[512];
    char buf[128], tmp[128], tmp_file_name[128], hwVer_str[24], swVer_str[24];
    int first = 1, count = 0, hwVer = 0, swVer = 0;
    image_header_t *head=NULL;
    FILE *p=NULL;
    uint32 eq = 0u;
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;

    ProfInstLowHigh(mibType, &max, &min);

	memset(tmp_path, '\0', sizeof(tmp_path));
    memset(product_model, 0, sizeof(product_model));
    memset(hw_version, 0, sizeof(hw_version));
    memset(sw_version, 0, sizeof(sw_version));
    memset(file_name, 0, sizeof(file_name));
    memset(file_size, 0, sizeof(file_size));
    memset(defaults, 0, sizeof(defaults));
    memset(tmp_file_name, 0, sizeof(tmp_file_name));
    memset(hwVer_str, 0, sizeof(hwVer_str));
    memset(swVer_str, 0, sizeof(swVer_str));

	/* 释放出内存 */
	sync();
	doSystem("echo 3 > /proc/sys/vm/drop_caches");

    websWrite(wp, "var outerFlashTotalSpace = \"\";\n");
    aspOutGlobalVar(wp, gMac, gIp, gPasswd,gSzName,gManaged);

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
		strcat(tmp_path, TFTP_IMAGE_ROOT);
#endif
	}

    dir=opendir(tmp_path);
    if (dir != NULL)
    {
	while ( (ptr = readdir(dir)) != NULL)
	{
	    if ((strcmp (ptr->d_name, ".") != 0) && (strcmp (ptr->d_name, "..") != 0))
	    {
		sprintf(tmp, "%s%s", tmp_path, ptr->d_name);
		strncpy(tmp_file_name, tmp, sizeof(tmp_file_name));
		p = fopen(tmp, "rb");
		if(p == NULL)
		{
		    printf("Open file error !!!");
		}
		else
		{
		    if(fread(buf, sizeof(buf), 1, p) != 1)
		    {
			printf("File read error !\n");
		    }
		    fclose(p);

		    if(first == 1)
		    {
			first = 2;
		    }
		    else
		    {
			strcat(product_model, ",");
			strcat(hw_version, ",");
			strcat(sw_version, ",");
			strcat(file_name, ",");
			strcat(file_size, ",");
			strcat(defaults, ",");
		    }

		    if((stat(tmp, &stat_buf)==0) && (S_ISREG(stat_buf.st_mode)))
		    {
			size1 = stat_buf.st_size;;
			memset(tmp, 0, sizeof(tmp));
			if(size1 >= 1024)
			{
			    /*KB*/
			    size1 = size1>>10;
			    sprintf(tmp, "\"%dKB\"", (int)size1);
			}
			else
			{
			    /*B*/
			    sprintf(tmp, "\"%dB\"", (int)size1);
			}
			strcat(file_size, tmp);
		    }
		    else
		    {
			printf("%s: stat error\n", __func__);
		    }

		    memset(tmp, 0, sizeof(tmp));
		    sprintf(tmp, "\"%s\"",ptr->d_name);
		    strcat(file_name, tmp);

		    head = (image_header_t *)buf;
		    memset(tmp, 0, sizeof(tmp));
		    sprintf(tmp, "\"%s\"", head->name);
		    strcat(product_model, tmp);

		    get_verinfo_from_file(tmp_file_name, &hwVer, &swVer);
		    hwverinfo_to_str(hwVer, hwVer_str);
		    swverinfo_to_str(swVer, swVer_str);

		    memset(tmp, 0, sizeof(tmp));
		    sprintf(tmp,"\"%s\"", swVer_str);
		    SOFT_DEBUG("\n< %s %d > s_tmp:%s\n",__func__,__LINE__,tmp);
		    strcat(sw_version, tmp);

		    memset(tmp, 0, sizeof(tmp));
		    sprintf(tmp,"\"%s\"", hwVer_str);
		    SOFT_DEBUG("\n< %s %d > h_tmp:%s\n",__func__,__LINE__,tmp);
		    strcat(hw_version, tmp);

		    for(index = min; index < max; index++)
		    {
			prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
			if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
			{
//				printf("sw_name = %s, sw_version = %s, sw_model = %s, hw_version = %s, default = %u\n", prof->sw_name, prof->sw_version, prof->sw_model, prof->hw_version, prof->defaulted);
			    if(strcmp(prof->sw_name, ptr->d_name) == 0)
			    {
				eq = 1u;
				break;	
			    }
			}
		    }
		    memset(tmp, 0, sizeof(tmp));
		    if(eq == 1u)
		    {
			sprintf(tmp, "\"yes\"");
		    }
		    else
		    {
			sprintf(tmp, "\"\"");
		    }
		    strcat(defaults, tmp);
		    eq = 0u;

		    count++;
		    if (count >= 24)	/* 最多显示24个条目 */
		    {
			break;
		    }
		}
	    }
	}

	closedir(dir);
    }

    websWrite(wp, "var softwareName = new Array(%s);\n", file_name);
    websWrite(wp, "var softwareVersion = new Array(%s);\n", sw_version);
    websWrite(wp, "var typesForFile = new Array(%s);\n", product_model);
    websWrite(wp, "var firmwareVersion = new Array(%s);\n", hw_version);
    websWrite(wp, "var fileSize = new Array(%s);\n", file_size);
    websWrite(wp, "var defaults = new Array(%s);\n", defaults);
    websWrite(wp, "var totalrecs = %d;\n",count);
    websWrite(wp, "var errorArr = new Array(\"\");\n");

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

static int aspOutPutUploadFirmware(int eid, webs_t wp, int argc, char_t **argv)
{
    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
}
/*
 * Function: formUploadFirmware()
 *
 * Description: upload AP firmware.
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
static void formUploadFirmware(webs_t wp, char * path, char * query) 
{
	static int uploadFlag = 0;
	char *     fn;
	char *     bn = NULL;
	unsigned char *postData;
	int fnlen, len, count = 0, ret = 0, isUsb = 0; 
	image_header_t *head;
	FILE *p = NULL;
	int fd;
	char tp[128];
	const char *ErrMsg = NULL;
	char tmp_path[256];
	long long freeDisk = 0;

	a_assert(websValid(wp));

	fnlen = wp->lenPostData;
	postData = wp->postData;
	wp->postData = NULL;
	wp->lenPostData = 0;

	memset(tmp_path, '\0', sizeof(tmp_path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (1 == isUSBStorageExist(tmp_path)) {
		if (-1 != storageFree(tmp_path, NULL, &freeDisk)) {
			if (freeDisk > IMAGE_SIZE) {
				SOFT_DEBUG("freeDisk = %lld\n", freeDisk);
				strcat(tmp_path, USB_IMAGE_PATH);
				isUsb = 1;
			} else {
				SOFT_DEBUG("line = %d\n", __LINE__);
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
		strcat(tmp_path, USB_IMAGE_PATH);
		isUsb = 0;
#else
		SOFT_DEBUG("line = %d\n", __LINE__);
		strcat(tmp_path, TFTP_IMAGE_ROOT);
		isUsb = 0;
#endif
	}

	SOFT_DEBUG("tmp_path = %s\n", tmp_path);
	ret = num_of_file_in_dir(tmp_path, &count);
	SOFT_DEBUG("count = %d\n", count);
	if ((ret == 0) && (count == 24)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_FILE_NUM_IS_MAX);
		setTheErrorMsg((char *)ErrMsg);
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

	if(uploadFlag == 1u)
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_UPLOADING);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("软件上传中，请勿重复操作!"));
		goto OUT;
	}
	if( fnlen <= sizeof(image_header_t) ){
		ErrMsg = getTransDataForC(C_LANG_INDEX_INVALID_FILE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效的文件"));
		goto OUT;
	}
	head = (image_header_t*)postData;
#if 0
	if((strcmp(MODEL_2200N, head->name) != 0) && (strcmp(MODEL_2000N, head->name) != 0) && (strcmp(MODEL_1800N, head->name) != 0) && (strcmp(MODEL_500N, head->name) != 0) && (strcmp(MODEL_1500N, head->name) != 0) && (strcmp(MODEL_510W, head->name) != 0)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_SOFTWARE_TYPE_ERROR);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("软件型号错误，请重新选择软件"));
		goto OUT;
	}
#endif

	if (crc32 (0, postData+8, fnlen - 8) != ntohl(head->imageCrc)) {
		ErrMsg = getTransDataForC(C_LANG_INDEX_CHECK_SUM_ERROR);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("软件校验和错误"));
		goto OUT;
	}

	if (isUsb == 0) {
		// 没有U盘的情况
		memset(tp, 0, sizeof(tp));
		strcat(tp, tmp_path);
		strcat(tp, fn);
		SOFT_DEBUG("open file:%s\n",tp);
		p = fopen(tp, "wb");
		if(p != NULL)
		{
			uploadFlag = 1;
			if(fwrite(postData, fnlen, 1, p) != 1)
			{
				printf("write file error !!!\n");
			}
			uploadFlag = 0;
			fclose(p);
		}
		else
		{
			printf("open file error !!!");
			goto OUT;
		}
	} else {
		// 有U盘的情况
		strcat(tmp_path, fn);
		fd = open(tmp_path, O_WRONLY | O_CREAT);
		if (fd != -1) {
			len = write(fd, postData, fnlen);
			if (len != fnlen) {
				close(fd);
				unlink(tmp_path);
			}

			close(fd);
		}
	}

OUT:
	sync();
	doSystem("echo 3 > /proc/sys/vm/drop_caches");

	if(postData) {
		bfree(B_L, postData);
	}

	websRedirect(wp, "wlanUpdateUpload.asp");
	return;
}

/*
 * Function: formApSoftDel()
 *
 * Description: delete the software
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
static void formApSoftDel(webs_t wp, char * path, char * query) 
{
    char *delstr = NULL, *next = NULL, *tmp = NULL;
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;
    uint32 ischanged = 0u;
	char tmp_path[256];

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
		strcat(tmp_path, TFTP_IMAGE_ROOT);
#endif
	}

    /*get delstr from web*/
    delstr = websGetVar(wp, T("delstr"), T(""));
    if ((strcmp(delstr, "") == 0) || delstr == NULL)
    {
	setTheErrorMsg(T(""));
    }
    else
    {
	ProfInstLowHigh(mibType, &max, &min);
	next = strtok(delstr, DELSEPARATE);
	while(next != NULL)
	{
	    tmp = next;
	    next = strtok(NULL, DELSEPARATE);
	    for(index = min; index < max; index++)
	    {
		prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
		if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
		{
		    if((strcmp(tmp, prof->sw_name) == 0) && (prof->defaulted == 1u))
		    {
			ProfDelInstByIndex(mibType, index);
			ischanged = 1u;
			break;
		    }
		}
	    }
	    doSystem("rm \"%s%s\" -f", tmp_path, tmp);
	}

	/* profile is changed ? */
	if(ischanged == 1u)
	{
	    nvramWriteCommit(); /*save*/
	}
    }

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);
    websRedirect(wp, "apSoftMng.asp?urcpSet=1");
    return;    
}

/*
 * Function: formApSoftDelAll()
 *
 * Description: delete all software.
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
static void formApSoftDelAll(webs_t wp, char * path, char * query) 
{
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;
    uint32 ischanged = 0u;
	char tmp_path[256];

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
		strcat(tmp_path, TFTP_IMAGE_ROOT);
#endif
	}

    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
	    ProfDelInstByIndex(mibType, index);
	    ischanged = 1u;
	}
    }
    
    if(ischanged == 1u)
    {
	nvramWriteCommit(); 
    }

    doSystem("rm %s* -f", tmp_path);

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);
    websRedirect(wp, "apSoftMng.asp?urcpSet=1");
    return;    
}

/*
 * Function: formSetSoftAsDefault()
 *
 * Description: set software is default.
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
static void formSetSoftAsDefault(webs_t wp, char * path, char * query) 
{
    char *def_name = NULL, *soft_ver = NULL;
    char *for_type = NULL, *firm_ver = NULL;
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;
    uint32 ischanged = 0u, exist =0u;
    const char *ErrMsg = NULL;

    def_name = websGetVar(wp, T("defName"), T(""));
    soft_ver = websGetVar(wp, T("softVer"), T(""));
    for_type = websGetVar(wp, T("forType"), T(""));
    firm_ver = websGetVar(wp, T("firmVer"), T(""));


    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
	    if((strcmp(for_type, prof->sw_model) == 0) && (strcmp(firm_ver, prof->hw_version) == 0) 
		&& (prof->defaulted == 1u))
	    {		
		ErrMsg = getTransDataForC(C_LANG_INDEX_ONLY_ONE_DEFAULT_SOFTEWARE);
		setTheErrorMsg((char *)ErrMsg);
		//setTheErrorMsg(T("无效的设置，相同型号相同硬件版本的软件只能设置一个为默认"));
		exist = 1u;
		break;
	    }

	    if(strcmp(def_name, prof->sw_name) == 0)
	    {
		exist = 1u;
		break;
	    }
	}
    }
    
    if(exist != 1u)
    {
	index = ProfGetInstIndexFree(mibType);
	if(index != PROFFAIL)
	{
	    prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(prof != NULL)
	    {
		strcpy(prof->sw_name, def_name);
		strcpy(prof->sw_version, soft_ver);
		strcpy(prof->sw_model, for_type);
		strcpy(prof->hw_version, firm_ver);
		prof->defaulted = 1u;
		sprintf(prof->head.name, "APSwMng_%d", index);
		prof->head.active = TRUE;
		ischanged = 1u;
	    }
	}
    }

    if(ischanged == 1u)
    {
	nvramWriteCommit(); 
    }

    saveGlobalVar(wp, gMac, gIp, gPasswd,gSzName);
    websRedirect(wp, "apSoftMng.asp?urcpSet=1");
    return;    
}

/*
 * Function: formClearSoftAsDefault()
 *
 * Description: clear software is default.
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
static void formClearSoftAsDefault(webs_t wp, char * path, char * query) 
{
    char *def_name = NULL;
    MibProfileType mibType = MIB_PROF_AP_SW_MNG;
    ApSwMngProfile     *prof = NULL;
    int min = 0, max = 0, index = 0;

    def_name = websGetVar(wp, T("defName"), T(""));

    ProfInstLowHigh(mibType, &max, &min);
    for(index = min; index < max; index++)
    {
	prof = (ApSwMngProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(((Pointer)prof)) == 0) && (prof->head.active == TRUE))
	{
	    if(strcmp(def_name, prof->sw_name) == 0)
	    {
		ProfDelInstByIndex(mibType, index);
		nvramWriteCommit(); 
		break;
	    }
	}
    }

    websRedirect(wp, "apSoftMng.asp?urcpSet=1");
    return;    
}

/*
 * Function: formDefineApSoftwareManage()
 *
 * Description: 页面与交互转换函数
 *
 * Histroy: added by Shiyu.Chen 2012.07.03
 */
void formDefineApSoftwareManage(void)
{
    websAspDefine(T("aspOutPutRemoteApSoft"), aspOutPutRemoteApSoft);
    websAspDefine(T("aspOutPutUploadFirmware"), aspOutPutUploadFirmware);
    websFormDefine(T("formUploadFirmware"), formUploadFirmware);
    websFormDefine(T("formApSoftDel"), formApSoftDel);
    websFormDefine(T("formApSoftDelAll"), formApSoftDelAll);
    websFormDefine(T("formSetSoftAsDefault"), formSetSoftAsDefault);
    websFormDefine(T("formClearSoftAsDefault"), formClearSoftAsDefault);
}
#endif
