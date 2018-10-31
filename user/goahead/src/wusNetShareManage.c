#include "uttMachine.h"
#if (NET_SHARE_MANAGE == FYES)
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/sysinfo.h>
#include    <fcntl.h>
#include	"wsIntrn.h"
#include	"um.h"
#include	"utils.h"
#include	"mib.h"
#include	"profacce.h"
#include	"uttfunction.h"
#include	"wuScript.h"
#include	"translate.h"
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include    <usb-user.h>
#endif
#include <iconv-user.h>


#define FTP_ROOT "/ftpRoot"
#define FTP_NULL "/ftpNull"

#if (UTT_SMART_UI == FYES)
unsigned int netShareDelDirCount = 1;
extern int umountPath(char* dev_path);
extern int getftpNull(char mount_str[][MAX_PATH_LEN]);
/**
 * umount卸载目录
 * 目录繁忙返回 0
 * 目录不存在或参数错误返回 -1
 * 卸载成功返回 1
 **/
int umountPath(char* dev_path)
{
    int rst = -1;
    FILE * fd = NULL;
    char *p = NULL;
    char line[128];
    char umount_str[128];
    sprintf(umount_str,"umount %s 2>&1",dev_path);
    p = iconv_string(umount_str,"GB2312","UTF-8");
    fd = popen(p,"r");
    fgets(line, sizeof(line), fd);
    if(strstr(line, "resource busy") != NULL) rst = 0;
    else if(strstr(line, "Invalid argument") != NULL || strstr(line, "No such file") != NULL) rst = -1;
    else rst = 1;
    pclose(p);
    if(p!=NULL){
        free(p);
    }
    return rst;
}
int getftpNull(char mount_str[][MAX_PATH_LEN])
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL;
    int i = 0;

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }
    while(fgets(buf, sizeof(buf), fp)){
	if(strstr(buf, "/ftpNull/") == NULL)
	    continue;
	sscanf(buf, "%s %s", device_path, mount_path);
	sscanf(mount_path, "/ftpNull/%[^/]/",mount_str[i]);
	i++;
    }
    fclose(fp);
    return i;
}
#endif


static int aspOutputNetShareManage(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile* prof = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
	int ret=0;
    long long total_space = 0,used_space = 0,unused_space = 0;
    int i = 0;
    unsigned int flag = 0u;
   
    unsigned int flag1 = 0u; 

//#if (UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES)
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
    flag |= SD_EXIST;
    flag1 |= USB_EXIST;
#else
    #if (UTT_SMART_UI == FYES)
	flag |= USB_EXIST;
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
    /*
     * 获得profile准备保存配置
     */
    prof = (NetShareManageProfile *)ProfGetInstPointByIndex(mibType, 0);

    if (prof == NULL) {
	    return -1;
    }else{
    /*
     * 页面输出
     */
    websWrite(wp, "var enable_smb = \"%s\";\n", prof->GlbEn == FUN_ENABLE ? "on" : "off"); /*启用开关*/
    websWrite(wp, "var need_passwd = \"%s\";\n", prof->need_passwd == FUN_ENABLE ? "on" : "off"); /*启用开关*/
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
    if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else
    if(isStorageExist(device_path,&count,flag)==1){
#endif
        for(i=0;i<count;i++){

        storageFree(device_path[i], &total_space, &unused_space);
        used_space = total_space - unused_space;
        total_space >>= 20;
        used_space >>= 20;
        unused_space >>= 20;

        if( total_space ){
            websWrite(wp, "deviceInfos[%d] = new Array(\"volume%d\",\"%d MB\",\"%d MB\",\"%d MB\",\"%d%%\",\"%s\",\"%s\");\n",
                    i,i,(int)total_space,(int)used_space,(int)unused_space,total_space ? (int)(used_space*100/total_space) : (int)0, !prof->smbEnable[i] ? "on" : "off","volume");

        }
        }
    }

    getTheErrorMsg(eid, wp, argc, argv);
	}
    return ret;
}

