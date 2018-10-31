

#include "um.h"
#include "utils.h"
#include "flash_api.h"
#include "webmsg.h"
#include <linux/autoconf.h>
#include <sys/utsname.h>
#include <string.h>
#include <dirent.h>
#include <uttMachine.h>
#include <stdio.h>
#include "wsIntrn.h"
#include "webs.h"
#include "internet.h"
#include "uttfunction.h"
#include "mib.h"
#include "profacce.h"
#include "translate.h"

#if (FEATURE_AC == FYES)
#include    <switch.h>
#include    <sw_cfg.h>
#include    <urcp_ipc.h>
#include    "webmsg.h"

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#define IMAGE_ROOT	    "/tmp/urcp/images/"  /*save firmware*/
#define MODEL_1800N	    "WA1800N"
#define MODEL_510W	    "nv510W"

typedef urcp_status_t (*urcp_fp)(unsigned char mac[6], unsigned int userIp, unsigned int op_type);

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);
extern void getErrorArr(webs_t wp, urcp_fp fp, uint32 op_type);
extern int getBatchInfo(webs_t wp, char* strMac, char*strPasswd, urcp_batch_msg_t* batch_info);
#if 0
extern int hwverinfo_to_str(int hwver, char* hwver_buf);
extern int swverinfo_to_str(int swver, char* swver_buf);
#endif
extern int hwverinfo_to_str(unsigned int hwver, char* hwver_buf);
extern int swverinfo_to_str(unsigned int swver, char* swver_buf);

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
extern int isUSBStorageExist(char *usb_mount_path);
#endif

/*
 * Function: aspOutRemoteApSoft()
 *
 * Description: output vars to web page.
 *
 * Histroy: added by Shiyu.Chen 2012.07.13
 */
