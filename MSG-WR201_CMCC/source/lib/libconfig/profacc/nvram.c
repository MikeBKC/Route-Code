/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***                                                               ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc.    This document may not be copied,     ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise dis-     ***
 ***    closed, except with the prior written consent of           ***
 ***    UTT Technologies, Inc.                                ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
 *   Copyright 1990-1995 UTT Technologies, Inc.
 *   All rights reserved. *   Use of copyright notice does not imply publication.
 */

/*
 * 
 * File:  nvram2.c
 * 
 * Overview:
 *   interface for access to the NVRAM in the system.
 */

#include "uttMachine.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "string.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "mxml.h"

#if (EASY_TASK == FYES)
static int IsDefault = 0;
int IsDefaultConf(void);
#endif
extern struct st_xmlConfig  rootConfig[];
extern int conf_flash_write(char *buf, off_t to, size_t len);
extern void parseProfileToXml(struct st_xmlConfig *conf, mxml_node_t *parent, MibProfileType mibtype, int instIndex, Pointer prof);
extern int conf_flash_read(char *buf, off_t from, size_t len);
extern void parseProfileFromXml(struct st_xmlConfig *conf, mxml_node_t *node);
int parseStrToFile(char *buf, const char *start, const char *next_start, const char *file); 
int parseFileToStr(char *buf, int buf_size, const char *start, const char *file, int); 
void ProfWiteToFlashNoSyn(void);

#if CONFIG_XPORT_LOG  
#define NET_MANGE_END "<NetManage />"
#define SMART_QOS  "<smartQosEn>"
/*******************
*return :
*	0 :������
*	1 :���Դ�����
*	2 :�����˾�ϸ������
*	3 :ͬʱ�������Դ���;�ϸ������
*
*******************/
int is_limit()
{
#if 0
    char    *tmppoint = NULL;
    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_FLASH_SIZE];/*��ѹ������*/
    unsigned long  zipLen= CONFIG_FLASH_SIZE;
    unsigned long  bufLen=CONFIG_BUFF_SIZE;
    int is_limit = 0;
    buf = (char *)malloc(CONFIG_BUFF_SIZE);

    if (!buf) {
	printf("malloc %d bytes failed\n", CONFIG_BUFF_SIZE);
	return;
    }
    memset(buf, 0, CONFIG_BUFF_SIZE);
    memset(zipBuf, 0, sizeof(zipBuf));
    conf_flash_read(zipBuf, 0, CONFIG_FLASH_SIZE);
    zipLen = strtoul(zipBuf, NULL,16);/*�����ļ�ͷ�������ļ�ѹ����ĳ���*/

    if(Z_OK != uncompress(buf, &bufLen, &zipBuf[CONFIG_HEADER_LEN], zipLen))
    {
	/*��ѹʧ����ϵͳ��������Ĭ������״̬*/
	printf("Config file uncompress error and system while start on default config\n");
    }
    else
    {
	buf[bufLen] = '\0';
	if(0 == buf[0])
	    return -1;
	tmppoint = strstr( buf, SMART_QOS);
	if(tmppoint != NULL )
	    is_limit |=0x1;
	tmppoint = strstr( buf, NET_MANGE_END);
	if(tmppoint == NULL)
	    is_limit |=0x2;
    }
    free(buf);
#else
    int is_limit = 0;
#if (SMART_QOS_FUN == FYES)
    MibProfileType mibType = MIB_PROF_INTERFACE;
    InterfaceProfile *prof = NULL;
    prof = (InterfaceProfile*)ProfGetInstPointByIndex(mibType, 1);
    if(prof->smartQosEn == FUN_ENABLE)
	is_limit |=0x1;
#endif
#if (GROUP_BASE == FYES)
    int min = 0, max = 0,i;
    GroupBaseType webGroupType = GBT_FINE_LIMIT;/*Ĭ�ϴ�ӡ��ϸ������ҳ��*/ 
    MibProfileType GmibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *profG = NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	profG = (GroupBaseProfile*)ProfGetInstPointByIndex(GmibType, i);
	if(profG != NULL && ProfInstIsFree(profG) == 0) {
	    if(profG->groupType == webGroupType)
	    {
		is_limit |=0x2;
	    }
	}
    }
#endif
#endif
    return is_limit;
}
#endif
#if (EASY_TASK == FYES)
/*
 * IsDefaultConf	-	�ж��Ƿ�Ϊ��������
 *
 * �ж��Ƿ�Ϊ���������Թ��׹�����ʹ��
 */
int IsDefaultConf(void)
{
	return IsDefault;/*�����Ƿ�Ϊ��������*/
}
#endif

