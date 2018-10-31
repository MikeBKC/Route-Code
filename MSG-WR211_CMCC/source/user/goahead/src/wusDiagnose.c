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

/*****************************��������  start*************************************/
static void formDiagnose(webs_t wp, char_t *path, char_t *query);
static int formDiagnoseResult(webs_t wp, char_t *path, char_t *query);
extern void formDefineDiagnose(void);
/*****************************��������  end***************************************/

/********************************************************************
 * ������ formDiagnose
 * ���ܣ� ����ҳ�洫�������������ִ��ping���������ؽ���ض���diagnoseResult�ļ���
 * ������ 2013-10-14
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
static void formDiagnose(webs_t wp, char_t *path, char_t *query)
{
    char_t *host = NULL;
    
    host = websGetVar(wp, T("host"), T(""));/*��ȡҪ��ϵ�������Ϣ*/
    doSystem("ping -c 4 -i 1 -w 4 %s > %s 2>&1 &",host,DIAGNOSE_RESULT_FILE);
    DIAGNOSE_PRINTF("func:%s,line %d\n",__func__,__LINE__);
}

/********************************************************************
 * ������ formDiagnoseResult
 * ���ܣ� ����Ͻ�����ص�ҳ�棬ҳ����ajax�첽��ʽ������ʾ
 * ������ 2013-10-14
 * ������ ��
 * ���أ� 1->���ʧ��;0->�ɹ�
 * ���ߣ� zhao.chong 
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

    host = websGetVar(wp, T("host"), T(""));/*��ȡҪ��ϵ�������Ϣ*/
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


    /*����ajax���ؽ��*/
    websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
    websWrite(wp, T("%s"), res);
    
    websDone(wp, 200);
    DIAGNOSE_PRINTF("func:%s,line %d,result=%s\n",__func__,__LINE__,res);

    return 0;
}

/********************************************************************
 * ������ formDefineDiagnose
 * ���ܣ� ����ҳ�ϵ��õ�asp����ת���ɸ��ļ��ж���ĺ���
 * ������ 2013-10-14
 * ������ ��
 * ���أ� ��
 * ���ߣ� zhao.chong 
 ********************************************************************/ 
void formDefineDiagnose(void)
{
    DIAGNOSE_PRINTF("func:%s,line %d\n",__func__,__LINE__);
    websFormDefine(T("formDiagnose"), formDiagnose);
    websFormDefine(T("formDiagnoseResult"), formDiagnoseResult);
}
#endif
