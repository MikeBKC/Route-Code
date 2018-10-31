#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<errno.h>
#include<unistd.h>
#include <sys/socket.h>

#define SWORD_DEB 0
#if SWORD_DEB
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args);
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define MAX_WAN_IF_NUM 6
#define AVERAGE_COUNT 10
extern int utt_ping(int sockfd, int pid, long interval, long count, unsigned int dstip);
typedef void* (*fun)(void*);

struct LineProf {
	char wanIfName[16];
	long interval;
	long count;
	unsigned int dstip;
};
struct LineProf Line[MAX_WAN_IF_NUM + 1];
static int wannum[MAX_WAN_IF_NUM];
static pthread_t tid, tid1, tid2, tid3, tid4, tid5, tid6;

/*
 * getOldLinkStatus - 获取原来的线路状态
 *
 * 通过WAN口索引获取此线路原来的状态
 * 
 * 参数：
 *	num	-	WAN口索引
 *
 * 返回值：
 *	ret	-	线路原来的状态
 *	0	-	线路原来断开
 *	1	-	线路原来是连接
 *
 */
static int getOldLinkStatus(unsigned int num)
{
	int ret = 0;
	FILE *fp = NULL;
	char command[30];
	char buffer[10];

	/*数组初始化*/
	memset(command, 0, 30);
	memset(buffer, 0, 10);
	sprintf(command, "cat /var/run/Wan%dlink.status", num);

	/*打开通道*/
	if ((fp = popen(command, "r")) != NULL) {
		if (fgets(buffer, sizeof(buffer), fp) != NULL) 
			ret = (int)strtol(buffer, 0, 10);
		pclose(fp);/*关闭通道*/
	}
	return ret;
}

/*
 * utt_begin_check - 线路检测函数
 *
 * 创建socket并循环发送ping包，每次发送
 * 完后统计丢包率，并根据丢包率决定down
 * 掉或者up接口
 * 
 * 参数：
 *	num	-	WAN口索引
 *
 */
static void utt_begin_check(unsigned int num)
{
	char command[20];
	int oldLinkStatus, sockfd;
	int loss_rate, pid;
	struct timeval uttRcvTimeout;
	int size = 10 * 1024;

	memset(command, 0, 20);
	/*间隔为0则不检测*/
	if (!(Line[num].interval))
		return;
	/*生成使用ICMP的原始套接字*/
	if ((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {/* 1 == ICMP */
		perror("socket error");
		return;
	}

	/*设置套接字接收缓冲区到10K*/
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	/*绑定到接口*/
	setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, Line[num].wanIfName, strlen(Line[num].wanIfName) + 1);
	/*设定接收超时时间*/
	uttRcvTimeout.tv_sec = 1;
	uttRcvTimeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&uttRcvTimeout, sizeof(uttRcvTimeout));

	/*获取进程id,用于设置ICMP的标志符*/
	pid = getpid();

	/*开始循环发包*/
	while (1) {
		/*计算丢包率*/
		loss_rate = utt_ping(sockfd, pid, Line[num].interval, Line[num].count, Line[num].dstip);
		/*取得线路原来的状态*/
		oldLinkStatus = getOldLinkStatus(num);
		SWORD_PRINTF("%s-%d, num = %d, loss_rate = %d, oldLinkStatus = %d\n", 
				__func__, __LINE__, num, loss_rate, oldLinkStatus);
		if ((loss_rate == 100) && (oldLinkStatus == 1)) {
			/*丢包率为100%且原来是已连接状态则down掉接口*/
			SWORD_PRINTF("%s-%d, down\n", __func__, __LINE__);
			sprintf(command, "snd2speed down %d", num); 
			system(command);
		} else if ((loss_rate < 90) && (oldLinkStatus == 0)) {
			/*丢包率小于90%且原来是断开状态则up接口*/
			SWORD_PRINTF("%s-%d, up\n", __func__, __LINE__);
			sprintf(command, "snd2speed up %d", num); 
			system(command);
		}
	}
	/*关闭socket，正常情况下永远不会走到*/
	close(sockfd);
	return;
}

/*
 * thread - 线程执行函数
 *
 * 创建线程后线程执行此函数去
 * 调用检测函数开始检测
 *
 * 参数：
 *	i	-	WAN口索引的地址
 *
 */
