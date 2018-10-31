#include "uttMachine.h"
#if (FTP_SERVER == FYES)
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
#include    <dirent.h>

#include  <iconv-user.h>

#define FTP_ROOT "/ftpRoot"
#define FTP_NULL "/ftpNull"

//#if(CONFIG_USER_GOAHEAD_LANG_EN == 1)
#define USB_DIR_NAME	"USB"
#define SD_DIR_NAME	"SD"
//#else
//#define USB_DIR_NAME	"U盘"
//#define SD_DIR_NAME	"SD卡"
//#endif

#if 0
#define DEBUG_PRINT(fmt, args...)		printf(fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif
static char dirLinkPath[512];
static char dirBackPath[512];
static char volumePath[64];
static char editNameAction[16];
#if (UTT_SMART_UI == FYES)
static int success = 1;
static void getRealPath(char *linkPath,char realPath[])
{
    int i = 0;
    char *p = NULL;

    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0,index;
    int upan,cycleIndex = 0;
    char relativePath[MAX_PATH_LEN] = {0};

    char partition_name[10][128];
    int flag = 0;
    flag = USB_EXIST | SD_EXIST;
    memset(device_path,0,sizeof(device_path));

    DEBUG_PRINT("func:%s,line:%d:linkPath=%s\n",__func__,__LINE__,linkPath);
    while(*(linkPath+i) == '/'){
        i++;
    }
    DEBUG_PRINT("func:%s,line:%d:linkPath+i=%s\n",__func__,__LINE__,linkPath+i);
    strcpy(realPath,linkPath+i);

    if((*realPath != '\0') && isStorageExist(device_path,&count,flag)==1) {
	//if (count > 1) {/*多分区*/
	if (count > 0) {
	    if (strncmp(linkPath+i,USB_DIR_NAME,2) == 0) {
		sscanf(linkPath+i,USB_DIR_NAME"%d%s",&upan,relativePath);
		DEBUG_PRINT("func:%s,line:%d,upan=%d\n",__func__,__LINE__,upan);
		for (index=0;index<count;index++)
		{
		    sscanf(device_path[index], "/media/%s/", partition_name[index]);
		    DEBUG_PRINT("func:%s,line:%d,partition_name[%d]=%s\n",__func__,__LINE__,index,partition_name[index]);
		    if (strstr(partition_name[index],"sd") != NULL) {
			cycleIndex++;
			DEBUG_PRINT("func:%s,line:%d,relativePath=%s\n",__func__,__LINE__,relativePath);
			if (upan == cycleIndex) {
			    if (*relativePath != '\0') {
				sprintf(realPath,"%s%s",partition_name[index],relativePath+1);
			    } else {
				sprintf(realPath,"%s%s",partition_name[index],relativePath);
			    }
			    break;
			}
		    }
		}
	    } else {
		sscanf(linkPath+i,SD_DIR_NAME"%s",relativePath);
		DEBUG_PRINT("func:%s,line:%d,relativePath=%s\n",__func__,__LINE__,relativePath);
		for (index=0;index<count;index++)
		{
		    sscanf(device_path[index], "/media/%s/", partition_name[index]);
		    if (strstr(partition_name[index],"mmcblk") != NULL) {
			if (*relativePath != '\0') {
			    sprintf(realPath,"%s%s",partition_name[index],relativePath+1);
			} else {
			    sprintf(realPath,"%s%s",partition_name[index],relativePath);
			}
			break;
		    }
		}
	    }
	    DEBUG_PRINT("func:%s,line:%d,realPath=%s\n",__func__,__LINE__,realPath);
	}
    }
}
#endif

static int aspOutputFtpServer(int eid, webs_t wp, int argc, char_t **argv)
{
    FtpServerProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FTP_SERVER;
    FtpShareDirProfile *profDir = NULL;
    MibProfileType mibTypeDir = MIB_PROF_FTP_SHARE_DIR;
    int min = 0, max = 0, i = 0,num = 0;

    prof = (FtpServerProfile*)ProfGetInstPointByIndex(mibType, 0);
    websWrite(wp, "var names=new Array();\n");
    websWrite(wp, "var zones=new Array();\n");
    websWrite(wp, "var dirctorys=new Array();\n");


    if (prof == NULL) {
	    return;
    }else{
        websWrite(wp, "var ftpEnable = \"%s\";\n", prof->ftpEnable == FUN_ENABLE ? "1" : "0"); /*启用开关*/
        websWrite(wp, "var wanEnable = \"%s\";\n", prof->allow_wan_visit == FUN_ENABLE ? "1" : "0"); /*启用开关*/
        websWrite(wp, "var ftp_port = \"%d\";\n", prof->ftp_port); /*启用开关*/

        ProfInstLowHigh(mibTypeDir, &max, &min);
        for (i = min; i < max; i++) {
            profDir = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibTypeDir, i);
            if((profDir != NULL) && (ProfInstIsFree(profDir) == 0) && (profDir->head.active == TRUE)){
                websWrite(wp,"names[%d] = \"%s\";\n", num, profDir->head.name);
                websWrite(wp,"zones[%d] = \"%s\";\n", num, profDir->net_share_volume_name);
                websWrite(wp,"dirctorys[%d] = \"%s\";\n", num, profDir->net_share_file_path);
                num++;
            }
        }
    }
    websWrite(wp, "var totalrecs = %d;\n",num);
    websWrite(wp,"var max_totalrecs =%d;\n",max);	/*output max inst num*/

    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/
    return 0;
}