static void formNetShareManage(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile* prof = NULL;
    prof = (NetShareManageProfile *)ProfGetInstPointByIndex(mibType, 0);
    struProfAlloc *profhead = NULL;
    FunEnableEnum enEnum = FUN_ENABLE;

    char_t *smbEnable=NULL,*passwdEnable=NULL;
    const char *ErrMsg = NULL;

    if (prof == NULL) {
	    return;
    }else{
        smbEnable = websGetVar(wp, T("enableDevice"), T(""));
        passwdEnable = websGetVar(wp, T("enablePwd"), T(""));

	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/
	    prof->head.active = TRUE;
        if(0 != strcmp(smbEnable, "on")) {
            enEnum = FUN_DISABLE;
        }else{
            enEnum = FUN_ENABLE;
        }
        prof->GlbEn = enEnum;
        if(0 != strcmp(passwdEnable, "on")) {
            enEnum = FUN_DISABLE;
        }else{
            enEnum = FUN_ENABLE;
        }
        prof->need_passwd = enEnum;

        ProfUpdate(profhead);/*发消息更新*/
        ProfFreeAllocList(profhead);/*释放malloc的内存*/
        nvramWriteCommit();/*写flash*/
    }
    websRedirect(wp, "NetShareManage.asp");
}

static void formSingleStatusChange(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile* prof = NULL;
    struProfAlloc *profhead = NULL;
    char_t *smb_no,*smb_en;
    int no;
    smb_no = websGetVar(wp,T("smbNo"),T(""));
    smb_en = websGetVar(wp,T("smbEn"),T(""));

    no = strtol(smb_no,NULL,10);
    prof = (NetShareManageProfile *)ProfGetInstPointByIndex(mibType, 0);
	ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/

    if( !strcmp("on",smb_en) ){
        prof->smbEnable[no] = 0;
    }else{
        prof->smbEnable[no] = 1;
    }

    ProfUpdate(profhead);/*发消息更新*/
    ProfFreeAllocList(profhead);/*释放malloc的内存*/
    nvramWriteCommit();/*写flash*/
    websRedirect(wp, "NetShareManage.asp");

}
static void formStoragePop(webs_t wp, char_t *path, char_t *query)
{
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
    int i = 0;
    unsigned int flag = 0u;
    unsigned int flag1 = 0u;
    char tmp[512];
#if (UTT_SMART_UI == FYES)
    int flag_opt = 0;
    int k = 0;
    const char *ErrMsg = NULL;
    char device_path_ftpNull[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int countftpNull = 0;
    char delstr[128];
    char *p = NULL;
    char partition_name[10][128];
    int index = 0;
    MibProfileType mibTypeFS = MIB_PROF_FTP_SERVER;
    FtpServerProfile *profFS = NULL;
    char_t *storage_type = NULL;
#endif

	MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    struProfAlloc *profhead = NULL;
	int min,max;
//#if(UTT_NV_750GW == FYES)||(UTT_NV_521GV2 == FYES)
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES)|| (UTT_NV_1220V2 == FYES))
	flag |= SD_EXIST;
	flag1 |= USB_EXIST;
#else
#if (UTT_SMART_UI == FYES)
    storage_type = websGetVar(wp, T("storage_type"), T("usb"));
    if (strcmp(storage_type,"usb") == 0) {
	flag |= USB_EXIST;
    } else {
	flag |= SD_EXIST;
    }
    profFS = (FtpServerProfile *)ProfGetInstPointByIndex(mibTypeFS, 0);
    if (ProfInstIsFree(profFS) != 0)
    {
	return;
    }
    profFS->status = 0U;
#else
//#if (UTT_NV_520WV3 == FYES)
#if ((UTT_NV_520WV3 == FYES)|| (UTT_NV_512WV3 == FYES)|| (UTT_NV_515W == FYES)||(UTT_NV_518WV2 == FYES)||(UTT_NV_518WV3 == FYES)||(UTT_NV_521GW == FYES)||(UTT_NV_842WV3 == FYES))
flag |= USB_EXIST;
#else    
flag |= SD_EXIST;
#endif
#endif
#endif
    memset(device_path,0,sizeof(device_path));
    memset(tmp,0,sizeof(tmp));

#if (UTT_SMART_UI != FYES)
    system("killall proftpd; killall smbd;");
