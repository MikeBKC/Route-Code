/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include <cwmp/http.h>
#include <cwmp/event.h>
#include "cwmp_httpd.h"
#include "cwmp_stun.h"
#include "profacce.h"
#include <unistd.h>  
#include <sys/msg.h>  


#define MAX_CLIENT_NUMS 8


static char * AuthRealm = "cwmpd";
static char * AuthQop = "auth";
static char   AuthOpaque[33] = {0};
static int	  AuthNonce = 0;
static int    stun_response_ok = 0;

const char * RESPONSE_200 = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 2\r\nContent-Type: text/xml; charset=\"utf-8\"\r\n\r\nOK";
const char * RESPONSE_400 = "HTTP/1.1 400 Bad request\r\nServer: CWMP-Agent\r\nConnection: close\r\nContent-Length: 5\r\n\r\nError";
const char * RESPONSE_401 = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Digest qop=\"%s\" nonce=\"%s\" opaque=\"%s\" realm=\"%s\"\r\nServer: TR069Agent\r\nContent-Length: 0\r\n\r\n";


struct http_session_fd_t
{
    //int fd;
    time_t time;
    http_socket_t * sock;
};


struct http_session_fd_t sessionfd[MAX_CLIENT_NUMS];


void setnonblocking(int fd)
{
#ifdef WIN32
#else
    int opts;
    opts=fcntl(fd, F_GETFL);
    if (opts < 0)
    {
        cwmp_log_error("setnonblocking fcntl GETFL failed: fd(%d)\n", fd);
        return;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0)
    {
        cwmp_log_error("setnonblocking fcntl SETFL failed: fd(%d)\n", fd);
        return;
    }
    return;
#endif
}



int httpd_response_unauthorization(http_socket_t * sock)
{

    char buffer[256];
    char nonce[33];
    FUNCTION_TRACE();
    AuthNonce ++;
    TRsnprintf(buffer, 256,  "%d", AuthNonce);
    MD5(nonce, buffer, NULL);

    nonce[32] = 0;

    TRsnprintf(buffer, 256, RESPONSE_401, AuthQop, nonce, AuthOpaque, AuthRealm);


    return	http_socket_write(sock, buffer, TRstrlen(buffer));
}

int httpd_response_ok(http_socket_t * sock)
{
    FUNCTION_TRACE();
    return	http_socket_write(sock, RESPONSE_200, TRstrlen(RESPONSE_200));
}

int httpd_response_unkonw_error(http_socket_t * sock)
{
    FUNCTION_TRACE();
    return	http_socket_write(sock, RESPONSE_400, TRstrlen(RESPONSE_400));
}

void cwmp_value_change_inform(cwmp_t *cwmp)
{
    cwmp->new_request = CWMP_YES;
    cwmp_event_set_value(cwmp, INFORM_VALUECHANGE, 1, NULL, 0, 0, 0);

    return;
}