static int aspOutputFtpServerLocalDir(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
	int ret=0;
    int i = 0;
    int num = 0;
    unsigned int flag = 0u;
    unsigned int flag1 = 0u; 
    int min = 0, max = 0;
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    char dir_path[512];
    char tmp[16];
    char *p = NULL;
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
	    flag |=	SD_EXIST;
	#endif
    #endif
#endif
    memset(device_path,0,sizeof(device_path));

    ProfInstLowHigh(mibType, &max, &min);
    while(1){
        sprintf(tmp,"folder%d",num);
        for(i=min;i<max;i++){
            prof = (FtpShareDirProfile*)ProfGetInstPointByIndex(mibType, i);
            if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)){
                if( !strcmp(tmp,prof->head.name) ){
                    break;
                }
            }
        }
        if( i>= max ){
            break;
        }
        num++;
    }
    websWrite(wp,"var filedirectories = new Array();\n");
    websWrite(wp, "editname= \"%s\";\n", editNameAction);
    if( strcmp(editNameAction,"")){
    websWrite(wp, "filename = \"%s\";\n", editNameAction);
    }else{
    websWrite(wp, "filename = \"folder%d\";\n", num);
    }
    memset(editNameAction,0,sizeof(editNameAction));

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
    if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else
    if(isStorageExist(device_path,&count,flag)==1){
#endif
        if(count>1){
	    for(i=0;i<count-1;i++){
		    strcpy(volumePath,"/media/");
		    strcpy(device_path[i],"/media/");
		    sprintf(dir_path,"%s%s",device_path[i],dirLinkPath);

		    dir = opendir(dir_path);
		    if(!dir){
			memset(dirLinkPath,0,sizeof(dirLinkPath));
			printf("Can not open %s!\n",dir_path);
			return -1;
		    }
		    if(strstr(dir_path,"sd")){
			websWrite(wp,"var volumeName = \"volume%d\";\n",1);
		    }else{
			websWrite(wp,"var volumeName = \"volume%d\";\n",0);
		    }
		    p = iconv_string(dirLinkPath,"UTF-8","GB2312");
		    websWrite(wp, "path = \"%s\";\n",p);
		    if ( p != NULL ){
			free(p);
            }

		    websWrite(wp,"var filedirectories = new Array(");
		    num = 0;
		    while(dp = readdir(dir)){
			if(strcmp(dp->d_name,".") == 0
			|| strcmp(dp->d_name,"..") == 0) {
                    continue;
			}
			sprintf(dir_path,"%s%s%s%s",device_path[i],dirLinkPath,dirLinkPath[i]?"/":"",dp->d_name);
			stat(dir_path,&statbuf);
			if((statbuf.st_mode & S_IFMT) == S_IFDIR ){
			    p = iconv_string(dp->d_name,"UTF-8","GB2312");
			    if( p!= NULL ){
				if(num == 0){
				    websWrite(wp,"\"%s\"",p);
				}else{
				    websWrite(wp,",\"%s\"",p);
				}
				free(p);
				num++;
			    }
			}
		    }
		    websWrite(wp,");\n");
		    closedir(dir);
		}
	    }else if(count == 1){
		strcpy(volumePath,device_path[0]);
		sprintf(dir_path,"%s%s",device_path[0],dirLinkPath);
		dir = opendir(dir_path);
		if(!dir){
		    memset(dirLinkPath,0,sizeof(dirLinkPath));
		    printf("Can not open %s!\n",dir_path);
		    return -1;
		}
		websWrite(wp,"var volumeName = \"volume%d\";\n",0);
		p = iconv_string(dirLinkPath,"UTF-8","GB2312");
		websWrite(wp, "path = \"%s\";\n",p);
		if ( p != NULL ){
		    free(p);
		}

		websWrite(wp,"var filedirectories = new Array(");
		num = 0;
		while(dp = readdir(dir)){
		    if(strcmp(dp->d_name,".") == 0
			    || strcmp(dp->d_name,"..") == 0){
			continue;
		    }
		    sprintf(dir_path,"%s%s%s%s",device_path[0],dirLinkPath,dirLinkPath[0]?"/":"",dp->d_name);
		    stat(dir_path,&statbuf);
		    if((statbuf.st_mode & S_IFMT) == S_IFDIR ){
			p = iconv_string(dp->d_name,"UTF-8","GB2312");
			if( p!= NULL ){
			    if(num == 0){
				websWrite(wp,"\"%s\"",p);
			    }else{
				websWrite(wp,",\"%s\"",p);
			    }
			    free(p);
			    num++;
			}
		    }
		}
		websWrite(wp,");\n");
		closedir(dir);
	    }else{
		;
	    }
	}
	memset(dirLinkPath,0,sizeof(dirLinkPath));

	getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/

	return 0;
    }


    static int getFtpServerDirOneInfo(int eid, webs_t wp, int argc, char **argv)
    {
	MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
	FtpShareDirProfile *prof = NULL;
	char *p = NULL;
	char *q = NULL;
	char tmpName[64];
	char filePath[512];
	char dir_path[512];
	char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
	int num = 0,count = 0;
	int min = 0, max = 0;
	int i,j;
	const char *ErrMsg = NULL;

	char *editName = NULL;
	DIR *dir;
	struct dirent *dp;
	struct stat statbuf;
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
    memset(tmpName,0,sizeof(tmpName));
    memset(filePath,0,sizeof(filePath));
    memset(dir_path,0,sizeof(dir_path));
    memset(device_path,0,sizeof(device_path));

    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ {
//        setTheErrorMsg("不存在此实例");
        ErrMsg = getTransDataForC(C_LANG_INDEX_CHANGED_CASE_NOT_EXITS);
        setTheErrorMsg(ErrMsg);
    }
    else {
        prof = ProfGetInstPointByName(mibType, editName);
	    if((prof != NULL) && (prof->head.active == TRUE)){

        p = iconv_string(prof->net_share_file_path,"GB2312","UTF-8");
        q = strrchr(p,'/');
        if( q != NULL ){
            strncpy(filePath,p,(q-p));
            strcpy(tmpName,q+1);
        }
        }
        if( p != NULL ){
            free(p);
        }

        websWrite(wp,"var filedirectories = new Array();\n");
        websWrite(wp, "filename = \"%s\";\n", prof->head.name);
        websWrite(wp, "volumeName = \"%s\";\n",prof->net_share_volume_name);
        p = iconv_string(filePath,"UTF-8","GB2312");
        websWrite(wp, "path = \"%s\";\n",p);
        if ( p != NULL ){
            free(p);
        }

#if ((UTT_NV_750GW == FYES) || (UTT_NV_521GV2 == FYES) || (UTT_NV_840GV2 == FYES) || (UTT_NV_750W == FYES) || (UTT_NV_1220GW == FYES) || (UTT_NV_1220V2 == FYES))
        if(isStorageExist1(device_path,&count,flag,flag1)==1){
#else
	if(isStorageExist(device_path,&count,flag)==1){
#endif
        if(count>1){
            for(i=0;i<count-1;i++){
		strcpy(volumePath,"/media/");
		strcpy(device_path[i],"/media/");
		sprintf(dir_path,"%s%s",device_path[i],dirLinkPath);

		dir = opendir(dir_path);
		if(!dir){
		    memset(dirLinkPath,0,sizeof(dirLinkPath));
		    printf("Can not open %s!\n",dir_path);
		    return -1;
		}
		if(strstr(dir_path,"sd")){
		    websWrite(wp,"var volumeName = \"volume%d\";\n",1);
		}else{
		    websWrite(wp,"var volumeName = \"volume%d\";\n",0);
		}
		p = iconv_string(dirLinkPath,"UTF-8","GB2312");
		websWrite(wp, "path = \"%s\";\n",p);
		if ( p != NULL ){
		    free(p);
		}

		websWrite(wp,"var filedirectories = new Array(");
		num = 0;
		while(dp = readdir(dir)){
		    if(strcmp(dp->d_name,".") == 0
			    || strcmp(dp->d_name,"..") == 0) {
			continue;
		    }
			sprintf(dir_path,"%s%s%s%s",device_path[i],dirLinkPath,dirLinkPath[i]?"/":"",dp->d_name);
			stat(dir_path,&statbuf);
			if((statbuf.st_mode & S_IFMT) == S_IFDIR ){
			    p = iconv_string(dp->d_name,"UTF-8","GB2312");
			    if( p!= NULL ){
				if(num == 0){
				    websWrite(wp,"\"%s\"",p);
				}else{
				    websWrite(wp,",\"%s\"",p);
				}
				free(p);
				num++;
			    }
			}
		    }
		    websWrite(wp,");\n");
		    closedir(dir);
		}
        }else if(count == 1){
            strcpy(volumePath,device_path[0]);
            sprintf(dir_path,"%s%s",device_path[0],filePath);
            dir = opendir(dir_path);
            if(!dir){
                memset(dirLinkPath,0,sizeof(dirLinkPath));
                printf("Can not open %s!\n",dir_path);
                return -1;
            }
            websWrite(wp,"var filedirectories = new Array(");
            num = 0;
            while(dp = readdir(dir)){
                if(strcmp(dp->d_name,".") == 0
                        || strcmp(dp->d_name,"..") == 0){
                    continue;
                }
                sprintf(dir_path,"%s%s/%s",device_path[0],filePath,dp->d_name);
                stat(dir_path,&statbuf);
                if((statbuf.st_mode & S_IFMT) == S_IFDIR ){
                    p = iconv_string(dp->d_name,"UTF-8","GB2312");
                    if( p != NULL ){
                    if(num == 0){
                        websWrite(wp,"\"%s\"",p);
                    }else{
                        websWrite(wp,",\"%s\"",p);
                    }
                    free(p);
                    num++;
                    }
                }
            }
            websWrite(wp,");\n");
            closedir(dir);
        }else{
            ;
        }
    }
    memset(dirLinkPath,0,sizeof(dirLinkPath));

    }

    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/

    return 0;

}
static void formFtpServerGloable(webs_t wp, char_t *path, char_t *query)
{
    FtpServerProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FTP_SERVER;
    struProfAlloc *profhead = NULL;
    FunEnableEnum enEnum = FUN_ENABLE;
    char_t *ftpEnable=NULL,*wanEnable=NULL,*ftpPort=NULL;
    const char *ErrMsg = NULL;

    prof = (FtpServerProfile*)ProfGetInstPointByIndex(mibType, 0);

    if (prof == NULL) {
	    return;
    }else{
        ftpEnable = websGetVar(wp, T("FTPServerEnable"), T(""));
        wanEnable = websGetVar(wp, T("WANEnable"), T(""));
        ftpPort = websGetVar(wp, T("ftpPort"), T(""));
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/

        if(0 != strcmp(ftpEnable, "on")) {
            enEnum = FUN_DISABLE;
        }else{
            enEnum = FUN_ENABLE;
        }
        prof->ftpEnable = enEnum;

        if(0 != strcmp(wanEnable, "on")) {
            enEnum = FUN_DISABLE;
        }else{
            enEnum = FUN_ENABLE;
        }
        prof->allow_wan_visit = enEnum;

        prof->ftp_port = strtol(ftpPort,NULL,10);

        ProfUpdate(profhead);/*发消息更新*/
        ProfFreeAllocList(profhead);/*释放malloc的内存*/
        nvramWriteCommit();/*写flash*/
    }
    websRedirect(wp, "FtpServer.asp");
}


static void FtpServerDirConfigAdd(webs_t wp)
{
    FtpShareDirProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    int instIndex = 0;
    int min,max;
    char_t *fileName=NULL,*volumeName=NULL;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;
#if (UTT_SMART_UI == FYES)
    char_t *savePathTmp=NULL;
    char savePath[MAX_PATH_LEN];

    savePathTmp = websGetVar(wp, T("resourcepath"), T(""));
    getRealPath(savePathTmp,savePath);
    fileName = websGetVar(wp, T("resourcename"), T(""));
#else
    savePath = websGetVar(wp, T("file_save_path"), T(""));
    fileName = websGetVar(wp, T("filename"), T(""));
#endif
    volumeName = websGetVar(wp, T("volume_name"), T(""));


    ProfInstLowHigh(mibType, &max, &min);
    for (instIndex = min; instIndex < max; instIndex++)
    {
        prof = (NetShareUserProfile *)ProfGetInstPointByIndex(mibType, instIndex);
#if (UTT_SMART_UI == FYES)
        if (!strcmp(fileName, prof->net_share_volume_name))
#else
        if (!strcmp(fileName, prof->head.name))
#endif
        {
            ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
            setTheErrorMsg(ErrMsg);
#if (UTT_SMART_UI == FYES)
	    success = 0;
#endif
            goto out;
        }
    }

    instIndex = ProfGetInstIndexFree(mibType);/*获取一个空闲实例*/
    if(instIndex == PROFFAIL) {
//        setTheErrorMsg("已达最大条目！");
        ErrMsg = getTransDataForC(C_LANG_INDEX_ALREADY_THE_MAX_TITLE);
        setTheErrorMsg(ErrMsg);
#if (UTT_SMART_UI == FYES)
	success = 0;
#endif

    } else {
        prof = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibType, instIndex);
        if(prof != NULL){
	        ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份*/
            prof->head.active=1;
#if (UTT_SMART_UI == FYES)
            strncpy(prof->head.name,fileName,12);
	    prof->head.name[13] = '\0';
            strcpy(prof->net_share_volume_name,fileName);
#else
            strcpy(prof->head.name,fileName);
            strcpy(prof->net_share_volume_name,volumeName);
#endif
            strcpy(prof->net_share_volume_path,volumePath);
            prof->select_all = savePath[0] ? FUN_DISABLE : FUN_ENABLE;
            strcpy(prof->net_share_file_path,savePath[0]?savePath:volumeName);

            ProfUpdate(profhead);/*发消息更新*/
            ProfFreeAllocList(profhead);/*释放malloc的内存*/
#if (UTT_SMART_UI != FYES)
            nvramWriteCommit();/*写flash*/
#endif
        }

#if (UTT_SMART_UI == FYES)
	success = 1;
#endif

    }

out:
#if (UTT_SMART_UI != FYES)
    websRedirect(wp, "FtpServer.asp");
#else
    return;
#endif
}

