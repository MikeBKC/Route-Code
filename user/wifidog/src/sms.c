#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iconv.h"
#include "uttMachine.h"
#include "mib.h"

#if (WIFIDOG_AUTH == FYES)
#if (SMS_ENABLE == 1)
#define OUTLEN 1024
void addr(char address[50])
{
    char b[30]="91";
    char a;
    int i,l;
    int n=0;
    l=strlen(address);
    for(i=1;i<l;i++)        /*去除”+”*/
	address[i-1]=address[i];
    address[i-1]='\0';
   /* printf("%s\n",address);*/
    
    for(i=0;i<l;i++)        /*统计数字数*/
	if(address[i]>='0'&&address[i]<='9')
	    n++;
    /*printf("n=%d\n",n);*/
   /* printf("%s\n",address);*/
    if(n%2==1)              /*若为奇数，则加‘‘F’*/
	address[l-1]='F';
   /* printf("%s\n",address);*/
    for(i=1;i<l;i++)        /*奇数位偶数位交换*/
    {a=address[i];address[i]=address[i-1];address[i-1]=a;i++;}
   /* printf("%s\n",address);*/
    
    
    strcat(b,address);        /*号码前加”91”*/
   /* printf("%s\n",b);*/
   
    l=strlen(b);           /*转16进制并加到号码前面*/
    l=l/2;
    address[0]='0';
    if(l<10) address[1]=48+l;
    else if(l>=10) address[1]=55+l;
    else printf("error1\n");
    address[2]='\0';
    /*printf("address=%s\n",address);*/
    strcat(address,b);
    //printf("address=%s\n",address);
    

}

void phon(char phone[50])
{   

    char a;
    int i,l;
    int n=0;
    l=strlen(phone);
    for(i=1;i<l;i++)        /*去除”+”*/
	phone[i-1]=phone[i];
    phone[i-1]='\0';
   /* printf("%s\n",addr);*/
    
    for(i=0;i<l;i++)        /*统计数字数*/
	if(phone[i]>='0'&&phone[i]<='9')
	    n++;
    /*printf("n=%d\n",n);*/
   /* printf("%s\n",addr);*/
    if(n%2==1)              /*若为奇数，则加‘‘F’*/
	phone[l-1]='F';
   /* printf("%s\n",addr);*/
    for(i=1;i<l;i++)        /*奇数位偶数位交换*/
    {a=phone[i];phone[i]=phone[i-1];phone[i-1]=a;i++;}
   //printf("phone=%s\n",phone);
}


void unicode(char in_utf8[512])
{
    
    char out[OUTLEN];
    int outbuflen = (strlen(in_utf8))*4;
    int len;
    int num;
    char add[]="0XFEFF";
    char *p;
    char msg[512],changed_msg[512];
    printf("strlen(in_utf8) = %d\n",strlen(in_utf8));
    memset(out, 0, sizeof(out));
    //u2u(in_utf8,strlen(in_utf8),out,OUTLEN);
    u2u(in_utf8,strlen(in_utf8),out,outbuflen);
    len=strlen(out);
    printf("len = %d,outbuflen =%d,out =%s\n",len,outbuflen,out);
    int i=0;
    // printf("%02x",add);
    //printf("len=%d,out=%02x\n",len,out);

    smsBytes2String(out,msg,len);
    printf("len = %d, msg =%s\n",len,msg);
    //printf("msg=%s\n",msg);
    for(i=4;i<strlen(msg);i++)
    {
	msg[i-4]=msg[i];
    }
    msg[i-4]='\0';
    //printf("msg=%s\n",msg);
    int  n,j;
    j=0;
    for(i=2;i<strlen(msg);i++)
    {    n=msg[i];
	msg[i]=msg[i-2];
	msg[i-2]=n;
	j++;
	if(j%2==0) i+=2;
    }
	    

    //printf("changed mes=%s\n",msg);

    len=strlen(msg);  /*转16进制并加到号码前面*/
   // printf("len=%d\n",len);
    len=len/2;
    printf("len = %d, msg =%s\n",len,msg);
    //printf("len=%d\n,len");
    changed_msg[0]='0';
    if(len<10) changed_msg[1]=48+len;
    else if(len>=10&&len<16) changed_msg[1]=55+len;
    else printf("error2\n");
    changed_msg[2]='\0';
    /*printf("addr=%s\n",addr);*/
  // printf("%c",msg[1]);
    strcat(changed_msg,msg);
    //printf("changed message=%s\n",changed_msg);
   strcpy(in_utf8,changed_msg);
   len=strlen(changed_msg);
   in_utf8[len]='\0';


}    
    

int u2u(char *inbuf,int inlen,char *outbuf,int outlen)
{

    return code_convert("utf-8","unicode",inbuf,inlen,outbuf,outlen);
}

int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen )
{
    iconv_t cd;
    int rc;
    char **pin=&inbuf;
    char **pout=&outbuf;
    cd=iconv_open(to_charset,from_charset);
   if(cd==0) return -1;
    //memset(outbuf,0,outlen);
    if(iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
   //printf("unicode is %s\n",*pout);
   //iconv(cd,pin,&inlen,pout,&outlen);
   printf("outlen =%d,out =%s\n",outlen,outbuf);

    iconv_close(cd);
    return 0;
}

int smsBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength) 

{ 
    const char tab[]="0123456789ABCDEF";    // 0x0-0xf的字符查找表 
    int i;
    for(i=0;i<nSrcLength;i++) 
    { 
	*pDst++ = tab[*pSrc >> 4];   // 输出高4位 
	*pDst++ = tab[*pSrc & 0x0f];	// 输出低4位 
	pSrc++; 
    } 

    // 输出字符串加个结束符 
    *pDst = '\0'; 
    //
    // // 返回目标字符串长度 
    return (nSrcLength * 2); 
}


