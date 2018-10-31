
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

#include <mib.h>
#include <profacce.h>
#include "common.h"

#ifdef URCP_WEB
#include <sw_cfg.h>
#include <switch.h>
#include <packet.h>
#else
#include "packet.h"
#endif
#include "urcp.h"

#define TIMEOUT    10000u /*us*/

#ifdef URCP_WEB
extern void urcp_ipc_init(void);
extern void rrcp_process(system_frame_header_t* header, uchar* frame);
#endif
extern void *urcp_tsk(void *arg);
extern void urcp_tftp_init(void);

static int _fd=-1;
static pthread_t urcp_thread;
static int recv_packet(int fd, char *frame, int max_length, unsigned long msec);
static void paket_process(char *frame, int len);
static int module_create(void);

extern unsigned int dbg=0;
extern FILE* ffp=NULL;

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
int main(int argc, char**argv)
{
    int len;
    char frame[MAX_FRAME_LENGTH+1];
    unsigned int delay=0, sn=0;
#ifdef URCP_AC
    int size, error;
#endif

    /*创建SOCKET*/
    _fd = create_raw_socket(ETH_P_ALL);
    if(_fd>=0)
    {
#if 1
	if(argc==2)
	{
	    dbg = strtoul(argv[1], NULL, 10);
	}
	//printf("dbg: %d\n", dbg);
//	live= (1000*live)/POLL_TIME; 
#endif
	unlink("/udbg");
#ifdef URCP_AC
        size = 300*1024;
        error = setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int)); 
        if(error<0){ 
             perror("set socket buffer "); 
             return 0;
        }
#endif
#if 0
#ifdef CONFIG_VSC7390
	/*之前采用将组播类型8899写入mac表的方式让联动组播包进cpu
	 * 现在已经通过设置0xf0寄存器实现
	 * 若还要使用此功能。需将mac地址学习函数的目的地址最大位增加一位
	 * */
	cooperate_mac_add();
#endif
#endif
	/*初始化*/
	ProfInit();
	urcp_ipc_init();
	urcp_init();
#ifdef URCP_AP
	sn = getSystemSn();
	srand(sn);
	delay = rand();
	//printf("rand: %d\n", delay);
	delay = delay % 7;
	//printf("delay: %d\n", delay);
	sleep(delay);

	init_ap_state();
	uttWifiNlTaskInit();
#endif
	urcp_tftp_init();
	module_create();
	while(1)
	{
	    memset(frame, 0, sizeof(frame));
	    /*收包*/
	    len = recv_packet(_fd, frame, MAX_FRAME_LENGTH, 0);
	    if(len>0)
	    {
		/*包处理函数*/
		paket_process(frame, len);
	    }
	    else
	    {
		if(len<0)
		{
		    /*出错*/
		    printf("recv error");
		}
	    }
	}
    }

    return 0;
}

/*
 * recevive packet
 * 
 * ke.huato
 *
 */

static int recv_packet(int fd, char *frame, int max_length, unsigned long msec)
{
    int n=0;
#if 0
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timerclear(&tv);
    tv.tv_usec = msec;
    n = select(fd + 1, &readfds, NULL, NULL, &tv);
    if (n > 0)
    {
#endif
	/*recv frome socket*/
	n = recv(fd, frame, max_length, 0);
#if 0
    }
#endif

    return n;
}

static void paket_process(char *frame, int len)
{
    char *pd=NULL;
    int pull_len=0;
    short tmp = 0;
    system_frame_header_t header;

	uchar mac[MAC_LEN];
	char *name=NULL;

    memset(mac, 0, sizeof(mac));
    name = getLanIfName();
    get_mac(name, mac);
	/*remove the ether header*/
    memset(&header, 0, sizeof(system_frame_header_t));
    pull_len = sw_frame_header_pull(frame, &header);
    header.length = len - pull_len;
	if(header.data != NULL)
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

}



/*
 *create thread
 *KeHuatao
 */
static int module_create(void)
{
    int ret;

    ret = pthread_create(&urcp_thread, (pthread_attr_t*)NULL, urcp_tsk, (void*)NULL);
    if(ret != 0)              
    {
	printf("Create pthread error!\n");
    }
    
    return  ret;
}
