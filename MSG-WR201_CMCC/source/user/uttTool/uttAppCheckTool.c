#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include "uttMachine.h"
#if (APP_POLICY_REMOTE_UPDATE == FYES) 
#include "mib.h"
#include "profacce.h"
#include "cJSON.h"

#define DEBUG_SWITCh 0
#if DEBUG_SWITCh
#define DBGPRINT(fmt, args...)		printf(fmt, ## args)
#else
#define DBGPRINT(fmt, args...)
#endif

#define SOFT_HOST "rjsj.utt.com.cn"
#define POLICY_HOST "clsj.utt.com.cn"

int main (int argc, char *argv[])
{
    int appType;
    SystemProfile *profSys = NULL;
#if (ADFILTER == FYES)
    AdfilterProfile *adfilterProfile = NULL;
#endif
    FILE *fp = NULL;
    unsigned flag = 0U, finished = 0U, needUpdate = 0U;
    char *st = NULL, *sv = NULL,  *dl = NULL, *rn = NULL;
    int ver,ckv,ck;
    int bt;
    char curSt[20];
    //char curSv[4] = {0},remoteSv[4] = {0};
    int curSv[3] = {0},remoteSv[3] = {0};
    int curVersion,remoteVersion;
    int curBt;

    char fileName[30];
    char httpRequest[300];
    char filePath[250];
    char command[50];

    char *dlPre, *an, *n;
    int v;
    cJSON *json = NULL, *apl, *ap,*pl, *p;
    int apl_size,pl_size,i,j;
    char *tmp;
    cJSON *tmpJson;

    /*共享内存初始化*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }

    DBGPRINT("func:%s,line:%d,argc=%d\n",__func__,__LINE__,argc);
    if (argc > 2) {
	printf("Usage[1]:%s(check all appType)\nUsage[2]:uttAppUpdateTool appType \n",argv[0]);
	goto err;
    }
    fp = popen("uname -v", "r");/*获取当前软件版本信息*/
    if (fp != NULL) {
	fscanf(fp,"%s v%d.%d.%d-%d",curSt,&curSv[0],&curSv[1],&curSv[2],&curBt);
	pclose(fp);
	fp = NULL;
    }
    if (argc == 2) {
	appType = atoi(argv[1]);
    } else {
	flag = 1U;/*检测所有APP*/
	appType = SOFT_UDTYPE;/*首先检测软件升级*/
    }
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0);
    if (profSys == NULL) {
	goto err;
    }
    profSys->appUpdateStatus[appType] = APP_CHECKING;/*置为检测中*/
#if (ADFILTER == FYES)
    adfilterProfile = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER,0);
    if (adfilterProfile == NULL) {
	goto err;
    }
#endif

    while (1) {
	switch (appType) {
	case SOFT_UDTYPE:/*构造固件更新路径*/
	    sprintf(filePath,"softUpdateCheck.php?st=%s&sv=%d.%d.%d&bt=%d",curSt,curSv[0],curSv[1],curSv[2],curBt);
	    strcpy(fileName,"firmwareUpdateFile");
	    break;
#if (ADFILTER == FYES)
	case ADFILTER_UDTYPE:
	    adfilterProfile = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER,0);
	    if (adfilterProfile == NULL) {
		goto err;
	    }
	    sprintf(filePath,"policyUpCheck.php?appName=%s&appVersion=%d&polciyName=%s&policyVersion=%d",adfilterProfile->head.name,ADFILTER_VERSION,adfilterProfile->head.name,adfilterProfile->policyVersion);
	    strcpy(fileName,"adfilterUpdateFile");
	    break;
#endif
	default:
	    finished = 1U;
	    break;
	}
	if (finished) {
	    break;
	}

	switch (appType) {
	case SOFT_UDTYPE:/*构造固件更新路径*/
	    snprintf(httpRequest,300,"wget -O %s \"http://%s/%s\"",fileName,SOFT_HOST,filePath);
	    break;
	default:
	    snprintf(httpRequest,300,"wget -O %s \"http://%s/%s\"",fileName,POLICY_HOST,filePath);
	    break;
	}

	DBGPRINT("func:%s,line:%d,httpRequest=%s\n",__func__,__LINE__,httpRequest);
#if 0
	snprintf(command,40,"rm -rf %s 1>/dev/null 2>&1",fileName);
	system(command);
