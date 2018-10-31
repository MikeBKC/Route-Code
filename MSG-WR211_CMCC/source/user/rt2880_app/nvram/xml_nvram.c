#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "mxml.h"

#include <linux/autoconf.h>

#define XML_NVRAM_DEBUG 1

#if XML_NVRAM_DEBUG
#define DEBUG_XMLPRT(fmt, argx...) printf(fmt, ##argx)
#else
#define DEBUG_XMLPRT(fmt, argx...)
#endif

int set_usage(char *aout)
{
	return -1;
}

int ra_nv_set(int argc,char **argv)
{
    return 0;
}

int get_usage(void)
{
    return -1;
}
/**
 * ����:��ȡxml�е�ĳ������
 * bhou
 * 2011-10-12
 */
int ra_nv_get(int argc, char *argv[])
{
    int ret = -1;
    char buf[CONFIG_FLASH_SIZE] = {0};
    mxml_node_t *doc = NULL, *config = NULL;

    if (argc >= 5)
    {
	conf_flash_read(buf, 0, CONFIG_FLASH_SIZE);/*��flash*/
	doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);/*����xml�ṹ*/
	if(doc != NULL) {
	    config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);/*��λ��config�ڵ�*/

	    if(config!=NULL)
	    {
		DEBUG_XMLPRT("%s\n", argv[2]);
		config = mxmlFindElement(config, doc, argv[2], NULL, NULL, MXML_DESCEND);/*��λ��������һ���ڵ�*/
		if(config!=NULL)
		{
		    DEBUG_XMLPRT("%s\n", argv[3]);
		    config = mxmlFindElement(config, doc,"case" , "name", argv[3], MXML_DESCEND);/*��λ����������Ӧcase*/
		    if(config != NULL)
		    {
			DEBUG_XMLPRT("%s\n", argv[4]);
			config = mxmlFindElement(config, doc, argv[4], NULL, NULL, MXML_DESCEND);/*��λ��case����Ӧ�ڵ�*/

			if(config != NULL)
			{
			    config = config->child;/*��λ�����ڲ��ı��ڵ�*/

			    if((config != NULL) && (config->type == MXML_TEXT))
			    {/*���ڲ��粻Ϊ�ı��ڵ������ʧ��*/
				printf("%s\n", config->value.text.string);/*��ӡ���ҵ���ֵ*/
				ret = 0;/*���ҳɹ�*/
			    }
			}
		    }
		}
	    }
	}

	mxmlDelete(doc);
    }
    if(ret != 0)
    {/*����ʧ�����ӡ��������ʾ*/
	get_usage();
    }
    return ret;
}

void usage(char *cmd)
{
	printf("Usage:\n");
	printf("  %s <command> [<file>]\n\n", cmd);
	printf("command:\n");
	printf("  show    - display values in nvram\n");
	printf("  renew   - replace nvram values with <file>\n");
	printf("  clear	  - clear all entries in nvram\n");
	printf("file:\n");
	printf("          - file name for renew command\n");
	exit(0);
}

int main(int argc, char *argv[])
{
    char *cmd;
    int ret = 0;

    if (argc < 2)
    {
	usage(argv[0]);
    }
    else if ((cmd = strrchr(argv[0], '/')) != NULL)
    {
	cmd++;
    }
    else
    {
	cmd = argv[0];
    }

    if (!strncmp(argv[1], "get", 4))
    {
	ret = ra_nv_get(argc, argv);
    }
    else if (!strncmp(argv[1], "set", 4))
    {
	ret = ra_nv_set(argc, argv);
    }
    else if (!strncasecmp(argv[1], "show", 5)) 
    {
	ret = nvram_show();
    }
    else if(!strncasecmp(argv[1], "clear", 6)) {
	ret = clear_nvram();
    }
    else if (argc == 3) {
	if (!strncasecmp(argv[1], "renew", 6))
	{
	    ret = renew_nvram(argv[2]);
	}
    }
    else
    {
	usage(argv[0]);
    }

    return ret;
}

/**
 * �������
 * return: 0 success, -1 fail
 * bhou
 */
int clear_nvram(void)
{
    int ret = 0;
    unsigned char buf[CONFIG_FLASH_SIZE];
    memset(buf, 0 ,sizeof(buf));

    conf_flash_write(buf, 0, CONFIG_FLASH_SIZE);/*д�����ôﵽ��յ�Ŀ��*/
    return ret;
}
/**
 * ���ø���
 * fname:�����ļ���
 * return: 0 success, -1 fail
 * bhou
 * 2011-10-12
 */
int renew_nvram(char *fname)
{
    FILE *fp;
    int ret = 0;
    mxml_node_t *doc;
    unsigned char buf[CONFIG_FLASH_SIZE];
    unsigned char *ptr = buf;
    int max_len = CONFIG_FLASH_SIZE;
    int n = 0;

    memset(buf, 0, sizeof(buf));

    if (NULL == (fp = fopen(fname, "ro")))/*�ļ���ʧ��*/
    {
	perror("fopen");
	ret= -1;
    }
    else 
    {
	/*���ж�ȡ����*/
	while (NULL != fgets(ptr, max_len, fp))
	{
	    n = strlen(ptr);
	    if (ptr[0] == '\n')/*���õ���*/
	    {
		memset(ptr, 0, n);/*��ձ��ζ�ȡ������*/
	    }
	    else
	    {
		ptr += n;/*ָ���´ζ�ȡ�����*/
		max_len -= n;/*�����д���ȼ���*/
	    }
	}

	/*����Ϊ���޷��ô˺�������ͨ��clear���*/
	if (buf[0] != '\0') 
	{
	    DEBUG_XMLPRT("%s", buf);
	    doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);
	    if (NULL != doc)/*�ж������ļ���ʽ�Ƿ���ȷ,��ֹ�����*/
	    {
		conf_flash_write(buf, 0, CONFIG_FLASH_SIZE);/*дflash*/
		mxmlDelete(doc);
	    }
	    else
	    {
		printf("file format error!\n");/*�����ļ���ʽ����*/
		ret = -1;
	    }
	}
	else
	{
	    printf("file format error!\n");/*�����ļ���ʽ����*/
	    ret = -1;
	}
    }
    fclose(fp);
    return ret;
}
/**
 *���ô�ӡ
 *bhou
 *2011-10-12
 */
int nvram_show(void)
{
#if 0
    char buf[CONFIG_BUFF_SIZE];/*��ѹ������*/
#endif
    char *buf = NULL; /* ��ѹ������ */
    char zipBuf[CONFIG_FLASH_SIZE];/*��ѹ������*/
    unsigned long  zipLen= CONFIG_FLASH_SIZE;
    unsigned long  bufLen=CONFIG_BUFF_SIZE;

    buf = (char *)malloc(CONFIG_BUFF_SIZE);
    if (!buf) {
	printf("malloc failed\n");
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
       printf("Config file uncompress error\n");
   }
   else
   {
#if 0
       printf("uncompress str len%d:%s\n",zipLen,&zipBuf[CONFIG_HEADER_LEN]);
#endif
       buf[bufLen] = '\0';
       printf("compressLen/unCompressLen:%d/%d \n%s\n",zipLen, bufLen ,buf);
   }

   free(buf);
   return 0;
}


