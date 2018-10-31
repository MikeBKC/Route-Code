
/*
 * add this for client for upnp
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <signal.h>
#include "uttUpnpC.h"
#include "nat.h"

#define MAX_MSG 100

int sd;
int wanIndex;

/*
 *	Write pid to the pid file
 */
int writeGoPid(int wanIndex)
{
	FILE *fp;
        char buf[50];
        sprintf(buf, "/var/run/upnp_wan%d.pid", wanIndex);

	fp = fopen(buf, "w+");
	if (NULL == fp) {
		return (-1);
	}
	fprintf(fp, "%d", getpid());
        fclose(fp);
	return 0;
}

static void fun_exit(int arg)
{
    char buf[100];
    snprintf(buf, 100, "iptables -t nat -F wan%d_%s", wanIndex,UPNP_CHAIN_WAN);
    system(buf);
    close(sd);
    exit(0);
}

int main(int argc, char** argv)
{
    int bd, rc;
    int stLen, rcvLen;
    char buf[200];

    struct sockaddr_in serAddr, cliAddr;
    struct utt_Upnp_data rcvData;
    if(argc != 4){
        printf("usage: uttUpnpDemo wanIndex interface port\n");
        exit(0);
    }

    signal(SIGTERM, fun_exit);
    wanIndex = strtol(argv[1], NULL, 0);

    if (writeGoPid(wanIndex) < 0) {
        exit(1);
    }
    rcvLen = sizeof(rcvData);
    stLen = sizeof(struct sockaddr);
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0) {
        printf("upnp cli error for creak socket\n");
        exit(1);
    }

    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serAddr.sin_port = htons(strtol(argv[3], NULL, 10));
    bzero(&(serAddr.sin_zero), 8);

    bd = bind(sd, (struct sockaddr*)&serAddr, stLen);
    if(bd < 0) {
        printf("upnp cli error for bind socket\n");
        exit(1);
    }

    while(1) {
        memset(&rcvData, 0, rcvLen);
        rc = recvfrom(sd, &rcvData, rcvLen, 
                0, (struct sockaddr*)&cliAddr, &stLen);
        if(rc < 0) {
            printf("upnp cli error for recv socket\n");
        }

        snprintf(buf, 200, "iptables -t nat -%s wan%s_%s -i %s -p %s --dport %s -j DNAT --to %s:%s",
                rcvData.action, argv[1], UPNP_CHAIN_WAN, argv[2], rcvData.protocol, rcvData.dport, 
                rcvData.innerIp, rcvData.innerPort);
        printf("%s \n",buf);
        system(buf);
#if 0
        sn = sendto(sd, send_buffer, MAX_MSG, 0,
                 (struct sockaddr*)&cliAddr, stLen);
#endif
    }
    close(sd);
    exit(0);
}