static void FtpServerDirConfigModify(webs_t wp)
{
    FtpShareDirProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    struProfAlloc *profhead = NULL;
    int instIndex = 0;
    int min = 0, max = 0, i = 0;
    char_t *fileName=NULL,*fileNameOld=NULL,*volumeName=NULL,*savePathOld=NULL;
    FILE * fd = NULL;
    char cmd[128];
    char line[128];
    char *p = NULL;
    const char *ErrMsg = NULL;

#if (UTT_SMART_UI == FYES)
    int k = 0;
    char_t *savePathTmp=NULL;
    char savePath[MAX_PATH_LEN];
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    char device_path_ftpNull[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
    int countftpNull = 0;
    unsigned int flag = 0u;
    char partition_name[10][128];
    int index = 0;

    flag |= USB_EXIST;
#if (SD_CARD_USAGE == FYES)

    flag |= SD_EXIST;
#endif
    memset(device_path,0,sizeof(device_path));
    memset(device_path_ftpNull,0,sizeof(device_path_ftpNull));
#endif
    memset(cmd,0,sizeof(cmd));
    memset(line,0,sizeof(line));

#if (UTT_SMART_UI == FYES)

    savePathTmp = websGetVar(wp, T("resourcepath"), T(""));
    fileName = websGetVar(wp, T("resourcename"), T(""));
#else
    savePath = websGetVar(wp, T("file_save_path"), T(""));
    fileName = websGetVar(wp, T("filename"), T(""));
#endif
    fileNameOld = websGetVar(wp, T("oldfilename"), T(""));
    volumeName = websGetVar(wp, T("volume_name"), T(""));
#if (UTT_SMART_UI == FYES)
    savePathOld = websGetVar(wp, T("olddirpass"), T(""));

    getRealPath(savePathTmp,savePath);

    if ((strcmp(fileName, fileNameOld) == 0) && (strcmp(savePath, savePathOld) == 0))
    {
	goto out;
    }
#endif

    if( strcmp(fileName,fileNameOld) ){
        ProfInstLowHigh(mibType, &max, &min);
        for (i = min; i < max; i++) 
        {
            prof = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibType, i);	/*get inst pointer by index*/
#if (UTT_SMART_UI == FYES)
            if((prof != NULL) && (strcmp(fileName, prof->net_share_volume_name) == 0)) {
#else
            if((prof != NULL) && (strcmp(fileName, prof->head.name) == 0)) {
#endif
//                setTheErrorMsg("已经有该名字的");
                ErrMsg = getTransDataForC(C_LANG_INDEX_CASE_ALREADY_EXITS);
                setTheErrorMsg(ErrMsg);
#if (UTT_SMART_UI == FYES)
		success = 0;
#endif
                goto out;
            }
        }
    }

    prof = ProfGetInstPointByName(mibType, fileNameOld);
    if(prof != NULL){
	int canDel = 0;
#if (UTT_SMART_UI == FYES)
	if(isStorageExist(device_path,&count,flag)==1)
	{
	    if (count > 0)
	    {
		if (prof->select_all)
		{
		    for (index=0;index<count;index++)
		    {
                         sscanf(device_path[index], "/media/%s/", partition_name[index]);
                         sprintf(cmd,"%s/%s/%s",FTP_ROOT,fileNameOld, partition_name[index]);
                         if(umountPath(cmd) == 0)
                         {
                             canDel = 1;
                         }
			 countftpNull = getftpNull(device_path_ftpNull);
			 for(k=0;k<countftpNull;k++)
			 {
			     sprintf(cmd,"%s/%s/%s",FTP_NULL,device_path_ftpNull[k], partition_name[index]);
			     if(umountPath(cmd) == 0)
			     {
				 canDel = 1;
			     }
			 }
		    }
		    if(canDel == 1)
		    {
			for (index=0;index<count;index++)
			{
			    sscanf(device_path[index], "/media/%s/", partition_name[index]);
			    sprintf(cmd,"%s/%s/%s",FTP_ROOT,fileNameOld, partition_name[index]);
			    if(umountPath(cmd) != 0)
			    {
				doSystem("mount --bind  \'/media/%s\' \'%s/%s/%s\'",partition_name[index],FTP_ROOT,fileNameOld,partition_name[index]);
			    }
			}
		    }
		}
		else
		{
		    countftpNull = getftpNull(device_path_ftpNull);
		    for(k=0;k<countftpNull;k++)
		    {
                         sprintf(cmd,"%s/%s/%s",FTP_NULL,device_path_ftpNull[k], FTP_STORAGE);
                         if(umountPath(cmd) == 0)
                         {
                             canDel = 1;
                         }
		    }
		    sprintf(cmd,"%s/%s/%s",FTP_ROOT,fileNameOld,FTP_STORAGE);
		    if(umountPath(cmd) == 0)
		    {
			canDel = 1;
		    }
		}
	    }
	}
#else
	sprintf(cmd,"umount \"%s/%s\" > /dev/null 2>&1; echo $?",FTP_ROOT,fileNameOld);
	p = iconv_string(cmd,"GB2312","UTF-8");
	fd = popen(p,"r");
	if(p!=NULL){
	    free(p);
	}
	fgets(line, sizeof(line), fd);
	canDel = strtol(line,NULL,10);
#endif

        if( !canDel ){
            ProfBackupByName(mibType, PROF_CHANGE_EDIT, fileNameOld, &profhead);/*备份*/
#if (UTT_SMART_UI == FYES)
            strncpy(prof->head.name,fileName,12);
	    prof->head.name[13] = '\0';
            strcpy(prof->net_share_volume_name,fileName);
#else
            strcpy(prof->head.name,fileName);
            strcpy(prof->net_share_volume_name,volumeName);
#endif
            strcpy(prof->net_share_volume_path,volumePath);
#if (UTT_SMART_UI == FYES)
            prof->select_all = savePath[0] ? FUN_DISABLE : FUN_ENABLE;
#endif
            strcpy(prof->net_share_file_path,savePath[0]?savePath:volumeName);
            ProfUpdate(profhead);/*发消息更新*/
            ProfFreeAllocList(profhead);/*释放malloc的内存*/
#if (UTT_SMART_UI == FYES)
	    success = 1;
#else
            nvramWriteCommit();/*写flash*/
#endif
        }else{
            ErrMsg = getTransDataForC(C_LANG_INDEX_SHARE_FILE_IS_BUSY);
            setTheErrorMsg(ErrMsg);
#if (UTT_SMART_UI == FYES)
	    success = 0;
#endif
            goto out;
        }
#if (UTT_SMART_UI != FYES)
        pclose(fd);
#endif
    }

out:
#if (UTT_SMART_UI != FYES)
    websRedirect(wp, "FtpServer.asp");
#else
    return;
#endif
}
static void formFtpServerDirConfig(webs_t wp, char_t *path, char_t *query)
{
	char_t *action = NULL;

	action = websGetVar(wp,T("Action"), T(""));

	/*add inst*/
	if (strcmp(action, "add") == 0) 
	{
        FtpServerDirConfigAdd(wp);
	}
	else    /*modify inst*/
	{
		FtpServerDirConfigModify(wp);
	}
	return;

}

static void formFtpServerShareDirSelcet(webs_t wp, char_t *path, char_t *query)
{
    char_t *linkPath=NULL,*fileName=NULL;
	char_t *editName= NULL;
    int i = 0;
    char *p = NULL;


    linkPath = websGetVar(wp, T("file_link_path"), T(""));
	editName= websGetVar(wp,T("oldfilename"), T(""));

    while(*(linkPath+i) == '/'){
        i++;
    }
    p = iconv_string(linkPath+i,"GB2312","UTF-8");
    strcpy(dirLinkPath,p);
    if( p != NULL ){
        free(p);
    }
    strcpy(dirBackPath,dirLinkPath);
    strcpy(editNameAction,editName);


    websRedirect(wp, "FtpServer_edit.asp");
}
static void formFtpServerShareDirGoBack(webs_t wp, char_t *path, char_t *query)
{
	char_t *editName= NULL;
    int i = 0;
    char *p = NULL;

	editName= websGetVar(wp,T("oldfilename"), T(""));

    p = strrchr(dirBackPath,'/');

    strncpy(dirLinkPath,dirBackPath,p?(p-dirBackPath):0);
    strcpy(dirBackPath,dirLinkPath);
    strcpy(editNameAction,editName);

    websRedirect(wp, "FtpServer_edit.asp");
}
static void formFtpServerDirDel(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

	char_t *tmp = NULL,*delStr = NULL;;
    FILE * fd = NULL;
    char cmd[128];
    char line[16];
    int allDel = 1;
    char *p = NULL;
    const char *ErrMsg = NULL;

    memset(cmd,0,sizeof(cmd));
    memset(line,0,sizeof(line));

    a_assert(wp);

	delStr = websGetVar(wp,T("delstr"),T("")); 

	if (strcmp(delStr,"")!=0){
        tmp = strtok(delStr, UTT_SPLIT_SIGN_STR);
        while(tmp != NULL){
            prof=(FtpShareDirProfile *)ProfGetInstPointByName(mibType, tmp);
            if(prof != NULL){
                int canDel;
                sprintf(cmd,"umount \"%s/%s\" > /dev/null 2>&1; echo $?",FTP_ROOT,tmp);
                p = iconv_string(cmd,"GB2312","UTF-8");
                fd = popen(p,"r");
                if( p!=NULL ){
                    free(p);
                }
                fgets(line, sizeof(line), fd);
                canDel = strtol(line,NULL,10);
                if( !canDel ){
                    ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
                    ProfDelInstByName(mibType,tmp);
                }else{
                    allDel = 0;
                }
                pclose(fd);
            }
            tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
        }
    }

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*保存flash*/

    if( !allDel ){
        ErrMsg = getTransDataForC(C_LANG_INDEX_SHARE_FILE_IS_BUSY);
        setTheErrorMsg(ErrMsg);
    }
	websRedirect(wp, "FtpServer.asp");

}
static void formFtpServerDirDelAll(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    struProfAlloc *profhead = NULL;

	int min,max,i;
    FILE * fd = NULL;
    char cmd[128];
    char line[16];
    int allDel = 1;
    char *p = NULL;
    const char *ErrMsg = NULL;

    memset(cmd,0,sizeof(cmd));
    memset(line,0,sizeof(line));

    ProfInstLowHigh(mibType,&max,&min);
    for(i=min;i<max;i++){
        prof=(FtpShareDirProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
        if((prof!=NULL)&&(ProfInstIsFree(prof)==0)&& (prof->head.active == TRUE)){
            int canDel;
            sprintf(cmd,"umount \"%s/%s\" > /dev/null 2>&1; echo $?",FTP_ROOT,prof->head.name);
            p = iconv_string(cmd,"GB2312","UTF-8");
            fd = popen(p,"r");
            if( p != NULL ){
                free(p);
            }
            fgets(line, sizeof(line), fd);
            canDel = strtol(line,NULL,10);
            if( !canDel ){
                ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profhead);
                ProfDelInstByIndex(mibType, i);
            }else{
                allDel = 0;
            }
            pclose(fd);
        }
    }

    ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();/*保存flash*/

    if( !allDel ){
        ErrMsg = getTransDataForC(C_LANG_INDEX_SHARE_FILE_IS_BUSY);
        setTheErrorMsg(ErrMsg);
    }
	websRedirect(wp, "FtpServer.asp");	/*页面跳转*/
}

#if (UTT_SMART_UI == FYES)
/*新风格UI A750W网络共享功能*/
static int aspOutputNetShare_Sui(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibTypeNSM = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile* profNSM = NULL;
    MibProfileType mibTypeFS = MIB_PROF_FTP_SERVER;
    FtpServerProfile *profFS = NULL;
    MibProfileType mibTypeNSU = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *profNSU = NULL;
    MibProfileType mibTypeDir = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *profDir = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
    unsigned int flag = 0u;
    long long total_space = 0,used_space = 0,unused_space = 0;
    int i = 0;

#if (UTT_SMART_UI == FYES)
    int index;
    int multiFlag = 0;
    char partition_name[10][128];
    int upan = 0;
#endif

    flag |= USB_EXIST;
    memset(device_path,0,sizeof(device_path));

    profNSM = (NetShareManageProfile *)ProfGetInstPointByIndex(mibTypeNSM, 0);
    if (ProfInstIsFree(profNSM) != PROFOK)
    {
	goto out;
    }
    profFS = (FtpServerProfile*)ProfGetInstPointByIndex(mibTypeFS, 0);
    if (ProfInstIsFree(profFS) != PROFOK)
    {
	goto out;
    }
    profNSU = (NetShareUserProfile *)ProfGetInstPointByIndex(mibTypeNSU, 3);
    if (ProfInstIsFree(profNSU) == PROFOK)
    {
	websWrite(wp, "usernames = \"%s\";", profNSU->head.name);
	websWrite(wp, "passwords = \"%s\";", profNSU->password);
	websWrite(wp, "usernameolds = \"%s\";", profNSU->head.name);
	websWrite(wp, "passwordolds = \"%s\";", profNSU->password);
    }
    profDir = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibTypeDir, 0);

    if(isStorageExist(device_path,&count,flag)==1)
    {
	for(i=0;i<count;i++)
	{
	    storageFree(device_path[i], &total_space, &unused_space);
	    used_space = total_space - unused_space;
	    total_space >>= 20;
	    used_space >>= 20;
	    unused_space >>= 20;
	    if( total_space )
	    {
		websWrite(wp, "usb=\"1\";useage=\"%dM\";totalage=\"%dM\";", (int)used_space, (int)unused_space);
	    }
	}
    }
#if (SD_CARD_USAGE == FYES)
    flag = SD_EXIST;
    if(isStorageExist(device_path,&count,flag))
    {
	storageFree(device_path[0], &total_space, &unused_space);
	used_space = total_space - unused_space;
	total_space >>= 20;
	used_space >>= 20;
	unused_space >>= 20;
	if(total_space)
	{
	    websWrite(wp, "sd=\"1\";sd_useage=\"%dM\";sd_totalage=\"%dM\";", (int)used_space, (int)unused_space);
	} else {
	    websWrite(wp, "sd=\"0\";");
	}
    } else {
	websWrite(wp, "sd=\"0\";");
    }
    websWrite(wp, "sd_surpport=\"1\";");
#else
    websWrite(wp, "sd_surpport=\"0\";");
    websWrite(wp, "sd=\"0\";");
#endif

    if (profNSM->netShareEn == FUN_ENABLE)
    {
	websWrite(wp, "enabledShare = \"on\";");
    }
    if (profNSM->sambaEn == FUN_ENABLE)
    {
	websWrite(wp, "sambaservers = \"1\";");
    }
    if (profNSM->ftpEn == FUN_ENABLE)
    {
	websWrite(wp, "ftpservers = \"1\";");
    }
    websWrite(wp, "ftpports = \"%d\";", profFS->ftp_port);
    if (profNSM->allow_wan_visit == FUN_ENABLE)
    {
	websWrite(wp, "allowInternet = \"1\";");
    }
    if (profNSM->need_passwd == FUN_ENABLE)
    {
	websWrite(wp, "allowPwd = \"1\";");
    }
    
    if (ProfInstIsFree(profDir) == PROFOK)
    {
	
	if(!strcmp(profDir->net_share_volume_name, "volume0"))
	    strcpy(profDir->net_share_volume_name, profDir->head.name);
	//websWrite(wp, "resourcenames = \"%s\";", profDir->head.name);
	websWrite(wp, "resourcenames = \"%s\";", profDir->net_share_volume_name);
	if (strcmp(profDir->net_share_file_path, "volume0"))
	{

	    isStorageExist(device_path,&count,USB_EXIST
#if (SD_CARD_USAGE == FYES)
		|SD_EXIST
#endif
		);
		{
		    multiFlag = 1;
		    if (strncmp(profDir->net_share_file_path,"mmcblk",6) == 0) 
		    {
			multiFlag = 2;
			websWrite(wp, "resourcespaths = \"/%s%s\";",SD_DIR_NAME, strstr(profDir->net_share_file_path,"/"));
		    }
		    else
		    {
			for (index=0;index<count;index++)
			{
			    sscanf(device_path[index], "/media/%s/", partition_name[index]);
			    if (strstr(partition_name[index],"sd") != NULL) {
				upan++;
				DEBUG_PRINT("uapn=%d, partition_name[%d]=%s\n",upan,index,partition_name[index]);
				if (strncmp(profDir->net_share_file_path,partition_name[index],strlen(partition_name[index])) == 0) 
				{
				    multiFlag = 2;
				    websWrite(wp, "resourcespaths = \"/%s%d%s\";",USB_DIR_NAME,upan,strstr(profDir->net_share_file_path,"/"));
				    break;
				}
			    }
			}
		    }
//		    for (index=0;index<count;index++)
//		    {
//			sscanf(device_path[index], "/media/%s/", partition_name[index]);
//			if (strncmp(profDir->net_share_file_path,"mmcblk",6) == 0) {
//			    websWrite(wp, "resourcespaths = \"/%s%s\";",SD_DIR_NAME, strstr(profDir->net_share_file_path,"/"));
//			    break;
//			} else {
//			    if (strstr(partition_name[index],"sd") != NULL) {
//				upan++;
//				DEBUG_PRINT("uapn=%d, partition_name[%d]=%s\n",upan,index,partition_name[index]);
//				if (strncmp(profDir->net_share_file_path,partition_name[index],strlen(partition_name[index])) == 0) 
//				{
//				    websWrite(wp, "resourcespaths = \"/%s%d%s\";",USB_DIR_NAME,upan,strstr(profDir->net_share_file_path,"/"));
//				    break;
//				}
//			    }
//			}
//
//		    }

		}
	    if (multiFlag == 0)
		websWrite(wp, "resourcespaths = \"/%s\";", profDir->net_share_file_path);
	    else if (multiFlag == 1)
		websWrite(wp, "resourcespaths = \"No Root !\";");
	}
    }

out:
    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/
#if (UTT_SMART_UI == FYES)
    getTheRightMsg(eid,wp,argc,argv);
#endif
    return 0;
}


