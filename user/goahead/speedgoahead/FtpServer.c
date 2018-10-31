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
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
#include    <usb-user.h>
#endif
#if (FTP_SERVER == FYES)

#include <iconv-user.h>


#define FTP_ROOT "/ftpRoot"
#define FTP_NULL "/ftpNull"

#if (UTT_SMART_UI == FYES)
extern int umountPath(char* dev_path);
unsigned int netShareDirCount = 1;/*mv文件夹时保证名字唯一*/
#endif

extern void ftpServerInit(void);
extern void ftpShareDirInit(void);
static void ftpShareDirAdd(FtpShareDirProfile *prof);
static void ftpShareDirEdit(FtpShareDirProfile *oldProf,FtpShareDirProfile *prof);
static void ftpShareDirDel(FtpShareDirProfile *prof);
extern void shareDirDel(void);
extern void ftpServerRestart(void);

#if (UTT_SMART_UI == FYES)
/*
 * 功能：umount卸载目录
 * 返回：0 目录繁忙; 1 卸载成功 ; -1 目录不存在或参数错误
 */
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
    pclose(fd);
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

void FtpServerProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const FtpServerProfile* oldProfileDataPtr, const FtpServerProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
#if (UTT_SMART_UI == FYES)
	    ftpServerRestart();
#else
            ftpServerInit();
#endif
            break;
        case PROF_CHANGE_DEL:
#if (UTT_SMART_UI == FYES)
	    ftpShareDirInit();
#endif
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}


void FtpShareDirProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType aChangeType, Uint32 index,
        const FtpShareDirProfile* oldProfileDataPtr, const FtpShareDirProfile* newProfileDataPtr )
{/*对于端口管理所有实例永久存在*/
    switch(aChangeType) {
        case PROF_CHANGE_ADD:
#if (UTT_SMART_UI != FYES)
            ftpShareDirAdd(newProfileDataPtr);
#endif
            break;
        case PROF_CHANGE_EDIT:/*编辑*/
            ftpShareDirEdit(oldProfileDataPtr,newProfileDataPtr);
            break;
        case PROF_CHANGE_DEL:
            ftpShareDirDel(oldProfileDataPtr);
            break;
        case PROF_CHANGE_DELALL:
            break;
        default:/*默认*/
            break;
    }
}


