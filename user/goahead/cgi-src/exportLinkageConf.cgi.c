/*
 *
 *����������Ľ�������ȡ�������ļ������pc��
 *KeHuatao
 *2011-05-03
 */
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <urcp_ipc.h> 
#include <uttMachine.h>

#define USB_STORAGE_SIGN    "/media/sd"
static char *decode(char *dest);
static int char2hex(char ch, char *hex);
static int findstr(char* buf, char*str, char *split, char* ret);
static void outPutFile(char* file, char* header);
#if 0
FILE* fd=NULL;
void fdebug(char *str)
{
    fd = fopen("/kk.log", "a");
    if(fd!=NULL)
    {
	fprintf(fd, "%s",str);
	fclose(fd);
    }
}
#endif
/*
*���������ļ�
*KeHuatao
*/
int main(int argc, char *argv[]) 
{
    unsigned int n=0u; 
    char webBuf[1024], fileName[64], inUSB[8];
    char *tmp=NULL, *strLen=NULL;
    char *header = NULL;

    /*������*/
    strLen = getenv("CONTENT_LENGTH");
    if(strLen!=NULL)
    {
	n = (unsigned int)strtoul(strLen, NULL, 10);
	if(n>=(sizeof(webBuf)))
	{
	    n = sizeof(webBuf);
	}
        /*��ʼ��*/
	memset(webBuf, 0, sizeof(webBuf));
	memset(fileName, 0, sizeof(fileName));
	memset(inUSB, 0, sizeof(inUSB));
	/*������*/
	fread((void*)webBuf, 1u, n, stdin);
        findstr(webBuf, "delstr=", "&", fileName);
        findstr(webBuf, "statustr=", "&", inUSB);
	tmp = decode(fileName);
	if(tmp==NULL)
	{
	    return -1;
	}

	/*��ӡhttpͷ*/
	printf("Pragma: no-cache\\n\n");
	printf("Cache-control: no-cache\\n\n");
	printf("Content-type: application/octet-stream\n");
	printf("Content-Transfer-Encoding: binary\n");
	printf("Content-Disposition: attachment; filename=\"%s\"\n", tmp);
	printf("\n");

	memset(webBuf, '\0', sizeof(webBuf));
#ifdef CONFIG_LIB_LIBUSB_USER_FORCE
	if (isUSBStorageExist(webBuf) == 1) {
		/* ��U�̵���� */
		strcat(webBuf, USB_CONFIG_PATH);
		strcat(webBuf, tmp);

		outPutFile(webBuf, header);
	} else 
#endif
	{
        memset(webBuf, 0, sizeof(webBuf));
#if (UTT_WX_2000 == FYES)
		strcat(webBuf, "/ff/");
		strcat(webBuf, USB_CONFIG_PATH);
#else
	    /*�ڴ���*/
	    strcat(webBuf, RAM_CONFIG_PATH);
#endif
	    strcat(webBuf, tmp);
	    outPutFile(webBuf, header);
	}

#if 0
	/*�ļ������λ��*/
	if(strcmp(inUSB, "1")==0) 
	{
	    /*u����*/
	    if(isUSBStorageExist(webBuf)==1)
	    {
		strcat(webBuf, USB_CONFIG_PATH);
		strcat(webBuf, tmp);
	        outPutFile(webBuf, header);
	    }
	}
	else
	{   
	    /*�ڴ���*/
            memset(webBuf, 0, sizeof(webBuf));
	    strcat(webBuf, RAM_CONFIG_PATH);
	    strcat(webBuf, tmp);
	    outPutFile(webBuf, header);
	}
#endif
	/*fflush*/
	fflush(stdout); 

	if(tmp!=NULL)
	{
	    free(tmp);
	}
    }

    return 0;
}
/*
 *�Ի�ȡ�ı�����н���
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
	/*Сд*/
	*hex = ch - 'a' + 10; 
    }  
    else if((ch>='A') && (ch<='Z'))
    {  
	/*��д*/
	*hex = ch - 'A' + 10; 
    }  
    else if((ch>='0') && (ch<='9'))
    {  
	/*����*/
	*hex = ch - '0';
    }  
    else
    {  
	/*����*/
	*hex = 0;
	ret=-1;
    }

    return ret;
}
/*
 *�����ַ���
 *KeHuatao
 */
static int findstr(char* buf, char*str, char *split, char* ret)
{
    char* tmp, *ptr;
    int len, r=0;

    ptr=ret;
    len = strlen(str);
    /*����str*/
    tmp = strstr(buf, str);
    if(tmp!=NULL)
    {
	if((tmp==buf)/*��ʼλ��*/
		|| (*(tmp-1)==split[0])/*�ǿ�ͷ*/
	  )
	{
	    /*�ҵ���*/
	    tmp = tmp + len;
	    while((*tmp != '\0') && (*tmp != '&'))
	    {
		/*����*/
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
 *����ļ�����
 *KeHuatao
 */
static void outPutFile(char* file, char* header)
{
    char tmp[1025];
    FILE*fp;

    /*��*/
    fp = fopen(file, "r");
    if(fp!=NULL)
    {
	/*�ļ�ͷ*/
	if(header!=NULL)
	{
	    printf("%s", header);
	}
	/*�Ƿ����*/
	while(feof(fp)==0)
	{
	    /*��ȡ����*/
	    memset(tmp, 0, sizeof(tmp));
	    fread((void*)tmp, 1u, 1024u, fp);
	    /*���*/
	    printf("%s", tmp);
	}
	fclose(fp);
    }

    return;
}
