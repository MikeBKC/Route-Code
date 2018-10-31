/*
 * wusNetSniper.c
 *
 *  Created on: Oct 15, 2013
 *      Author: bchen
 */
#include "webs.h"
#include "uttfunction.h"
#include "um.h"
#include "mib.h"
#include "string.h"
#include "translate.h"
#include "profacce.h"
#if (NET_SNIPER == FYES)
#include <include/linux/netfilter_ipv4/utt_netfilter.h>
//#define SWORD_DEBUG 1
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
#define SNIPER_HOST_PROC_FILE "/proc/net/statSniper"

/********************************************************************
 * 函数： readConfigFromFile
 * 功能： 从页面文件中读取title,manger和content参数，注意这3个参数要是数组
 * 创建： 2012-03-21
 * 参数： 
 *		  title: 保存标题
 *		  content: 保存通告内容
 * 返回： 成功，失败
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int readConfigFromFile(char *title, char *content, const char *path)
{
#if 0
		/* 从文件Notify1.asp中按格式化文件Notify.htm读取title,content参数 */
#endif
	FILE	*fp = NULL;
#if 0
	Uchar	bufback[ NOTIFY_HTM1_MOUDLE_SIZE ] = {0u};
#endif
	char	buf[ NOTIFY_HTM1_SIZE ] = {0u};
	char	*ptitlehead = NULL;
	char	*ptitletail = NULL;
	char	*pcontenthead = NULL;
	char	*pcontenttail = NULL;
	uint	rc = FAIL;
#if 1
	if( (NULL != title) && (NULL != content) && (NULL != path) )
	{
	    fp = fopen( path, "r" );
	    if( (fp != NULL) ) 
	    {
		fread( (void*)buf, sizeof(char), NOTIFY_HTM1_SIZE, fp );
		fclose( fp );
		ptitlehead = strstr( buf, "<h3 align=center>" );
		ptitletail = strstr( buf, "</h3>" );
		if( (NULL != ptitlehead) && (NULL != ptitletail) )
		{
			ptitlehead += strlen( "<h3 align=center>" );
			*ptitletail = '\0';
			pcontenthead = ptitletail + strlen("</h3>");
			pcontenttail = strstr( (ptitletail+1), "</div>" );
			if( NULL != pcontenttail )
			{
				*pcontenttail = '\0';
				strcpy( title, ptitlehead );
				strcpy( content, pcontenthead );
				rc = SUCCESS;
			}
		}
	    }
	} 
#endif
	return (int)rc;
}
/********************************************************************
 * 函数：aspSniperConfig
 * 功能：输出网络尖兵配置
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-15
 *
 ********************************************************************/
static int aspNetSniperConfig(int eid, webs_t wp, int argc,char_t **argv) {
	NetSniperProfile *netSniperProfile = NULL;
	MibProfileType mibType = MIB_PROF_NET_SNIPER;
	netSniperProfile = (NetSniperProfile*)ProfGetInstPointByIndex(mibType,0);

	if(netSniperProfile->head.active == TRUE){
		websWrite(wp,"var enable = \"on\";");
	}else{
		websWrite(wp,"var enable = \"off\";");
	}
	websWrite(wp,"var groupName = \"%s\";",netSniperProfile->ipGrpName);
	websWrite(wp,"var controlTime = \"%d\";",netSniperProfile->ctlTime);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
	
}

/********************************************************************
 * 函数：aspSniperHostList
 * 功能：输出共享主机
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-21
 *
 ********************************************************************/
static int aspNetSniperHostList(int eid, webs_t wp, int argc,char_t **argv) {
	int index = 0;
	char_t buffer[20];
	char_t ipAddr[20];
	char_t remainTime[20];
	char_t linestr[20];
	FILE *fp;

	fp = fopen(SNIPER_HOST_PROC_FILE,"r");

	if(fp != NULL){
		memset(linestr,0,sizeof(linestr));
		fgets(linestr,20,fp);
		while(feof(fp) == 0){ 
			if(fscanf(fp,"%s",buffer)==1){
				strcpy(ipAddr, buffer);
			}else{
				continue;
			}   
			if(fscanf(fp,"%s",buffer)==1){
				strcpy(remainTime, buffer);
			}else{
				continue;
			}   
			websWrite(wp,"indexID[%d]=\"ID%d\";ipAdds[%d]=\"%s\";remainTimes[%d]=\"%ss\";",index,index+1,index,ipAddr,index,remainTime);
			index++;
		}   
	}
	websWrite(wp,"\nvar totalrecs=%d;",index);

    getTheErrorMsg(eid, wp, argc, argv);
    return 0;
	
}

/********************************************************************
 * 函数：netSniperConfigForm
 * 功能：配置网络尖兵
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-15
 *
 ********************************************************************/