void ftpServerInit(void)
{
    MibProfileType mibType = MIB_PROF_FTP_SERVER;
    FtpServerProfile *prof = NULL;
    MibProfileType mibTypeIF = MIB_PROF_INTERFACE;
    InterfaceProfile *profIF = NULL;
    MibProfileType mibTypeUser = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *profUser = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char buf[512];
    char __allow_ip[32];
    char allow_ips[128];
    char __allow_users[128];
    char allow_users[128];
    char ftpIp[13];
#if (UTT_SMART_UI == FYES)
    char anoIp[13];
#endif
    int count = 0;
    int max,min;
    unsigned int i = 0u;
    int ftpEnable = 0;
    unsigned int flag = 0u;
    unsigned int flag1 = 0u;
  IPADDR netMask;

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
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
    memset(allow_ips,0,sizeof(allow_ips));
    memset(__allow_ip,0,sizeof(__allow_ip));
    memset(ftpIp,0,sizeof(ftpIp));
#if (UTT_SMART_UI == FYES)
    memset(anoIp,0,sizeof(anoIp));
#endif
    memset(__allow_users,0,sizeof(__allow_users));
    memset(allow_users,0,sizeof(allow_users));

    prof = (FtpServerProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (prof == NULL) {
	    return -1;
    }else{
        system("killall proftpd > /dev/null");
        system("rm /etc/proftpd.conf -f");
        if(prof->ftpEnable){
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES)  || (UTT_NV_1220V2 == FYES))
            if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else 
#if (SD_CARD_USAGE != FYES)
	    if(isStorageExist(device_path,&count,flag)==1)
#endif
	    {
#endif
#if (SD_CARD_USAGE != FYES)
                if( count > 0 )
#endif
		{
                    profIF = (InterfaceProfile *)ProfGetInstPointByIndex(mibTypeIF, 0);
                    int2ip(ntohl(profIF->ether.sta.ip),ftpIp);
#if (UTT_SMART_UI == FYES)
		    strcpy(anoIp, ftpIp);
#endif

                    if(prof->allow_wan_visit){
                        i = 0u;
                    }else{
                        netMask = ntohl(profIF->ether.sta.nm);
                        i = 0u;
                        while( !((netMask>>i) & 0x1u) ){
                            i++;
                        }
                        i = 32u - i;
                    }
                    strcat(allow_ips,"\"");
                    sprintf(__allow_ip,"Allow from %s/%d\n",ftpIp,i);
                    strcat(allow_ips,__allow_ip);

#if (MULTI_LAN == FYES)
                    if( !prof->allow_wan_visit ){
                        if ( profIF->ether.sta.ip2 ){
                        memset(ftpIp,0,sizeof(ftpIp));
                        memset(__allow_ip,0,sizeof(__allow_ip));
                        int2ip(ntohl(profIF->ether.sta.ip2),ftpIp);

                        netMask = ntohl(profIF->ether.sta.nm2);
                        i = 0u;
                        while( !((netMask>>i) & 0x1u) ){
                            i++;
                        }
                        i = 32u - i;
                        sprintf(__allow_ip,"Allow from %s/%d\n",ftpIp,i);
                        strcat(allow_ips,__allow_ip);
                        }
                    }
                    if( !prof->allow_wan_visit ){
                        if ( profIF->ether.sta.ip3 ){
                        memset(ftpIp,0,sizeof(ftpIp));
                        memset(__allow_ip,0,sizeof(__allow_ip));
                        int2ip(ntohl(profIF->ether.sta.ip3),ftpIp);

                        netMask = ntohl(profIF->ether.sta.nm2);
                        i = 0u;
                        while( !((netMask>>i) & 0x1u) ){
                            i++;
                        }
                        i = 32u - i;
                        sprintf(__allow_ip,"Allow from %s/%d\n",ftpIp,i);
                        strcat(allow_ips,__allow_ip);
                        }
                    }
                    if( !prof->allow_wan_visit ){
                        if ( profIF->ether.sta.ip4 ){
                        memset(ftpIp,0,sizeof(ftpIp));
                        memset(__allow_ip,0,sizeof(__allow_ip));
                        int2ip(ntohl(profIF->ether.sta.ip4),ftpIp);

                        netMask = ntohl(profIF->ether.sta.nm2);
                        i = 0u;
                        while( !((netMask>>i) & 0x1u) ){
                            i++;
                        }
                        i = 32u - i;
                        sprintf(__allow_ip,"Allow from %s/%d\n",ftpIp,i);
                        strcat(allow_ips,__allow_ip);
                        }
                    }
#endif
                    strcat(allow_ips,"\"");

                    ProfInstLowHigh(mibTypeUser, &max, &min);
                    strcat(allow_users,"\"");
#if (UTT_SMART_UI == FYES)
                    for (i = min + 3; i < max; i++) {
#else
                    for (i = min; i < max; i++) {
#endif
                        profUser = (NetShareUserProfile *)ProfGetInstPointByIndex(mibTypeUser, i);
                        if((profUser != NULL) && (ProfInstIsFree(profUser) == 0) && (profUser->head.active == TRUE)) {
                            if(profUser->allow_ftp == FUN_ENABLE){
                                sprintf(__allow_users,"AllowUser %s\n",profUser->head.name);
                                strcat(allow_users,__allow_users);
                            }
                        }
                    }
                    strcat(allow_users,"\"");
                    sprintf(buf,"proftpd.sh server %s %d %s %s",ftpIp,prof->ftp_port,allow_ips,allow_users);
                    system(buf);
#if (UTT_SMART_UI == FYES)
		    if (!prof->need_passwd)
		    {
			if (!prof->allow_wan_visit)
			{
			    sprintf(buf, "proftpd.sh anonymous 10 %s 24", anoIp);
			    system(buf);
			}
			else
			{
			    system("proftpd.sh anonymous");
			}
		    }
#endif
                    system("proftpd&");
                }
            }
        }
    }
}
static void ftpShareDirAdd(FtpShareDirProfile * prof)
{
    char tmp[128];
    char *p = NULL;
#if (UTT_SMART_UI == FYES)
    int count = 0;
    unsigned int flag = 0u;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char partition_name[10][128];
    int index = 0;

    flag |= USB_EXIST;
#if (SD_CARD_USAGE == FYES)
    flag |= SD_EXIST;
#endif
    if(isStorageExist(device_path,&count,flag)==1)
    {
	if (count > 0)
	{
	    if (count > 1)
	    {
		if (!strcmp(prof->net_share_volume_path, ""))
		{
		    strcpy(prof->net_share_volume_path,"/media/");
		    nvramWriteCommit();
		}
		if (prof->select_all)
		{
		    for (index=0;index<count;index++)
		    {
			sscanf(device_path[index], "/media/%s/", partition_name[index]);
			sprintf(tmp,"cd %s && mkdir -p \"%s/%s\" && mount --bind \'%s%s\' \'%s/%s\' || mv \"%s\" %s/\"s%u\"",
				FTP_ROOT,prof->head.name,partition_name[index],
				prof->net_share_volume_path,partition_name[index], prof->head.name,partition_name[index],prof->head.name,FTP_NULL, netShareDirCount++);
			p = iconv_string(tmp,"GB2312","UTF-8");
			system(p);
			free(p);
			p = NULL;
		    }
		    if( p != NULL){
			free(p);
		    }
		    return;
		}
	    }
	    else
	    {
		if (!strcmp(prof->net_share_volume_path, ""))
		{
		    strcpy(prof->net_share_volume_path,device_path[0]);
		    nvramWriteCommit();
		}
	    }
	}
    }
    sprintf(tmp,"cd %s && mkdir -p \"%s\" && mount --bind \'%s%s\' \'%s\' || mv \"%s\" %s/\"s%u\"",
            FTP_ROOT,prof->head.name,
            prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path, prof->head.name,prof->head.name,FTP_NULL, netShareDirCount++);
#else
    sprintf(tmp,"cd %s && mkdir -p \"%s\" && mount --bind \'%s%s\' \'%s\' || mv \"%s\" %s",
            FTP_ROOT,prof->head.name,
            prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path, prof->head.name,prof->head.name,FTP_NULL);
#endif
    p = iconv_string(tmp,"GB2312","UTF-8");
    system(p);
    if( p != NULL){
        free(p);
    }
}
static void ftpShareDirEdit(FtpShareDirProfile *oldProf,FtpShareDirProfile * prof)
{
    char tmp[128];
    char *p = NULL;
#if (UTT_SMART_UI == FYES)
    int count = 0;
    unsigned int flag = 0u;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char partition_name[10][128];
    int index = 0;

    flag |= USB_EXIST;
#if (SD_CARD_USAGE == FYES)
    flag |= SD_EXIST;
#endif
    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\" && mkdir -p \"%s\"",
	FTP_ROOT,oldProf->net_share_volume_name,FTP_NULL, netShareDirCount++,prof->net_share_volume_name);