void ProfWiteToFlash(void)
{
#if (CLOUD_MANAGE == FYES)
    CloudManageProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_CLOUD_MANAGE;
    prof = (CloudManageProfile*)ProfGetInstPointByIndex(mibType,0);
    if ((NULL != prof) && (0 == ProfInstIsFree((Pointer)prof)))
    {
	prof->configNeedSyn = 1;
	prof->configVer++;	
    }
/*�˴��Ƿ��tr069����Ϣ������*/
#endif
    ProfWiteToFlashNoSyn();
}
/*����ĺ�������������*/
void ProfWiteToFlashNoSyn(void)
{
#if 1
#if 0
    char buf[CONFIG_BUFF_SIZE];/*��ѹ������*/
#endif
    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_FLASH_SIZE];/*��ѹ������*/
    unsigned long zipLen = CONFIG_FLASH_SIZE - CONFIG_HEADER_LEN;
    int len = 0;
    int size = 0;

    buf = (char *)malloc(CONFIG_BUFF_SIZE);

    if (!buf) {
	printf("malloc %d bytes failed\n", CONFIG_BUFF_SIZE);
	return;
    }

#if 0
    memset(buf, 0, sizeof(buf));
#endif
    memset(buf, 0, CONFIG_BUFF_SIZE);
    memset(zipBuf, 0, sizeof(zipBuf));

    if( parseProfileToStr(buf, &len) == 0)
    {
#if (NOTICE == FYES)
	/* ���ļ�ת��Ϊ�ַ��� */
	size = parseFileToStr(&buf[len], CONFIG_BUFF_SIZE - len, NOTICE_HTML_START_1, NOTICE_HTML_1, 1);
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, NOTICE_HTML_START_2, NOTICE_HTML_2, 0);
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, NOTICE_HTML_START_3, NOTICE_HTML_3, 0);
#if (NET_SNIPER == FYES)
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, NOTICE_HTML_START_4, NOTICE_HTML_4, 0);
#endif
#else
#if(NET_SNIPER == FYES)
	size = parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, NOTICE_HTML_START_4, NOTICE_HTML_4, 0);
#endif
#endif
#if (WEB_AUTH == FYES)
#if (NOTICE == FYES)
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, WEB_AUTH_CONTACT_START, WEB_AUTH_CONTACT_PATH, 0);
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, WEB_AUTH_PICURL_START, WEB_AUTH_PICURL_PATH, 0);
#else
	size = parseFileToStr(&buf[len], CONFIG_BUFF_SIZE - len, WEB_AUTH_CONTACT_START, WEB_AUTH_CONTACT_PATH, 1);
	size += parseFileToStr(&buf[len +size], CONFIG_BUFF_SIZE - len - size, WEB_AUTH_PICURL_START, WEB_AUTH_PICURL_PATH, 0);
#endif
#endif
	buf[CONFIG_BUFF_SIZE - 1u] = '\0';
#if 0
	printf("bufwr   %s\n",buf);
#endif
	/*�˴�destLenLen����ǰ���븳ֵ*/
	if(Z_OK != compress(&zipBuf[CONFIG_HEADER_LEN], &zipLen, buf, strlen(buf) + size))
	 /*Ϊ�˼ӿ�ѹ���ٶȹ���strlen*/
	{
	    /*ѹ��ʧ����дflash*/
	    printf("Config file compress error and write to flash fail\n");
	}
	else
	{
	    sprintf(zipBuf,"%x",zipLen);/*�����ļ�ͷ����ѹ������ļ�����*/
#if 0
	    printf("srcLen/compressedLen : %d/%d\n", strlen(buf), zipLen);
#endif
	    conf_flash_write(zipBuf, 0, CONFIG_FLASH_SIZE);
	}
    }
    free(buf);
    return;
#else
    FILE *fp;
	mxml_node_t *doc, *root;

	doc = mxmlNewXML("1.0");
	root = mxmlNewElement(doc, "config");
        parseProfileToXml(rootConfig, root, 0, 0, NULL);

	fp = fopen("/tmp/filename.xml", "w+");
	mxmlSaveFile(doc, fp, MXML_NO_CALLBACK);
//	mxmlSaveFile(doc, fp, fun);
	fclose(fp);
    mxmlDelete(doc);
    return 0;
