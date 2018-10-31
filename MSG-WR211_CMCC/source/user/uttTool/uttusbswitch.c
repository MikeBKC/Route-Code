#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <sys/un.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <linux/types.h> 
#include <linux/netlink.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "uttMachine.h"

#if (WIFIDOG_AUTH == FYES )
//#if (SMS_ENABLE == 1)

#define SWORD_DEBUG 1
#if SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define UEVENT_BUFFER_SIZE 4096

/*
 * init_hotplug_sock - 初始化热插拔socket
 *
 * 现在是需要读取proc文件下的usb状态查看是否有3G卡插上
 * 然后通过判断是否有拨号进程来判断3G拨号状态
 *
 * 返回值：
 * sock_fd   -     socket描述符
 */
static int init_hotplug_sock(void) 
{ 
    struct sockaddr_nl snl; 
    int buffersize = UEVENT_BUFFER_SIZE / 4; 
    int sock_fd; 

    bzero(&snl, sizeof(struct sockaddr_nl)); 
    snl.nl_family = AF_NETLINK; 
    snl.nl_pid = getpid(); 
    snl.nl_groups = 1; 

    /*建立socket*/
    sock_fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT); 
    if (sock_fd == -1) {
	perror("socket error !\n"); 
	return -1; 
    } 

    /*设置接收缓冲区*/
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, 
		(const char *)&buffersize, sizeof(int)) == -1) {
	perror("setsockopt error !\n"); 
	return -1;
    }

    /*绑定socket*/
    if (bind(sock_fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) == -1) {
	perror("bind error !\n"); 
	close(sock_fd); 
	return -1; 
    } 

    return sock_fd; 
}


/*
 * utt_3g_cat - 从设备文件中读取3G卡厂商名和产品号
 *
 * 现在是需要读取proc文件下的usb状态查看是否有3G卡插上
 * 然后通过判断是否有拨号进程来判断3G拨号状态
 * 若有能识别的卡，则将厂商名和产品号存入输入参数中
 *
 * 输入参数：
 *  vendor - 厂商名
 *  prodid - 设备id
 * 返回值：
 *  0     3G卡未插或不能识别
 *  1     存在能够识别的3G卡
 */
int utt_3g_cat(char *vendor ,char *prodid)
{
    FILE *fp = NULL;
    char *p = NULL;
    int count = 0, ret = 0;
    char buffer[256];
    char command[100];

    /*数组初始化*/
    memset(buffer, 0, sizeof(buffer));
    memset(command, 0, sizeof(command));

    strcpy(command,"cat /proc/bus/usb/devices");
    fp = popen(command,"r");/*执行命令*/
    if (fp != NULL) {/*执行成功*/
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
#if(KERNEL_2_6_36 == FYES)
	    p = strstr(buffer, "Vendor=");
	    if(p != NULL) {
		p = p + 7;/*跳到厂商名处*/
		memcpy(vendor, p, 4);
		p = p + 12;/*跳到产品号处*/
		memcpy(prodid, p, 4);
	    } else {
		p = strstr(buffer, "Manufacturer=");
		if (p != NULL) {
		    /* Manufacturer不为ReOS 即为3G卡 */
		    if (memcmp(p + 13, "ReOS SE", 7) != 0) {
			ret = 1;
			break;
		    } else {
			memset(vendor, 0, 10);
			memset(prodid, 0, 10);
		    }
		}
	    }
#else
	    p = strstr(buffer,"Vendor=");
	    if(p != NULL) {
		count++;/*计算Vendor个数*/
		p = p + 7;/*跳到厂商名处*/
		if(count == 2) {/*第一个厂商名跳过，因为默认有一个为0的*/
		    memcpy(vendor, p, 4);
		    p= p + 12;/*跳到产品号处*/
		    memcpy(prodid, p, 4);
		}	  
	    }
#endif
	}
	pclose(fp);
    }

