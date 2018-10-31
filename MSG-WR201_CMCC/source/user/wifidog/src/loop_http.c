#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "common.h"
#include "conf.h"
#include "debug.h"
#include "http.h"

#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"
#include "loop_http.h"

char *getValueFromUrl(char *str, char *key, char *value, int maxValueLen)
{
    int i = 0, j = 0, ret = 0;
    int dbg = 0;

    while (str[i])
    {
	if (memcmp(&str[i], key, strlen(key)) == 0)
	{
	    if (i>0 && str[i-1]!='&' && str[i-1] != '?')
	    {
		i++;
		debug(LOG_ERR, "it's not the real: str[%d]=[%c]", i>0?i-1:0, i>0?str[i-1]:str[i-1]);
		continue;
	    }

	    i += strlen(key);
	    if (str[i] == '=')
	    {
		i++;
		while (str[i] && str[i] != '&' && j < maxValueLen)
		{
		    value[j++] = str[i++];
		}
		break;
	    }
	}
	i++;
    }

    value[j] = '\0';
    return value;
}

/*获取字符串中参数的值，形如
 * aaa=123 或 aaa = 123 或 "aaa"="123" 或 'aaa'='123'
 * aaa:123 或 aaa : 123 或 "aaa":"123" 或 'aaa':'123' 等*/
char *getValueFromString(char *str, char *key, char *value, int maxValueLen)
{
    int i = 0, j = 0, ret = 0;
    int dbg = 0;

    while (str[i])
    {
	if (memcmp(&str[i], key, strlen(key)) == 0)
	{
	    if (i>0 && str[i-1]!='\"' && str[i-1]!=',' && str[i-1]!='{' && str[i-1] != '?' && str[i-1] != '\n')
	    {
		i++;
		//debug(LOG_ERR, "it's not the real: str[%d]=[%c]", i>0?i-1:0, i>0?str[i-1]:str[i-1]);
		continue;
	    }

	    i += strlen(key);
	    if (str[i] == '\'' || str[i] == '\"')
		i++;
	    while (str[i] == ' ')
		i++;
	    if (str[i] == '=' || str[i] == ':')
	    {
		i++;
		while (str[i] == ' ')
		    i++;
		if (str[i] == '\'' || str[i] == '\"')
		    i++;
		while (str[i] && str[i] != '\n' && str[i] != '\'' 
			&& str[i] != '\"' && str[i] != '}' && str[i] != ',' && j < maxValueLen)
		    value[j++] = str[i++];
		break;
	    }
	}
	i++;
    }

    value[j] = '\0';
    return value;
}

static int loop_send(char *host, char *sendMsg, char *recvMsg)
{
    int i, sockfd, nfds, done;
    size_t totalbytes;
    ssize_t	numbytes;
    fd_set readfds;
    struct	timespec timeout;
    char recvData[MAX_BUF];
    int recvDataLen = 0;
    struct sockaddr_in sin_addr_in;
    struct in_addr *h_addr;
    
    h_addr = wd_gethostbyname(host);
    if (h_addr == NULL)
    {
	free (h_addr);
	return 0;
    }
    //printf("sendMsg : %s\n", sendMsg);
#if 1
    sin_addr_in.sin_family = AF_INET;
    sin_addr_in.sin_port = htons(80);
    sin_addr_in.sin_addr = *h_addr;
    memset(&(sin_addr_in.sin_zero), '\0', sizeof(sin_addr_in.sin_zero));
    free (h_addr);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	debug(LOG_ERR, "Failed to create a new SOCK_STREAM\n");
	return 0;
    }
    if (connect(sockfd, (struct sockaddr *)&sin_addr_in, sizeof(struct sockaddr)) == -1)
    {
	debug(LOG_ERR, "Failed to connect to server\n");   
	return 0;
    }