int httpd_build_server(cwmp_t * cwmp)
{
    http_socket_t * lsnsock;
    pool_t * pool;
    int rc;
    int lsnfd, maxfd, nready;
    int i;

    int fd, newfd;
    http_socket_t * s;
    http_request_t * request;

    char * auth;
    time_t now;
    fd_set readset, rdset;
    struct timeval timeout;
    int port;

    char  cpe_user[INI_BUFFERSIZE] = {0};
    char  cpe_pwd[INI_BUFFERSIZE] = {0};

    port = cwmp->httpd_port;

    pool = pool_create(POOL_DEFAULT_SIZE);
    rc = http_socket_server(&lsnsock, port, 5, -1, pool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("build httpd server faild. %s\n", strerror(errno));
        exit(-1);
    }


    lsnfd = http_socket_get_fd(lsnsock);

    for (i=0; i < MAX_CLIENT_NUMS; i++)
    {

        sessionfd[i].time = 0;
        sessionfd[i].sock = NULL;
    }

    FD_ZERO(&readset);
    FD_SET(lsnfd, &readset);

    maxfd = lsnfd;
    /*maxi = -1;*/
    while (1)
    {
        FD_ZERO(&rdset);
        rdset = readset;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        if ((nready = select(maxfd + 1, &rdset, NULL, NULL, &timeout)) <= 0)
        {
            sleep(1);
            cwmp_log_info("select new connection timeout. no new request.\n");
            now = time(NULL);
            for (i=0; i<MAX_CLIENT_NUMS; i++)
            {
                //cwmp_log_debug("socket time: %d, timeout %d, fd is %d\n", sessionfd[i].time, now -  sessionfd[i].time,
                //               sessionfd[i].sock == NULL? -1 : http_socket_get_fd(sessionfd[i].sock));
                fd = http_socket_get_fd(sessionfd[i].sock);
                if ((sessionfd[i].sock != NULL) && (now -  sessionfd[i].time > 15))
                {
                    cwmp_log_info("close a timeout socket. fd is %d.\n", fd);
                    FD_CLR(fd, &readset);
                    //http_socket_close(sessionfd[i].sock);
                    http_socket_destroy(sessionfd[i].sock);
                    sessionfd[i].time = 0;
                    sessionfd[i].sock = NULL;
                }
            }
            continue;
        }

        if (FD_ISSET(lsnfd, &rdset))
        {
            http_socket_t * newsock;
            //FIXME
            http_socket_accept(lsnsock, &newsock);
            newfd = http_socket_get_fd(newsock);

            for (i=0; i<MAX_CLIENT_NUMS; i++)
            {
                if (sessionfd[i].sock == NULL)
                {
                    sessionfd[i].sock = newsock;
                    sessionfd[i].time = time(NULL);
                    break;
                }
            }
            if (i == MAX_CLIENT_NUMS)
            {
                //http_socket_close(newsock);
                http_socket_destroy(newsock);

                cwmp_log_error("too many ACS request connection");
                continue;
            }
            FD_SET(newfd, &readset);
            if (newfd > maxfd)
            {
                maxfd = newfd;
            }

            newfd = -1;
            if (--nready <= 0)
            {
                continue;
            }

        }

        //readpool = pool_create(POOL_DEFAULT_SIZE);
        cwmp_log_debug("nready is %d.\n", nready);
        for (i=0; (i<MAX_CLIENT_NUMS) && (nready > 0) ; i++)
        {
            s = sessionfd[i].sock;
            fd = http_socket_get_fd(s);

            if ((fd != -1) && FD_ISSET(fd, &rdset))
            {
                nready--;
                sessionfd[i].time = time(NULL);
                http_request_create(&request, http_socket_get_pool(s));
                rc = http_read_request(s, request, http_socket_get_pool(s));
                if (rc <= 0)
                {
                    httpd_response_unkonw_error(s);
                    goto faild;
                }

                if (request->method != HTTP_GET)
                {
                    httpd_response_unkonw_error(s);
                    goto faild;
                }

                if (cwmp->cpe_auth)
                {
                    auth = http_get_variable(request->parser, "Authorization");

                    if (!auth)
                    {
                        httpd_response_unauthorization(s);
                        goto faild;
                    }


                    cwmp_conf_get("cwmp:cpe_username", cpe_user);
                    cwmp_conf_get("cwmp:cpe_password", cpe_pwd);

                    cwmp_log_debug("cpe username: %s, cpe password: %s\n", cpe_user, cpe_pwd);


                    if (http_check_digest_auth(AuthRealm, auth, cpe_user, cpe_pwd) != 0)
                    {
                        httpd_response_unauthorization(s);
                        goto faild;
                    }
                }

                httpd_response_ok(s);

                //get a new request from acs
                cwmp->new_request = CWMP_YES;
                cwmp_log_debug("set cwmp new request to %d\n", cwmp->new_request);
                cwmp_event_set_value(cwmp, INFORM_CONNECTIONREQUEST, 1, NULL, 0, 0, 0);

faild:

                FD_CLR(fd, &readset);
                sessionfd[i].time = 0;
                sessionfd[i].sock = NULL;
                http_socket_destroy(s);


            }

        }



    }

}

/* 下面是为了实现stun穿透用，
 * 尝试新建stun.c文件，编译出错
 * 因为时间紧迫，先在这里写相关代码
 * 后续再移到stun.c文件中 */

int stun_rand()
{
    srand(time(NULL));
    return rand();
}