#endif

	fp = popen(httpRequest,"r");/*发出请求并接受返回*/
	if (fp == NULL) {
	    DBGPRINT("func:%s,line:%d,errstr=%s\n",__func__,__LINE__,strerror(errno));
	    if (flag) {/*请求所有APP标志*/
		/*下一个请求广告过滤更新*/
		switch (appType) {
		case SOFT_UDTYPE:
#if (ADFILTER == FYES)
		    /*广告过滤关闭或者设置成自动更新*/
		    if ((adfilterProfile->head.active == FALSE) || (adfilterProfile->updateType == 0)) {
			profSys->appUpdateStatus[ADFILTER_UDTYPE] = APP_INITIAL;/*置为初始化状态*/
			appType = MAX_UDTYPE;
		    } else {
			appType = ADFILTER_UDTYPE;
		    }
#else
		    appType = MAX_UDTYPE;
#endif
		    break;
#if (ADFILTER == FYES)
		case ADFILTER_UDTYPE:
		    appType = MAX_UDTYPE;
		    break;
#endif
		default:
		    break;
		}
		continue;
	    } else {
		break;
	    }
	}
	else
	{
	    pclose(fp);
	    fp = NULL;
	}
	DBGPRINT("func:%s,line:%d\n",__func__,__LINE__);
	if (access(fileName,F_OK) != 0) {/*文件没有从远程服务器请求到*/
#if 1
	    profSys->appUpdateStatus[appType] = APP_CHECK_FAILED;/*置为检测失败*/
#endif
	    if (flag) {/*请求所有APP标志*/
		/*下一个请求广告过滤更新*/
		switch (appType) {
		case SOFT_UDTYPE:
#if (ADFILTER == FYES)
		    /*广告过滤关闭或者设置成自动更新*/
		    if ((adfilterProfile->head.active == FALSE) || (adfilterProfile->updateType == 0)) {
			profSys->appUpdateStatus[ADFILTER_UDTYPE] = APP_INITIAL;/*置为初始化状态*/
			appType = MAX_UDTYPE;
		    } else {
			appType = ADFILTER_UDTYPE;
		    }
#else
		    appType = MAX_UDTYPE;
#endif
		    break;
#if (ADFILTER == FYES)
		case ADFILTER_UDTYPE:
		    appType = MAX_UDTYPE;
		    break;
#endif
		default:
		    break;
		}
		continue;
	    } else {
		break;
	    }
	}
	json = parseFile(fileName);
	DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
	if (json == NULL) {
	    DBGPRINT("func:%s,line:%d,json is null\n",__func__,__LINE__);
	    profSys->appUpdateStatus[appType] = APP_CHECK_FAILED;/*请求服务器失败,置为检测失败*/
	    break;
	}
	DBGPRINT("func:%s,line:%d,appType=%d\n",__func__,__LINE__,appType);
	switch(appType) {
	case SOFT_UDTYPE:
	    if (json != NULL) {
		tmp = cJSON_Print(json);
		DBGPRINT("func:%s,line:%d,tmp=%s\n",__func__,__LINE__,tmp);
		free(tmp);
		tmpJson = cJSON_GetObjectItem(json,"ver");
		if (tmpJson->type == cJSON_String) {
		    ver = *tmpJson->valuestring;
		} else {
		    ver = tmpJson->valueint;
		}
#if (ADFILTER == FYES)
		adfilterProfile->newVersion = cJSON_GetObjectItem(json,"ckv")->valueint;
		adfilterProfile->checkCycle = cJSON_GetObjectItem(json,"ckc")->valueint;
#endif
		if (ver == 0) {
		    DBGPRINT("func:%s,line:%d,ver is null\n",__func__,__LINE__);
		    profSys->appUpdateStatus[appType] = APP_STATS_UNCHECKED;/*未检测到更新*/
		} else {
		    st = cJSON_GetObjectItem(json,"st")->valuestring;/*型号*/
		    sv = cJSON_GetObjectItem(json,"sv")->valuestring;/*最新版本*/
		    tmpJson = cJSON_GetObjectItem(json,"bt");
		    if (tmpJson->type == cJSON_String) {/*软件生成日期*/
			bt = atoi(tmpJson->valuestring);
		    } else {
			bt = tmpJson->valueint;
		    }
		    dl = cJSON_GetObjectItem(json,"dl")->valuestring;/*软件升级链接，日后扩展*/
		    rn = cJSON_GetObjectItem(json,"rn")->valuestring;/*软件发布说明*/
		    DBGPRINT("st=%s,sv=%s,bt=%d,dl=%s,rn=%s\n",st,sv,bt,dl,rn);
#if 1
		    if (sscanf(sv,"%d.%d.%d",&remoteSv[0],&remoteSv[1],&remoteSv[2]) == 3) {
			//remoteVersion = atoi(remoteSv);
			remoteVersion = remoteSv[0]*10000 + remoteSv[1]*100 + remoteSv[2];
			//curVersion = atoi(curSv);
			curVersion = curSv[0]*10000 + curSv[1]*100 + curSv[2];
		    }
#endif
		    if ((strcmp(st,curSt) == 0) && (remoteVersion - curVersion > 0 || curBt - bt < 0)) {/*型号相同且当前软件生成日期较小*/
			profSys->appUpdateStatus[appType] = APP_WAITING_UPDATE;/*检测到更新*/
		    } else {
			profSys->appUpdateStatus[appType] = APP_STATS_UNCHECKED;/*未检测到更新*/
		    }
		}
	    }
	    if (flag) {/*请求所有APP标志*/
#if (ADFILTER == FYES)
		/*广告过滤关闭或者设置成自动更新*/
		if ((adfilterProfile->head.active == FALSE) || (adfilterProfile->updateType == 0)) {
		    profSys->appUpdateStatus[ADFILTER_UDTYPE] = APP_INITIAL;/*置为初始化状态*/
		    appType = MAX_UDTYPE;
		} else {
		    appType = ADFILTER_UDTYPE;
		}
#else
		appType = MAX_UDTYPE;
#endif
	    } else {
		finished = 1U;
	    }
	    break;
#if (ADFILTER == FYES)
	case ADFILTER_UDTYPE:
	    if (json != NULL) {
		tmp = cJSON_Print(json);
		DBGPRINT("func:%s,line:%d,tmp=%s\n",__func__,__LINE__,tmp);
		free(tmp);
		ver = cJSON_GetObjectItem(json,"ver")->valueint;
		adfilterProfile->newVersion = cJSON_GetObjectItem(json,"ckv")->valueint;
		adfilterProfile->checkCycle = cJSON_GetObjectItem(json,"ckc")->valueint;
		if (adfilterProfile->newVersion != adfilterProfile->oldVersion) {
		    nvramWriteCommit();/*新旧版本不一致，则写flash，从而使得用户重启机器仍然生效*/
		}
		if (ver == APP_DATA_JSON_TYPE) {
		    dlPre = cJSON_GetObjectItem(json,"dlPre")->valuestring;/*下载链接前缀地址*/
		    apl = cJSON_GetObjectItem(json,"apl");/*ap list, a cJSON Array*/
		    apl_size = cJSON_GetArraySize(apl);
		    for (i = 0; i < apl_size; i++) {
			ap = cJSON_GetArrayItem(apl,i);/*APP Policy, a cJSON Object*/
			an = cJSON_GetObjectItem(ap,"an")->valuestring;/*app name*/
			pl = cJSON_GetObjectItem(ap,"pl");/*Policy List,a cJSON Array*/
			pl_size = cJSON_GetArraySize(pl);/*policy list size*/
			for (j = 0; j < pl_size; j++) {
			    p = cJSON_GetArrayItem(pl,j);/*APP Policy, a cJSON Object*/
			    n = cJSON_GetObjectItem(p,"n")->valuestring;/*policy name*/
			    v = cJSON_GetObjectItem(p,"v")->valueint;/*policy version*/
			    dl = cJSON_GetObjectItem(p,"l")->valuestring;/*policy xml path*/
			    if (strcmp(n,adfilterProfile->head.name) == 0 && v - adfilterProfile->policyVersion > 0) {/*有更新*/
				needUpdate = 1U;
			    }
			}
		    }
		    if (needUpdate) {
			profSys->appUpdateStatus[appType] = APP_WAITING_UPDATE;/*检测到更新*/
		    } else {
			profSys->appUpdateStatus[appType] = APP_STATS_UNCHECKED;/*未检测到更新*/
		    }
		} else {
		    profSys->appUpdateStatus[appType] = APP_STATS_UNCHECKED;/*未检测到更新*/
		}
	    }
	    if (flag) {/*请求所有APP标志*/
		/*下一个请求广告过滤更新*/
		appType = MAX_UDTYPE;
	    } else {
		finished = 1U;
	    }
	    break;
#endif
	default:
	    finished = 1U;
	    break;
	} 
	if (json != NULL) {
	    cJSON_Delete(json);
	    json = NULL;
	}
	if (finished) {
	    break;
	}
    }

err:
    if (fp != NULL) {
	pclose(fp);
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
