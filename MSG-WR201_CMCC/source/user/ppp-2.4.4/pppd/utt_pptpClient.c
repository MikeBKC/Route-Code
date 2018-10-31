

/*
 * added by brwang
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "utt_pppoeClient.h"
#include "utt_conf_define.h"
#include "pppoe-server.h"
#include "utt_ip_assign.h"

struct st_uttPppdClient pptpClientInfo;

static void _utt_writeInfoToFile(void);

extern struct flock* file_lock(short type, short whence);

static int fd = -1;
void _utt_pptp_setevn(char *name, char *value)
{
    if(strncmp(name, "PEERNAME", 8) == 0) {
        strncpy(pptpClientInfo.user, value, UTT_PPP_CLIENT_NAME_LEN);
    } else if((strncmp(name, "CONNECT_TIME", 12) == 0) && (strcmp(pptpClientInfo.ourName, PPTP_SRV_NAME) == 0)) {
        if(pptpClientInfo.ConTime == 0) 
        {
            pptpClientInfo.ConTime = strtol(value, NULL, 10);
            if(pptpClientInfo.ConTime != 0){
                _utt_writeInfoToFile();
                close(fd);
                fd = -1;
            }
        }
    } else if((strncmp(name, "PPPD_PID", 8) == 0) && (strcmp(pptpClientInfo.ourName, PPTP_SRV_NAME) == 0)) {
        pptpClientInfo.pid = strtol(value, NULL, 10);
        _utt_writeInfoToFile();
    } else if(strncmp(name, "IFNAME", 6) == 0) {
        strncpy(pptpClientInfo.ifname, value, UTT_PPP_CLIENT_IFNAME_LEN);
    } else if(strncmp(name, "DEVICE", 6) == 0) {
        strncpy(pptpClientInfo.device, value, UTT_PPP_CLIENT_DEVICE_LEN);
    } else if((strncmp(name, "IPREMOTE", 8) == 0) && (strcmp(pptpClientInfo.ourName, PPTP_SRV_NAME) == 0)) {
        pptpClientInfo.ConTime = 0;
        strncpy(pptpClientInfo.ip, value, UTT_PPP_CLIENT_IP_LEN);
        _utt_writeInfoToFile();
    } else if((strncmp(name, "IPGWREMOTE", 10) == 0) && (strcmp(pptpClientInfo.ourName, PPTP_SRV_NAME) == 0)) {
        strncpy(pptpClientInfo.gwip , value, UTT_PPP_CLIENT_IP_LEN);
        _utt_writeInfoToFile();
    } else if(strncmp(name, "MACREMOTE", 8) == 0) {
        strncpy(pptpClientInfo.mac, value, UTT_PPP_CLIENT_MAC_LEN);
    } else if(strncmp(name, "START_TIME", 8) == 0) {
        memset(&pptpClientInfo, 0, sizeof(struct st_uttPppdClient));
        pptpClientInfo.STime = strtol(value, NULL, 10);
        pptpClientInfo.LTime = strtol(value, NULL, 10);
    } else if (strncmp(name, "OURNAME", 7) == 0) {
	strncpy(pptpClientInfo.ourName, value, UTT_PPP_CLIENT_OURNAME_LEN);
    }
}

static void _utt_writeInfoToFile()
{ 
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    int readNum = 0,freeNum = 0;
    struct st_uttPppdClient info;
    int found = 0;
    if(fd == -1) {
        fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
    }
    st_size = sizeof(struct st_uttPppdClient);
    start = lseek(fd, 0, SEEK_SET);
    end = lseek(fd, 0, SEEK_END);
    fileLen = end - start;
    lseek(fd, 0, SEEK_SET);
    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
    while(fileLen >= st_size) {
        read(fd, &info, st_size);
        readNum++;
#if 0
        if(info.ConTime != 0 || info.pid == pppoeClientInfo.pid ||
                (strncmp(info.user, pppoeClientInfo.user, UTT_PPP_CLIENT_NAME_LEN) == 0) ||
                (strncmp(info.ifname, pppoeClientInfo.ifname, UTT_PPP_CLIENT_IFNAME_LEN) == 0)) {
            found = 1;
            break;
        }
#else
        if(info.ConTime != 0) {
            if(freeNum == 0) {
                freeNum = readNum;
            }
        }
        if((info.pid == pptpClientInfo.pid )
#if 0
                || ((strncmp(info.user, pppoeClientInfo.user, UTT_PPP_CLIENT_NAME_LEN) == 0) &&
                (strncmp(info.ifname, pppoeClientInfo.ifname, UTT_PPP_CLIENT_IFNAME_LEN) == 0))
#endif
                )
        {
            
            found = 1;
            break;
        }
#endif
        fileLen -= st_size;
    }

    if(found == 1) {
#if 0
        lseek(fd, 0-st_size, SEEK_CUR);
#endif
        lseek(fd, (readNum - 1) * st_size, SEEK_SET);
    } else {
        if(freeNum != 0) {
            lseek(fd, (freeNum - 1) * st_size, SEEK_SET);
        }
    }

    write(fd, &pptpClientInfo, st_size);
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return;
}

int utt_pptp_controlOneUserConnCount(char *user)
{
    int val = 0;
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;

    if(fd == -1) {
        fd = open(UTT_PPTP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
    }
    st_size = sizeof(struct st_uttPppdClient);
    start = lseek(fd, 0, SEEK_SET);
    end = lseek(fd, 0, SEEK_END);
    fileLen = end - start;
    lseek(fd, 0, SEEK_SET);
    fcntl(fd, F_SETLKW, file_lock(F_WRLCK|F_RDLCK, SEEK_SET));
    while(fileLen >= st_size) {
        read(fd, &info, st_size);
        if(info.ConTime == 0 &&
                (strncmp(info.user, user, UTT_PPP_CLIENT_NAME_LEN) == 0)
                ) {
            val = 1;
            break;
        }
        fileLen -= st_size;
    }
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return val;
}
#ifdef UTT_IP_ASSIGN
/*
 * Used for storing a sequence of words.  Usually malloced. 
 * same define in pppd.h
 */