#define PHONE_NUM_LEN (14+1)
int string_unicode(char *addre, char *phone, int isphoneNum, char *msg1, char *encode)
{
    /* Modify
     * addre pho msg 修改为已参数的形式传递
     * */
    
    char addreNum[51];
    char phoNum[51];
    char msg[300];
    char con[9];
    char msgcontent1[53] = "0008B0663010827E6CF079D1628030114E0A7F515BC67801FF1A";
    char msgcontent2[145] = "FF0C51ED6B64624B673A53F7548C5BC6780153EF514D8D394F7F75286B64590453CA51765B83005500540054514D8D390057006900460069FF0C5BC678016C384E4567096548FF01";
    char msgUnicode[17];
    int lph=0;
    if((addre!=NULL)&&(phone!=NULL)&&(msg1!=NULL)&&(encode!=NULL)) {
	memset(addreNum, 0, sizeof(addreNum));
	memset(phoNum, 0, sizeof(phoNum));
	memset(msg, 0, sizeof(msg));
	if(strncmp(addre, "+86", 3)==0){
	    snprintf(addreNum, 15, "%s",addre);
	} else {
	    snprintf(addreNum, 15, "+86%s",addre);
	}
	if(strncmp(phone, "+86", 3)==0) {
	    snprintf(phoNum, 15,"%s",phone);
	} else {
	    snprintf(phoNum, 15,"+86%s",phone);
	}
	printf("addreNum =%s, phoNum =%s, msg1 =%s,strlen(msg1)=%d\n",addreNum,phoNum,msg1,strlen(msg1));
    /*短消息中心号码处理*/
    addr(addreNum);
    /*发送号码处理*/
    phon(phoNum);
    /*发送短消息内容转码处理*/
#if 0
    char format[] ="【艾泰科技】上网密码：%s，凭此手机号和密码可免费使用此处及其它UTT免费WiFi，密码永久有效！";
    char note[512];
    snprintf(msg,512,format, msg1);
    unicode(msg);
    
    /* Modify
     * 修改为可选的方式发送到手机为：11000D91；小灵通为：11000D81*/
    char con[512]="11000D91"; 
    strcat(con,phoNum);
    char ad[]="000800";
    strcat(con,ad);
    strcat(con,msg1);
    //strcpy(phone,con);
    lph=strlen(con)/2;	/*程序调用发送内容长度*/
    //strcpy(note,addreNum);
    //strcat(note,con);	/*程序调用发送消息内容*/
    snprintf(encode, 520, "%s%s",addreNum,con);
    printf("changed phone=%s,",con);
    printf("len=%d\n",lph);
    //printf("note=%s\n",note);
    printf("encode=%s\n",encode);
    /* Modify
     * 需要将lph note的值返回给发短信的程序使用*/
#else
    if(isphoneNum == 1) {
	snprintf(con, 9, "%s","11000D91");
    } else {
	snprintf(con, 9, "%s","11000D81");
    }
    memset(msgUnicode, 0, sizeof(msgUnicode));
    snprintf(msgUnicode, 17,"00%x00%x00%x00%x",(unsigned char)msg1[0],(unsigned char)msg1[1],(unsigned char)msg1[2],(unsigned char)msg1[3]);
#if 0
    snprintf(msg,300,"%s%s%s%s%s%s",addreNum,con,phoNum,msgcontent1,msgUnicode,msgcontent2);
    lph=(strlen(msg)-strlen(addreNum))/2;	/*程序调用发送内容长度*/
    printf("lph =%d, message =%s\n",lph,msg);
#else
    snprintf(encode, MAX_SMS_DATA_LEN,"%s%s%s%s%s%s",addreNum,con,phoNum,msgcontent1,msgUnicode,msgcontent2);
    lph=(strlen(encode)-strlen(addreNum))/2;	/*程序调用发送内容长度*/
#endif

    printf("address=%s\n",addreNum);
    printf("phone=%s\n",phoNum);  
    printf("msgUnicode=%s\n",msgUnicode);  
#endif
 
    }
    return lph;

}

#if 0
void main()
{
#if 1
    int ret = 0;
    char encode[300];
    char msg[512]="工作愉快！";
    memset(encode, 0, sizeof(encode));
ret = string_unicode("13800100500", "13007110996", 1, "1405", encode);
    printf("ret = %d, encode =%s\n",ret,encode);
#else
    /* Modify
     * addre pho msg 修改为已参数的形式传递
     * */
    
    char addre[50]="+8613800100500";
    char pho[50]="+8613007110996";
    //char msg[512]="【艾泰科技】上网密码：1405，凭此手机号和密码可免费使用此处及其它UTT免费WiFi，密码永久有效！";
    char msg[512]="工作愉快！";
    char note[512];
    int lph;
    /*短消息中心号码处理*/
    addr(addre);
    /*发送号码处理*/
    phon(pho);
    /*发送短消息内容转码处理*/
    unicode(msg);

    printf("address=%s\n",addre);
    printf("phone=%s\n",pho);  
    printf("message=%s\n",msg);
 
    /* Modify
     * 修改为可选的方式发送到手机为：11000D91；小灵通为：11000D81*/
    char con[100]="11000D91"; 
    strcat(con,pho);
    char ad[]="000800";
    strcat(con,ad);
    strcat(con,msg);
    //strcpy(phone,con);
    lph=strlen(con)/2;	/*程序调用发送内容长度*/
    strcpy(note,addre);
    strcat(note,con);	/*程序调用发送消息内容*/
    printf("changed phone=%s,",con);
    printf("len=%d\n",lph);
    printf("note=%s\n",note);
    /* Modify
     * 需要将lph note的值返回给发短信的程序使用*/

#endif
}
#endif

#endif
#endif
