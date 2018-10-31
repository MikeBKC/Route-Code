
/*
 *netbios name 处理程序
 *
 *
 *KeHuatao
 *2011-09-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pthread.h>
#include <linux/autoconf.h>
#include <config/autoconf.h>

#if defined(CONFIG_USER_CAPWAP_AC) || defined(CONFIG_USER_CAPWAP_WTP)
#include "../CWAC.h"
#include <ctype.h>
#include <signal.h>
#endif
#include <mib.h>
#include <profacce.h>
#include "common.h"

#ifdef URCP_WEB
//#include <sw_cfg.h>
//#include <switch.h>
//#include <packet.h>
#else
//#include "packet.h"
#endif
#include "urcp.h"

#if (CLIENT_HOST_NAME == FYES)
#include "dhcp.h"
#endif
#include "urcp_debug.h"

 
#ifdef URCP_WEB
extern void urcp_ipc_init(void);
extern void urcp_process(system_frame_header_t* header);
#endif
extern void *poll_tsk(void *arg);
extern void urcp_tftp_init(void);

static pthread_t urcp_thread;
static pthread_t poll_thread;
static void paket_process(char *frame, int len, int from);
static int module_create(void);
static void l2_loop(void);
static int init_socket(void);
static void *urcp_l2_tsk(void *arg);
int gSocketL2=-1;
int gSocketL3=1;
#if AP_WL_POWER_AUTO
#ifdef URCP_AP
around_env_ssid_info_t around_env_ssid_info;
#endif
#endif


/*
 *联动主函数
 *
 *Ke.huatao
 */
int urcp_main(int argc, char**argv)
{
    /*初始化*/
    ProfInit();
    urcp_ipc_init();
    urcp_init();

    if(init_socket() < 0) {
	cprintf("Failed to initialize the socket!");
	return -1;
    }
#ifdef URCP_AP
    init_ap_state();
#ifdef CONFIG_RT2860V2_AP_NETLINK
    uttWifiNlTaskInit();
#endif
    apRandomSleep();
#endif

    urcp_tftp_init();
#ifdef URCP_AC
#ifdef URCP_STA_MAC_TABLE 
     ac_sta_table_init();
#endif
    ac_timer_init();
#if (STA_ONOFF_LOG == FYES)
    ac_sta_table_log_init();
#endif
#endif

#ifdef URCP_AP
#if (CLIENT_HOST_NAME == FYES)
    ap_sta_table_init();
#endif
    ap_timer_init();
#ifdef URCP_TRAP_CMD_AP_STAT
    ap_sendRate_init();
#endif
#endif

#ifndef URCP_REL
    ap_ac_debug_init();
#endif
    module_create();
    
    return 0;
}
static int init_socket(void)
{
    int ret = 0;
#ifdef URCP_AC
    int size;
#endif

    /*创建SOCKET*/
    gSocketL2 = create_raw_socket(ETH_P_ALL);

#ifdef URCP_AC
    if(gSocketL2 >= 0)
    {
        size = 300*1024;
        if(setsockopt(gSocketL2, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)) < 0) {
            perror("set socket buffer"); 
	}
    }
#endif
    if(gSocketL2 < 0) {
	ret = -1;
    }

    return ret;
}

#define MAX_FRAME_LENGTH         1518u
static char frame[MAX_FRAME_LENGTH+1];
static void l2_loop(void)
{
    int len;

    while(1)
    {
	len = recv(gSocketL2, frame, MAX_FRAME_LENGTH, 0);
	if(len>0) {
	    paket_process(frame, len, 0);
	}
    }
}

static void paket_process(char *frame, int len, int from)
{
    int pull_len=0;
    short tmp = 0;
    system_frame_header_t header;

    if(isValidMac(frame+MAC_LEN) != 1)
    {
	return; 
    }
    /*remove the ether header*/
    memset(&header, 0, sizeof(system_frame_header_t));
    pull_len = sw_frame_header_pull(frame, &header);
    header.length = len - pull_len;
    header.fromL3 = from;

#if (CLIENT_HOST_NAME == FYES)
    if((header.data != NULL) && (header.packet_type == PACKET_URCP))
#else
    if(header.data != NULL)
#endif
    {
	tmp = *(uint16*)(&header.data[AUTH_KEY_OFFSET - (2*MAC_LEN) - sizeof(uint16)]) ;
#ifdef URCP_WEB
#ifdef HAS_RRCP
	if (tmp == ntohs(RRCP_AUTH_KEY))
	{
	    rrcp_process(&header, pd); /*v1 v2*/
	}
	else
#endif
#if ((UTT_AC_300 == FYES)||(UTT_AP_801 == FYES))
	    if (tmp == ntohs(URCP_AUTH_KEY_FRL))
#else
		if (tmp == ntohs(URCP_AUTH_KEY))
#endif

#else

#if ((UTT_AC_300 == FYES)||(UTT_AP_801 == FYES))
		    if (tmp == ntohs(URCP_AUTH_KEY_FRL))
#else
			if (tmp == ntohs(URCP_AUTH_KEY))
#endif
#endif
			{
			    urcp_process(&header);/*v3*/
			}
    }
#if (CLIENT_HOST_NAME == FYES)
    else if ((header.data != NULL) && (header.packet_type == PACKET_DHCP))
    {
	db_info(URCP_MODULE_ID_HOST_NAME,"\n=============== 67 or 68 packet ==============\n");
	/*处理DHCP包*/
	dhcpport_receive(&header);
    }
#endif
}


/*
 *create thread
 *KeHuatao
 */
static int module_create(void)
{
    int ret, error=0;

    ret = pthread_create(&urcp_thread, (pthread_attr_t*)NULL, urcp_l2_tsk, (void*)NULL);
    if(ret != 0)              
    {
	printf("Create urcp pthread error!\n");
	error = -1;
    }
    
    ret = pthread_create(&poll_thread, (pthread_attr_t*)NULL, poll_tsk, (void*)NULL);
    if(ret != 0)              
    {
	printf("Create urcp pthread error!\n");
	error = -1;
    }

    return  ret;
}

static void *urcp_l2_tsk(void *arg)
{
    CWThreadSetSignals(SIG_BLOCK, 1, SIGALRM);
    l2_loop();
    return ((void *)0);
}

#ifdef URCP_AC
CWBool CWParseUrcpPDURequest(int wtpIdx, unsigned char *buff, int len)
#else
CWBool CWParseUrcpPDURequest(unsigned char *buff, int len)
#endif
{
#ifdef URCP_AC
    unsigned char *mac=NULL;
#endif
    short type, elemLen;

    type = *((short*)buff);
    type = ntohs(type);

    elemLen = *((short*)(buff+2));
    elemLen = ntohs(elemLen);
    if((type==CW_MSG_ELEMENT_URCP_PDU_CW_TYPE) &&  (elemLen == (len-4))) {
	paket_process(buff+4, elemLen, 1);
#ifdef URCP_AC
	mac = buff + 4 + MAC_ADDR_LEN;
	memcpy(gWTPs[wtpIdx].priv.WTPMac, mac, MAC_ADDR_LEN);
#endif
    }
    return CW_TRUE;
}

