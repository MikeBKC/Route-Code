#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "internet.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"
#include "comExeFun.h"
#include <signal.h>
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include    <usb-user.h>
#endif
#if (NET_SHARE_MANAGE == FYES)
#define FTP_ROOT "/ftpRoot"
#define FTP_NULL "/ftpNull"

#if (UTT_SMART_UI == FYES)
extern unsigned int netShareDirCount;/*mv文件夹时保证名字唯一*/
#endif
extern void netShareManageInit(void);

void netShareManageProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const NetShareManageProfile* oldProfileDataPtr, const NetShareManageProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
            netShareManageInit();
            break;
        case PROF_CHANGE_DEL:
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}

void netShareManageInit(void)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile *prof = NULL;
    MibProfileType mibTypeUser = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *profUser = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char bindStr[8];
    char buf[512];
    char tmp[512];
#if (UTT_SMART_UI == FYES)
    char *p = NULL;
#endif
    char valid_users[128];
    char read_lists[128];
    char write_lists[128];
    int count = 0;
    int i = 0;
    int max,min;
    int smbEnable = 0;
    unsigned int flag = 0u;
    unsigned int flag1 = 0u;

//#if (UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES)
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) ||(UTT_NV_1220V2 == FYES))
    flag |= SD_EXIST;
    flag1 |= USB_EXIST;
#else
    #if (UTT_SMART_UI == FYES)
	flag |= USB_EXIST;
    #if (SD_CARD_USAGE == FYES)
	flag |= SD_EXIST;
    #endif
    #else
//	#if (UTT_NV_520WV3 == FYES)
	#if ((UTT_NV_520WV3 == FYES)|| (UTT_NV_512WV3 == FYES)|| (UTT_NV_515W == FYES)||(UTT_NV_518WV2 == FYES)||(UTT_NV_518WV3 == FYES)||(UTT_NV_521GW == FYES)||(UTT_NV_842WV3 == FYES))
	    flag |= USB_EXIST;
	#else    
	    flag |= SD_EXIST;
	#endif
    #endif
#endif

    memset(device_path,0,sizeof(device_path));
    memset(buf,0,sizeof(buf));
    memset(tmp,0,sizeof(tmp));
    memset(valid_users,0,sizeof(valid_users));
    memset(read_lists,0,sizeof(read_lists));
    memset(write_lists,0,sizeof(write_lists));
    memset(bindStr, 0, sizeof(bindStr));

    prof = (NetShareManageProfile *)ProfGetInstPointByIndex(mibType, 0);

    system("killall smbd > /dev/null");
    system("rm /etc/smb.conf -f");
    
    if (prof == NULL) {
	    return -1;
    }else{
	if(prof->GlbEn){
	    if(prof->need_passwd){
		sprintf(buf,"samba.sh valid ");
            }else{
                sprintf(buf,"samba.sh novalid ");
                strcat(write_lists,"guest");
                strcat(write_lists,",");
#if (UTT_SMART_UI == FYES)
		strcat(valid_users, "guest");
                strcat(valid_users,",");
                strcat(read_lists, "guest");
                strcat(read_lists,",");
#endif
            }
	    strcpy(bindStr, "no ");
#if (UTT_SMART_UI == FYES)
	    if (prof->allow_wan_visit)
	    {
                strcpy(bindStr, "yes ");
	    }
#endif
	    strcat(buf, bindStr);
            ProfInstLowHigh(mibTypeUser, &max, &min);
#if (UTT_SMART_UI == FYES)
            for (i = min + 3; i < max; i++) {
#else
            for (i = min; i < max; i++) {
#endif
                profUser = (NetShareUserProfile *)ProfGetInstPointByIndex(mibTypeUser, i);
                if((profUser != NULL) && (ProfInstIsFree(profUser) == 0) && (profUser->head.active == TRUE)) {
                    strcat(valid_users,profUser->head.name);
                    strcat(valid_users,",");
                    if(profUser->jurisdiction == SHARE_READ_WRITE ){
                        strcat(write_lists,profUser->head.name);
                        strcat(write_lists,",");
                    }else{
                        strcat(read_lists,profUser->head.name);
                        strcat(read_lists,",");
                    }
                }
            }

            if(valid_users[strlen(valid_users)-1] == ',' ){
                valid_users[strlen(valid_users)-1] = '\0';
            }
            if(read_lists[strlen(read_lists)-1] == ',' ){
                read_lists[strlen(read_lists)-1] = '\0';
            }
            if(write_lists[strlen(write_lists)-1] == ',' ){
                write_lists[strlen(write_lists)-1] = '\0';
            }

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) ||(UTT_NV_1220V2 == FYES))
            if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else
#if (SD_CARD_USAGE != FYES)
	    if(isStorageExist(device_path,&count,flag)==1)
#endif
	    {
#endif
                strcat(buf,"\"");
#if (UTT_SMART_UI == FYES)
#if (SD_CARD_USAGE != FYES)
                if(count > 0)
#endif
		{
                    if(prof->smbEnable[0]){
                        sprintf(tmp,"[%s]\n path = %s/%s\n read only = no\n guest ok = yes\n valid users = %s\n read list = %s\n write list = %s\n",
                                prof->dirName,FTP_ROOT,prof->dirName,valid_users,read_lists,write_lists);
#else
                for(i=0;i<count;i++){
                    if(prof->smbEnable[i]){
                        sprintf(tmp,"[volume%d]\n path = %s\n read only = no\n guest ok = yes\n valid users = %s\n read list = %s\n write list = %s\n",
                                i,device_path[i],valid_users,read_lists,write_lists);
#endif
                        strcat(buf,tmp);
                        smbEnable = 1;
                    }
                }
            }
                strcat(buf,"\"");
                if(smbEnable){
#if (UTT_SMART_UI == FYES)
		    p = iconv_string(buf,"GB2312","UTF-8");
		    system(p);
		    free(p);
		    p = NULL;
#else
                    system(buf);
#endif
                    system("smbd&");
		    /*mantis:14166*/
		    sleep(1);
                }
        }
    }
}

