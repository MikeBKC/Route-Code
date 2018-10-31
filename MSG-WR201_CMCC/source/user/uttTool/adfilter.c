
#include <stdio.h>  
#include <sys/types.h>  
#include <errno.h> 
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <string.h>  
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <string.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <linux/if_ether.h>

#include <arpa/inet.h>
#include <net/if_arp.h>
//#include <linux/if_arp.h>

#include <linux/netlink.h>
#include <linux/netfilter/xt_HTTPPROXY.h>
#include <getopt.h>   
  
#include <netinet/ip.h>   
#include <netinet/tcp.h>  
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"
#include "ifmisc.h"

/*enddst:end poninter + 1 (eg.enddst = dst + dstLength)*/
#define memcpy_endck(dst,src,cplen,enddst)\
    memcpy(dst, src, ( ( (dst + cplen ) <= enddst) ? cplen:( ((dst) <(enddst)) ?( (enddst ) - (dst)):0 ) ) )

#if 0
#define DEBUG_PRINTF(fmt, args...)              printf(fmt, ## args)
#else
#define DEBUG_PRINTF(fmt, args...)              
#endif

#define AD_DEBUG 0
#define RETRY_TIMES 5

#define WEB_NAME "GoAhead-Webs"

static int getContentLen(char *data)
{
    char *clen = NULL;
    char buflen[10] = {0};
    int  i = 0, len = -1;
    clen = strstr(data, "Content-Length");
    if(clen != NULL) {
	clen += 16;
	for(; *clen != 0x0d && i<10; clen++) {
	    buflen[i] = *clen;
	    i++;
	}
	if(i < 10)
	{
	    len = strtol(buflen, NULL, 10);
	}
    }

    DEBUG_PRINTF("%s :%d",buflen,len);
    return len;
}

static int getHttpHeadLen(char *data)
{
    char* dataHead = NULL;
    char  endStr[] = {0x0d,0x0a,0x0d,0x0a,0x0};
    dataHead = strstr(data, endStr);
    if (dataHead == NULL) {
	return -1;
    }
    dataHead += 4;
    
    return dataHead - data;
}
  
int getIpByDns(char *domainName, char *ipStr)
{
    struct hostent *host;
    int success = 0;

    host = gethostbyname(domainName);
    DEBUG_PRINTF("domainName = %s\n",domainName);
    if (host != NULL) {
	DEBUG_PRINTF("hostName = %s\n",host->h_name);	
	if ((char *)inet_ntop(host->h_addrtype,host->h_addr,ipStr,16) != NULL) {
	    success = 1;
	}
    }

    return success;
}

ssize_t recvMsg(int sockfd, void *buf, size_t len, unsigned int retry)
{
    ssize_t bytesRecv = 0;
    int i = 0;

    while (retry--) {
	if ((bytesRecv = recv(sockfd,buf,len,MSG_DONTWAIT)) == -1) {
	    sleep(1U);
	} else {
	    break;
	}
    }
    return bytesRecv;
}

ssize_t sendMsgTo(int sockfd, const void *buf, size_t len, unsigned int retry, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t bytesSend = 0;
    int i = 0;

    while (retry--) {
	if ((bytesSend = sendto(sockfd,buf,len,MSG_DONTWAIT,dest_addr,addrlen)) == -1) {
	    sleep(1U);
	} else {
	    break;
	}
    }
    return bytesSend;
}

int getDateString(char *dateBuf)
{
    time_t  now,ret;
    char dateStr[26] = {0};

    if ((ret = time(&now)) == (time_t)-1){
	DEBUG_PRINTF("func:%s,line:%d,time error\n",__func__,__LINE__);
	return -1;
    }
    
    if (ctime_r(&now, dateStr) == NULL) {
	DEBUG_PRINTF("func:%s,line:%d,ctime_r error\n",__func__,__LINE__);
	return -1;
    }
    dateStr[strlen(dateStr)-1] = '\0';
    strcpy(dateBuf,dateStr);

    return 0;
}

int stringBuffer(char *buffer, const char *fmt, ...)
{
    char *tmpBuf = NULL;
    va_list vargs;

    tmpBuf = buffer + strlen(buffer);
    va_start(vargs, fmt);
    vsprintf(tmpBuf, fmt, vargs);
    va_end(vargs);

    return 0;
}