#if 0
    if(isStorageExist(device_path,&count,flag)==1)
    {
	if (count > 0)
	{
	    if (count >1)
	    {
		strcpy(prof->net_share_volume_path,"/media/");
		nvramWriteCommit();
		if (prof->select_all)
		{
		    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\"", FTP_ROOT,oldProf->head.name,FTP_NULL, netShareDirCount++);
		    p = iconv_string(tmp,"GB2312","UTF-8");
		    system(p);
		    free(p);
		    p = NULL;
		    for (index=0;index<count;index++)
		    {
			sscanf(device_path[index], "/media/%s/", partition_name[index]);
			sprintf(tmp,"cd %s && mkdir -p \"%s/%s\" && mount --bind \'%s%s\' \'%s/%s\' || mv \"%s\" %s/\"s%u\"",
				FTP_ROOT,prof->head.name,partition_name[index],
				prof->net_share_volume_path,partition_name[index], prof->head.name,partition_name[index],prof->head.name,FTP_NULL,netShareDirCount++);
			p = iconv_string(tmp,"GB2312","UTF-8");
			system(p);
			free(p);
			p = NULL;
		    }
		    if( p != NULL){
			free(p);
		    }
		    return;
		}
	    }
	    else
	    {
		strcpy(prof->net_share_volume_path,device_path[0]);
		nvramWriteCommit();
	    }
	}
    }
    sprintf(tmp,"cd %s && mkdir -p \"%s\"", FTP_ROOT, oldProf->head.name);
    p = iconv_string(tmp,"GB2312","UTF-8");
    system(p);
    free(p);
    p = NULL;
    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\" && mkdir -p \"%s\" && mount --bind \'%s%s\' \'%s\' || mv \"%s\" %s/\"s%u\"",
            FTP_ROOT,oldProf->head.name,FTP_NULL,netShareDirCount++, prof->head.name,
            prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path ,prof->head.name,prof->head.name,FTP_NULL, netShareDirCount++);
