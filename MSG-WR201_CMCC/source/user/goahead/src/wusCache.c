/*
 * wusCache.c
 *
 *  Created on: May 21, 2012
 *      Author: bchen
 */
#include "webs.h"
#include "uttfunction.h"
#include "um.h"
#include "mib.h"
#include "string.h"
#include "translate.h"
#include "profacce.h"

#if (HTTP_CACHE == FYES)

/********************************************************************
 * 函数：getHitRatios
 * 功能：获取缓存请求流量和命中流量
 * 参数：略
 * 输出：无
 * 返回：整型 1：成功；0失败
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
int getHitRatios(char *hitRatios) {
	FILE *fp;
	char buf[500];
	int flag=0;
	fp = popen("traffic_line -r proxy.process.http.user_agent_response_document_total_size", "r");
	long long int userResponseBytes= 0, originResponseBytes= 0;
	if(fp!=NULL){
		if (fgets(buf, 500, fp) != NULL) {
			sscanf(buf, "%lld\n", &userResponseBytes);
		} 
	}
	pclose(fp);
	fp=NULL;
	fp = popen("traffic_line -r proxy.process.http.origin_server_response_document_total_size", "r");
	if(fp!=NULL){
		if (fgets(buf, 500, fp) != NULL) {
			sscanf(buf, "%lld\n", &originResponseBytes);
		} 
	}
	pclose(fp);
	fp=NULL;

	char result[500];
	sprintf(result, T("{\"%s\":%lld,\"%s\":%lld,\"%s\":%lld}"),
	getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),userResponseBytes,
	getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),originResponseBytes,
        getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE),userResponseBytes-originResponseBytes);
	strcpy(hitRatios, result);
	return 1;
}

/********************************************************************
 * 函数：getStoreDirUtilization
 * 功能：获取缓存磁盘大小和已使用大小
 * 参数：略
 * 输出：无
 * 返回：整型 1：成功；0失败
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
int getStoreDirUtilization(char *dirUtilization) {
	FILE *fp;
	char buf[500];
	long long int cacheTotal= 0, cacheUsed= 0;
	fp = popen("traffic_line -r proxy.process.cache.bytes_total", "r");
	if(fp!=NULL){	
		if (fgets(buf, 500, fp) != NULL) {
			sscanf(buf, "%lld\n", &cacheTotal);
		}
	}
	pclose(fp);
	fp=NULL;
	fp = popen("traffic_line -r proxy.process.cache.bytes_used", "r");
	if(fp!=NULL){
		if (fgets(buf, 500, fp) != NULL) {
			sscanf(buf, "%lld\n", &cacheUsed);
		} 
	}
	pclose(fp);
	fp=NULL;
	char result[500];
	sprintf(result, "{\"%s\":%lld,\"%s\":%lld,\"%s\":%lld}", 
	getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),cacheTotal,
	getTransDataForC(C_LANG_INDEX_CACHE_USED),cacheUsed,
        getTransDataForC(C_LANG_INDEX_CACHE_FREE),cacheTotal-cacheUsed);
	strcpy(dirUtilization, result);
	return 1;
}




/********************************************************************
 * 函数：getCheckedList
 * 功能：获取配置中已配置的缓存列表
 * 参数：略
 * 输出：无
 * 返回：整型 1：成功；0失败
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
int getCheckedList(char *checkedList) {
	char list[256];
	CacheProfile *prof = NULL;
	prof = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);
	char *p=list;
	if(prof!=NULL)
	{
		if(prof->youkuVideoEnable)
		{
			strcpy(p,"\"youkuVideo\"");
			p= p + strlen("\"youkuVideo\"");
			*p = ',';
			p++;
		}

		if(prof->tudouVideoEnable)
		{
			strcpy(p,"\"tudouVideo\"");
			p= p + strlen("\"tudouVideo\"");
			*p = ',';
			p++;
		}
		
		if(prof->tcVideoEnable)
		{
			strcpy(p,"\"tencentVideo\"");
			p= p + strlen("\"tencentVideo\"");
			*p = ',';
			p++;
		}

		if(prof->sinaVideoEnable)
		{
			strcpy(p,"\"sinaVideo\"");
			p= p + strlen("\"sinaVideo\"");
			*p = ',';
			p++;
		}

		if(prof->ifengVideoEnable)
		{
			strcpy(p,"\"ifengVideo\"");
			p= p + strlen("\"ifengVideo\"");
			*p = ',';
			p++;
		}

		if(prof->letvVideoEnable)
		{
			strcpy(p,"\"letvVideo\"");
			p= p + strlen("\"letvVideo\"");
			*p = ',';
			p++;
		}

		if(prof->v56VideoEnable)
		{
			strcpy(p,"\"v56Video\"");
			p= p + strlen("\"v56Video\"");
			*p = ',';
			p++;
		}

		if(prof->funshionVideoEnable)
		{
			strcpy(p,"\"funshionVideo\"");
			p= p + strlen("\"funshionVideo\"");
			*p = ',';
			p++;
		}

		if(prof->baiduMp3Enable)
		{
			strcpy(p,"\"baiduMp3\"");
			p= p + strlen("\"baiduMp3\"");
			*p = ',';
			p++;
		}

		if(prof->sinaPageEnable)
		{
			strcpy(p,"\"sina\"");
			p= p + strlen("\"sina\"");
			*p = ',';
			p++;
		}

		if(p-list>0)
		{
			p--;
			*p='\0';
		}else{
			strcpy(p,"");
		}
	}
	strcpy(checkedList,list);
	return 1;


}

/********************************************************************
 * 函数：isCacheRun
 * 功能：查看traffic server是否运行
 * 参数：略
 * 输出：无
 * 返回：整型 1：启动；0:未启动；－1：查询失败
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/

int isCacheRun() {
	FILE *fp;
	char buf[256];
	char *cmd = "cache-stats.sh";
	if ((fp = popen(cmd, "r")) == NULL) {
                return -1;
        }

        if (fgets(buf, 256, fp) != NULL) {
                int stats = 0;
                sscanf(buf, "%d", &stats);
                pclose(fp);
                if (!stats) {
                        return 0;
                } else {
                        return 1;
                }

                return 1;
        } else {
                pclose(fp);
                return -1;
        }
}

/********************************************************************
 * 函数：getCacheStats
 * 功能：获取缓存状态
 * 参数：略
 * 输出：略
 * 返回：略
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
static int getCacheStats(int eid, webs_t wp, int argc, char_t **argv) {
	char buf[500];
	CacheProfile *prof = NULL;
	
	prof = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);

	if(getCheckedList(buf)){
		websWrite(wp, T("var checklist = new Array(%s);\n"), buf);
    }else{
		websWrite(wp, T("var checklist = new Array();\n"));
        setTheErrorMsg(T(getTransDataForC(C_LANG_INDEX_CACHE_LIST_FAIL)));
		getTheErrorMsg(eid,wp,argc,argv);
	}

	int cacheStats=isCacheRun();
	if(prof==NULL||prof->cacheEnable==FUN_DISABLE )
	{
		websWrite(wp, T("var isServerRun = 0;\n"));
		websWrite(wp, T("var cacheSize = %d;\n"),prof->size);
		websWrite(wp, "var cloudFlowBarValues = {\"%s\":0,\"%s\":0,\"%s\":0};\n",
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE));
        websWrite(wp, T("var dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
		getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
		getTransDataForC(C_LANG_INDEX_CACHE_USED),
		getTransDataForC(C_LANG_INDEX_CACHE_FREE));
		getTheErrorMsg(eid,wp,argc,argv);
  		return 0;
	}else{
	
		websWrite(wp, T("var isServerRun = %d;\n"),prof->cacheEnable);	
		websWrite(wp, T("var cacheSize = %d;\n"),prof->size);
		if(cacheStats){
		if(getHitRatios(buf)){
			websWrite(wp, "var cloudFlowBarValues = %s;\n",buf);
		}else{
			websWrite(wp, "var cloudFlowBarValues = {\"%s\":0,\"%s\":0,\"%s\":0};\n",
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE));
			websWrite(wp, T("var dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
			getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
			getTransDataForC(C_LANG_INDEX_CACHE_USED),
			getTransDataForC(C_LANG_INDEX_CACHE_FREE));
           setTheErrorMsg(T(getTransDataForC(C_LANG_INDEX_BANDWIDTH_FAIL)));
			getTheErrorMsg(eid,wp,argc,argv);
			return 0;
		}
	
		if(getStoreDirUtilization(buf)){
			websWrite(wp, "var dirUtilizationPieValues= %s;\n",buf);
		}else{
			websWrite(wp, T("var dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
			getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
			getTransDataForC(C_LANG_INDEX_CACHE_USED),
			getTransDataForC(C_LANG_INDEX_CACHE_FREE));
		    setTheErrorMsg(T(getTransDataForC(C_LANG_INDEX_CACHE_SIZE_FAIL)));
			getTheErrorMsg(eid,wp,argc,argv);
			return 0;
		}
		}else{
			websWrite(wp, "var cloudFlowBarValues = {\"%s\":0,\"%s\":0,\"%s\":0};\n",
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),
			getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE));
			websWrite(wp, T("var dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
			getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
			getTransDataForC(C_LANG_INDEX_CACHE_USED),
			getTransDataForC(C_LANG_INDEX_CACHE_FREE));
		}
        
		getTheErrorMsg(eid,wp,argc,argv);
		return 0;
	}
}


static void getCacheData(webs_t wp, char_t *path, char_t *query) {
	char flow[500],dirUtilization[500];
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain;charset=gb2312\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	if(isCacheRun()){
	if(getHitRatios(flow)){
		websWrite(wp,"cloudFlowBarValues = %s;\n",flow);
	}else{
		websWrite(wp,T("cloudFlowBarValues = {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE));
	}

	if(getStoreDirUtilization(dirUtilization)){
		websWrite(wp, "dirUtilizationPieValues= %s;\n",dirUtilization);
    }else{
	 	websWrite(wp, T("dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
		getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
		getTransDataForC(C_LANG_INDEX_CACHE_USED),
		getTransDataForC(C_LANG_INDEX_CACHE_FREE));
	}
	}else{
		websWrite(wp,T("cloudFlowBarValues = {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_REQUEST),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_RESPONSE),
		getTransDataForC(C_LANG_INDEX_BANDWIDTH_SAVE));

	 	websWrite(wp, T("dirUtilizationPieValues =  {\"%s\":0,\"%s\":0,\"%s\":0};\n"),
		getTransDataForC(C_LANG_INDEX_CACHE_TOTAL),
		getTransDataForC(C_LANG_INDEX_CACHE_USED),
		getTransDataForC(C_LANG_INDEX_CACHE_FREE));
	}
	websDone(wp, 200);
}

/********************************************************************
 * 函数：cacheConfigForm
 * 功能：配置缓存
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
static void cacheConfigForm(webs_t wp, char_t *path, char_t *query) {
	char_t *serverEnable;
	char_t *youkuVideo;
	char_t *tudouVideo;
	char_t *tencentVideo;
	char_t *sinaVideo;
	char_t *ifengVideo;
	char_t *letvVideo;
	char_t *v56Video;
	char_t *funshionVideo;
	char_t *baiduMp3;
	char_t *sina;
	char_t *cacheSize;
	CacheProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	serverEnable = websGetVar(wp, T("serverEnable"), "");
	youkuVideo = websGetVar(wp, T("youkuVideo"), "");
	tudouVideo  = websGetVar(wp, T("tudouVideo"), "");
	tencentVideo  = websGetVar(wp, T("tencentVideo"), "");
	sinaVideo  = websGetVar(wp, T("sinaVideo"), "");
	ifengVideo  = websGetVar(wp, T("ifengVideo"), "");
	letvVideo  = websGetVar(wp, T("letvVideo"), "");
	v56Video  = websGetVar(wp, T("v56Video"), "");
	funshionVideo  = websGetVar(wp, T("funshionVideo"), "");
	baiduMp3 = websGetVar(wp, T("baiduMp3"), "");
	sina  = websGetVar(wp, T("sina"), "");
	cacheSize = websGetVar(wp, T("cacheSize"), "");

	prof = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);

	
	if((prof!=NULL)&&(ProfInstIsFree(prof)==0))
	{
		ProfBackupByIndex(MIB_PROF_CACHE,PROF_CHANGE_EDIT,0,&profhead);	
		if (strlen(serverEnable) > 0 && strcmp(serverEnable, "on")==0)
		{
			prof->cacheEnable = FUN_ENABLE;
		}
		else
		{
			prof->cacheEnable = FUN_DISABLE;
		}

		if(strlen(youkuVideo) > 0 && strcmp(youkuVideo,"on")==0)
		{
			prof->youkuVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->youkuVideoEnable = FUN_DISABLE;
		}

		if(strlen(tudouVideo) > 0 && strcmp(tudouVideo,"on")==0)
		{
			prof->tudouVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->tudouVideoEnable = FUN_DISABLE;
		}
		
		if(strlen(tencentVideo) > 0 && strcmp(tencentVideo,"on")==0)
		{
			prof->tcVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->tcVideoEnable = FUN_DISABLE;
		}

		if(strlen(sinaVideo) > 0 && strcmp(sinaVideo,"on")==0)
		{
			prof->sinaVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->sinaVideoEnable = FUN_DISABLE;
		}

		if(strlen(ifengVideo) > 0 && strcmp(ifengVideo,"on")==0)
		{
			prof->ifengVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->ifengVideoEnable = FUN_DISABLE;
		}

		if(strlen(letvVideo) > 0 && strcmp(letvVideo,"on")==0)
		{
			prof->letvVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->letvVideoEnable = FUN_DISABLE;
		}

		if(strlen(v56Video) > 0 && strcmp(v56Video,"on")==0)
		{
			prof->v56VideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->v56VideoEnable = FUN_DISABLE;
		}

		if(strlen(funshionVideo) > 0 && strcmp(funshionVideo,"on")==0)
		{
			prof->funshionVideoEnable = FUN_ENABLE;
		}
		else
		{
			prof->funshionVideoEnable = FUN_DISABLE;
		}

		if(strlen(baiduMp3) > 0 && strcmp(baiduMp3,"on")==0)
		{
			prof->baiduMp3Enable = FUN_ENABLE;
		}
		else
		{
			prof->baiduMp3Enable = FUN_DISABLE;
		}

		if(strlen(sina) > 0 && strcmp(sina,"on")==0)
		{
			prof->sinaPageEnable = FUN_ENABLE;
		}
		else
		{
			prof->sinaPageEnable = FUN_DISABLE;
		}
		
		if(strlen(cacheSize) > 0 )
		{
			prof->size = strtoul(cacheSize,0,10);
		}
		
		prof->head.active = TRUE;
	}	

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
	/*
	if(prof->cacheEnable)
	{
		int times=0;
		do
		{
			times++;
			sleep(5);
			if(times>6)
		{
			break;
			}
		}while(!isCacheRun());
	}
	*/
	websRedirect(wp, "cachePage.asp");

}