void stun_build_pkt(struct stun_pkt *pkt, int change_flag,char* serno)
{
    struct stun_attr *attr;
    char *p;

    pkt->type = htons(0x0001);
    pkt->length = 0;	/* count later */
    pkt->transId1 = stun_rand();
    pkt->transId2 = stun_rand();
    pkt->transId3 = stun_rand();
    pkt->transId4 = stun_rand();
    attr = (struct stun_attr*)pkt->attributes;


    /* Attribute Type: PASSWORD */
    attr->type = htons(0x0007);
    strcpy(attr->value,serno);
    attr->length = htons(strlen(attr->value));
    pkt->length += (4 + ntohs(attr->length));
    attr = (struct stun_attr *)(pkt->attributes + pkt->length);

    /* Attribute Type: CONNECTION-REQUEST-BINDING */
    attr->type = htons(0xc001);
    strcpy(attr->value, "dslforum.org/TR-111 ");
    attr->length = htons(strlen(attr->value));
    pkt->length += (4 + ntohs(attr->length));
    attr = (struct stun_attr *)(pkt->attributes + pkt->length);

    if (change_flag == 1)
    {
        /* Attribute Type: BINDING-CHANGE */
        attr->type = htons(0xc002);
        attr->length = 0;
        pkt->length += (4 + attr->length);
        attr = (struct stun_attr *)(pkt->attributes + pkt->length);
    }

    pkt->length = htons(pkt->length);

    return;
}

int is_stun_recv_request(char *data)
{
    if (memcmp(data, "GET", 3)  && memcmp(data, "get", 3))
        return 0;

    return 1;
}

void stun_handle_recv_request(cwmp_t *cwmp, char *pkt)
{
    static char *p, ts[20] = {0};
    int i = 0;

    /*get ts infor from packet*/
    p = strstr(pkt, "ts=");
    if (p == NULL)
    {
        cwmp_log_error("STUN: but http data have no ts information, error.\n");
        return;
    }

    /*if this ts same as last ts, means this is the repeated request, ignore it*/
    while (*p<='0' || *p>'9')
        p++;

    if (ts[0] != 0 && memcmp(ts, p, strlen(ts)) == 0)
    {
        return;
    }
    else
    {
        i = 0;
        while (p[i]>='0' && p[i]<='9')
        {
            ts[i] = p[i];
            i++;
        }
        ts[i] = '\0';
    }

#if 0
    cwmp_log_error("STUN: handle new request.\n");
#endif
    cwmp->new_request = CWMP_YES;
    cwmp_event_set_value(cwmp, INFORM_CONNECTIONREQUEST, 1, NULL, 0, 0, 0);

    return;
}

int is_stun_recv_response(struct stun_pkt *pkt)
{
    if (ntohs(pkt->type) == 0x0101) /* binding response */
    {
        stun_response_ok = 5;
        return 1;
    }
    else 
    {
        return 0;
    }
}

void stun_handle_recv_response(cwmp_t *cwmp, struct stun_pkt *pkt)
{
    struct stun_attr *attribute;
    struct stun_mapped_address *maddr;
    unsigned short type;
    unsigned short len, count = 0;
    unsigned char  *value;
    int i, get_addr_flag = 0;

    if (ntohs(pkt->length) < 4)
    {
        cwmp_log_error("STUN: recv stun pkt data length error: %d\n", ntohs(pkt->length));
        return;
    }

    while (count < ntohs(pkt->length))
    {
        attribute = (struct stun_attr *)(pkt->attributes + count);	
        type = ntohs(attribute->type);
        len = ntohs(attribute->length);
        value = attribute->value;
        count += (4 + len);

        if (type == 0x0001)
        {
            /* Attribute Type: MAPPED-ADDRESS */
            if (len != 8)
            {
                cwmp_log_error("STUN: attribute->type %#x length error: %d\n", type, len);
                return;
            }
            maddr = (struct stun_mapped_address *)value;
            if (maddr->padding != 0x0 || maddr->family != 0x1)
            {
#if 0
                cwmp_log_error("STUN: attribute->type %#x value error: %02x%02x(protocol family not ipv4)\n", type, maddr->padding, maddr->family);
#endif
                return;
            }

            get_addr_flag = 1;
            break;
        } 
        else
        {
            continue;
        }
    }

    if (get_addr_flag)
    {
        if (memcmp(cwmp->stun_ip, maddr->address, 4) != 0 || cwmp->stun_port != ntohs(maddr->port))
        {
#if 0
            cwmp_log_error("STUN: get new stun address: %d.%d.%d.%d:%d\n", 
                    cwmp->stun_ip[0], cwmp->stun_ip[1], cwmp->stun_ip[2], cwmp->stun_ip[3], cwmp->stun_port);
#endif
            memcpy(cwmp->stun_ip, maddr->address, 4);
            cwmp->stun_port = ntohs(maddr->port);
            while (cwmp->new_request == CWMP_YES)
                sleep(1);
            cwmp->new_request = CWMP_YES;
            cwmp_event_set_value(cwmp, INFORM_VALUECHANGE, 1, NULL, 0, 0, 0);
        }
    }
    else
    {
        cwmp_log_error("STUN: not find server ip and port.\n");
    }

    return;
}