#if(KERNEL_2_6_36 == FYES)
#else
    if(count != 1)
    {
	ret = 1;
    }
#endif
    return ret;
}
#if 0
#define MAX_PS_STR_LEN 2000
/*
 * test_comgt_sleep - 杀pppd进程前检查是否有3G拨号进程
 *
 * 由于在3G拨号时杀掉此进程会导致goahead等进程死掉，故
 * 在发现有3G拨号后睡眠3秒后再杀pppd进程
 *
 * 返回值：
 *	0	-	没有3G拨号进程了
 *	1	-	仍有拨号进程
 *
 */
static int test_comgt_sleep(void)
{
	FILE *fp;
	char *p = NULL;
	char ps_str[MAX_PS_STR_LEN + 1];
	int ret = 0;

	fp = popen("ps", "r");/*执行命令*/
	if (fp != NULL) {/*执行成功*/
		memset(ps_str, 0, MAX_PS_STR_LEN + 1);
		fread(ps_str, 1, MAX_PS_STR_LEN, fp);
		/*若此时3G正在拨号，则睡眠等待3秒后再杀pppd进程，
		 * 否则可能造成mantis11609的bug*/
		SWORD_PRINTF("%s-%d:ps_str = \n%s\n\n", __func__, __LINE__, ps_str);
		if ((p = strstr(ps_str, "/bin/comgt -d")) != NULL) { 
			SWORD_PRINTF("%s-%d:有3G进程正在拨号，睡眠3秒\n", __func__, __LINE__);
			ret = 1;
			sleep(3u);
		}
		pclose(fp);
	}
	return ret;
}
#endif
/*
 * kill_smssend - 3G卡拔出后杀pppd进程
 *
 * 由于现在有了自动检测热插拔，因此没有必要在3G
 * 卡拔出后继续拨号
 *
 */
static void kill_smssend(void)
{
	FILE *fp;
	char *p = NULL;
	char command[20];
	char buffer[50];
	char pid[10];

#if 0
	/*直到没有3G拨号进程才杀pppd进程*/
	while(test_comgt_sleep())
		;
#endif
	fp = popen("ps", "r");/*执行命令*/
	if (fp != NULL) {/*执行成功*/
		memset(buffer, 0, 50);
		while (fgets(buffer, sizeof(buffer), fp) != NULL) {
			if ((p = strstr(buffer,"smssend")) != NULL) {
				memset(pid, 0, 10);
				memset(command, 0, 20);
				sscanf(buffer, "%s", pid);
				SWORD_PRINTF("%s-%d:pid = %s\n", __func__, __LINE__, pid);
				sprintf(command, "kill -9 %s", pid);
				SWORD_PRINTF("%s-%d:command = %s\n", __func__, __LINE__, command);
				system(command);
				SWORD_PRINTF("%s-%d:command = %s\n", __func__, __LINE__, command);
			}
		}
		pclose(fp);
	}
	return;
}
/*
 * auto_dial_3g - 3G卡自动拨号
 *
 * 当此进程收到来自内核通过netlink传来的数据后开始自动拨号
 *
 * 输入参数：
 *  Vendor_last - 上次备份的厂商名
 *  ProdID_last - 上次备份的产品号
 *
 */