static void thread(void *i)
{
	unsigned int num;
	num = *(int *)i;/*取得WAN口索引*/

	/*调用开始检测函数*/
	utt_begin_check(num);
	return;
}

/*
 * setLinePara - 将传入参数保存到全局结构体Line中
 *
 * 由于此进程可能创建多个线程，为了防止线程间数据
 * 独立，故创建了一个Line全局结构体数据分别来保存
 * 各自的数据，下标由WAN口索引来决定
 *
 * 此函数将传入参数保存至Line中
 *
 * 参数：
 *	num		-	WAN口索引
 *	wanIfName	-	WAN口接口名
 *	interval	-	检测间隔
 *	count		-	检测次数
 *	dstip		-	检测ip
 *
 */
static void setLinePara(unsigned int num, char *wanIfName, char *interval, char *count, char *dstip)
{
	/*保存检测相关变量到全局结构体*/
	strcpy(Line[num].wanIfName, wanIfName); 
	Line[num].interval = strtol(interval, 0, 10);
	Line[num].count = strtol(count, 0, 10);
	Line[num].dstip = (unsigned int) strtoul(dstip, 0, 10);
	SWORD_PRINTF("%s-%d, Line[%d]:wanIfName = %s, interval = %d, count = %d, dstip = %x\n",
			__func__, __LINE__, num, Line[num].wanIfName, 
			Line[num].interval, Line[num].count, Line[num].dstip);
	return;
}

/*
 * main - 线路检测进程
 *
 * 当用户配置线路检测后此进程(及其所创建线程)发送
 * ping包检测接口对应线路是否畅通
 * 当只有一条线路需要检测时进程自身发送ping包即可，
 * 当两条及以上线路时(假设共有n条线路):
 *	1. 创建n-1个线程(每一个对应一条线路)
 *	2. 进程本身检查剩下的线路
 *
 * 参数：
 *            0          1 2 3   4    5 6      7        8    9 10     11 
 *	uttLineDetection 2 1 2 eth2.2 1 10 134744072 eth2.3  2 11 134743044
 *	参数1代表需检测的线路条数，参数2和3代表检测的线路的索引号
 *	参数4和8代表检测线路的wan口名，参数5和9代表线路检测间隔
 *	参数6和10带便线路检测次数，参数7和11代表线路检测ip
 *
 */
int main(int argc, char** argv)
{
	SWORD_PRINTF("enter main\n");
	int rc1, rc2, rc3, rc4, rc5, rc6;
	int i = 1, j, k, l, num;

	rc1 = rc2 = rc3 = rc4 = rc5 = rc6 = 0;
	num = strtol(argv[1], 0, 10);

	if (num > 1) {
		for (i = 1; i < num; i++) {
			k = strtol(argv[i + 1], 0, 10);
			wannum[k] = k;
			/*每次跳到相应的wan口名处*/
			j = (i - 1) * 4 + num + 2; 
			setLinePara(k, argv[j], argv[j + 1], argv[j + 2], argv[j + 3]);
			switch (k) {
				case 1:
					rc1 = pthread_create(&tid1, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid1;
					if(rc1 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc1));
					break;
				case 2:
					rc1 = pthread_create(&tid2, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid2;
					if(rc2 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc2));
					break;
				case 3:
					rc3 = pthread_create(&tid3, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid3;
					if(rc3 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc3));
					break;
				case 4:
					rc4 = pthread_create(&tid4, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid4;
					if(rc4 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc4));
					break;
				case 5:
					rc5 = pthread_create(&tid5, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid5;
					if(rc5 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc5));
					break;
				case 6:
					rc6 = pthread_create(&tid6, NULL, (void*)thread, (void *)&wannum[k]);
					tid = tid6;
					if(rc6 != 0)
						SWORD_PRINTF("%s: %d\n",__func__, strerror(rc6));
					break;
				default:/*default*/
					break;
			}
		}
	} 
	k = strtol(argv[i + 1], 0, 10);
	/*每次跳到相应的wan口名处*/
	j = (i - 1) * 4 + num + 2; 
	setLinePara(k, argv[j], argv[j + 1], argv[j + 2], argv[j + 3]);
	utt_begin_check(k);

	SWORD_PRINTF("leave main\n");
	exit(0);
}

