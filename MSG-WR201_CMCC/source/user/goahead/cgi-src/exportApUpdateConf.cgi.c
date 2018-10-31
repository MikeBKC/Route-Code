/*
 *
 *联动操作里的将联动读取的配置文件保存的pc中
 *KeHuatao
 *2011-05-03
 */
#include <stdio.h> 
#include <string.h> 
#include <stdarg.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <urcp_ipc.h> 
#include <uttMachine.h>
#include <time.h>

#define USB_STORAGE_SIGN    "/media/sd"
#define USB_UPDATE_PATH    "ap_configs"
#define RAM_UPDATE_PATH    "/tmp/urcp/"
static char *decode(char *dest);
static int char2hex(char ch, char *hex);
static int findstr(char* buf, char*str, char *split, char* ret);
static void outPutFile(char* file, char* header);
extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

int dbck(const char *format, ...)
{
#if 0
	va_list valst;
	va_start(valst, format);

	int fd;
	char buf[128];
	memset(buf, 0, 128);
	fd = open("/dev/console", O_RDWR);

	vsnprintf(buf, 128, format, valst);

	write(fd, buf, sizeof(buf));
	close(fd);

	va_end(valst);

	return 0;
#endif
}

/*
*导出配置文件
*KeHuatao
*/
int main(int argc, char *argv[])
{
    unsigned int n=0u;
    char webBuf[1024], fileName[64], inUSB[8], path[128], tmp[256];
    char *strLen=NULL;
    char *header = NULL;
    time_t timep;
    struct tm *p;
    int32 a, b;

    /*读长度*/
    strLen = getenv("CONTENT_LENGTH");
    if(strLen!=NULL)
    {
	n = (unsigned int)strtoul(strLen, NULL, 10);
	if(n>=(sizeof(webBuf)))
	{
	    n = sizeof(webBuf);
	}
        /*初始化*/
	memset(webBuf, 0, sizeof(webBuf));
	memset(fileName, 0, sizeof(fileName));
	memset(inUSB, 0, sizeof(inUSB));
	memset(tmp, 0, sizeof(tmp));
	/*读数据*/
	fread((void*)webBuf, 1u, n, stdin);
#if 0
        findstr(webBuf, "delstr=", "&", fileName);
#endif
        findstr(webBuf, "statustr=", "&", inUSB);
#if 0
	tmp = decode(fileName);
	dbck("tmp = %s, n = %d\n", tmp, n);
	if(tmp==NULL)
	{
	    return -1;
	}
#endif

	timep = time(NULL);
	p = localtime(&timep);
	a = 1900 + (p->tm_year);
	b = 1 + (p->tm_mon);
	sprintf(tmp, "apUpdateConf_%d%02d%02d_%02d%02d%02d.tar", a, b, (p->tm_mday),
			(p->tm_hour), (p->tm_min), (p->tm_sec));
	dbck("tmp = %s", tmp);

	/*打印http头*/
	printf("Pragma: no-cache\\n\n");
	printf("Cache-control: no-cache\\n\n");
	printf("Content-type: application/octet-stream\n");
	printf("Content-Transfer-Encoding: binary\n");
	printf("Content-Disposition: attachment; filename=\"%s.gz\"\n", tmp);
	printf("\n");

	memset(webBuf, '\0', sizeof(webBuf));
	memset(path, '\0', sizeof(path));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (isUSBStorageExist(webBuf) == 1) {
		/* 有U盘的情况 */
		sprintf(path, "cd %s; tar cf %s %s; gzip %s;", webBuf, tmp, USB_UPDATE_PATH, tmp);
		system(path);
		strcat(webBuf, tmp);
	    strcat(webBuf, ".gz");

		outPutFile(webBuf, header);
	} else 
#endif
	{
        memset(webBuf, 0, sizeof(webBuf));
#if (UTT_WX_2000 == FYES)
		strcat(webBuf, "/ff/");
		sprintf(path, "cd %s; tar cf %s %s; gzip %s", webBuf, tmp, USB_UPDATE_PATH, tmp);
		system(path);
#else
	    /*内存中*/
		sprintf(path, "cd /tmp/urcp; tar cf %s %s; gzip %s;", tmp, USB_UPDATE_PATH, tmp);
		system(path);
	    strcat(webBuf, RAM_UPDATE_PATH);
#endif
	    strcat(webBuf, tmp);
	    strcat(webBuf, ".gz");
	    outPutFile(webBuf, header);
	}

	/*fflush*/
	fflush(stdout);

	memset(path, 0, sizeof(path));
	sprintf(path, "rm %s;", webBuf);
	system(path);

	if(tmp!=NULL)
	{
	    free(tmp);
	}
    }

    return 0;
}
/*
 *对获取的编码进行解码
 *KeHuatao
 *
 */