/*新风格UI A750W网络共享功能 提交函数*/
static void formNetShare_Sui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibTypeNSM = MIB_PROF_NET_SHARE_MANAGE;
    NetShareManageProfile* profNSM = NULL;
    MibProfileType mibTypeFS = MIB_PROF_FTP_SERVER;
    FtpServerProfile *profFS = NULL;
    MibProfileType mibTypeNSU = MIB_PROF_NET_SHARE_USER;
    NetShareUserProfile *profNSU = NULL;
    MibProfileType mibTypeFSD = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *profFSD = NULL;
    struProfAlloc *profhead = NULL;
    const char *ErrMsg = NULL;
    FunEnableEnum need_passwdEn = FUN_ENABLE;
    FunEnableEnum allow_wanEn = FUN_ENABLE;
    FunEnableEnum ftpEnable = FUN_DISABLE;

    char *netShareEn = NULL, *sambaEn = NULL, *ftpEn = NULL;
    char *ftpPort = NULL, *wanEnable = NULL, *passwdEnable = NULL;
    char *userName = NULL, *passWord = NULL, *userNameOld = NULL, *passWordOld = NULL, *fileName = NULL, *savePath = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
    unsigned int flag = 0u;

    flag |= USB_EXIST;
#if (SD_CARD_USAGE == FYES)

    flag |= SD_EXIST;