static void netSniperConfigForm(webs_t wp, char_t *path, char_t *query) {
	char_t *enable;
	char_t *groupName;
	char_t *time;
	NetSniperProfile *prof = NULL;
	struProfAlloc *profhead = NULL;
	enable = websGetVar(wp, T("enable"), "");
	groupName = websGetVar(wp, T("address"), "");
	time  = websGetVar(wp, T("time"), "");


	prof = (NetSniperProfile *)ProfGetInstPointByIndex(MIB_PROF_NET_SNIPER,0);

	
	if((prof!=NULL)&&(ProfInstIsFree(prof)==0))
	{
		ProfBackupByIndex(MIB_PROF_NET_SNIPER,PROF_CHANGE_EDIT,0,&profhead);	
		if (strlen(enable) > 0 && strcmp(enable, "on")==0)
		{
			prof->head.active = FUN_ENABLE;
		}
		else
		{
			prof->head.active = FUN_DISABLE;
		}

		if(strlen(time) > 0 )
		{
			prof->ctlTime = strtoul(time,0,10);
		}
		strcpy(prof->ipGrpName, groupName);
		
	}	

	ProfUpdate(profhead);
	ProfFreeAllocList(profhead);
	nvramWriteCommit();
	websRedirect(wp, "NetSniperConf.asp");

}

extern int uttNfNlSock;

/********************************************************************
 * 函数：formSniperHostDel
 * 功能：删除管制主机
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-15
 *
 ********************************************************************/
static void formSniperHostDel(webs_t wp, char_t *path, char_t *query) {
	char *delstr = NULL;
    UttNlNfNetSniperConf nlConf;
	memset(&nlConf,0,sizeof(nlConf));
	delstr = websGetVar(wp,T("delstr"),T(""));
	if(delstr != NULL){
		//tmp = strtok(delstr,UTT_SPLIT_SIGN_STR);
		//while(tmp!=NULL){
			
		    nlConf.msgHead.nlType = UTT_NLNF_NET_SNIPER;
			nlConf.confType = UTT_NFCONF_DEL;
			strcpy(nlConf.delstr,delstr);	
			uttNfNlSend(uttNfNlSock,(char*)&nlConf,sizeof(nlConf),0U,0U);
		//	tmp = strtok(NULL,UTT_SPLIT_SIGN_STR);
		//}
	}
	websRedirect(wp, "NetSniperList.asp");
}

/********************************************************************
 * 函数：formSniperNotice
 * 功能：保存内网网络尖兵通告页面
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-15
 *
 ********************************************************************/
static void formSniperNotice(webs_t wp, char_t *path, char_t *query) {
	char_t *title = NULL;
	char_t *content = NULL;

	title = websGetVar(wp,T("noticeTitle"),T(""));
	content = websGetVar(wp,T("hidBdy"),T(""));

	SWORD_PRINTF( "save==== title--read1:%s\n==== content--read1:%s\n", title, content );
	saveConfigToFile(title, content, NOTIFY_HTM, NOTICE_HTML_4);
	nvramWriteCommit(); /*save*/
	websRedirect(wp, "NetSniperNotice.asp");
}

/********************************************************************
 * 函数：aspNetSniperNotice
 * 功能：输出共享主机
 * 参数：略
 * 输出：无
 * 返回：略
 * 作者：bchen
 * 日期：2013-10-21
 *
 ********************************************************************/
static int aspNetSniperNotice(int eid, webs_t wp, int argc,char_t **argv) {
    char_t title[NOTICE_TILTLE_LEN + 1] = {0}; 
    char_t content[NOTICE_CONTENT_LEN + 1] = {0};
	readConfigFromFile(title, content, NOTICE_HTML_4);
	websWrite(wp, "var NoticeTitle=\"%s\";", title);
	return 0;
}

static int aspNetSniperNoticeContent(int eid, webs_t wp, int argc,char_t **argv) {
    char_t title[NOTICE_TILTLE_LEN + 1] = {0}; 
    char_t content[NOTICE_CONTENT_LEN + 1] = {0};
    char_t *write_content = NULL;
	readConfigFromFile(title, content, NOTICE_HTML_4);
	int i,j;
	for(i = 0; content[i] != '\0'; i++)
     {
	 write_content = strstr(&content[i],"<br>");
	 if(write_content != NULL)
	 {
	     i = write_content - content;
	     content[i] = '\n';
	     for(j = i + 1; content[j + 3] != '\0';j++)
	     {
		 content[j] = content[j + 3];
	     }
	     content[j] = '\0';
	 }
     }
     for(i = 0; content[i] != '\0'; i++)
     {
	 write_content = strstr(&content[i],"&nbsp;");
	 if(write_content != NULL)
	 {
	     i = write_content - content;
	     content[i] = ' ';
	     for(j = i + 1; content[j + 5] != '\0';j++)
	     {
		 content[j] = content[j + 5];
	     }
	     content[j] = '\0';
	 }
     }
    SWORD_PRINTF("%s - %d, content = %s\n", __FUNCTION__, __LINE__, content);
    websWrite(wp, "%s", content);

    return 0;


}

//Define Form
void formDefineNetSniper(void) {
	websFormDefine(T("netSniperConfigForm"), netSniperConfigForm);
	websFormDefine(T("formSniperHostDel"), formSniperHostDel);
	websFormDefine(T("formSniperNotice"), formSniperNotice);
	websAspDefine(T("aspNetSniperConfig"), aspNetSniperConfig);
	websAspDefine(T("aspNetSniperHostList"), aspNetSniperHostList);
	websAspDefine(T("aspNetSniperNotice"), aspNetSniperNotice);
	websAspDefine(T("aspNetSniperNoticeContent"), aspNetSniperNoticeContent);
}

#endif