#endif
}
#if  (DHCP_POOL == FYES)
int updateDhcp2DhcpPool(char * buf)
{
    int ret = -1;

    mxml_node_t *doc, *config;
    mxml_node_t * child;

    MibProfileType mibType = MIB_PROF_DHCP_POOL;
    DhcpPoolProfile *prof = NULL;

    doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);
    if(doc != NULL) {/*�Ϸ�xml��ʽ*/
	config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
	if(config != NULL)/*����config*/
	{
	    parseProfileFromXml(rootConfig, config->child);/*�������ã��뵱ǰ���úϲ�*/
	    mxml_node_t* dhcp = mxmlFindElement(config, config, "dhcpServer", NULL, NULL, MXML_DESCEND);
	    prof = (DhcpPoolProfile *)ProfGetInstPointByIndex(mibType,0);
	    if(dhcp != NULL && (0 == ProfInstIsFree((Pointer)prof)))
	    {
		child = dhcp->child;
		if(child != NULL){
		    child = child->child;
		}else{
		    return ;
		}
		while(child != NULL){
#if 0
		    printf("name =  %s --------value =%s\n",(child->value.element.name)?(child->value.element.name):"NULL",(child->child->value.text.string)?(child->child->value.text.string):" NULL ") ;
#endif
		    if(child->value.element.name != NULL&& child->child != NULL && child->child->value.text.string != NULL){
			 struct in_addr ip;
			  int rt = 0;
			  rt = inet_aton(buf, &ip);

			  if((strcmp(child->value.element.name,"DhcpSEn")) == 0){
			      if((strcmp(child->child->value.text.string,"Disable")) == 0){
				  prof->dhcpEn = PROF_DISABLE;
			      }else{
				  prof->dhcpEn = PROF_ENABLE;
			      }
			  }else if((strcmp(child->value.element.name,"ipStart")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->ipStart=(Uint32)(ip.s_addr);
			      }
			  }else if((strcmp(child->value.element.name,"ipEnd")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->ipEnd=(Uint32)(ip.s_addr);
			      }

			  }else if((strcmp(child->value.element.name,"nm")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->netmask=(Uint32)(ip.s_addr);
			      }
			  }else if((strcmp(child->value.element.name,"gw")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->gateway=(Uint32)(ip.s_addr);
			      }
			  }else if((strcmp(child->value.element.name,"lease")) == 0)
			  {
				    prof->lease = (Uint32)strtol(child->child->value.text.string, NULL, 10);
			  }else if((strcmp(child->value.element.name,"pDns")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->pDns=(Uint32)(ip.s_addr);
			      }
			  }else if((strcmp(child->value.element.name,"sDns")) == 0)
			  {
			      rt = inet_aton(child->child->value.text.string, &ip);		
			      if(rt != 0){
				  prof->sDns=(Uint32)(ip.s_addr);
			      }
			  }
		    }
		    child = child->next;
		}
		nvramWriteCommit();
	    }

	}
	mxmlDelete(doc);
    }

    return ret;
}
#endif
void ProfReadFromFlash(void)
{
#if 1
#if 0
    char buf[CONFIG_BUFF_SIZE];/*��ѹ������*/
#endif
    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_FLASH_SIZE];/*��ѹ������*/
    unsigned long  zipLen= CONFIG_FLASH_SIZE;
    unsigned long  bufLen=CONFIG_BUFF_SIZE;
#if (EASY_TASK == FYES)
    char *tmp = NULL;
#endif

    buf = (char *)malloc(CONFIG_BUFF_SIZE);

    if (!buf) {
	printf("malloc %d bytes failed\n", CONFIG_BUFF_SIZE);
	return;
    }
#if 0
    memset(buf, 0, sizeof(buf));
#endif
    memset(buf, 0, CONFIG_BUFF_SIZE);
    memset(zipBuf, 0, sizeof(zipBuf));


    conf_flash_read(zipBuf, 0, CONFIG_FLASH_SIZE);

    zipLen = strtoul(zipBuf, NULL,16);/*�����ļ�ͷ�������ļ�ѹ����ĳ���*/

   if(Z_OK != uncompress(buf, &bufLen, &zipBuf[CONFIG_HEADER_LEN], zipLen))
   {
       /*��ѹʧ����ϵͳ��������Ĭ������״̬*/
       printf("Config file uncompress error and system while start on default config\n");
   }
   else
   {
#if (EASY_TASK == FYES)
	   if ((tmp = strstr(buf, "case")) != NULL) {
		   if (!memcmp(tmp + 5, "name", 4))/*case������һ��ASCII�����Ϊ6���ַ�(ack)*/
				   IsDefault = 1;
	   }
#endif
       buf[bufLen] = '\0';
#if (NOTICE == FYES)
//       printf( "%s-%d, buf:%s\n", __FUNCTION__, __LINE__, buf );
       parseStrToFile(buf, NOTICE_HTML_START_1, NOTICE_HTML_START_2, NOTICE_HTML_1);
       parseStrToFile(buf, NOTICE_HTML_START_2, NOTICE_HTML_START_3, NOTICE_HTML_2);
#if (WEB_AUTH == FYES)
	#if(NET_SNIPER == FYES)
	   parseStrToFile(buf, NOTICE_HTML_START_3, NOTICE_HTML_START_4, NOTICE_HTML_3);
	   parseStrToFile(buf, NOTICE_HTML_START_4, WEB_AUTH_CONTACT_START, NOTICE_HTML_4);
    #else
	   parseStrToFile(buf, NOTICE_HTML_START_3, WEB_AUTH_CONTACT_START, NOTICE_HTML_3);
    #endif
#else
	#if(NET_SNIPER == FYES)
       parseStrToFile(buf, NOTICE_HTML_START_3, NOTICE_HTML_START_4, NOTICE_HTML_3);
	   parseStrToFile(buf, NOTICE_HTML_START_4, NULL, NOTICE_HTML_4);
	#else
       parseStrToFile(buf, NOTICE_HTML_START_3, NULL, NOTICE_HTML_3);
	#endif
#endif
//       printf( "%s-%d, buf:%s\n", __FUNCTION__, __LINE__, buf );
#endif
#if (WEB_AUTH == FYES)
       parseStrToFile(buf, WEB_AUTH_CONTACT_START, WEB_AUTH_PICURL_START, WEB_AUTH_CONTACT_PATH);
#if 0
	#if(NET_SNIPER == FYES)
       parseStrToFile(buf, WEB_AUTH_PICURL_START, NOTICE_HTML_START_4, WEB_AUTH_PICURL_PATH);
	   parseStrToFile(buf, NOTICE_HTML_START_4, NULL, NOTICE_HTML_4);
	#else
       parseStrToFile(buf, WEB_AUTH_PICURL_START, NULL, WEB_AUTH_PICURL_PATH);
	#endif
#endif
       parseStrToFile(buf, WEB_AUTH_PICURL_START, NULL, WEB_AUTH_PICURL_PATH);
#endif
#if (NOTICE != FYES && WEB_AUTH != FYES)
#if(NET_SNIPER == FYES)
       parseStrToFile(buf, WEB_AUTH_PICURL_START, NULL, WEB_AUTH_PICURL_PATH);
#endif
#endif
       parseProfileFromStr(buf, 0);
#if (DHCP_POOL == FYES)
       updateDhcp2DhcpPool(buf);
#endif
   }
   free(buf);
   return;