#endif
    memset(device_path,0,sizeof(device_path));

    netShareEn = websGetVar(wp, T("nershareEn"), T(""));
    sambaEn = websGetVar(wp, T("sambaEn"), T(""));
    ftpEn = websGetVar(wp, T("ftpEn"), T(""));
    ftpPort = websGetVar(wp, T("ftpport"), T(""));
    wanEnable = websGetVar(wp, T("WANEnable"), T(""));
    passwdEnable = websGetVar(wp, T("enablePwd"), T(""));
    userName = websGetVar(wp, T("username"), T("")); 
    passWord = websGetVar(wp, T("ftppwd"), T("")); 
    userNameOld = websGetVar(wp, T("usernameold"), T(""));
    passWordOld = websGetVar(wp, T("passwordold"), T(""));
    fileName = websGetVar(wp, T("resourcename"), T(""));
    savePath = websGetVar(wp, T("resourcepath"), T(""));


    if (strlen(userName) > (UTT_INST_NAME_LEN - 1))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_USER_NAME_LEN_LESS_11);
	setTheErrorMsg(ErrMsg);
	goto out;
    }
    if (strlen(passWord) > (UTT_PASSWD_LEN - 1))
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_USER_PASSWD_LEN_LESS_31);
	setTheErrorMsg(ErrMsg);
	goto out;
    }

    profNSM = (NetShareManageProfile *)ProfGetInstPointByIndex(mibTypeNSM, 0);
    if (ProfInstIsFree(profNSM) != PROFOK)
    {
	goto out;
    }
    profFS = (FtpServerProfile*)ProfGetInstPointByIndex(mibTypeFS, 0);
    if (ProfInstIsFree(profFS) != PROFOK)
    {
	goto out;
    }
    profNSU = (NetShareUserProfile *)ProfGetInstPointByIndex(mibTypeNSU, 3);

    if(isStorageExist(device_path,&count,flag)==1)
    {
	if (count > 0)
	{
	    if (count > 1)
	    {
		strcpy(volumePath, "/media/");
	    }
	    else
	    {
		strcpy(volumePath, device_path[0]);
	    }
	}
    }
    profFSD = (FtpShareDirProfile *)ProfGetInstPointByIndex(mibTypeFSD, 0);
    if ((ProfInstIsFree(profFSD) == PROFOK) && (profFSD->head.active == TRUE))
    {
	FtpServerDirConfigModify(wp);
    }
    else
    {
	FtpServerDirConfigAdd(wp);
    }
    usleep(200);

    if (!success)
    {
	goto out;
    }

    if (ProfInstIsFree(profNSU) != PROFOK)
    {
	ProfBackupByIndex(mibTypeNSU, PROF_CHANGE_ADD, 3, &profhead);
	strcpy(profNSU->head.name, userName);
	profNSU->head.active = TRUE;
	strcpy(profNSU->password, passWord);
	profNSU->jurisdiction = SHARE_READ_WRITE;
	profNSU->allow_ftp = FUN_ENABLE;
	ProfUpdate(profhead);/*发消息更新*/
	ProfFreeAllocList(profhead);/*释放malloc的内存*/
	profhead = NULL;
	usleep(200);
    }
    else if (strcmp(userName, userNameOld) || strcmp(passWord, passWordOld))
    {
	ProfBackupByIndex(mibTypeNSU, PROF_CHANGE_EDIT, 3, &profhead);
	strcpy(profNSU->head.name, userName);
	profNSU->head.active = TRUE;
	strcpy(profNSU->password, passWord);
	profNSU->jurisdiction = SHARE_READ_WRITE;
	profNSU->allow_ftp = FUN_ENABLE;
	ProfUpdate(profhead);/*发消息更新*/
	ProfFreeAllocList(profhead);/*释放malloc的内存*/
	profhead = NULL;
	usleep(200);
    }

    ProfBackupByIndex(mibTypeNSM, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/
    profNSM->head.active = TRUE;
    if (strncmp(netShareEn, "1", 2))
    {
	profNSM->netShareEn = FUN_DISABLE;
	profNSM->GlbEn = FUN_DISABLE;
	ftpEnable = FUN_DISABLE;
    }
    else
    {
	profNSM->netShareEn = FUN_ENABLE;
	if (!strncmp(sambaEn, "on", 3))
	{
	    profNSM->GlbEn = FUN_ENABLE;
	}
	else
	{
	    profNSM->GlbEn = FUN_DISABLE;
	}
	if (!strncmp(ftpEn, "on", 3))
	{
	    ftpEnable = FUN_ENABLE;
	}
	else
	{
	    ftpEnable = FUN_DISABLE;
	}
    }
    if (strncmp(sambaEn, "on", 3))
    {
	profNSM->sambaEn = FUN_DISABLE;
    }
    else
    {
	profNSM->sambaEn = FUN_ENABLE;
    }
    if (strncmp(ftpEn, "on", 3))
    {
	profNSM->ftpEn = FUN_DISABLE;
    }
    else
    {
	profNSM->ftpEn = FUN_ENABLE;
    }
    if (strncmp(wanEnable, "on", 3))
    {
	profNSM->allow_wan_visit = FUN_DISABLE;
	allow_wanEn = FUN_DISABLE;
    }
    else
    {
	profNSM->allow_wan_visit = FUN_ENABLE;
	allow_wanEn = FUN_ENABLE;
    }
    if (strncmp(passwdEnable, "on", 3))
    {
	profNSM->need_passwd = FUN_DISABLE;
	need_passwdEn = FUN_DISABLE;
    }
    else
    {
	profNSM->need_passwd = FUN_ENABLE;
	need_passwdEn = FUN_ENABLE;
    }
    strncpy(profNSM->dirName, fileName, 32);
    savePath++;
    strcpy(profNSM->net_share_file_path, savePath);
    profNSM->change = profNSM->change + 1;
    ProfUpdate(profhead);/*发消息更新*/
    ProfFreeAllocList(profhead);/*释放malloc的内存*/
    profhead = NULL;
    usleep(200);

    ProfBackupByIndex(mibTypeFS, PROF_CHANGE_EDIT, 0, &profhead);/*备份*/
    profFS->ftpEnable = ftpEnable;
    profFS->ftp_port = strtol(ftpPort, NULL, 10);
    profFS->allow_wan_visit = allow_wanEn;
    profFS->need_passwd = need_passwdEn;
    profFS->change = profFS->change + 1;
    ProfUpdate(profhead);/*发消息更新*/
    ProfFreeAllocList(profhead);/*释放malloc的内存*/

    nvramWriteCommit();/*写flash*/
#if (UTT_SMART_UI == FYES)
    setSuccessTips();
#endif
out:
#if (UTT_SMART_UI == FYES) 
    setSuccessTips();
#endif
    websRedirect(wp, "NetShare_Sui.asp");
}