char *decode(char *dest)
{
    int x=0, y=0;
    char *temp=NULL, hex1,hex2;

    temp = (char*)malloc(strlen(dest)*sizeof(char)+1);
    while(dest[y])
    {
	temp[x]=dest[y];
	if(temp[x]=='%')
	{
	    if((char2hex(dest[y+1], &hex1)==0) && (char2hex(dest[y+2], &hex2)==0))
	    {
		temp[x] = hex1*16 + hex2;
		y+=2;
	    }
	}
	x++;
	y++;
    }
    temp[x]='\0';

    return temp;
}
int char2hex(char ch, char *hex)
{
    int ret=0;

    if((ch>='a') && (ch<='z'))   
    {  
	/*小写*/
	*hex = ch - 'a' + 10; 
    }  
    else if((ch>='A') && (ch<='Z'))
    {  
	/*大写*/
	*hex = ch - 'A' + 10; 
    }  
    else if((ch>='0') && (ch<='9'))
    {  
	/*数字*/
	*hex = ch - '0';
    }  
    else
    {  
	/*错误*/
	*hex = 0;
	ret=-1;
    }

    return ret;
}
/*
 *搜索字符串
 *KeHuatao
 */
static int findstr(char* buf, char*str, char *split, char* ret)
{
    char* tmp, *ptr;
    int len, r=0;

    ptr=ret;
    len = strlen(str);
    /*搜索str*/
    tmp = strstr(buf, str);
    if(tmp!=NULL)
    {
	if((tmp==buf)/*开始位置*/
		|| (*(tmp-1)==split[0])/*非开头*/
	  )
	{
	    /*找到了*/
	    tmp = tmp + len;
	    while((*tmp != '\0') && (*tmp != '&'))
	    {
		/*复制*/
		*ptr = *tmp;
		tmp++;
		ptr++;
	    }
	}
	r=1;
    }

    return r;
}

/*
 *输出文件内容
 *KeHuatao
 */
static void outPutFile(char* file, char* header)
{
	dbck("file = %s\n", file);
	char tmp[1025];
	FILE*fp;
	int size = 0, ret = 0;
	struct stat buf;
	char *buf1;
	unsigned long crc = 0u;

	if (0 != stat(file, &buf)) {
		dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
		return;
	}

	size = buf.st_size;
	buf1 = (char *)malloc(size);
	if (buf1 == NULL) {
		dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
		return;
	}

	/*打开*/
	fp = fopen(file, "rb");
	if(fp!=NULL)
	{
		/*文件头*/
		if(header!=NULL)
		{
			dbck("%d\n", strlen(header));
			fwrite(header, 1u, strlen(header), stdout);
		}

		ret = fread(buf1, 1u, size, fp);
		if (ret != size) {
			dbck("%d in %s of %s\n", __LINE__, __func__, __FILE__);
			return;
		}
		crc = crc32(0, buf1, size);
		dbck("crc = %d\n", crc);

		fwrite(&crc, 1u, sizeof(crc), stdout);
		fwrite(buf1, 1u, size, stdout);

		fclose(fp);
	}

	return;
}
