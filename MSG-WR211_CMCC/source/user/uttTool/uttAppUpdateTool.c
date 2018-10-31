#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include "uttMachine.h"
#if (APP_POLICY_REMOTE_UPDATE == FYES) 
#include "mib.h"
#include "profacce.h"
#include "cJSON.h"
#include "md5.h"

#define DEBUG_SWITCh 0
#if DEBUG_SWITCh
#define DBGPRINT(fmt, args...)		printf(fmt, ## args)
#else
#define DBGPRINT(fmt, args...)
#endif
#define KERNEL_IMAGE "/kernelImage"
/*成功返回1，失败返回0*/
static int doUpdate(char *fileName,MibProfileType proftype)
{
    MD5_CONTEXT		md5ctx;
    unsigned char	hash[16];
    FILE *fp;
    int fileS, fileE, fileL;
    unsigned char *tmp;
    char md5[512] = {'\0'};
    char temp[3] = {'\0'};
    int i;
    char command[40];
    ProfChangeType profAction = PROF_CHANGE_EDIT;
    struProfAlloc *profhead = NULL;

    if((fp=fopen(fileName,"r")) == NULL)
    {
	return 0;
    }
    fseek(fp, 0, SEEK_SET);
    fileS = ftell(fp);
    fseek(fp, 0, SEEK_END);
    fileE = ftell(fp);
    fileL = fileE - fileS;
    if(fileL < 32) {
	fclose(fp);
	return 0;
    }
    tmp = (unsigned char*)malloc(fileL);
    memset(tmp, 0, fileL);
    if(tmp == NULL) {
	fclose(fp);
	return 0;
    }else {
	fseek(fp, 0, SEEK_SET);
	fread(tmp, fileL, 1, fp);
	fclose(fp);
	tmp[fileL-1]='\0';
	MD5Init(&md5ctx);
	MD5Update(&md5ctx, tmp, (unsigned int)(fileL-32));
	MD5Final(hash, &md5ctx);
	for(i=0; i< 16; i++)
	{   
	    sprintf(temp, "%02X",hash[i]);
	    strcat(md5,temp);
	}   
	DBGPRINT("hash[%s]\n",md5);
	DBGPRINT("tmp %s\n",tmp);
	if(memcmp(md5, tmp+strlen((char*)tmp)-32+1, 30) == 0) {
	    DBGPRINT("md5 check ok\n");
	    *(tmp+fileL-32) = 0;
	    DBGPRINT("%s :%d policyName tmp = \n%s\n",__func__,__LINE__,tmp);
	    snprintf(command,sizeof(command),"cat /dev/null > %s",fileName);
	    system(command); 
	    if((fp=fopen(fileName,"w+")) == NULL)
	    {
		DBGPRINT("%s %d\n",__func__,__LINE__);
		return 0;
	    }
	    fputs((char*)tmp, fp);
	    fclose(fp);
	    free(tmp);
	    switch (proftype) {
#if (ADFILTER == FYES)
		case MIB_PROF_ADFILTER :
		    uttSemP(SEM_ADFILTER,0);
		    break;
#endif
		default:
		break;
	    }
	    ProfBackupByIndex(proftype, profAction, 0, &profhead);
	    if(converConfigFromXmlFile(fileName) == 1){
		switch (proftype) {
#if (ADFILTER == FYES)
		    case MIB_PROF_ADFILTER :
			uttSemV(SEM_ADFILTER,0);
			break;
#endif
		    default:
		    break;
		}
		ProfUpdate(profhead);
		ProfFreeAllocList(profhead);
		nvramWriteCommit();
		DBGPRINT("up upconfig OK!\n");
		return 1;
	    }
	    else
	    {
		switch (proftype) {
#if (ADFILTER == FYES)
		    case MIB_PROF_ADFILTER :
			uttSemV(SEM_ADFILTER,0);
			break;
#endif
		    default:
		    break;
		}
		DBGPRINT("up upconfig error!\n");
		return 0;
	    }
	} else {
	    return 0;
	}
    }
}
int main (int argc, char *argv[])
{
    int appType;
    SystemProfile *profSys = NULL;
#if (ADFILTER == FYES)
    AdfilterProfile *adfilterProfile = NULL;
#endif
    FILE *fp = NULL;
    char *dl = NULL;

    char *fileName;
    char httpRequest[300];
    char command[50];

    char *dlPre, *an, *n;
    int v;
    cJSON *json = NULL, *apl, *ap,*pl, *p;
    int apl_size,pl_size,i,j;

    char *tmp;
    /*共享内存初始化*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }

    if (argc != 2) {
	return -1;
    }
    appType = atoi(argv[1]);

    DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if (profSys == NULL) {
	ProfDetach();
	return -1;
    }
    DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
    switch (appType) {
    case SOFT_UDTYPE:
	fileName = "firmwareUpdateFile";
	json = parseFile(fileName);
	if (json != NULL) {
	    tmp = cJSON_Print(json);
	    DBGPRINT("func:%s,line:%d,tmp=%s\n",__func__,__LINE__,tmp);
	    free(tmp);
    DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
	    dl = cJSON_GetObjectItem(json,"dl")->valuestring;/*软件升级链接，日后扩展*/
    DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
	    if (*dl != '\0') {
    DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
		snprintf(httpRequest,sizeof(httpRequest),"wget -O %s \"%s\"",KERNEL_IMAGE,dl);
		DBGPRINT("func:%s,line:%d,httpRequest=%s\n",__func__,__LINE__,httpRequest);
		fp = popen(httpRequest,"r");
		if (fp == NULL) {
		    perror("popen");
		    profSys->appUpdateStatus[appType] = APP_UPDATE_FAILED;/*置为更新失败*/
		}
		pclose(fp);
		fp = NULL;
	    }
	}
	break;