static void auto_dial_3g(char *Vendor_last, char *ProdID_last)
{
    int ret = -1;
    char ProdID[10];
    char Vendor[10];

    /*数组初始化*/
    memset(Vendor, 0, sizeof(Vendor));
    memset(ProdID, 0, sizeof(ProdID));

    /*获取厂商号和产品号*/
    ret = utt_3g_cat(Vendor ,ProdID);	

    SWORD_PRINTF("%s-%d:Vendor_last = %s, ProdID_last = %s\n", 
	    __func__, __LINE__, Vendor_last, ProdID_last);
    SWORD_PRINTF("%s-%d:Vendor = %s, ProdID = %s, ret = %d\n", 
	    __func__, __LINE__, Vendor, ProdID, ret);

    if((ret == 1) && (*Vendor != '\0') && (*ProdID != '\0') 
	    &&((strcmp(Vendor_last,Vendor) != 0) 
		|| (strcmp(ProdID_last,ProdID) != 0))) {
	/*与上一次检测到的厂商名或产品号不同则开始拨号*/
	sleep(3u);/*拨号前睡眠一下，等待内核检测完毕*/
	system("/sbin/usb_modeswitch.sh");/*执行拨号脚本*/
	if(utt_3g_cat(Vendor ,ProdID) == 1) {
	    /*备份上次拨号的厂商名和产品号*/
	    SWORD_PRINTF("%s-%d:Vendor = %s, ProdID = %s\n",
		    __func__, __LINE__, Vendor, ProdID);
	    strcpy(Vendor_last,Vendor);
	    strcpy(ProdID_last,ProdID);
	}
    } else if((ret == 0) && (*Vendor == '\0') && (*ProdID == '\0')
	    && (*Vendor_last != '\0') && (*ProdID_last != '\0')) { 
	/*usb拔出时杀掉拨号进程并清空备份*/
	SWORD_PRINTF("%s-%d:usb has been removed !\n",
		__func__, __LINE__);
	kill_smssend();
	memset(Vendor_last, 0, sizeof(Vendor_last));
	memset(ProdID_last, 0, sizeof(ProdID_last));
	sleep(2u);/*杀掉进程后睡眠一下，等待内核处理完usb拔出*/
    } else {
	/*test*/
    }
    return;
}

/*
 * main - 3G卡自动检测拨号主函数
 *
 * 当用户配置3G拨号后启用此进程检测3G卡的热插拔
 * 若3G卡插上则开始拨号
 *
 */
int main(int argc, char* argv[]) 
{ 
    int hotplug_sock; 
    fd_set fds;
    int select_ret = -1;
    char buf[UEVENT_BUFFER_SIZE];
    char ProdID_last[10];
    char Vendor_last[10];

    /*数组初始化*/
    memset(ProdID_last, 0, sizeof(ProdID_last));
    memset(Vendor_last, 0, sizeof(Vendor_last));
    memset(buf, 0, UEVENT_BUFFER_SIZE);

    hotplug_sock = init_hotplug_sock();
    if (hotplug_sock == -1) {
	return -1;
    }
    utt_3g_cat(Vendor_last ,ProdID_last);
    while (1) {
	printf("%s-%d: =====\n",__func__,__LINE__);
	FD_ZERO(&fds);/*清空fds集合*/
	FD_SET(hotplug_sock, &fds);/*将hotplug_sock加入fds集合*/
	select_ret = select(hotplug_sock + 1, &fds, NULL, NULL, NULL);
	if (select_ret == -1) {/*select失败*/
	    printf("select failed !!!\n");
	    return -1;			
	}
	if (select_ret == 0) {/*select超时*/
	    sleep(1u);
	}
	if (FD_ISSET(hotplug_sock, &fds)) {/*检测hotplug_sock是否在fds集合中*/
	    recv(hotplug_sock, &buf, sizeof(buf), 0); 
	}
	if (*buf != '\0') {/*接收到了数据*/
	    SWORD_PRINTF("%s-%d:recive :%s\n", __func__, __LINE__, buf);
	    if (strstr(buf, "usb") != NULL) {
		    auto_dial_3g(Vendor_last, ProdID_last);
	    }
	}
	memset(buf, 0, sizeof(buf));
	sleep(3);
    }
    return 0; 
}
//#endif
#else
/*
 * main - 3G卡自动检测拨号主函数
 *
 * 当用户配置3G拨号后启用此进程检测3G卡的热插拔
 * 若3G卡插上则开始拨号
 *
 */
int main(int argc, char** argv)
{
    return 0;
}
#endif