#else
    FILE *fp;
    mxml_node_t *doc, *config;
    fp = fopen("/tmp/filename.xml", "r");
    if(fp == NULL)
        return;
    doc = mxmlLoadFile(NULL, fp,MXML_TEXT_CALLBACK);
    if(doc != NULL){
    config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);
    parseProfileFromXml(rootConfig, config->child);
    mxmlDelete(doc);
    }
    fclose(fp);
    return;
#endif
}


#if ((NOTICE == FYES) || (WEB_AUTH ==FYES))
/*
* ���ܣ�
* ��start�ַ�����file�ж������ַ�����ƴ����buf��
* ���������
* buf_size: buf�ռ��С
* start: Ҫƴ�ӵ���ʼ�ַ���
* file: Ҫ��ȡ���ļ�
* ���������
* buf: ����ƴ�ӵ��ַ���
* ����ֵ��
* ���浽buf�е��ַ�������
* ע�⣺ �ַ���ƴ�Ӳ�ҪԽ��
*/
int parseFileToStr(char *buf, int buf_size, const char *start, const char *file, int first) 
{
         /* ��ȡfile��ƴ�� */
	FILE 	*fp = NULL;
	Uchar	filebuf[ NOTIFY_HTM1_SIZE ] = {0u};
	size_t	size = 0u;
	size_t	start_size = strlen( start );
	size_t	filebuf_size = 0U;

	if( (NULL != buf) && (NULL != start) && (NULL != file) )
	{
	    fp = fopen( file, "r" );
	    if( NULL != fp )
	    {
		fread( (void*)filebuf, sizeof(char), NOTIFY_HTM1_SIZE, fp );
		fclose( fp );
		filebuf_size = strlen( (char*)filebuf );
		/*��ֹԽ��*/
		if( buf_size > ( (int)start_size + (int)filebuf_size ) )
		{
			if(1==first)
			{
			/*��ǰ���profile���ַֿ�*/
			    *buf = '\0';
			    strcpy( buf + 1, start );
			    strcpy( buf + 1 + start_size, filebuf );
			    size = start_size + filebuf_size + 1;
			}else{
			    strcpy( buf, start );
			    strcpy( buf + start_size, filebuf );
			    size = start_size + filebuf_size;
			}

		}
	    }
	}
	return (int)size;
}

/*
* ���ܣ�
* ����start֮����ַ��������ļ���,ͬʱ��buf�а���start���ڵ�֮���ַ�����0
* ���������
* buf�� Ҫ�������ַ���
* start: ��ʶ��Ҫ�������ļ��е��ַ�����ʼλ��
* file : ���ַ������������ļ�
* ����ֵ��
* ��buf�н������ַ�������
*/
int parseStrToFile(char *bufAll, const char *start, const char *next_start, const char *file) 
{
        /* �����ַ��� */
	FILE	*fp = NULL;
	char	tmp = '0';
	char	*tmppoint = NULL;
	char	*next_tmppoint = NULL;
	Uchar	bufbak[ NOTIFY_HTM1_SIZE ] = {0u};
	size_t	file_size = 0u;
	size_t	start_size = strlen( start );
	char	*buf = NULL;

	if( (NULL != bufAll) && (NULL != start) && (NULL != file) )
	{
	    buf = bufAll + strlen( bufAll ) + 1;    /*ָ��notice����*/		
	    fp = fopen( file, "w" );
	    if( (NULL != fp) )
	    {
		//printf( "filebuf_write:%s\n", buf );
		//memcpy( bufbak, buf, NOTIFY_HTM1_SIZE + strlen(start) );
		/*����start*/
		tmppoint = strstr( buf, start );
		if( NULL != tmppoint )
		{
		    tmppoint += start_size;
		    //printf( "tmppoint_write:%s\n", tmppoint );
		    /*��next_start*/
		    if( NULL != next_start )
		    {
			next_tmppoint = strstr( buf, next_start );
			if( NULL != next_tmppoint )
			{
			    tmp = *next_tmppoint;
			    *next_tmppoint = '\0';			    /*�ı�next_start*/
			}
		    }
		    file_size = strlen(tmppoint);
		    fwrite( (void*)tmppoint, sizeof(char), file_size, fp ); /*������file��*/
		    if( NULL != next_tmppoint )
		    {
			*next_tmppoint = tmp;				    /*��ԭnext_start*/
		    }
		    if( NULL == next_start )
		    {
#if (NOTICE == FYES)
			tmppoint = strstr( buf, NOTICE_HTML_START_1 );
#else
#if (WEB_AUTH== FYES)
			tmppoint = strstr( buf, WEB_AUTH_CONTACT_START);
#else
			tmppoint = strstr( buf, NOTICE_HTML_START_4);
#endif
#endif
			if( NULL != tmppoint )
			{
			    memset( tmppoint, 0, strlen(tmppoint));	    /*�����һ��ȫ������*/
			}
		    }
		}
		fclose( fp );
	    }
	}
	//printf( "filebuf_write2:%s\n", buf );
	return (int)file_size;
}
#endif

