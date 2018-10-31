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
 * 功能:读取xml中的某条配置
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
	conf_flash_read(buf, 0, CONFIG_FLASH_SIZE);/*读flash*/
	doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);/*构建xml结构*/
	if(doc != NULL) {
	    config = mxmlFindElement(doc, doc, "config", NULL, NULL, MXML_DESCEND);/*定位到config节点*/

	    if(config!=NULL)
	    {
		DEBUG_XMLPRT("%s\n", argv[2]);
		config = mxmlFindElement(config, doc, argv[2], NULL, NULL, MXML_DESCEND);/*定位到功能项一级节点*/
		if(config!=NULL)
		{
		    DEBUG_XMLPRT("%s\n", argv[3]);
		    config = mxmlFindElement(config, doc,"case" , "name", argv[3], MXML_DESCEND);/*定位到功能中相应case*/
		    if(config != NULL)
		    {
			DEBUG_XMLPRT("%s\n", argv[4]);
			config = mxmlFindElement(config, doc, argv[4], NULL, NULL, MXML_DESCEND);/*定位到case中相应节点*/

			if(config != NULL)
			{
			    config = config->child;/*定位到最内层文本节点*/

			    if((config != NULL) && (config->type == MXML_TEXT))
			    {/*最内层如不为文本节点则查找失败*/
				printf("%s\n", config->value.text.string);/*打印查找到的值*/
				ret = 0;/*查找成功*/
			    }
			}
		    }
		}
	    }
	}

	mxmlDelete(doc);
    }
    if(ret != 0)
    {/*查找失败则打印命令行提示*/
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
 * 配置清空
 * return: 0 success, -1 fail
 * bhou
 */
int clear_nvram(void)
{
    int ret = 0;
    unsigned char buf[CONFIG_FLASH_SIZE];
    memset(buf, 0 ,sizeof(buf));

    conf_flash_write(buf, 0, CONFIG_FLASH_SIZE);/*写空配置达到清空的目的*/
    return ret;
}
/**
 * 配置更新
 * fname:配置文件名
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

    if (NULL == (fp = fopen(fname, "ro")))/*文件打开失败*/
    {
	perror("fopen");
	ret= -1;
    }
    else 
    {
	/*逐行读取配置*/
	while (NULL != fgets(ptr, max_len, fp))
	{
	    n = strlen(ptr);
	    if (ptr[0] == '\n')/*无用的行*/
	    {
		memset(ptr, 0, n);/*清空本次读取到的行*/
	    }
	    else
	    {
		ptr += n;/*指向下次读取的起点*/
		max_len -= n;/*数组可写长度减少*/
	    }
	}

	/*配置为空无法用此函数，可通过clear清空*/
	if (buf[0] != '\0') 
	{
	    DEBUG_XMLPRT("%s", buf);
	    doc = mxmlLoadString(NULL, buf, MXML_TEXT_CALLBACK);
	    if (NULL != doc)/*判断配置文件格式是否正确,防止误操作*/
	    {
		conf_flash_write(buf, 0, CONFIG_FLASH_SIZE);/*写flash*/
		mxmlDelete(doc);
	    }
	    else
	    {
		printf("file format error!\n");/*配置文件格式错误*/
		ret = -1;
	    }
	}
	else
	{
	    printf("file format error!\n");/*配置文件格式错误*/
	    ret = -1;
	}
    }
    fclose(fp);
    return ret;
}
/**
 *配置打印
 *bhou
 *2011-10-12
 */
int nvram_show(void)
{
#if 0
    char buf[CONFIG_BUFF_SIZE];/*无压缩配置*/
#endif
    char *buf = NULL; /* 无压缩配置 */
    char zipBuf[CONFIG_FLASH_SIZE];/*已压缩配置*/
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

    zipLen = strtoul(zipBuf, NULL,16);/*配置文件头存配置文件压缩后的长度*/

   if(Z_OK != uncompress(buf, &bufLen, &zipBuf[CONFIG_HEADER_LEN], zipLen))
   {
       /*解压失败则系统将运行在默认配置状态*/
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