#endif


    //debug(LOG_ERR, "send:\n%s", sendMsg);
    send(sockfd, sendMsg, strlen(sendMsg), 0);
    numbytes = totalbytes = 0;
    done = 0;
    do {
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	timeout.tv_sec = 10; /* XXX magic... 30 second */
	//timeout.tv_usec = 0;
	nfds = sockfd + 1;

	nfds = select(nfds, &readfds, NULL, NULL, &timeout);

	if (nfds > 0) {
	    /** We don't have to use FD_ISSET() because there  was only one fd. */
	    numbytes = read(sockfd, recvMsg + totalbytes, MAX_BUF - (totalbytes + 1));
	    if (numbytes < 0) {
		debug(LOG_ERR, "An error occurred while reading from auth server: %s", strerror(errno));
		close(sockfd);
		return -1;
	    }
	    else if (numbytes == 0) {
		done = 1;
	    }
	    else {
		totalbytes += numbytes;
		//debug(LOG_DEBUG, "Read %d bytes, total now %d", numbytes, totalbytes);
	    }
	}
	else if (nfds == 0) {
	    debug(LOG_ERR, "Timed out reading data via select() from auth server");
	    close(sockfd);
	    return -1;
	}
	else if (nfds < 0) {
	    //debug(LOG_ERR, "Error reading data via select() from auth server: %s", strerror(errno));
	    close(sockfd);
	    return -1;
	}
    } while (!done);
    close(sockfd);
    recvMsg[totalbytes] = '\0';
    //debug(LOG_ERR, "Done reading reply, total %d bytes", totalbytes);
    //debug(LOG_ERR, "HTTP Response from Server: [%s]", recvMsg);

#if 1
    for (i=0; i<totalbytes; i++)
    {
	if (recvMsg[i] == '\r' && recvMsg[i+1] == '\n' &&
		recvMsg[i+2] == '\r' && recvMsg[i+3] == '\n')
	{
	    i += 4;
	    strcpy(recvData, &recvMsg[i]);
	    strcpy(recvMsg, recvData);
//	    debug(LOG_ERR, "recvData[%d]:\n%s", recvDataLen, recvData);
	    break;
	}
    }

    return strlen(recvMsg);
#else
    return totalbytes;
#endif
}

int loop_http_get(char *host, char *url, char *recvMsg)
{

    char sendMsg[MAX_BUF];
    t_auth_serv	*auth_server = NULL;
    auth_server = get_auth_server();

    snprintf(sendMsg, sizeof(sendMsg) - 1,
	    "GET /%s HTTP/1.1\r\n"
	    "User-Agent: utt WiFi Auth %s\r\n"
	    "Host: %s\r\n"
	    "Connection: Close\r\n\r\n", 
	    url,
	    "20090925",
	    host);

    return loop_send(host, sendMsg, recvMsg);
}
int loop_http_post(char *host, char *url, char *parameter, char *recvMsg)
{

    char sendMsg[MAX_BUF];
    t_auth_serv	*auth_server = NULL;
    auth_server = get_auth_server();
    int len = strlen(parameter);
    snprintf(sendMsg, sizeof(sendMsg) - 1,
	    "POST /%s HTTP/1.1\r\n"
	    "User-Agent: utt WiFi Auth %s\r\n"
	    "Host: %s\r\n"
	    "Accept: */*\r\n"
	    "Content-Length: %d\r\n"
	    "Content-Type: application/x-www-form-urlencoded\r\n"
	    "Connection: Close\r\n\r\n"
	    "%s", 
	    url,
	    "20090925",
	    host,
	    len,
	    parameter);

    return loop_send(host, sendMsg, recvMsg);
}
int loop_http_get_agent(char *host, char *agent, char *url, char *recvMsg)
{

    char sendMsg[MAX_BUF];
    t_auth_serv	*auth_server = NULL;
    auth_server = get_auth_server();

    snprintf(sendMsg, sizeof(sendMsg) - 1,
	    "GET /%s HTTP/1.1\r\n"
	    "User-Agent: %s\r\n"
	    "Host: %s\r\n"
	    "Connection: Close\r\n\r\n", 
	    url,
	    agent,
	    host);

    return loop_send(host, sendMsg, recvMsg);
}
