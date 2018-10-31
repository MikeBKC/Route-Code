#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#if (WEBAUTH_AUTO == FYES)
#define  SERVER_PORT 54321
#define  SERVERIP   "116.236.120.162"
//#define  SERVERIP   "200.200.202.67"

#define  SECRET_LEN   50
#define  TELNUM_LEN   11
#define  CONTENT_LEN  140

struct SMS_MSG{
    char secret[SECRET_LEN + 6];
    char telnum[TELNUM_LEN + 1];
    char content[CONTENT_LEN + 2];
    int  productSn;
};
int ConnectNonb(int sockfd, struct sockaddr *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    errno = 0;

    if ((n = connect(sockfd, saptr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            return EXIT_FAILURE;
        }
    }
    /*可以做任何其它的操作*/
    if (n == 0)
    {
        goto done; // 一般是同一台主机调用，会返回 0
    }
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset; // 这里会做 block copy
    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    /*如果nsec 为0，将使用缺省的超时时间，即其结构指针为 NULL*/
    /*如果tval结构中的时间为0，表示不做任何等待，立刻返回*/
    if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0)
    {
        close(sockfd);
        errno = 10;
        return EXIT_FAILURE;
    }
    if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
    {
        len = sizeof(error);/* 如果连接成功，此调用返回 0*/
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR,&error,&len))
                return EXIT_FAILURE;
    }else{
        printf("select error: sockfd not set");
    }

done:
         fcntl(sockfd, F_SETFL, flags); // 恢复socket 属性
    if (error)
    {
    close(sockfd);
    errno = error;
    return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int getServerIP(char* ip)
{
    struct hostent *h;
    char   *file = "/etc/smsserver";
    FILE   *pp;
    char   addr[65] = {0};
    int    ret = 0;
    FILE   *fp = fopen(file, "r");

    if(fp == NULL){
	printf("cannot read file %s\n", file);
	return 0;
    }

    fscanf(fp, "%s", addr);
    fclose(fp);

    printf("get smsserver: %s\n", addr);
    h = gethostbyname(addr);
    if (h != NULL)
	strcpy(ip, inet_ntoa(*(struct in_addr *)h->h_addr));
    else
	return 0;

    return 1;
}
void pack(char *src,int len)
{
    int i = 0 ;
    for(i = 0;i < len;i++)
    {
        src[i]=~(src[i] + i);
    }
}
int main(int argc, char *argv[])
{
    int    sockfd;
    char   buffer[1024];
    char   temp[1024];
    struct sockaddr_in server_addr;
    int    nbytes;
    char   sendMsg[256] = {0};
    struct SMS_MSG  *sms;
    char   serverIP[256] = {0};
    sms =  (struct SMS_MSG*)sendMsg;
    if (argc != 5)
    {
        fprintf(stderr, "Usage:%s\n\a", argv[0]);
        exit(-1);
    }
    if(strlen(argv[1]) > SECRET_LEN || strlen(argv[2]) > TELNUM_LEN || strlen(argv[3]) > CONTENT_LEN)
    {
        perror("argv error.\n");
        exit(-1);
    }
    memset(sendMsg,0,sizeof(sendMsg));

    memcpy(sms->secret,argv[1],strlen(argv[1]));
    memcpy(sms->telnum,argv[2],strlen(argv[2]));
    memcpy(sms->content,argv[3],strlen(argv[3]));
#if 1
    pack(sms->content,CONTENT_LEN);
    pack(sms->telnum,TELNUM_LEN);
#endif
    sms->productSn = atoi(argv[4]);

    printf("get server ip...\n");
    if(!getServerIP(serverIP))
    {
        memcpy(serverIP,SERVERIP,sizeof(SERVERIP));
    }

    printf("server ip is : %s\n", serverIP);
    if (inet_aton(serverIP, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "the hostip is not right!");
        exit(-1);
    }
    printf("create socket...\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        exit(-1);
    }

    printf("ok!\n");
    printf("try connet...\n");
    int  opt = SO_REUSEADDR;
    int  result = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);

    if (ConnectNonb(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr),3) == -1) {
        fprintf(stderr, "Connect Error:%s\n\a", strerror(errno));
        exit(-1);
    }
    printf("ok!\n");
    printf("send message...\n");
    if(send(sockfd,sendMsg,sizeof(sendMsg),0) == -1)
    {
        perror("send error.\n"); 
        exit(-1);
    }
    printf("ok!\n");
    close(sockfd);
    exit(0);
}
#else
int main()
{
    return 0;
}
#endif