#if (UTT_NV_D908W == FYES)
#define AP_FLASH_DEBUG 0
#if AP_FLASH_DEBUG
#define AP_FLASH_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define AP_FLASH_PRINTF(fmt, args...)
#endif
#define AP_CONFIG_ROOT "/tmp/urcp/ap_configs/"
#define AP_DRAWING_CONFIG_ROOT "/tmp/urcp/drawing/"
#define AP_READ_BUF_SIZE 1024
#define STANDARD_FORMAT_AP_CONFIG		"<```AP-%s %ld-AP```>"
#define STANDARD_FORMAT_AP_DRAWING		"<```AP-%s-AP```>"
#define STANDARD_MATCH_START			"<```AP-"
#define STANDARD_MATCH_END			"-AP```>"

int parseApConfigToFile(char *buf, int flag);

/*
* ���ܣ�
* ֧��AC���ܵ��豸��������AP�������ļ����浽FLASH��
* ��Ҫ�����豸�ķ������֣�Ȼ�󱣴�'apConfig'�ķ���
* ���������
* ��
* ����ֵ��
* ����ɹ���ʧ��ֵ
*/
int apConfigWriteToFlash(void) 
{
    
    DIR *dir = NULL;
    struct dirent *next;
    FILE *fp = NULL;
    char filename[100];
    char buffer[AP_READ_BUF_SIZE+1];
    
    struct stat buf1;

    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_AP_FLASH_SIZE];/*��ѹ������*/
    unsigned long zipLen = CONFIG_AP_FLASH_SIZE - CONFIG_HEADER_LEN;
    int len = 0;

    buf = (char *)malloc(CONFIG_AP_BUFF_SIZE);

    if (!buf) {
	fprintf(stderr,"malloc %d bytes failed\n", CONFIG_AP_BUFF_SIZE);
	return 0;
    }

    memset(buf, 0, CONFIG_AP_BUFF_SIZE);
    memset(zipBuf, 0, sizeof(zipBuf)); 
    
    /*
     * get all ap device config
     * */
    dir = opendir(AP_CONFIG_ROOT);
    if (!dir)
    {
	fprintf(stderr,"Cannot open %s\n",AP_CONFIG_ROOT);
	return 0;
    }
    /*
     * ��AP_CONFIG_ROOTĿ¼�µ�����ap�����ļ�����ͳһ�ĸ�ʽ������һ��
     * Ȼ�󾭹�ѹ�����浽FLASH��
     * */
    while ((next = readdir(dir)) != NULL) {
	/* Must skip "" since that is outside /proc */
	if (strcmp(next->d_name, "..") == 0)
	    continue;
	
	if(strstr(next->d_name, ".xml") == NULL) {
	    continue;
	}
	sprintf(filename, "%s%s", AP_CONFIG_ROOT, next->d_name);
    memset(&buf1, 0, sizeof(struct stat));
    if ((0 == stat(filename, &buf1)) && (S_ISREG(buf1.st_mode))) {
	AP_FLASH_PRINTF("%s-%d: file name: %s, buf1.st_atime: %ld, buf1.st_mtime: %ld, buf1.st_ctime: %ld\n",
		__func__,__LINE__,filename,buf1.st_atime,buf1.st_mtime,buf1.st_mtime);
	if (! (fp = fopen(filename, "r")) )
	{
	    continue;
	}
	/*
	 * ����ͳһ�ĸ�ʽ������������
	 * */
	snprintf(buf+strlen(buf), (CONFIG_AP_BUFF_SIZE-strlen(buf)), STANDARD_FORMAT_AP_CONFIG,  next->d_name, buf1.st_mtime);
	//Read first line in /proc/?pid?/status
	while (fread(buffer, 1, AP_READ_BUF_SIZE, fp) > 0) {
	    
	    snprintf(buf+strlen(buf), (CONFIG_AP_BUFF_SIZE-strlen(buf)), "%s",buffer);
	    memset(buffer, 0, sizeof(buffer));
	}
	AP_FLASH_PRINTF("%s-%d: len: %d, buf: %s\n",__func__,__LINE__,strlen(buf),buf);

	AP_FLASH_PRINTF("%s-%d: =============================\n\n",__func__,__LINE__);
	fclose(fp);
	memset(filename, 0, sizeof(filename));
	memset(buffer, 0, sizeof(buffer));
    }
    
    }
    closedir(dir);
    len = strlen(buf);
    AP_FLASH_PRINTF("%s-%d: len: %d, buf: %s\n",__func__,__LINE__,len,buf);