/*����ظ����ݰ�*/
int constructReplyPacket(char *buf)
{
    char dateBuf[25] = {0};

    if (-1 == getDateString(dateBuf)) {
	DEBUG_PRINTF("getDateString error\n");
	return -1;
    }
    stringBuffer(buf,"HTTP/1.0 200 OK\r\nDate: %s\r\n",dateBuf);
    stringBuffer(buf,"Server: %s\r\n",WEB_NAME);
    stringBuffer(buf,"Cache-Control: private\r\n");
    stringBuffer(buf,"Cache-Control: max-age\r\n");
    stringBuffer(buf,"Last-modified: %s\r\n",dateBuf);
    stringBuffer(buf,"Content-Length: 12\r\n");
    stringBuffer(buf,"Content-type: text/html\r\n\r\n");
    stringBuffer(buf,"no content\r\n");
    
    return 0;
}

/********************************************************************
* ������ reConstructGetPacket
* ���ܣ� �����������ݰ����ͣ������ݰ��������¹���
* ������ 2014-07-23
* ������ oldBuf->���ܵĹ�������;newBuf->���¹�������ݰ�;hostIp->Զ�˷�����IP(�ڴ˺����н��и�ֵ)
* ���أ� 0��ʾ�ɹ���1��ʾʧ�ܡ�
* ����� ��
* ���ߣ� �Գ�
********************************************************************/
static int reConstructGetPacket(unsigned char *oldBuf,unsigned char *newBuf, char *hostIp)
{
    AdfilterProfile *adfilterProfile;
    int index;
    unsigned char *tmpData,*oldUri,*oldHost, *newBufEnd;
    unsigned int uriLen,hostLen;
    unsigned int parStartIndex, parEndIndex,urlParLen;
    unsigned char *httpP = NULL;
    int i, dataLen;
    unsigned char *tmpP = NULL;
    unsigned char *endFlag = NULL;
    int strLen;
    unsigned int flag = 0U;
    char ipStr[16];

    memset(ipStr, 0, sizeof(ipStr));
    newBufEnd = newBuf + BUFSIZ;
    
    if ((strncmp(oldBuf,"GET",3) != 0)) {
	return 1;
    }
    endFlag = strrchr(oldBuf,'\n');
    dataLen = endFlag - oldBuf + 1;

    if ((httpP = strstr(oldBuf+4," HTTP")) == NULL) {
	return 1;
    }
    oldUri = oldBuf + 5;//GET /xxxx?aaaa
    if ((tmpP = strstr(oldBuf,"Host")) == NULL) {
	return 1;
    }
    oldHost = tmpP + 6;
    adfilterProfile = (AdfilterProfile *)ProfGetInstPointByIndex(MIB_PROF_ADFILTER, 0);
    if (ProfInstIsFree(adfilterProfile) == 1) {
	return 1;
    }
    for (index = 0; index < MAX_ADFILTER_ITEMS; index++) {
	if (strncmp(oldHost,adfilterProfile->originalHost[index],strlen(adfilterProfile->originalHost[index])) == 0) {
	    if (strncmp(oldUri,adfilterProfile->originalUri[index],strlen(adfilterProfile->originalUri[index])) == 0) {/*��ȫƥ�䣬ȷ��Ϊ��������*/
		/*����Ԥ����Ĺ����������127.0.0.1�������ݣ���ֱ�ӷ��أ�
		 �ɽ�����������������������ݰ��ظ�PC����*/
		if (strcmp(adfilterProfile->host[index],"127.0.0.1") == 0) {
		    strcpy(hostIp,adfilterProfile->host[index]);
		    return 0;
		}
		flag = 1U;/*��ȫƥ��ı�־Ϊ1*/
		if (getIpByDns(adfilterProfile->host[index], ipStr) == 0) {/*��ȡIP��ַʧ��*/
		    strcpy(hostIp,adfilterProfile->normalHostIp[index]);/*���˹���ʧЧ������ԭ�����ԣ�����Ӱ�������ۿ�*/
		} else {
		    strcpy(hostIp,ipStr);
		}
		break;
	    } else {
		flag = 2U;/*����ȫƥ�䣬�������¹����*/
		if (getIpByDns(adfilterProfile->originalHost[index], ipStr) == 0) {/*��ȡIP��ַʧ��*/
		    strcpy(hostIp,adfilterProfile->normalHostIp[index]);
		} else {
		    strcpy(hostIp,ipStr);
		}
		break;
	    }
	}
    }

    if (flag == 2U) {/*�ǹ���������ֱ�ӷ��أ�������ԭʼ��*/
	return 1;
    }

    for(parStartIndex = 4U, parEndIndex = 4U; parEndIndex < BUFSIZ ; parEndIndex++) {
	if (*(oldBuf+parEndIndex) == '?') {
	    parStartIndex = parEndIndex;
	} else if (*(oldBuf+parEndIndex) == (char)0x20) {/*�����ո�url��������*/
	    break;
	}
    }
    urlParLen = parEndIndex - parStartIndex;
    memcpy_endck(newBuf,oldBuf,5, newBufEnd);
    if (flag == 1U) {
	uriLen = strlen(adfilterProfile->uri[index]);
	memcpy_endck(newBuf + 5,adfilterProfile->uri[index],uriLen, newBufEnd);
#if 0
    } else if (flag == 2U) {
	if (parStartIndex == 4U) {
	    uriLen = urlParLen - 1;
	} else {
	    uriLen = parStartIndex -  5;
	}
	memcpy(newBuf + 5,oldUri,uriLen);
#endif
    }
    hostLen = strlen(adfilterProfile->host[index]);

    if (parStartIndex != 4U) {
	memcpy_endck(newBuf + 5 + uriLen,oldBuf + parStartIndex,urlParLen, newBufEnd); 
	uriLen += urlParLen; 
    } 

    memcpy_endck(newBuf + 5 + uriLen,httpP,11, newBufEnd);/*HTTP/1.x\r\n*/
    tmpData = httpP + 11;/*ָ��oldBuf�ĵڶ���*/
    i = 5 + uriLen + 11;

    while (tmpData - oldBuf < dataLen) {
	if (memcmp(tmpData,"Host",4) == 0) {
	    memcpy_endck(newBuf + i, "Host: ",6, newBufEnd);
	    i += 6;
	    memcpy_endck(newBuf +i, adfilterProfile->host[index], hostLen, newBufEnd);
	    i += hostLen;
	    memcpy_endck(newBuf +i, "\r\n",2, newBufEnd);
	    i += 2;
	} else if (memcmp(tmpData,"Connection",10) == 0) {
	    memcpy_endck(newBuf + i, "Connection: ",12, newBufEnd);
	    i += 12;
	    memcpy_endck(newBuf + i, "close\r\n",7, newBufEnd);
	    i += 7;
	} else {
	    tmpP = (unsigned char *)strchr(tmpData,0x0a);
	    if (tmpP == NULL) {
		return 1;
	    }
	    strLen = tmpP - tmpData + 1;
	    memcpy_endck(newBuf +i, tmpData, strLen, newBufEnd);
	    i += strLen;
	}
	if ((tmpP = (unsigned char *)strchr(tmpData,0x0a)) == NULL) {
		return 1;
	}
	tmpData = tmpP + 1;
    }

    return 0;
}