/********************************************************************
 * 函数：clearCacheForm
 * 功能：清除缓存
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2012-08-21
 *
 ********************************************************************/
static void clearCacheForm(webs_t wp, char_t *path, char_t *query) {
	char ip[16]={0};
	int count = 0;
	getIfIp(getLanIfName(), ip);
	int serverStat = isCacheRun();
	CacheProfile *prof = NULL;
	prof = (CacheProfile *)ProfGetInstPointByIndex(MIB_PROF_CACHE,0);
	if(prof!=NULL)
	{
	if(serverStat==1&&prof->cacheEnable){
		char ip[16]={0};
		getIfIp(getLanIfName(), ip);
		//remove iptables
#if 0
		doSystem("iptables -t nat -D PREROUTING  ! -d %s -p tcp --dport 80 -j DNAT --to %s:3128",ip,ip);
#endif
		//stop server
		doSystem("ats_stop");
		do{
			sleep(5);
		}while(isCacheRun());
		//clear
		doSystem("traffic_server -Cclear");
		count = 0;
		do{
			sleep(5);
			count++;
		}while(count<5);
		//start server
		//doSystem("ats_clear");
		doSystem("ats_start&");
		//add nat
		count = 0;
		do{
			sleep(1);
			count++;
			if(count>=15)
				break;
		}while(!isCacheRun());
#if 0
		if(isCacheRun())
		{
			doSystem("iptables -t nat -A PREROUTING  ! -d %s -p tcp --dport 80 -j DNAT --to %s:3128",ip,ip);
		}
#endif
	}else if(serverStat!=1&&!prof->cacheEnable)
	{
		//clear
		doSystem("traffic_server -Cclear");
		//doSystem("ats_clear");
	}
	}
	websRedirect(wp, "cachePage.asp");

}

//Define ATS Cache Form
void formDefineCache(void) {
	websFormDefine(T("cacheConfigForm"), cacheConfigForm);
	websFormDefine(T("clearCacheForm"), clearCacheForm);
	websAspDefine(T("getCacheStats"), getCacheStats);
	websFormDefine(T("getCacheData"), getCacheData);
}

#endif