#if 1
    /*
     * compress config of all ap device
     * */
    if(len > 0) {
	if(Z_OK != compress(&zipBuf[CONFIG_HEADER_LEN], &zipLen, buf, len))
	/*Ϊ�˼ӿ�ѹ���ٶȹ���strlen*/
	{
	    /*ѹ��ʧ����дflash*/
	    fprintf(stderr,"ap Config file compress error and write to flash fail\n");
	}
	else
	{
	    sprintf(zipBuf,"%x",zipLen);/*�����ļ�ͷ����ѹ������ļ�����*/
	    ap_conf_flash_write(zipBuf, 0, CONFIG_AP_FLASH_SIZE, "apConfig");
	}
    }
#endif
    free(buf);
    
    return 1;
}

/*
* ���ܣ�
* ֧��AC���ܵ��豸��������AP�������ļ���FLASH�л�ȡ
* Ȼ�󾭹����ļ��������ݣ������ÿ��ap�������ļ�
* ���������
*   buf:    ��FLASH��ȡ����ѹ������ı�
*   flag:   ��ʾ����1��ʾ�����ļ�����
* ����ֵ��
* �����ɹ���ʧ��ֵ
*/
int parseApConfigToFile(char *buf, int flag) 
{

    /* �����ַ��� */
    FILE    *fp = NULL;
    char    tmp = '0';
    char    *tmppoint = NULL;
    char    *next_tmppoint = NULL;
    char    *fileStart = NULL;
    char    *fileEnd = NULL;
    char    filename[100];
    int	    filenameLen = 0;
    size_t  file_size = 0u;
    size_t  start_size = strlen( STANDARD_MATCH_START );
    size_t  end_size = strlen( STANDARD_MATCH_END );
    
    char tmp_name[50];
    time_t file_mtime = 0;
    struct stat buf1;
    struct tm *t = NULL;
    unsigned char timeStr[256];
    struct utimbuf timebuf;

    /*
     * ���ձ���ĸ�ʽ���������ı�����
     * �ֱ�����ļ������ļ���ʱ�����ԣ��ļ����ݵ���Ϣ
     * */
    if( (NULL != buf) )
    {
	/*����start*/
	tmppoint = strstr( buf, STANDARD_MATCH_START );
	while( NULL != tmppoint )
	{
	    fileStart = tmppoint + start_size;
	    fileEnd = strstr(buf, STANDARD_MATCH_END);
	    /*
	     * ���Ȼ�ȡ�ļ���
	     * */
	    if(fileEnd != NULL) {
		memset(filename, 0, sizeof(filename));
		filenameLen = fileEnd - fileStart;
#if 1
		/*
		 * ��ȡ�ļ���ʱ������
		 * */
		file_mtime = 0;
		memcpy(filename, fileStart, filenameLen);
		sscanf(filename, "%s%ld", tmp_name, &file_mtime);
		AP_FLASH_PRINTF("%s-%d: file name: %s, tmp name: %s, file mtime: %ld\n",__func__,__LINE__,filename,tmp_name,file_mtime);
		
#endif
		/*
		 * �ļ��ı���·����Ϣ
		 * */
		if((filenameLen > 0)&&filenameLen <100) {
		    if(flag == 1) {	    /*AP�����ļ�*/
			memset(filename, 0, sizeof(filename));
			strcpy(filename, AP_CONFIG_ROOT);
			strcpy(filename+strlen(AP_CONFIG_ROOT), tmp_name);
			AP_FLASH_PRINTF("%s-%d: file name: %s\n",__func__,__LINE__,filename);
		    } 
		} else {
		    fprintf(stderr, "ap config parse, parse end.\n");
		    return;
		}
	    } else {
		fprintf(stderr, "ap config parse error, match file end error.\n");
	    }
	    buf = fileEnd + end_size;
	    fp = fopen( filename, "w" );
	    if( (NULL != fp) ) {
		tmppoint = buf;

		next_tmppoint = strstr( buf, STANDARD_MATCH_START );
		if( NULL != next_tmppoint )
		{
		    tmp = *next_tmppoint;
		    *next_tmppoint = '\0';                          /*�ı�next_start*/
		} else {
		    /*lase file buf*/
		}
		file_size = strlen(tmppoint);
		fwrite( (void*)tmppoint, sizeof(char), file_size, fp ); /*������file��*/
		fclose( fp );
#if 1
		/*
		 * �޸��ļ���ʱ������
		 * */
		if(file_mtime > 0) {
		    timebuf.actime = file_mtime;
		    timebuf.modtime = file_mtime;
		    utime(filename, &timebuf);
		}
#endif
		if( NULL != next_tmppoint )
		{
		    *next_tmppoint = tmp;                               /*��ԭnext_start*/
		}
	    } else {
		fprintf(stderr,"create file %s error.\n",filename);
		//continue;
	    }
	    tmppoint = next_tmppoint;
	}
    }
    return 1;
}

