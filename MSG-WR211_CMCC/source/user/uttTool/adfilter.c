
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

/*构造回复数据包*/
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
* 函数： reConstructGetPacket
* 功能： 根据请求数据包类型，对数据包进行重新构造
* 创建： 2014-07-23
* 参数： oldBuf->可能的广告请求包;newBuf->重新构造的数据包;hostIp->远端服务器IP(在此函数中进行赋值)
* 返回： 0表示成功；1表示失败。
* 输出： 略
* 作者： 赵冲
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
	    if (strncmp(oldUri,adfilterProfile->originalUri[index],strlen(adfilterProfile->originalUri[index])) == 0) {/*完全匹配，确定为广告请求包*/
		/*根据预定义的规则，如果需向127.0.0.1请求数据，则直接返回，
		 由接下来的其他函数构造空数据包回复PC即可*/
		if (strcmp(adfilterProfile->host[index],"127.0.0.1") == 0) {
		    strcpy(hostIp,adfilterProfile->host[index]);
		    return 0;
		}
		flag = 1U;/*完全匹配的标志为1*/
		if (getIpByDns(adfilterProfile->host[index], ipStr) == 0) {/*获取IP地址失败*/
		    strcpy(hostIp,adfilterProfile->normalHostIp[index]);/*过滤规则失效，请求原广告策略，不会影响正常观看*/
		} else {
		    strcpy(hostIp,ipStr);
		}
		break;
	    } else {
		flag = 2U;/*不完全匹配，则不用重新构造包*/
		if (getIpByDns(adfilterProfile->originalHost[index], ipStr) == 0) {/*获取IP地址失败*/
		    strcpy(hostIp,adfilterProfile->normalHostIp[index]);
		} else {
		    strcpy(hostIp,ipStr);
		}
		break;
	    }
	}
    }

    if (flag == 2U) {/*非广告请求包，直接返回，仍请求原始包*/
	return 1;
    }

    for(parStartIndex = 4U, parEndIndex = 4U; parEndIndex < BUFSIZ ; parEndIndex++) {
	if (*(oldBuf+parEndIndex) == '?') {
	    parStartIndex = parEndIndex;
	} else if (*(oldBuf+parEndIndex) == (char)0x20) {/*遇到空格，url参数结束*/
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
    tmpData = httpP + 11;/*指向oldBuf的第二行*/
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
* 函数： dataTrans
* 功能： 广告过滤代理进行数据包的中转,对于不需要中转的数据包则构造空内容包回复PC
* 创建： 2014-07-23
* 参数： pcSocket->代理跟PC间建立的连接;getdata->需要中转的数据包;hostIp->远端服务器IP
* 返回： 0表示成功；1表示失败。
* 输出： 略
* 作者： 赵冲
********************************************************************/
int dataTrans(int pcSocket, char *getdata,char *hostIp)
{
    struct sockaddr_in remote_addr; //服务器端网络地址结构体  
    char buf[BUFSIZ + 1] = {0};  //数据传送的缓冲区  
    unsigned int flag = 0U;
    int client_sockfd;
    int dataLen, len, send_bytes = 0, recv_bytes = 0;
    int contentLen,httpHeadLen;

    if (strcmp("127.0.0.1",hostIp) == 0) {/*手动构造回复包*/
	if (constructReplyPacket(buf) == -1) {
	    DEBUG_PRINTF("constructReplyPacket error!\n");
	    return 1;
	}
	dataLen = strlen(buf);
	sendMsgTo(pcSocket, buf, dataLen, RETRY_TIMES, NULL, 0);
	return 0;
    }
    dataLen = strlen(getdata);

    memset(&remote_addr,0,sizeof(remote_addr)); //数据初始化--清零  
    remote_addr.sin_family=AF_INET; //设置为IP通信  
    remote_addr.sin_addr.s_addr=inet_addr(hostIp);//服务器IP地址  
    remote_addr.sin_port=htons(80); //服务器端口号 
    
    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/  
    if((client_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)  {  
	perror("socket");  
	return 1;  
    }  
    /*将套接字绑定到服务器的网络地址上*/  
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

    contentLen = getContentLen(buf);/*一个完整的接收过程中，需要接收的HTTP的内容部分总长度*/
    httpHeadLen = getHttpHeadLen(buf);/*HTTP数据包的HTTP头长度*/
    if( (contentLen < 0)  || (httpHeadLen < 0))
    {	
	return 1;
    }
    if (sendMsgTo(pcSocket, buf, len, RETRY_TIMES, NULL, 0) <= 0) {/*将第一次接收的数据包发出去*/
	close(client_sockfd);
	DEBUG_PRINTF("func:%s,line:%d,sendMsg error\n",__func__,__LINE__);
	return 1;
    }

    contentLen -= (len - httpHeadLen);/*剩下还需要接收的content部分长度*/

    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/  
    while(contentLen > 0)
    {  
	if ((len = recvMsg(client_sockfd,buf,BUFSIZ,RETRY_TIMES)) > 0) {
	    contentLen -= len;
	    sendMsgTo(pcSocket, buf, len, RETRY_TIMES, NULL, 0);
	}
	else
	{/*假如发生错误需跳出，否者可能死循环*/
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
    proxy_serv_addr.sin_family=AF_INET; //设置为IP通信  
    proxy_serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//代理服务器IP地址  
    proxy_serv_addr.sin_port=htons(PROXY_PORT); //代理服务器端口号 
    
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
    /*共享内存初始化*/
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