#endif
#else
    sprintf(tmp,"cd %s && mv \"%s\" %s && mkdir -p \"%s\" && mount --bind \'%s%s\' \'%s\' || mv \"%s\" %s",
            FTP_ROOT,oldProf->head.name,FTP_NULL,prof->head.name,
            prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path ,prof->head.name,prof->head.name,FTP_NULL);
#endif
    p = iconv_string(tmp,"GB2312","UTF-8");
    system(p);
    if( p != NULL){
        free(p);
    }
}
static void ftpShareDirDel(FtpShareDirProfile *prof)
{
    char tmp[128];
    char *p = NULL;

#if (UTT_SMART_UI == FYES)
    sprintf(tmp,"cd %s && mv \"%s\" %s/\"s%u\"",FTP_ROOT,prof->head.name,FTP_NULL, netShareDirCount++);
#else
    sprintf(tmp,"cd %s && mv \"%s\" %s",FTP_ROOT,prof->head.name,FTP_NULL);
#endif
    p = iconv_string(tmp,"GB2312","UTF-8");
    system(p);
    if( p != NULL){
        free(p);
    }
}
void ftpShareDirInit(void)
{
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    int min = 0, max = 0;
	int i = 0,j = 0;
    char tmp[256];
    int count = 0;
    unsigned int flag = 0u;
    unsigned int flag1 = 0u;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char *p = NULL;
#if (UTT_SMART_UI == FYES)
    char partition_name[10][128];
    char netShareVolumePath[128];
    int index = 0;
#endif

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES)  || (UTT_NV_1220V2 == FYES))
//#if (UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES)
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
	    flag |=	SD_EXIST;
	#endif
    #endif
#endif

    memset(device_path,0,sizeof(device_path));

    shareDirDel();
#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES)  || (UTT_NV_1220V2 == FYES))
    if(isStorageExist1(device_path,&count,flag,flag1)!=1){
#else
    if(isStorageExist(device_path,&count,flag)!=1){
#endif
        return;
    }

    ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
        prof = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
#if (UTT_SMART_UI == FYES)
		if(!strcmp(prof->net_share_volume_name, "volume0"))
		    strcpy(prof->net_share_volume_name, prof->head.name);
		strcpy(netShareVolumePath,"/media/");
		if (prof->select_all)
		{
		    for (index=0;index<count;index++)
		    {
			sscanf(device_path[index], "/media/%s/", partition_name[index]);
			sprintf(tmp,"cd %s && mkdir -p \"%s/%s\" && mount --bind \'%s%s\' \'%s/%s\' || mv \"%s\" %s/\"s%u\"",
				//FTP_ROOT,prof->head.name,partition_name[index],
				//netShareVolumePath,partition_name[index], prof->head.name,partition_name[index],prof->head.name,FTP_NULL,netShareDirCount++);
				FTP_ROOT,prof->net_share_volume_name,partition_name[index],
				netShareVolumePath,partition_name[index], prof->net_share_volume_name,partition_name[index],prof->net_share_volume_name,FTP_NULL,netShareDirCount++);
			p = iconv_string(tmp,"GB2312","UTF-8");
			system(p);
			free(p);
			p = NULL;
		    }
		    if( p != NULL){
			free(p);
		    }
		    return;
		}
#else
            for(j=0;j<count;j++){
                if( !strcmp(prof->net_share_volume_path,device_path[j]) ){
                    break;
                }
            }
            if(j<count)
#endif
	    {
#if (UTT_SMART_UI)
                //sprintf(tmp,"cd %s && mkdir -p \"%s/%s\" && mount --bind \'%s%s\' \'%s/%s\' || mv \"%s\" %s/\"s%u\"",
                //        FTP_ROOT,prof->head.name,FTP_STORAGE,
                //        netShareVolumePath,prof->net_share_file_path, prof->head.name,FTP_STORAGE,prof->head.name,FTP_NULL, netShareDirCount++);
                sprintf(tmp,"cd %s && mkdir -p \"%s/%s\" && mount --bind \'%s%s\' \'%s/%s\' || mv \"%s\" %s/\"s%u\"",
                        FTP_ROOT,prof->net_share_volume_name,FTP_STORAGE,
                        netShareVolumePath,prof->net_share_file_path, prof->net_share_volume_name,FTP_STORAGE,prof->net_share_volume_name,FTP_NULL, netShareDirCount++);
#else
                sprintf(tmp,"cd %s && mkdir -p \"%s\" && mount --bind \'%s%s\' \'%s\' || mv \"%s\" %s",
                        FTP_ROOT,prof->head.name,
                        prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path, prof->head.name,prof->head.name,FTP_NULL);
#endif
                p = iconv_string(tmp,"GB2312","UTF-8");
                system(p);
                if( p != NULL){
                    free(p);
                }
            }
        }
    }
}