/*
* ���ܣ�
* ֧��AC���ܵ��豸���������FLASH�ж�ȡ��������
* Ȼ���ѹ���������ݺ󱣴浽��ap�����ļ�
* ���������
* ��
* ����ֵ��
* �ɹ���ʧ��ֵ
*/
int apConfigReadFromFlash(void)
{
    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_AP_FLASH_SIZE];/*��ѹ������*/
    unsigned long  zipLen= CONFIG_AP_FLASH_SIZE;
    unsigned long  bufLen=CONFIG_AP_BUFF_SIZE; 

    buf = (char *)malloc(CONFIG_AP_BUFF_SIZE);

    if (!buf) {           
	fprintf(stderr,"malloc %d bytes failed\n", CONFIG_AP_BUFF_SIZE);
	return 0;
    }
#if 0
    memset(buf, 0, sizeof(buf));
#endif
    memset(buf, 0, CONFIG_AP_BUFF_SIZE);
    memset(zipBuf, 0, sizeof(zipBuf));

    /*
     * ��'apConfig'�ķ����ж�ȡap����������
     * */
    ap_conf_flash_read(zipBuf, 0, CONFIG_AP_FLASH_SIZE, "apConfig");

    zipLen = strtoul(zipBuf, NULL,16);/*�����ļ�ͷ�������ļ�ѹ����ĳ���*/
    /*
     * ���ý�ѹ����Ȼ���������
     * */
    if(Z_OK != uncompress(buf, &bufLen, &zipBuf[CONFIG_HEADER_LEN], zipLen))
    {
	/*��ѹʧ����ϵͳ��������Ĭ������״̬*/
	fprintf(stderr,"ap Config file uncompress error and system while start on default config\n");
	return 0;
    }
    else
    {
	AP_FLASH_PRINTF("%s-%d: buf: %s\n",__func__,__LINE__,buf);
	parseApConfigToFile(buf, 1);	/*�����������ݣ�����Ϊ����ap�����ļ�*/
    }
    
    free(buf);
    return 1;
}

/*
* ���ܣ�
* ֧��AC���ܵ��豸���������FLASH�ж�ȡAP�ֲ�ͼ����
* Ȼ��������ݺ󱣴浽AP�ֲ�ͼ�ļ���
* ���������
*   buf:    ��ȡ��ͼƬ����
*   flag:   ��ʾ��
* ����ֵ��
* �����ɹ���ʧ��ֵ
*/
int parseApDrawingToFile(char *buf, int flag) {
    /* �����ַ��� */
    FILE    *fp = NULL;
    char    *tmppoint = NULL;
    char    *fileStart = NULL;
    char    *fileEnd = NULL;
    char    filename[100];
    int	    filenameLen = 0;
    size_t  start_size = strlen( STANDARD_MATCH_START );
    size_t  end_size = strlen( STANDARD_MATCH_END );
    int fileLen = 0;

    if( (NULL != buf) )
    {
	if(flag == 1) {
	    fileLen = strtoul(buf, NULL,16);/*�����ļ�ͷ�������ļ�ѹ����ĳ���*/
	    buf += CONFIG_HEADER_LEN;
	}
	/*
	 * �����ı�����
	 * �ֱ��ȡ�ļ������ļ����ݵ���Ϣ
	 * */
	/*����start*/
	tmppoint = strstr( buf, STANDARD_MATCH_START );
	if( NULL != tmppoint )
	{
	    fileStart = tmppoint + start_size;
	    fileEnd = strstr(buf, STANDARD_MATCH_END);
	    if(fileEnd != NULL) {
		memset(filename, 0, sizeof(filename));
		filenameLen = fileEnd - fileStart;
		if((filenameLen > 0)&&filenameLen <100) {
		    if(flag == 1) {	    /*AP�����ļ�*/
			strcpy(filename, AP_DRAWING_CONFIG_ROOT);
			strncpy(filename+strlen(AP_DRAWING_CONFIG_ROOT), fileStart, filenameLen);
			AP_FLASH_PRINTF("%s-%d: file name: %s, file len: %d\n",__func__,__LINE__,filename,fileLen);
		    }
		} else {
		    fprintf(stderr, "ap drawing parse, parse end.\n");
		    return;
		}
	    } else {
		fprintf(stderr, "ap drawing parse error, match file end error.\n");
	    }
	    if(fileLen > 0) {
		buf = fileEnd + end_size;
		fp = fopen( filename, "w" );
		if( (NULL != fp) ) {
		    if(NULL !=buf) {
			/*AP �ֲ�ͼ�ļ�����*/
			fwrite( (void*)buf, sizeof(char), fileLen, fp ); /*������file��*/
		    }
		    fclose( fp );
		    //break;
		} else {
		    fprintf(stderr,"create file %s error.\n",filename);
		    //continue;
		}
	    }
	}
    }
    return 1;
}

