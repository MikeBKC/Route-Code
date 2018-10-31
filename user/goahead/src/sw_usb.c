/* vi: set sw=4 ts=4 sts=4: */
/*
 *	usb.c -- USB Application Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: sw_usb.c,v 1.9 2014/09/17 11:57:51 ke.huatao Exp $
 */

#include	<stdlib.h>
#include	<dirent.h>
#include	<arpa/inet.h>
#include     <sys/vfs.h>
#include	"webs.h"
#include	"usb.h"
#include 	"utils.h"
#include 	"internet.h"
#include	<linux/autoconf.h> //user config
#include    <uttMachine.h>
#include    "uttfunction.h"

#if defined CONFIG_RALINKAPP_MPLAYER
#include	"media.h"
#endif

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include     <switch.h>
#include     <urcp_ipc.h>
#endif

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include     <usb-user.h>
#endif

#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
/*
 * *�ж�u�����Ƿ����Ŀ¼�����򴴽�Ŀ¼
 * *Kehuatao
 * */
static void createUSBDir(char *tmp)
{
    char tp[268];

    memset(tp, 0, sizeof(tp));
    /*����*/
    sprintf(tp, "cp -af %s %s; rm -rf %s/*", RAM_CONFIG_PATH, tmp, RAM_CONFIG_PATH);
    system(tp);

    memset(tp, 0, sizeof(tp));
    /*����*/
    sprintf(tp, "mkdir %s%s -p", tmp, USB_LOG_PATH);
    system(tp);

    memset(tp, 0, sizeof(tp));
    sprintf(tp, "cp -af %s %s; rm -rf %s/*", RAM_IMAGE_PATH, tmp, RAM_IMAGE_PATH);
    system(tp);

    memset(tp, 0, sizeof(tp));
    sprintf(tp, "cp -af %s %s; rm -rf %s/*", RAM_AP_CONFIT_ROOT, tmp, RAM_AP_CONFIT_ROOT);
    system(tp);

    memset(tp, 0, sizeof(tp));
    sprintf(tp, "cp -af %s %s; rm -rf %s/*", RAM_DRAWING_PATH, tmp, RAM_DRAWING_PATH);
    system(tp);

    return;
}
#endif
/*��ʼ��u��
* 
*KeHuatao
*
*/
int initUSB(void)
{
	char usb_mount_path[256];
	int isUSBExit=0;

    memset(usb_mount_path, 0, sizeof(usb_mount_path));
    /*�ж��Ƿ����u��*/
    isUSBExit = isUSBStorageExist(usb_mount_path);

    if(isUSBExit==1)
    {
#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
        createUSBDir(usb_mount_path);
#endif
    }
    return 0;
}
/*
 *  Hotpluger signal handler
 */
void hotPluglerHandler(int signo)
{
	initUSB();
}

/*
 *���ļ����浽u��ָ��Ŀ¼
 *KeHuatao
 *
 */
int saveFile2USB(char* file_name, char * src_path, char* dst_path)
{
	char tmp[256], mount_path[256];
	int error_index=-1;
	long long freeDisk=0, size;
	struct stat buf;

	/*ÿ�ζ����¶�ȡʣ��ռ��С*/
	memset(mount_path, 0, sizeof(mount_path));
	isUSBStorageExist(mount_path);
	if(storageFree(mount_path, NULL, &freeDisk)!=-1)
	{
		/*�ļ���*/
		memset(tmp, 0, sizeof(tmp));
		strcat(tmp, src_path); 
		strcat(tmp, file_name); 

		/*�ļ��޸�ʱ��*/
		if((stat(tmp, &buf)==0) && (S_ISREG(buf.st_mode)))
		{
			size = (long long)(buf.st_size);
			/*�жϴ�С*/
			if(freeDisk>=size)
			{
				/*���ļ����浽u��*/
				strcat(mount_path, dst_path); 
				strcat(mount_path, file_name); 
				if(copy_file(tmp, mount_path, 1, 0)==-1)
				{
					error_index = WEB_MSG_FAIL;
				}
			}
			else
			{
				/*ʣ��ռ䲻��*/
				error_index = WEB_MSG_NOT_ENOUGH;
			}
		}
	}
	else
	{
        /*����*/
		error_index = WEB_MSG_FAIL;
	}

	return error_index;
}
