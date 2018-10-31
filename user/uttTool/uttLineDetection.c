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
 * getOldLinkStatus - ��ȡԭ������·״̬
 *
 * ͨ��WAN��������ȡ����·ԭ����״̬
 * 
 * ������
 *	num	-	WAN������
 *
 * ����ֵ��
 *	ret	-	��·ԭ����״̬
 *	0	-	��·ԭ���Ͽ�
 *	1	-	��·ԭ��������
 *
 */
static int getOldLinkStatus(unsigned int num)
{
	int ret = 0;
	FILE *fp = NULL;
	char command[30];
	char buffer[10];

	/*�����ʼ��*/
	memset(command, 0, 30);
	memset(buffer, 0, 10);
	sprintf(command, "cat /var/run/Wan%dlink.status", num);

	/*��ͨ��*/
	if ((fp = popen(command, "r")) != NULL) {
		if (fgets(buffer, sizeof(buffer), fp) != NULL) 
			ret = (int)strtol(buffer, 0, 10);
		pclose(fp);/*�ر�ͨ��*/
	}
	return ret;
}

/*
 * utt_begin_check - ��·��⺯��
 *
 * ����socket��ѭ������ping����ÿ�η���
 * ���ͳ�ƶ����ʣ������ݶ����ʾ���down
 * ������up�ӿ�
 * 
 * ������
 *	num	-	WAN������
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
	/*���Ϊ0�򲻼��*/
	if (!(Line[num].interval))
		return;
	/*����ʹ��ICMP��ԭʼ�׽���*/
	if ((sockfd = socket(AF_INET, SOCK_RAW, 1)) < 0) {/* 1 == ICMP */
		perror("socket error");
		return;
	}

	/*�����׽��ֽ��ջ�������10K*/
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
	/*�󶨵��ӿ�*/
	setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, Line[num].wanIfName, strlen(Line[num].wanIfName) + 1);
	/*�趨���ճ�ʱʱ��*/
	uttRcvTimeout.tv_sec = 1;
	uttRcvTimeout.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&uttRcvTimeout, sizeof(uttRcvTimeout));

	/*��ȡ����id,��������ICMP�ı�־��*/
	pid = getpid();

	/*��ʼѭ������*/
	while (1) {
		/*���㶪����*/
		loss_rate = utt_ping(sockfd, pid, Line[num].interval, Line[num].count, Line[num].dstip);
		/*ȡ����·ԭ����״̬*/
		oldLinkStatus = getOldLinkStatus(num);
		SWORD_PRINTF("%s-%d, num = %d, loss_rate = %d, oldLinkStatus = %d\n", 
				__func__, __LINE__, num, loss_rate, oldLinkStatus);
		if ((loss_rate == 100) && (oldLinkStatus == 1)) {
			/*������Ϊ100%��ԭ����������״̬��down���ӿ�*/
			SWORD_PRINTF("%s-%d, down\n", __func__, __LINE__);
			sprintf(command, "snd2speed down %d", num); 
			system(command);
		} else if ((loss_rate < 90) && (oldLinkStatus == 0)) {
			/*������С��90%��ԭ���ǶϿ�״̬��up�ӿ�*/
			SWORD_PRINTF("%s-%d, up\n", __func__, __LINE__);
			sprintf(command, "snd2speed up %d", num); 
			system(command);
		}
	}
	/*�ر�socket�������������Զ�����ߵ�*/
	close(sockfd);
	return;
}

/*
 * thread - �߳�ִ�к���
 *
 * �����̺߳��߳�ִ�д˺���ȥ
 * ���ü�⺯����ʼ���
 *
 * ������
 *	i	-	WAN�������ĵ�ַ
 *
 */
static void thread(void *i)
{
	unsigned int num;
	num = *(int *)i;/*ȡ��WAN������*/

	/*���ÿ�ʼ��⺯��*/
	utt_begin_check(num);
	return;
}

/*
 * setLinePara - ������������浽ȫ�ֽṹ��Line��
 *
 * ���ڴ˽��̿��ܴ�������̣߳�Ϊ�˷�ֹ�̼߳�����
 * �������ʴ�����һ��Lineȫ�ֽṹ�����ݷֱ�������
 * ���Ե����ݣ��±���WAN������������
 *
 * �˺������������������Line��
 *
 * ������
 *	num		-	WAN������
 *	wanIfName	-	WAN�ڽӿ���
 *	interval	-	�����
 *	count		-	������
 *	dstip		-	���ip
 *
 */
static void setLinePara(unsigned int num, char *wanIfName, char *interval, char *count, char *dstip)
{
	/*��������ر�����ȫ�ֽṹ��*/
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
 * main - ��·������
 *
 * ���û�������·����˽���(�����������߳�)����
 * ping�����ӿڶ�Ӧ��·�Ƿ�ͨ
 * ��ֻ��һ����·��Ҫ���ʱ����������ping�����ɣ�
 * ��������������·ʱ(���蹲��n����·):
 *	1. ����n-1���߳�(ÿһ����Ӧһ����·)
 *	2. ���̱�����ʣ�µ���·
 *
 * ������
 *            0          1 2 3   4    5 6      7        8    9 10     11 
 *	uttLineDetection 2 1 2 eth2.2 1 10 134744072 eth2.3  2 11 134743044
 *	����1�����������·����������2��3���������·��������
 *	����4��8��������·��wan����������5��9������·�����
 *	����6��10������·������������7��11������·���ip
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
			/*ÿ��������Ӧ��wan������*/
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
	/*ÿ��������Ӧ��wan������*/
	j = (i - 1) * 4 + num + 2; 
	setLinePara(k, argv[j], argv[j + 1], argv[j + 2], argv[j + 3]);
	utt_begin_check(k);

	SWORD_PRINTF("leave main\n");
	exit(0);
}