/*
* ���ܣ�
* ֧��AC���ܵ��豸������AP�ֲ�ͼ���ݵ�FLASH��
* ���������
* ��
* ����ֵ��
* �ɹ���ʧ��ֵ
*/
int apDrawingWriteToFlash(void) 
{
    
    DIR *dir = NULL;
    struct dirent *next;
    FILE *fp = NULL;
    char filename[100];
    char buffer[CONFIG_AP_FLASH_SIZE+1];

    char *buf = NULL; /* ��ѹ������ */
    int read_file = 0;
    struct stat buf1;
    int size1 = 0;
    int formatLen = 0;
    
    /*APͼֽ���治��ѹ������*/

    buf = (char *)malloc(CONFIG_AP_FLASH_SIZE);

    if (!buf) {
	fprintf(stderr,"malloc %d bytes failed\n", CONFIG_AP_FLASH_SIZE);
	return 0;
    }

    memset(buf, 0, CONFIG_AP_FLASH_SIZE);
    memset(buffer, 0, CONFIG_AP_FLASH_SIZE);
    
    /*
     * get all ap draw picture
     * */
    dir = opendir(AP_DRAWING_CONFIG_ROOT);
    if (!dir)
    {
	fprintf(stderr,"Cannot open %s\n",AP_DRAWING_CONFIG_ROOT);
	return 0;
    }
    /*
     * ��ȡAP_DRAWING_CONFIG_ROOTĿ¼�µ�AP�ֲ�ͼ�ļ�������
     * �����ո�ʽ���浽FLASH��
     * Ŀǰ��֧��һ��ͼƬ����
     * */
    while ((next = readdir(dir)) != NULL) {
	formatLen = strlen(STANDARD_FORMAT_AP_DRAWING) - 2;
	/* Must skip "" since that is outside /proc */
	if (strcmp(next->d_name, "..") == 0)
	    continue;
	
	if(strstr(next->d_name, ".jpg") == NULL) {
	    continue;
	}
	if(strstr(next->d_name, "default.jpg") != NULL) {
	    continue;
	}
	sprintf(filename, "%s%s", AP_DRAWING_CONFIG_ROOT, next->d_name);
	AP_FLASH_PRINTF("%s-%d: filename: %s\n",__func__,__LINE__,filename);
#if 1
	if ((0 == stat(filename, &buf1)) && (S_ISREG(buf1.st_mode))) {
	    size1 = buf1.st_size;
	    if ((size1 >= 1024) && (size1 < (60*1024))) {
		if (! (fp = fopen(filename, "r")) )
		{
		    fprintf(stderr,"open %s error.\n",filename);
		    continue;
		}
    
		if(fread(buffer, 1, size1, fp) == size1) {
		    AP_FLASH_PRINTF("%s-%d: file size: %d\n",__func__,__LINE__,size1);
		    formatLen += strlen(next->d_name);
		    snprintf(&buf[CONFIG_HEADER_LEN], (CONFIG_AP_BUFF_SIZE), STANDARD_FORMAT_AP_DRAWING,  next->d_name);
		    memcpy(&buf[CONFIG_HEADER_LEN]+formatLen, buffer,size1);
		    read_file += 1;
		}
	    }
	}
#endif
	AP_FLASH_PRINTF("%s-%d: =============================\n\n",__func__,__LINE__);
	fclose(fp);
	if(read_file >= 1) {
	    break;
	}
	memset(filename, 0, sizeof(filename));
	memset(buffer, 0, sizeof(buffer));
    
    }
    closedir(dir);
    sprintf(buf,"%x",(size1));/*�����ļ�ͷ����ѹ������ļ�����*/
    AP_FLASH_PRINTF("%s-%d: size1: %d, read_file: %d\n",__func__,__LINE__,size1,read_file);

    ap_conf_flash_write(buf, 0, CONFIG_AP_FLASH_SIZE, "apDrawing");
    //parseApDrawingToFile(buf,1);
    free(buf);
    
    return 1;
}

/*
* ���ܣ�
* ֧��AC���ܵ��豸����FLASH�ж�ȡAP�ֲ�ͼ����
* ���������
* ��
* ����ֵ��
* �ɹ���ʧ��ֵ
*/
int apDrawingReadFromFlash(void)
{
    char Buf[CONFIG_AP_FLASH_SIZE];/*��ѹ������*/

    memset(Buf, 0, sizeof(Buf));

    /*AP�ֲ�ͼ����ʱδ��ѹ������*/
    ap_conf_flash_read(Buf, 0, CONFIG_AP_FLASH_SIZE, "apDrawing");
    {
	parseApDrawingToFile(Buf, 1);
    }
    
    return 1;
}
#endif