static void ftpServerShareDirSelcetSui(char *linkPath)
{
    char_t *fileName=NULL;
	char_t *editName= "";
    int i = 0;
    char *p = NULL;


    char realPath[MAX_PATH_LEN];

    getRealPath(linkPath,realPath);
    p = iconv_string(realPath,"GB2312","UTF-8");
    strcpy(dirLinkPath,p);
    if( p != NULL ){
        free(p);
    }
    strcpy(dirBackPath,dirLinkPath);
    strcpy(editNameAction,editName);
}


/*网络共享输出目录*/
static void formFtpServerLocalDirSui(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_FTP_SHARE_DIR;
    FtpShareDirProfile *prof = NULL;
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int count = 0;
	int ret=0;
    int i = 0;
    int num = 0;
    unsigned int flag = 0u;
    int min = 0, max = 0;
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    char dir_path[512];
    char tmp[16];
    char *p = NULL;
    char *currentPath = NULL;
    char partition_name[10][128];
    int index = 0;
    int upan = 0;

    flag |= USB_EXIST;
#if (SD_CARD_USAGE == FYES)
    flag |= SD_EXIST;
#endif
    memset(device_path,0,sizeof(device_path));

    currentPath = websGetVar(wp, T("path"), T(""));
    ftpServerShareDirSelcetSui(currentPath);
     
    /*ajax头*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=GB2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));

    ProfInstLowHigh(mibType, &max, &min);
    while(1){
        sprintf(tmp,"folder%d",num);
        for(i=min;i<max;i++){
            prof = (FtpShareDirProfile*)ProfGetInstPointByIndex(mibType, i);
            if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)){
#if (UTT_SMART_UI == FYES)
                if( !strcmp(tmp,prof->net_share_volume_name) ){
#else
                if( !strcmp(tmp,prof->head.name) ){
#endif
                    break;
                }
            }
        }
        if( i>= max ){
            break;
        }
        num++;
    }
    websWrite(wp,"var filedirectories = new Array();\n");
    websWrite(wp, "editname= \"%s\";\n", editNameAction);
    if( strcmp(editNameAction,"")){
    websWrite(wp, "filename = \"%s\";\n", editNameAction);
    }else{
    websWrite(wp, "filename = \"folder%d\";\n", num);
    }
    memset(editNameAction,0,sizeof(editNameAction));

    if(isStorageExist(device_path,&count,flag)==1){
	websWrite(wp,"exist = 1;\n");
        if(count>0){
	    if (count >= 1)
	    {
		for (index=0;index<count;index++)
		{
		    sscanf(device_path[index], "/media/%s/", partition_name[index]);
		}
		strcpy(device_path[0], "/media/");
	    }
	    strcpy(volumePath,device_path[0]);
	    sprintf(dir_path,"%s%s",device_path[0],dirLinkPath);
	    dir = opendir(dir_path);
	    websWrite(wp,"var volumeName = \"volume%d\";\n",0);
	    p = iconv_string(dirLinkPath,"UTF-8","GB2312");
	    websWrite(wp, "path = \"%s\";\n",p);
	    if ( p != NULL ){
		free(p);
	    }

	    if(!dir){
		memset(dirLinkPath,0,sizeof(dirLinkPath));
		printf("Can not open %s!\n",dir_path);
		websWrite(wp, "error = 1;\n");
		goto out;
	    }
	    websWrite(wp,"var filedirectories = new Array(");
	    if (
		(count >= 1) 
		&& (!strcmp(dirLinkPath, "")))/*多分区*/
	    {	
		for (index=0;index<count;index++)
		{
		    DEBUG_PRINT("func:%s,line:%d,count=%d,index=%d\n",__func__,__LINE__,count,index);
		    if (strstr(partition_name[index],"sd") != NULL) {
			memset(partition_name[index],0,sizeof(partition_name[index]));
			sprintf(partition_name[index],USB_DIR_NAME"%d",++upan);
		    } else {
			memset(partition_name[index],0,sizeof(partition_name[index]));
			strcpy(partition_name[index],SD_DIR_NAME);
		    }
		    if(index == 0){
			websWrite(wp,"\"%s\"",partition_name[index]);
		    }else{
			websWrite(wp,",\"%s\"",partition_name[index]);
		    }
		}
	    }
	    else/*单分区*/
	    {
		num = 0;
		while(dp = readdir(dir)){
		    if(strcmp(dp->d_name,".") == 0
			    || strcmp(dp->d_name,"..") == 0){
			continue;
		    }
		    sprintf(dir_path,"%s%s%s%s",device_path[0],dirLinkPath,dirLinkPath[0]?"/":"",dp->d_name);
		    stat(dir_path,&statbuf);
		    if((statbuf.st_mode & S_IFMT) == S_IFDIR ){
			p = iconv_string(dp->d_name,"UTF-8","GB2312");
			if( p!= NULL ){
			    if(num == 0){
				websWrite(wp,"\"%s\"",p);
			    }else{
				websWrite(wp,",\"%s\"",p);
			    }
			    free(p);
			    num++;
			}
		    }
		}
	    }
            websWrite(wp,");\n");
            closedir(dir);
        }else{
            ;
        }
    }
    else
    {
	websWrite(wp, "error = 1;\n");
	websWrite(wp,"exist = 0;\n");
    }
    memset(dirLinkPath,0,sizeof(dirLinkPath));
    