void stun_handle_recv(cwmp_t *cwmp, struct stun_pkt *pkt, int pkt_len)
{
    if (is_stun_recv_request((char*)pkt))
    {
        stun_handle_recv_request(cwmp, (char*)pkt);
    }
    else if (is_stun_recv_response(pkt))
    {
        stun_handle_recv_response(cwmp, pkt);
    }
    else
    {
        cwmp_log_error("STUN: unkown stun pkt.\n");
    }

    return;
}
/*  "http://112.124.31.88:80/mcp/tr069Servlet"  */
int stun_get_server_addr(cwmp_t *cwmp, char *serveraddr,int *serverport,int len)
{
    char *addrhead1 = "http://";
    char *addrhead2 = "https://";
    char * tmp = NULL;
    int addrheadlen = 0;
    int i = 0;

    if (memcmp(cwmp->stun_url, addrhead1, strlen(addrhead1)) == 0)
        addrheadlen = strlen(addrhead1);
    else if (memcmp(cwmp->stun_url, addrhead2, strlen(addrhead2)) == 0)
        addrheadlen = strlen(addrhead2);

    i = addrheadlen;
    while (cwmp->stun_url[i]	!= '\0' && cwmp->stun_url[i] != ':' && cwmp->stun_url[i] != '/')
    {
        if((i - addrheadlen) > len)
        {
            cwmp_log_error("STUN: get server address fail!");
            strcpy(serveraddr, "112.124.31.88");
            serverport = 3478;
            return 0;
        }
        serveraddr[i-addrheadlen] = cwmp->stun_url[i]; 
        i++;
    }

    serveraddr[i - addrheadlen] = 0;

    if(cwmp->stun_url[i] == ':')
    {
        tmp = &(cwmp->stun_url[i + 1]);
        *serverport = strtol(tmp , NULL , 10);
    }
    return 1;
}