/********************************************************************
* ������ dataTrans
* ���ܣ� �����˴���������ݰ�����ת,���ڲ���Ҫ��ת�����ݰ���������ݰ��ظ�PC
* ������ 2014-07-23
* ������ pcSocket->�����PC�佨��������;getdata->��Ҫ��ת�����ݰ�;hostIp->Զ�˷�����IP
* ���أ� 0��ʾ�ɹ���1��ʾʧ�ܡ�
* ����� ��
* ���ߣ� �Գ�
********************************************************************/
int dataTrans(int pcSocket, char *getdata,char *hostIp)
{
    struct sockaddr_in remote_addr; //�������������ַ�ṹ��  
    char buf[BUFSIZ + 1] = {0};  //���ݴ��͵Ļ�����  
    unsigned int flag = 0U;
    int client_sockfd;
    int dataLen, len, send_bytes = 0, recv_bytes = 0;
    int contentLen,httpHeadLen;

    if (strcmp("127.0.0.1",hostIp) == 0) {/*�ֶ�����ظ���*/
	if (constructReplyPacket(buf) == -1) {
	    DEBUG_PRINTF("constructReplyPacket error!\n");
	    return 1;
	}
	dataLen = strlen(buf);
	sendMsgTo(pcSocket, buf, dataLen, RETRY_TIMES, NULL, 0);
	return 0;
    }
    dataLen = strlen(getdata);

    memset(&remote_addr,0,sizeof(remote_addr)); //���ݳ�ʼ��--����  
    remote_addr.sin_family=AF_INET; //����ΪIPͨ��  
    remote_addr.sin_addr.s_addr=inet_addr(hostIp);//������IP��ַ  
    remote_addr.sin_port=htons(80); //�������˿ں� 
    
    /*�����ͻ����׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/  
    if((client_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)  {  
	perror("socket");  
	return 1;  
    }  
    /*���׽��ְ󶨵��������������ַ��*/  
    if(connect(client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
	perror("connect");  
	close(client_sockfd);
	return 1;
    }  

#if AD_DEBUG
    len=send(client_sockfd, getdata, dataLen, 0); 
#else
    while ((len =  sendMsgTo(client_sockfd, getdata + send_bytes, dataLen - send_bytes, RETRY_TIMES, NULL, 0)) > 0) {
	send_bytes += len;
	if (send_bytes >= dataLen) {
	    break;
	}
    }
    if (len <= 0) {
	close(client_sockfd);
	return 1;
    }
#endif

    len = recvMsg(client_sockfd,buf,BUFSIZ,RETRY_TIMES);
    if (len <= 0) {
	close(client_sockfd);
	DEBUG_PRINTF("func:%s,line:%d,recvMsg error\n",__func__,__LINE__);
	return 1;
    }

    contentLen = getContentLen(buf);/*һ�������Ľ��չ����У���Ҫ���յ�HTTP�����ݲ����ܳ���*/
    httpHeadLen = getHttpHeadLen(buf);/*HTTP���ݰ���HTTPͷ����*/
    if( (contentLen < 0)  || (httpHeadLen < 0))
    {	
	return 1;
    }
    if (sendMsgTo(pcSocket, buf, len, RETRY_TIMES, NULL, 0) <= 0) {/*����һ�ν��յ����ݰ�����ȥ*/
	close(client_sockfd);
	DEBUG_PRINTF("func:%s,line:%d,sendMsg error\n",__func__,__LINE__);
	return 1;
    }

    contentLen -= (len - httpHeadLen);/*ʣ�»���Ҫ���յ�content���ֳ���*/

    /*ѭ���ķ��ͽ�����Ϣ����ӡ������Ϣ--recv���ؽ��յ����ֽ�����send���ط��͵��ֽ���*/  
    while(contentLen > 0)
    {  
	if ((len = recvMsg(client_sockfd,buf,BUFSIZ,RETRY_TIMES)) > 0) {
	    contentLen -= len;
	    sendMsgTo(pcSocket, buf, len, RETRY_TIMES, NULL, 0);
	}
	else
	{/*���緢�����������������߿�����ѭ��*/
	    break;
	}   
    }

    close(client_sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    int sfd,connfd;
    struct sockaddr_in proxy_serv_addr,cli_addr;
    struct in_addr in_proxy_ip;
    socklen_t sock_len;
    unsigned char get_buffer[BUFSIZ + 1];
    unsigned char newBuf[BUFSIZ + 1];
    int recv_len = 0,recv_bytes = 0;
    char hostIp[16];
    unsigned int recv_over = 0U;

    memset(&proxy_serv_addr,0,sizeof(proxy_serv_addr)); 
    proxy_serv_addr.sin_family=AF_INET; //����ΪIPͨ��  
    proxy_serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//���������IP��ַ  
    proxy_serv_addr.sin_port=htons(PROXY_PORT); //����������˿ں� 
    
    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("socket open failed");
	return 1;
    } 
    if (bind(sfd, (struct sockaddr *)&proxy_serv_addr, sizeof(proxy_serv_addr)) == -1) {
	perror("socket bind failed");
	return 1;
    }
    if (listen(sfd,SOMAXCONN) == -1) {
	perror("socket listen failed");
	return 1;
    }
    /*�����ڴ��ʼ��*/
    if(ProfInit() == PROFFAIL) 
    {
	return -1;
    }

    while(1)
    {
	sock_len = sizeof(cli_addr);
	if ((connfd = accept(sfd,(struct sockaddr *)&cli_addr,&sock_len)) == -1) {
	    perror("socket accept failed");
	    continue;
	}
	memset(get_buffer,0,sizeof(get_buffer));
	memset(newBuf,0,sizeof(newBuf));
	recv_len = recv_bytes = 0;
	while ((recv_len = recvMsg(connfd,get_buffer + recv_bytes, BUFSIZ - recv_bytes,RETRY_TIMES)) > 0) {  
	    recv_bytes += recv_len;
	    if ((strncmp(get_buffer+recv_bytes-4,"\r\n\r\n",4)) == 0) {
		recv_over = 1U;
		break;
	    }
	}
	DEBUG_PRINTF("recv_over = %d,recv_len = %d\n get_buffer=%s",recv_over,recv_len,get_buffer);
	if (recv_over == 1U) {
	    if (recv_bytes > 0) {
		memset(hostIp,0,sizeof(hostIp));
		if (reConstructGetPacket(get_buffer,newBuf,hostIp) == 0) {
		    dataTrans(connfd,newBuf,hostIp);
		} else {
		    dataTrans(connfd,get_buffer,hostIp);
		}
	    }
	}
	close(connfd);
    }

    return 0;
}