#endif

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
    if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else
    if(isStorageExist(device_path,&count,flag)==1){
#endif

        ProfInstLowHigh(mibType,&max,&min);
        for(i=min;i<max;i++){
            prof=(FtpShareDirProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
            if((prof!=NULL)&&(ProfInstIsFree(prof)==0)&& (prof->head.active == TRUE)){
#if (UTT_SMART_UI == FYES)
		if (prof->select_all)
		{
		    for(index=0;index<count;index++)
		    {
			sscanf(device_path[index], "/media/%s/", partition_name[index]);
			//sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->head.name, partition_name[index]);
			sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->net_share_volume_name, partition_name[index]);
			if (umountPath(delstr) == 0)
			{
			    flag_opt = -1;
			}
			countftpNull = getftpNull(device_path_ftpNull);
			for(k=0;k<countftpNull;k++)
			{
			    sprintf(delstr,"%s/%s/%s",FTP_NULL,device_path_ftpNull[k], partition_name[index]);
			    if(umountPath(delstr) == 0)
			    {
				flag_opt = -1;
			    }
			}
		    }
		    if(flag_opt == -1)
			for(index=0;index<count;index++)
			{
			    sscanf(device_path[index], "/media/%s/", partition_name[index]);
			    //sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->head.name, partition_name[index]);
			    sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->net_share_volume_name, partition_name[index]);
			    if (umountPath(delstr) != 0)
			    {
				//doSystem("mount --bind  \'/media/%s\' \'%s/%s/%s\'",partition_name[index],FTP_ROOT,prof->head.name,partition_name[index]);
				doSystem("mount --bind  \'/media/%s\' \'%s/%s/%s\'",partition_name[index],FTP_ROOT,prof->net_share_volume_name,partition_name[index]);
			    }
			}
		}
		else
		{
		    countftpNull = getftpNull(device_path_ftpNull);
		    for(k=0;k<countftpNull;k++)
		    {
			sprintf(delstr,"%s/%s/%s",FTP_NULL,device_path_ftpNull[k], FTP_STORAGE);
			if(umountPath(delstr) == 0)
			{
			    flag_opt = -1;
			}
		    }
		    //sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->head.name, FTP_STORAGE);
		    sprintf(delstr,"%s/%s/%s",FTP_ROOT, prof->net_share_volume_name, FTP_STORAGE);
		    if (umountPath(delstr) == 0)
		    {
			flag_opt = -1;
		    }
		}
		if(flag_opt == -1) goto PopOut;
		//sprintf(delstr,"cd %s && mv \"%s\" %s/\"g%u\"",FTP_ROOT, prof->head.name,FTP_NULL, netShareDelDirCount++);
		sprintf(delstr,"cd %s && mv \"%s\" %s/\"g%u\"",FTP_ROOT, prof->net_share_volume_name,FTP_NULL, netShareDelDirCount++);
		p = iconv_string(delstr,"GB2312","UTF-8");
		system(p);
		free(p);
		p = NULL;
#else
                ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profhead);
                ProfDelInstByIndex(mibType, i);
#endif
            }
        }

        for(i=0;i<count;i++){
#if (UTT_SMART_UI == FYES)
	    sscanf(device_path[i], "/media/%s", partition_name[i]);
            sprintf(tmp,"umount %s",device_path[i]);
#else
            sprintf(tmp,"umount %s;rm -rf %s",device_path[i],device_path[i]);
#endif
            system(tmp);
        }
#if (UTT_SMART_UI == FYES)
	ProfBackupByIndex(mibTypeFS, PROF_CHANGE_DEL, 0, &profhead);
	profFS->change = profFS->change + 1;
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit();/*保存flash*/
	if(flag_opt == 0) flag_opt = 1;
#else
        ProfUpdate(profhead);
        ProfFreeAllocList(profhead);
        nvramWriteCommit();/*保存flash*/
        system("cd /ftpNull;rm -rf *;"); 
#endif
    }
    

#if (UTT_SMART_UI == FYES)
PopOut:
    if(flag_opt == 1)
    {
        setSuccessTips();
    }
    else if(flag_opt == -1)
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_SHARE_FILE_IS_BUSY);
        setTheErrorMsg(ErrMsg);
    }
    websRedirect(wp, "NetShare_Sui.asp");
#else
    websRedirect(wp, "NetShareManage.asp");
#endif
}
extern void formDefineNetShareManage(void) {
    websAspDefine(T("aspOutputNetShareManage"), aspOutputNetShareManage);
	websFormDefine(T("formNetShareManage"), formNetShareManage);
	websFormDefine(T("formSingleStatusChange"), formSingleStatusChange);
	websFormDefine(T("formStoragePop"), formStoragePop);
}

#endif