void udp_bind_device(int sockfd,struct sockaddr *addr,int len)
{
    int  index = 0;
    int  wanCount = 0;
    char *wanName = NULL, if_addr[16];
    struct ifreq interface;
    InterfaceProfile *profIf = NULL;
    unsigned int state = 0U, phylinkStatus = 0U;/*物理链路连接状态，默认为0表示网线未连接*/
    static int current_interface_index = -1;/*当前连接不同的接口索引*/
    int interface_index = current_interface_index + 1;/*从该索引开始绑定接口*/

    wanCount = getWanSlotCount();
    for(index = 0; index <= wanCount; index++) 
    {
        /*printf("%s ---- %d;interface_index : %d; interfac//wanCount:%d\n",__func__,__LINE__,interface_index,interface_index%(wanCount+1));*/
        if(interface_index%(wanCount+1) != 0)
        {
            profIf = (InterfaceProfile*)ProfGetInstPointByIndex(MIB_PROF_INTERFACE, interface_index%(wanCount+1));
            phylinkStatus = (unsigned int)get_phylink_status(interface_index%(wanCount+1));/*获取wan1口状态*/
            switch(profIf->ether.connMode) {
                case ETH_CONN_DHCP:
                    /*printf("DHCP\n");*/
                    wanName = getWanIfName(interface_index%(wanCount+1));
                    /*printf("func:%s,line %d,wanName=%s \n",__func__,__LINE__,wanName);*/
                    if(getIfIp(wanName,NULL) == 0) {/*如果DHCP获取到了IP,则置1*/
                        state = 1U;/*如果获取到了IP，但仍不能上网，连接状态仍然是1*/
                    }
                    /*printf("func:%s,line %d,state=%d \n",__func__,__LINE__,state);*/
                    state &= phylinkStatus;/*如果网线断开或者DHCP获取不到IP地址，都会为0*/ 	    
                    break;
                case ETH_CONN_STATIC:
                    /*printf("STATIC\n");*/
                    state = phylinkStatus;/*如果物理网络正常，但配置的固定IP错误不能上网，仍然为1*/
                    break;
                case ETH_CONN_PPPOE:
                default:
                    /*printf("PPPOE\n");*/
                    /*获取PPPoE的连接状态*/
                    if(getIfIp(getWanIfNamePPP(interface_index%(wanCount+1)), if_addr) == 0) {
                        state = 1U;
                    }
                    state &= phylinkStatus;
                    break;
            }
            if(state == 1U)
            {
                wanName = getWanIfNamePPP(interface_index%(wanCount+1)); 
                strncpy(interface.ifr_name, wanName, IFNAMSIZ);
                /*printf("func:%s, line %d ,interface[%d] : %s ;state : %d ; phylinkStatus=%d\n",__func__,__LINE__,interface_index%(wanCount+1),wanName,state ,phylinkStatus);*/
                if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE,(char *)&interface, sizeof(interface)) < 0) 
                {
                    fprintf(stderr,"SO_BINDTODEVICE  %s failed",wanName);
                }
                current_interface_index = interface_index%(wanCount+1);
                break;
           } 
            /*printf("func:%s, line %d ,interface[%d] : %s ;state : %d ; phylinkStatus=%d\n",__func__,__LINE__,interface_index%(wanCount+1),wanName,state ,phylinkStatus);*/
        }
        interface_index++;
    }
}

int stun_task_build(cwmp_t *cwmp)
{
    int socketfd = -1;
    struct hostent *he;
    struct sockaddr_in server, peer;
    struct stun_pkt send_pkt;
    struct stun_pkt recv_pkt;
    struct timeval tv_out;
    unsigned long tm0, tm1, interval;
    size_t length;
    int recv_len,nRecvBuf=2048;
    char serverip[50];
    char x_stun_url[100] = {0};
    int serverport = 3478;
    unsigned int  stun_ip =0;

xstart:
    memset(serverip, 0, sizeof(serverip));
    memset(x_stun_url,0,sizeof(x_stun_url));

    strcpy(x_stun_url,cwmp->stun_url);
    stun_get_server_addr(cwmp, serverip,&serverport,sizeof(serverip) - 1);

#if 0
    cwmp_log_debug("%s---------%d;ip : %s ; port : %u\n",__func__,__LINE__,serverip,serverport);
    cwmp_log_error("STUN: get server addr is %s", serverip);
#endif

    while(he == NULL)
    {
        he = gethostbyname(serverip);
        if (he != NULL)
            break;
        cwmp_log_error("STUN: gethostbyname of %s error. And regethostbyname again in 3 seconds.\n", serverip);
        sleep(3);
    }
newrequest:
    if(socketfd != -1)
    {
        close(socketfd); 
    }

#if 0
    cwmp_log_error("STUN: get server addr is %s", serverip);
#endif
    socketfd = socket(AF_INET , SOCK_DGRAM , 0);
    if (socketfd == -1)
    {
        cwmp_log_error("STUN: socket() error.\n");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr = *((struct in_addr *)he->h_addr);
    stun_ip = server.sin_addr.s_addr;
    server.sin_port = htons(serverport);
    he = NULL;

    tv_out.tv_sec = 5;
    tv_out.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
    setsockopt(socketfd, SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));

    udp_bind_device(socketfd,(struct sockaddr*)&server,sizeof(server));
    stun_response_ok = 5;

    /* 系统启动后需要连续发送两次stun交互*/
#if 0
    cwmp_log_error("STUN: for boot\n");
#endif
    stun_build_pkt(&send_pkt, 0,cwmp->cpe_sn);
    sendto(socketfd , &send_pkt , 20 + ntohs(send_pkt.length) , 0, (struct sockaddr*)&server , sizeof(server));
    recv_len = recvfrom(socketfd , (void*)&recv_pkt , sizeof(recv_pkt) , 0, (struct sockaddr*)&peer , &length);
    stun_handle_recv(cwmp, &recv_pkt, recv_len);
#if 0
    cwmp_log_error("STUN: for boot recv\n");
#endif

    while (1) 
    {	
        if(strcmp(x_stun_url,cwmp->stun_url) != 0)
        {
           goto xstart;
        }
        /*printf("%s ---- %d;stun_response_ok : %d\n",__func__,__LINE__,stun_response_ok);*/
        if(stun_response_ok == 0)
        {
           goto xstart; 
        }

        stun_build_pkt(&send_pkt, 0,cwmp->cpe_sn);

#if 0
	cwmp_log_debug("STUN: send\n");
#endif
        sendto(socketfd , &send_pkt , 20 + ntohs(send_pkt.length) , 0, (struct sockaddr*)&server , sizeof(server));

        length = sizeof(struct sockaddr);
        tm0 = time(NULL);
        stun_response_ok--;
        while (1) 
        {
            recv_len = recvfrom(socketfd , (void*)&recv_pkt , sizeof(recv_pkt) , 0, (struct sockaddr*)&peer , &length);
            if (recv_len > 0)
            {
#if 0
		cwmp_log_error("STUN: stun_handle_recv\n");
#endif
                stun_handle_recv(cwmp, &recv_pkt, recv_len);
            }

            tm1 = time(NULL);
	    if(tm1 - tm0 > cwmp->stun_period)
	    {
		he = gethostbyname(serverip);
		if (he != NULL){
		    unsigned int  ip = ((struct in_addr *)he->h_addr)->s_addr;
		    if(ip != stun_ip){
			stun_ip = ip;
			goto newrequest; 
		    }
		}
//		cwmp_log_error("STUN: New period.\n");
		break;
	    }
        }
    }
    return 0;
}

