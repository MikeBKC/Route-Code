#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>
#include    <dirent.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <net/if.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/msg.h>
#include    <sys/stat.h>
#include    <signal.h>
#include    <sys/shm.h>
#include    <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "ifmisc.h"



/* send message  to the message queue, from which the speedweb
 * process will read it, and do conressponding actions.
 * it is now called by the shell script linkcheck.sh when link state changes
 * */
int main(int argc, char* argv[])
{
    struct st_linkCheckMsg tmpmsg;

#if (NET_SHARE_MANAGE == FYES)
    struct st_mdevMsg mdevMsg;
#endif

    /* Usage */
    if ( argc < 3 ) {
        printf("usage: \nsnd2speed <InitType> [ <wanIndex> <action> <string> ].\nsnd2speed stats <seconds> for utt stats update behavior interval.\nsnd2speed autodel <minutes> for utt autodel interval.\nsnd2speed cache <up|down>\nsnd2speed link error\nsnd2speed wireless <start|down|date>");
    } else {
        tmpmsg.wanIndex = strtol(argv[2], NULL, 10);;
        if(strcmp(argv[1], "up") == 0){
            tmpmsg.status = 1;
#if (FEATURE_AP == FYES)
	    sleep(1); /*等IP设置好，否则默认路由可能加不上*/
#endif
            UttSendMsgForSpeed(MSG_LINECHK_STA_CHANGE, 0,
                    &tmpmsg, sizeof(tmpmsg));
        } else if(strcmp(argv[1], "down") == 0){
            tmpmsg.status = 0;
            UttSendMsgForSpeed(MSG_LINECHK_STA_CHANGE, 0,
                    &tmpmsg, sizeof(tmpmsg));
        } 
#if (USER_STATUS == FYES)
	else if (strcmp(argv[1], "stats") == 0) {
	    int invl = 0;
	    invl = strtol(argv[2], NULL, 10);
	    if (invl == 0) {
		printf("usage:snd2speed stats <number>\n");
		exit(0);
	    }
	    printf("send interval = %d\n", invl);
	    UttSendMsgForSpeed(MSG_NL_UPDATE_STATS_INTERVAL_CHANGE, 0, &invl, sizeof(int));
	}
#endif
#if (DHCP_AUTO_BIND == FYES)
	else if (strcmp(argv[1], "autodel") == 0) 
	{
	    int deltime = 0;
	    deltime = strtol(argv[2], NULL, 10);
	    if (deltime == 0) {
		printf("usage:snd2speed autodel <number>\n");
		exit(0);
	    }
	    printf("send interval = %d\n", deltime);
	    UttSendMsgForSpeed(MSG_NL_UPDATE_AUTODEL_INTERVAL_CHANGE, 0, &deltime, sizeof(int));
	}
#endif
#if (IP_SEC == FYES)
#if ((PPTP_SERVER == FYES) || (PPTP_CLIENT == FYES))
	else if(strcmp(argv[1], "ipsecUp") == 0)
	{
	    char buf[20];
	    memset(buf, 0, sizeof(buf));
	    strcpy(buf, argv[2]);
	    printf("pptp instname = %s, sizeof(buf)=%d\n", buf,sizeof(buf));
	    UttSendMsgForSpeed(MSG_IPSEC_AUTOUP_OVER_PPTP_CHANGE, 0, buf, sizeof(buf));
	}
#endif
#endif
#if defined(CACHE_SERVER)
	else if(strcmp(argv[1],"cache")==0){
		int sta = -1;
		if(strcmp(argv[2],"up")==0){
			sta = 1;
		}else{
			sta = 0;
		}
		if(sta!=-1){
			UttSendMsgForSpeed(MSG_CACHE_STA_CHANGE, 0, &sta, sizeof(int));
		}
	}
#endif
#if (EASY_TASK == FYES)
	else if (strcmp(argv[1], "link") == 0) {
		int status = 0;
		if (strcmp(argv[2], "error") == 0) {
			status = 7;
		} else {
			exit(0);
		}
		UttSendMsgForSpeed(MSG_LINK_STA_CHANGE, 0, &status, sizeof(int));
	}
#if 0
#if (WIRELESS == FYES)
	else if (strcmp(argv[1], "wireless") == 0) {
		int status = 0;
		if (strcmp(argv[2], "start") == 0) {
			status = 1;
		} else if (strcmp(argv[2], "stop") == 0) {
			status = 0;
		} else if (strcmp(argv[2], "date") == 0) {
			status = 2;
		} else {
			exit(0);
		}
		printf("%s-%d:status is %d\n", __func__, __LINE__, status);
		UttSendMsgForSpeed(MSG_WIRELESS_SWITCH_CHANGE, 0, &status, sizeof(int));
	}
#endif
#endif
#endif

#if (NET_SHARE_MANAGE == FYES)
	else if ( (argc > 3) && (strcmp(argv[1], "mdev") == 0)) {
        /*   /etc/mdev.conf
	 *   /sbin/automount.sh  */
	    memset(&mdevMsg, 0, sizeof(mdevMsg));
	    if (strlen(argv[2]) < MDEV_LEN ) {
		strcpy(mdevMsg.dev, argv[2]);
		if(!strcmp(argv[3], "add"))
		{
		    mdevMsg.status = 1;
		}
		else
		{
		    mdevMsg.status = 0;
		}
		UttSendMsgForSpeed(MSG_MDEV_CHG, 0, &mdevMsg, sizeof(mdevMsg));
	    } 
	}
#endif

    }
    exit(0);
}
