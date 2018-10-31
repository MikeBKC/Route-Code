#include "uttMachine.h"
#if (DIAGNOSE_DETECT == FYES)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "webs.h"
#include "utils.h"
#include "uttfunction.h"
#include "comExeFun.h"
#include "translate.h"
#include "um.h"

#define LENGTH 100
#define DIAGNOSE_RESULT_FILE "/tmp/diagnoseResult"

#define DEBUG_SWITCh 0
#if DEBUG_SWITCh
#define DIAGNOSE_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define DIAGNOSE_PRINTF(fmt, args...)
#endif

/*****************************函数声明  start*************************************/
static void formDiagnose(webs_t wp, char_t *path, char_t *query);
static int formDiagnoseResult(webs_t wp, char_t *path, char_t *query);
extern void formDefineDiagnose(void);
/*****************************函数声明  end***************************************/

/********************************************************************
 * 函数： formDiagnose
 * 功能： 接收页面传来的诊断主机，执行ping命令，将命令返回结果重定向到diagnoseResult文件中
 * 创建： 2013-10-14
 * 参数： 略
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
static void formDiagnose(webs_t wp, char_t *path, char_t *query)
{
    char_t *host = NULL;
    
    host = websGetVar(wp, T("host"), T(""));/*获取要诊断的主机信息*/
    doSystem("ping -c 4 -i 1 -w 4 %s > %s 2>&1 &",host,DIAGNOSE_RESULT_FILE);
    DIAGNOSE_PRINTF("func:%s,line %d\n",__func__,__LINE__);
}

/********************************************************************
 * 函数： formDiagnoseResult
 * 功能： 将诊断结果返回到页面，页面以ajax异步形式进行显示
 * 创建： 2013-10-14
 * 参数： 略
 * 返回： 1->诊断失败;0->成功
 * 作者： zhao.chong 
 ********************************************************************/ 
static int formDiagnoseResult(webs_t wp, char_t *path, char_t *query)
{
    FILE *fp = NULL;
    char_t *buf,*res;
    char_t tmp[LENGTH];
    char_t *host = NULL;
    static int count = 0;
    
    buf = (char*)malloc(LENGTH*8);
    res = buf;
    count++;

    host = websGetVar(wp, T("host"), T(""));/*获取要诊断的主机信息*/
    DIAGNOSE_PRINTF("func:%s,line %d\n",__func__,__LINE__);
    if (buf != NULL) {
	memset(buf,0,8*LENGTH);
	fp = fopen(DIAGNOSE_RESULT_FILE,"r");
	if (fp != NULL) {
	    while (!feof(fp)) {
		if (fgets(tmp,LENGTH,fp) != NULL) {
		    strncpy(buf,tmp,LENGTH);
		    buf += strlen(tmp);
		}
	    }
	}
    }
    if ((strstr(res,"BusyBox") != NULL) || (strstr(res,"bad") != NULL)) {
	memset(res,0,8*LENGTH);    
    }
#if 0
    if (strstr(res,"bad") != NULL) {
	memset(res,0,8*LENGTH);    
    }
#endif
    if ((count % 5 == 0) && (*res == '\0')) {
	doSystem("killall ping > /dev/null 2>&1 &");
	snprintf(res,LENGTH,"ping: bad address '%s'",host);
    }


    /*构造ajax返回结果*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("%s"), res);
    
    websDone(wp, 200);
    DIAGNOSE_PRINTF("func:%s,line %d,result=%s\n",__func__,__LINE__,res);

    return 0;
}

/********************************************************************
 * 函数： formDefineDiagnose
 * 功能： 把网页上调用的asp函数转换成该文件中定义的函数
 * 创建： 2013-10-14
 * 参数： 无
 * 返回： 无
 * 作者： zhao.chong 
 ********************************************************************/ 
void formDefineDiagnose(void)
{
    DIAGNOSE_PRINTF("func:%s,line %d\n",__func__,__LINE__);
    websFormDefine(T("formDiagnose"), formDiagnose);
    websFormDefine(T("formDiagnoseResult"), formDiagnoseResult);
}
#endif