struct wordlist {
    struct wordlist	*next;
    char		*word;
};

#define UTT_MAXLINE 256
/********************************************************************
 * 函数： utt_pptp_srv_bindip_ok
 * 功能： 检查wordlist中是否包含指定IP，而不是通配符,用来判断一个账号是否是固定IP分配账号
 * 创建： 2012-10-11
 * 参数： addrs     -     wordlist
 * 返回： 0	    -     wordlist中不包含指定IP
 *	  1	    -     wordlist中包含指定IP
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int utt_pptp_srv_bindip_ok(struct wordlist *addrs) 
{
    while (addrs != NULL) 
    {
	if (addrs->word[0] == '-') {
	    break;
	}
	if (addrs->word[0] == '*') {
	    break;
	}
	if (addrs->word[0] != '!') {
	    return 1;
	}
	addrs = addrs->next;
    }
    return 0;
}

/********************************************************************
 * 函数： isPptpBindIp
 * 功能： 判断一个IP地址是否是绑定IP地址
 * 创建： 2012-10-11
 * 参数： ip	    -	  判断的IP地址，网络字节序
 * 返回： 0	    -     非绑定IP
 *	  1	    -     绑定IP
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int isPptpBindIp(u_int32_t ip) 
{
    FILE *fp = NULL;
    char line[UTT_MAXLINE];
    unsigned char bufIp[4];
    unsigned int a, b, c, d;
    int rst = 0;

    fp = fopen(PPTP_SERVER_BIND_IP_FILE, "r");
    if (fp == NULL) 
    {
	return rst;
    }

    while (feof(fp) == 0) {
	if (fgets(line, UTT_MAXLINE, fp) == NULL) {
	    break;
	}
	if ((sscanf(line, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) && 
		(a < 256) && (b < 256) && (c < 256) && (d < 256)) {
	    bufIp[0] = (unsigned char)a;
	    bufIp[1] = (unsigned char)b;
	    bufIp[2] = (unsigned char)c;
	    bufIp[3] = (unsigned char)d;
	    if (ip == *((unsigned int *)bufIp)) {
		rst = 1;
		break;
	    }
	}
    }
    fclose(fp);
    return rst;

}
#endif
