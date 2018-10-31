#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#define UTT_HOST_DOMAIN "ddns.utt.com.cn"
#define REMOTESEVER_PORT 80
#define MAX_BUF_SIZE 1024

void desDecrypt(char *inputStr,char *deStr,char *key);
void desEncrypt(char *inputStr,char *outputStr,char *key);

int account_auth(unsigned int sn, char *hostStr, char *keyStr)
{
    int    sock_fd = 0;
    char   recv[MAX_BUF_SIZE + 1];
    struct sockaddr_in   server_addr; 
    int    nbytes = 0;
    char   request[1024] = ""; 
    int    opt = SO_REUSEADDR; 
    int    timeOut = 10;
    char   url[MAX_BUF_SIZE]; 
    int    i,j,num;
    char   value[100]; 
    char   deStr[500]={0},enStr[2000]={0};
    struct hostent *host = NULL;
    char urlencodestr[400];
    char *p;
    unsigned int timeStamp = time(0);
    char snStr[11];

    printf("%s %d...\n", __func__, __LINE__);
    i=0;
    while(1){
	host = gethostbyname(UTT_HOST_DOMAIN);
	if (host == NULL) {
	    printf("host[NULL],wait 1 sec\n");
	    sleep(1);
	    i++;
	    if(i<3)
		continue;
	    else
		return 1; /* 访问不了服务器，作请求成功处理 */
	}    
	break;
    }
    printf("%s %d...\n", __func__, __LINE__);
    memset(url, 0, sizeof(url));
    sprintf(url, "time=%u&sn=%u&host=%s&key=%s",
	    timeStamp, sn, hostStr, keyStr); 
    desEncrypt(url, deStr, "tz7y1p0j");
    sprintf(url, "checkDDNS.php?param=%s", deStr); 

    printf("SEND: %s\n", url);
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeOut, sizeof(timeOut)); 
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeOut, sizeof(timeOut));
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if((sock_fd=socket(AF_INET,SOCK_STREAM,0)) == -1)
    {   
	printf("Socket   Error:%s\a\n ",strerror(errno));
	close(sock_fd);
	return 1;
    } 
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(REMOTESEVER_PORT);
    server_addr.sin_addr=*((struct in_addr *)(host->h_addr));

    if(connect(sock_fd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
    {   
	printf("Connect   Error:%s\a\n ",strerror(errno)); 
	close(sock_fd);
	return 1;                                                                           
    }                                                                                      
    sprintf(request, "GET /%s HTTP/1.1\r\n"
	    "Accept: */*\r\n"
	    "Accept-Language: zh_cn\r\n"
	    "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
	    "Accept-Encoding: gzip,deflate\r\n"
	    "Host: %s\r\n"
	    //"Connection: Keep-Alive\r\n\r\n", 
	    "Connection: Close\r\n\r\n",               
	    url, UTT_HOST_DOMAIN);
    nbytes=strlen(request);
    write(sock_fd, request, nbytes);

    nbytes = read(sock_fd, recv, MAX_BUF_SIZE);
    recv[nbytes] = 0;

    i = 0;
    while (i < nbytes)
    {
        if (recv[i] == '\r' && recv[i+1] == '\n'
                && recv[i+2] == '\r' && recv[i+3] == '\n')
        {
            i += 4;
	    printf("RECV: %s\n", &recv[i]);
	    sprintf(snStr, "%u", sn);
	    printf("desKey=%s\n",snStr);
	    desDecrypt(&recv[i],deStr,snStr);	
	    printf("deStr=%s\n",deStr);
	    p = strstr(deStr, "time:");
	    if (p && atoi(p+strlen("time:")) == timeStamp)
	    {
		printf("%s %d...\n", __func__, __LINE__);
		p = strstr(deStr, "status:");
		if (p && p[strlen("status:")] == '1')
		{
		    printf("%s %d...\n", __func__, __LINE__);
		    return 1;
		}
		else
		{
		    printf("%s %d...\n", __func__, __LINE__);
		    return 0;
		}
	    }
        }

        i++;
    }
    printf("%s %d...\n", __func__, __LINE__);
    close(sock_fd);
    return 1;
}