#if (UTT_SMART_UI == FYES)
#if 0
void netShareStatusCheck(void)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile *prof = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
    unsigned int flag = 0u;
    int i = 0;

    flag |= USB_EXIST;
    memset(device_path,0,sizeof(device_path));

    prof = (NetShareManageProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == PROFFAIL)
    {
	return;
    }

    if(isStorageExist(device_path,&count,flag)==1)
    {
	if(prof->smbEnable[0])
	{
	    system("killall smbd > /dev/null");
	    system("smbd&");
	}
    }
}
#endif
#endif

void netShareReset(struct st_mdevMsg mdevMsg)
{
#if (UTT_SMART_UI == FYES)
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    MibProfileType mibTypeFS = MIB_PROF_FTP_SERVER;
    FtpServerProfile *profFS = NULL;
    char tmp[256];
    char * p = NULL;

    profFS = (FtpServerProfile *)ProfGetInstPointByIndex(mibTypeFS, 0);
    if (ProfInstIsFree(profFS) != 0)
    {
	return;
    }
#if (SD_CARD_USAGE != FYES) 
    if (profFS->status && mdevMsg.status)
    {
	return;
    }
    profFS->status = mdevMsg.status;
#endif

    if (mdevMsg.status == 0U)
    {
	prof = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibType, 0);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) 
	{ 
            sleep(1);/*mv时可能导致设备忙，从而automount.sh无法卸载该分区*/
#if (UTT_SMART_UI == FYES)
	    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\"",FTP_ROOT, prof->net_share_volume_name,FTP_NULL,netShareDirCount++);
#else
	    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\"",FTP_ROOT, prof->head.name,FTP_NULL,netShareDirCount++);
#endif
	    p = iconv_string(tmp,"GB2312","UTF-8");
	    system(p);
	    free(p);
	    p = NULL;
	}
    }
    else
    {
	sleep(3);
    }
#if (SD_CARD_USAGE == FYES)
ftpShareDirInit();
#else
system("killall proftpd > /dev/null");
netShareManageInit();
ftpServerRestart();
#endif
#else
system("killall proftpd > /dev/null");
netShareManageInit();
ftpServerRestart();
#endif
}
void funInitNetShareManage(void)
{
    registerForProfileUpdates(MIB_PROF_NET_SHARE_MANAGE, (Pointer)NULL, (ProfileUpdateFnc)netShareManageProfileChange);
    netShareManageInit();//配置初始化生效
#if 0
    signal(SIGTTIN, netShareReset);
#endif
#if 0
#if (UTT_SMART_UI == FYES)
    system("echo 5120 > /proc/sys/vm/min_free_kbytes");
#endif
#endif
    return;
}
#endif