out:
    websDone(wp, 200);/*ajax尾*/
}
#endif

extern void formDefineFtpServer(void) {
    websAspDefine(T("aspOutputFtpServer"), aspOutputFtpServer);
    websAspDefine(T("aspOutputFtpServerLocalDir"), aspOutputFtpServerLocalDir);
	websAspDefine(T("aspOutputFtpServerDirOneInfo"), getFtpServerDirOneInfo);
	websFormDefine(T("formFtpServerGloable"), formFtpServerGloable);
	websFormDefine(T("formFtpServerShareDirSelcet"), formFtpServerShareDirSelcet);
	websFormDefine(T("formFtpServerShareDirGoBack"), formFtpServerShareDirGoBack);
	websFormDefine(T("formFtpServerDirConfig"), formFtpServerDirConfig);
	websFormDefine(T("formFtpServerDirDel"), formFtpServerDirDel);
	websFormDefine(T("formFtpServerDirDelAll"), formFtpServerDirDelAll);
#if (UTT_SMART_UI == FYES)
	websAspDefine(T("aspOutputNetShare_Sui"), aspOutputNetShare_Sui);
	websFormDefine(T("formNetShare_Sui"), formNetShare_Sui);
	websFormDefine(T("formFtpServerLocalDirSui"), formFtpServerLocalDirSui);
#endif
}

#endif