void cwmp_periodic_inform(cwmp_t *cwmp)
{
    cwmp->new_request = CWMP_YES;
    cwmp_event_set_value(cwmp, INFORM_PERIODIC, 1, NULL, 0, 0, 0);

    return;
}

int webauth_task_build(cwmp_t *cwmp)
{
    int cwmpPeriodicTimeCount = 0;
    int interval = 5;

#if 0
    int msgid = 0;
    struct msg_st data;  
    long int msgtype = 0;

    msgid = cwmp_msg_init();

    if(msgid == -1)
    {
	    printf("cwmp msg init error\n");
        exit(EXIT_FAILURE);  
    }
#endif

#if (CLOUD_MANAGE == FYES)
    CloudManageProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_CLOUD_MANAGE;
    char *childname = "InternetGatewayDevice.DeviceConfig.ConfigVersion";
    parameter_node_t * pn;
    prof = (CloudManageProfile*)ProfGetInstPointByIndex(mibType,0);
    pn = cwmp_get_parameter_node(cwmp->root, childname);
    if ((0 == ProfInstIsFree((Pointer)prof)))
    {
	if(pn->value == NULL){
	    pn->value = (char *) malloc(128);
	    if(pn->value != NULL){
		pn->value_length = 128;
	    }
	}
	snprintf(pn->value,128,"%d",prof->configVer);	
   }
#endif
    while (1)
    {
        sleep(interval);

        /*每60秒发送一次心跳报文, 有可能会不止60秒*/
        cwmpPeriodicTimeCount += interval;
        if (cwmpPeriodicTimeCount >= cwmp->acs_period)
        {
            cwmp_periodic_inform(cwmp);
            cwmpPeriodicTimeCount = 0;
        }
#if (CLOUD_MANAGE == FYES)
    if (prof->configNeedSyn == 1)
    {
	snprintf(pn->value,128,"%d",prof->configVer);	
	prof->configNeedSyn = 0;
        cwmp_value_change_inform(cwmp);
    }
#endif
#if(CLOUD_CONFIG == FYES)
    if (nvramProfile->needSendCount> 0 ){
	cwmp_event_set_value(cwmp, INFORM_CLIENTCHANGE, 1, NULL, 0, 0, 0);
	cwmp->new_request = CWMP_YES;
    }
#endif
#if 0
        if(msgrcv(msgid, (void*)&data, MAX_TEXT, msgtype, IPC_NOWAIT) != -1)  
        {
            cwmp_value_change_inform(cwmp);
        }
#endif
    }

    return 0;
}
