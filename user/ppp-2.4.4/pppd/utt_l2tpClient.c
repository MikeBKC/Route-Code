

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
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "comExeFun.h"
#include "ifmisc.h"

struct st_uttPppdClient l2tpClientInfo;

static void _utt_writeInfoToFile(void);

extern struct flock* file_lock(short type, short whence);

static int fd = -1;
void _utt_l2tp_setevn(char *name, char *value)
{
    if(strncmp(name, "PEERNAME", 8) == 0) {
        strncpy(l2tpClientInfo.user, value, UTT_PPP_CLIENT_NAME_LEN);
    } else if((strncmp(name, "CONNECT_TIME", 12) == 0) && (strcmp(l2tpClientInfo.ourName, L2TP_SRV_NAME) == 0)) {
        if(l2tpClientInfo.ConTime == 0) 
        {
            l2tpClientInfo.ConTime = strtol(value, NULL, 10);
            if(l2tpClientInfo.ConTime != 0){
                _utt_writeInfoToFile();
#if (L2TP_SERVER == FYES)
		nvramProfile->vpnProfile.count++;
		notice("[VPN]l2tp server accout %s down, free count +1.\n",l2tpClientInfo.user);
		if(nvramProfile->vpnProfile.count > MAX_VPN_SESSION_NUM) {
		    nvramProfile->vpnProfile.count = MAX_VPN_SESSION_NUM;
		}
#endif
                close(fd);
                fd = -1;
            }
        }
    } else if((strncmp(name, "PPPD_PID", 8) == 0) && (strcmp(l2tpClientInfo.ourName, L2TP_SRV_NAME) == 0)) {
        l2tpClientInfo.pid = strtol(value, NULL, 10);
        _utt_writeInfoToFile();
    } else if(strncmp(name, "IFNAME", 6) == 0) {
        strncpy(l2tpClientInfo.ifname, value, UTT_PPP_CLIENT_IFNAME_LEN);
    } else if(strncmp(name, "DEVICE", 6) == 0) {
        strncpy(l2tpClientInfo.device, value, UTT_PPP_CLIENT_DEVICE_LEN);
    } else if((strncmp(name, "IPREMOTE", 8) == 0) && (strcmp(l2tpClientInfo.ourName, L2TP_SRV_NAME) == 0)) {
        l2tpClientInfo.ConTime = 0;
        strncpy(l2tpClientInfo.ip, value, UTT_PPP_CLIENT_IP_LEN);
        _utt_writeInfoToFile();
#if (L2TP_SERVER == FYES)
	notice("[VPN]l2tp server accout %s up, free count -1.\n",l2tpClientInfo.user);
	if(nvramProfile->vpnProfile.count<=0) {
	    nvramProfile->vpnProfile.count=0;
	    notice("[VPN]free num =0.\n");
	} else {
	    nvramProfile->vpnProfile.count--;
	}
#endif
    } else if(strncmp(name, "MACREMOTE", 8) == 0) {
        strncpy(l2tpClientInfo.mac, value, UTT_PPP_CLIENT_MAC_LEN);
    } else if(strncmp(name, "START_TIME", 8) == 0) {
        memset(&l2tpClientInfo, 0, sizeof(struct st_uttPppdClient));
        l2tpClientInfo.ConTime = 255;
        l2tpClientInfo.STime = strtol(value, NULL, 10);
        l2tpClientInfo.LTime = strtol(value, NULL, 10);
    } else if (strncmp(name, "OURNAME", 7) == 0) {
	strncpy(l2tpClientInfo.ourName, value, UTT_PPP_CLIENT_OURNAME_LEN);
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
        fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
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
        if((info.pid == l2tpClientInfo.pid )
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

    write(fd, &l2tpClientInfo, st_size);
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return;
}

int utt_l2tp_controlOneUserConnCount(char *user)
{
    int val = 0;
    static int     st_size;
    off_t   start, end;
    int     fileLen;
    struct st_uttPppdClient info;

    if(fd == -1) {
        fd = open(UTT_L2TP_CLIENT_INFO_FILE, O_RDWR|O_CREAT, 0666);
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
#if 0
                && (strncmp(l2tpClientInfo.device, info.device, UTT_PPP_CLIENT_DEVICE_LEN) == 0)
#endif
                ) {
            val = 1;
            break;
        }
        fileLen -= st_size;
    }
    fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
    return val;
}