static int aspOutRemoteAPSoft(int eid, webs_t wp, int argc, char_t **argv)
{
    DIR *dir;
    struct dirent *ptr;
    struct stat stat_buf;
    off_t size1 = 0;
    char product_model[1024], hw_version[512], sw_version[512], hwVer_str[24], swVer_str[24], tmp_file_name[128];
    char file_name[1400], file_size[512];
    char buf[128], tmp[128];
    int first = 1, count = 0, hwVer = 0, swVer = 0;
    image_header_t *head=NULL;
    FILE *p=NULL;
    unsigned char *strMac = NULL, *strIp = NULL, *strPasswd = NULL, *web_type = NULL,*strSzName = NULL,*strManaged = NULL;
    unsigned char *waitOver = NULL, *optType = NULL;
    char tmp_path[256];

    
    memset(product_model, 0, sizeof(product_model));
    memset(hw_version, 0, sizeof(hw_version));
    memset(sw_version, 0, sizeof(sw_version));
    memset(file_name, 0, sizeof(file_name));
    memset(file_size, 0, sizeof(file_size));
    memset(tmp_file_name, 0, sizeof(tmp_file_name));
    memset(hwVer_str, 0, sizeof(hwVer_str));
    memset(swVer_str, 0, sizeof(swVer_str));
    memset(tmp_path, '\0', sizeof(tmp_path));

    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    strIp = websGetVar(wp, "ipStr", T(""));
    web_type = websGetVar(wp, "devType", T(""));
    strSzName = websGetVar(wp, "szName", T(""));
    strManaged = websGetVar(wp, "managedEn", T(""));

    websWrite(wp, "var macStr = \"%s\";\n", strMac);
    websWrite(wp, "var ipStr = \"%s\";\n", strIp);
    websWrite(wp, "var passStr = \"%s\";\n", strPasswd);
    websWrite(wp, "var szName = \"%s\";\n", strSzName);
    websWrite(wp, "var managedEn = \"%s\";\n", strManaged);

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
	    strcat(tmp_path, IMAGE_ROOT);
#endif
    }

    dir=opendir(tmp_path);
    if (dir != NULL) 
    {
	while ( (ptr = readdir(dir)) != NULL) 
	{
	    if ((strcmp (ptr->d_name, ".") != 0) && (strcmp (ptr->d_name, "..") != 0))
	    {
		sprintf(tmp, "%s%s",tmp_path, ptr->d_name);
		    //printf("\n< %s %d > tmp:%s\n", __func__, __LINE__,tmp);
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
		    //printf("\n< %s %d > first:%d\n", __func__, __LINE__, first);
		    head = (image_header_t *)buf; 
		    //printf("\n< %s %d > head-name:%s web_type:%s\n", __func__, __LINE__, head->name, web_type);
		    if (strcmp(head->name, web_type) == 0)
		    {
			if(first != 1)
			{
			    strcat(product_model, ",");
			    strcat(hw_version, ",");
			    strcat(sw_version, ",");
			    strcat(file_name, ",");
			    strcat(file_size, ",");
			}
			if((stat(tmp, &stat_buf)==0) && (S_ISREG(stat_buf.st_mode)))
			{
			    first = 2;
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
			
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "\"%s\"", head->name);
			strcat(product_model, tmp);
			
			get_verinfo_from_file(tmp_file_name, &hwVer, &swVer);
			hwverinfo_to_str(hwVer, hwVer_str);
			swverinfo_to_str(swVer, swVer_str);

			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp,"\"%s\"", swVer_str);
			strcat(sw_version, tmp);
			
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp,"\"%s\"", hwVer_str);
			strcat(hw_version, tmp);
	
			count++;
		    }
		}
	    }
	}
    }
    
    websWrite(wp, "var softwareName = new Array(%s);\n", file_name);
    websWrite(wp, "var softwareVersion = new Array(%s);\n", sw_version);
    websWrite(wp, "var typesForFile = new Array(%s);\n", product_model);
    websWrite(wp, "var firmwareVersion = new Array(%s);\n", hw_version);
    websWrite(wp, "var fileSize = new Array(%s);\n", file_size);
    websWrite(wp, "var totalrecs = %d;\n",count);
    
    /*错误信息*/
    waitOver = websGetVar(wp, "waitOver", T(""));
    if(strcmp(waitOver, T(""))!=0)
    {
	optType = websGetVar(wp, "optType", T(""));
	if(strcmp(optType, "getAPSoft")==0)
	{
	    /*读取*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_GET_GENERAL);
	}
	else if(strcmp(optType, "sendAPSoft")==0)
	{
	    /*下发*/
	    getErrorArr(wp, urcp_read_config_conf, MISC_SET_GENERAL);
	}
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/*
 * Function: aspRemoteApSoftSend()
 *
 * Description: output vars to web page.
 *
 * Histroy: added by Shiyu.Chen 2012.07.13
 */
static int aspRemoteApSoftSend(int eid, webs_t wp, int argc, char_t **argv)
{
    int i=0;
    char *strMac, *strPasswd, *strValue=NULL;
    urcp_send_conf_msg_t urcp_send_conf_msg;

    memset(&urcp_send_conf_msg, 0, sizeof(urcp_send_conf_msg_t));
    /*读页面数据*/
    strMac = websGetVar(wp, "macStr", T(""));
    strPasswd = websGetVar(wp, "passStr", T(""));
    //printf("mac=%s, passwd=%s\n",strMac,strPasswd);
    /*取mac地址*/
    i = getBatchInfo(wp, strMac, strPasswd, &(urcp_send_conf_msg.batch_info));
    if(i>0)
    {
	/*文件名*/
	strValue = websGetVar(wp, "remoteSrcSoft", T(""));
	i = sizeof(urcp_send_conf_msg.src_file);
	strncpy(urcp_send_conf_msg.src_file, strValue, i);

	/*发消息,通知交换机进程发包*/
	sendwebmsgbuffer(MSG_TYPE_URCP, SET_IMAGE, (char*)(&urcp_send_conf_msg), sizeof(urcp_send_conf_msg_t));
    }

    getTheErrorMsg(eid, wp, argc, argv);

    return 0;
}

/*
 * Function: formDefinePushSoftware()
 *
 * Description: 页面与交互转换函数
 *
 * Histroy: added by Shiyu.Chen 2012.07.13
 */
void formDefinePushSoftware(void)
{
    websAspDefine(T("aspOutRemoteAPSoft"), aspOutRemoteAPSoft);
    websAspDefine(T("aspRemoteApSoftSend"), aspRemoteApSoftSend);
}
#endif