#if (ADFILTER == FYES)
    case ADFILTER_UDTYPE:
	profSys->appUpdateStatus[appType] = APP_UPDATING;/*置为正在更新*/
	adfilterProfile = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER,0);
	if (adfilterProfile == NULL) {
	    break;
	}
	fileName = "adfilterUpdateFile";
	json = parseFile(fileName);
	if (json != NULL) {
	    tmp = cJSON_Print(json);
	    DBGPRINT("func:%s,line:%d,tmp=%s\n",__func__,__LINE__,tmp);
	    free(tmp);
	    dlPre = cJSON_GetObjectItem(json,"dlPre")->valuestring;/*下载链接前缀地址*/
	    apl = cJSON_GetObjectItem(json,"apl");/*ap list, a cJSON Array*/
	    apl_size = cJSON_GetArraySize(apl);
	    DBGPRINT("func:%s,line:%d,apl_size=%d\n",__func__,__LINE__,apl_size);
	    for (i = 0; i < apl_size; i++) {
		ap = cJSON_GetArrayItem(apl,i);/*APP Policy, a cJSON Object*/
		an = cJSON_GetObjectItem(ap,"an")->valuestring;/*app name*/
		pl = cJSON_GetObjectItem(ap,"pl");/*Policy List,a cJSON Array*/
		pl_size = cJSON_GetArraySize(pl);/*policy list size*/
		for (j = 0; j < pl_size; j++) {
		    p = cJSON_GetArrayItem(pl,j);/*APP Policy, a cJSON Object*/
		    n = cJSON_GetObjectItem(p,"n")->valuestring;/*policy name*/
		    v = cJSON_GetObjectItem(p,"v")->valueint;/*policy version*/
		    dl = cJSON_GetObjectItem(p,"dl")->valuestring;/*policy xml path*/
		    if (strcmp(n,adfilterProfile->head.name) == 0 && v - adfilterProfile->policyVersion > 0) {/*有更新*/
			snprintf(httpRequest,sizeof(httpRequest),"wget -O %s \"http://%s/%s\"",dl,dlPre,dl);
			DBGPRINT("func:%s,line:%d,httpRequest=%s\n",__func__,__LINE__,httpRequest);
			fp = popen(httpRequest,"r");
			if (fp == NULL) {
			    perror("popen");
			    profSys->appUpdateStatus[appType] = APP_UPDATE_FAILED;/*置为更新失败*/
			}
			pclose(fp);
			fp = NULL;
			if (doUpdate(dl,MIB_PROF_ADFILTER) == 1) {/*更新成功*/
			    profSys->appUpdateStatus[appType] = APP_UPDATE_SUCCESS;/*置为更新成功*/
			    //adfilterProfile->policyVersion = v;
			} else {
			    profSys->appUpdateStatus[appType] = APP_UPDATE_FAILED;/*置为更新失败*/
			}
#if 1
			snprintf(command,sizeof(command),"rm -rf %s",dl);
			system(command);
#endif
		    }
		}
	    }
#if 1
	    snprintf(command,sizeof(command),"rm -rf %s",fileName);
	    system(command);
#endif
	} else {
	    profSys->appUpdateStatus[appType] = APP_UPDATE_FAILED;/*置为更新失败*/
	}
	break;
#endif
    default:
	break;
    }

    if (json != NULL) {
	cJSON_Delete(json);
	json = NULL;
    }
    ProfDetach();
    return 0;
}
#else
int main(int argc, char** argv)
{
    exit(0);
}
#endif