void shareDirDel(void)
{
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    int min = 0, max = 0;
	int i = 0,j = 0;
    char tmp[256];
    int count = 0;
    struct stat statbuf;
    char * p = NULL;
#if (UTT_SMART_UI == FYES)
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char delDir[256] = {0};
    int countftpNull = 0;
    int k = 0;
    unsigned int flag = 0u;
    int index = 0;
    char partition_name[10][128];
#endif

    ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) {
        prof = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
            sprintf(tmp,"%s%s", prof->net_share_volume_path,prof->select_all ? "" : prof->net_share_file_path);
            p = iconv_string(tmp,"GB2312","UTF-8");
            memcpy(tmp,p,strlen(p));
            if(p!=NULL){
                free(p);
            }
#if (UTT_SMART_UI != FYES)
            if( (stat(tmp,&statbuf) != 0) || ((statbuf.st_mode & S_IFMT) != S_IFDIR ))
#endif
            {
#if (UTT_SMART_UI == FYES)
		doSystem("umount %s/%s/%s",FTP_ROOT, prof->net_share_volume_name, FTP_STORAGE);
		doSystem("umount %s/%s/%s*",FTP_ROOT, prof->net_share_volume_name, USB_STORAGE_PR);
		doSystem("umount %s/%s/%s*",FTP_ROOT, prof->net_share_volume_name, SD_STORAGE_PR);
		doSystem("cd %s && mv \"%s\" %s/\"s%u\"",FTP_ROOT, prof->net_share_volume_name,FTP_NULL, netShareDirCount++);
		doSystem("umount %s/s*/*",FTP_NULL);
		doSystem("umount %s/g*/*",FTP_NULL);
		countftpNull = getftpNull(device_path);
		strcpy(delDir,"");
		for(k = 0;k<countftpNull;k++)
		{
		    sprintf(tmp," |grep -v %s",device_path[k]);
		    strcat(delDir,tmp);
		}
		doSystem("cd %s && rm -r \`ls %s\` &",FTP_NULL,delDir);
#else
                sprintf(tmp,"cd %s && umount \"%s\" && mv \"%s\" %s",FTP_ROOT,prof->head.name,prof->head.name,FTP_NULL);
                p = iconv_string(tmp,"GB2312","UTF-8");
                system(p);
                if( p != NULL){
                    free(p);
                }
#endif

#if (UTT_SMART_UI != FYES)
                ProfDelInstByIndex(mibType,i);
#endif
            }
        }
    }
}
void ftpServerRestart(void)
{
#if (UTT_SMART_UI == FYES)
    ftpShareDirInit();
#else
    shareDirDel();
#endif
    ftpServerInit();
}
void funInitFtpServer(void)
{
    registerForProfileUpdates(MIB_PROF_FTP_SERVER, (Pointer)NULL, (ProfileUpdateFnc)FtpServerProfileChange);
    ftpServerInit();
    return;
}

void funInitFtpShareDir(void)
{
    char tmp[128];

    sprintf(tmp,"mkdir -p %s ; mkdir -p %s",FTP_ROOT,FTP_NULL);
    system(tmp);
    registerForProfileUpdates(MIB_PROF_FTP_SHARE_DIR, (Pointer)NULL, (ProfileUpdateFnc)FtpShareDirProfileChange);
    ftpShareDirInit();
    return;
}
#endif
