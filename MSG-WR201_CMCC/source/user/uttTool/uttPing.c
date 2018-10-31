#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#define DATA_LEN  32
#define MAX_WAIT_TIME 50
#define PACKET_SIZE     1024


#define SWORD_DEB 0
#if SWORD_DEB
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args);
#else
#define SWORD_PRINTF(fmt, args...)
#endif

/*
 * statistics - ͳ�ƶ�����
 *
 * ���ݷ��������հ���ͳ�ƶ����ʲ����ѽ���
 * ��Ϊ1���հ�����ֹͣ�հ�
 *
 * ���������
 *	nsend		-	���Ͱ���
 *	nreceived	-	���հ���
 *
 * ����ֵ��
 *	loss_rate	-	������
 * 
 */
static int statistics(int nsend, int nreceived)
{
	int loss_rate;

	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
			__func__, __LINE__, nsend, nreceived);
	/*���Ͱ�Ϊ0��ֱ�ӷ���*/
	if (!nsend)
		loss_rate = 0;
	else 
		loss_rate = (nsend - nreceived) * 100 / nsend; /*���㶪����*/

	SWORD_PRINTF("%s-%d, loss_rate = %d\n", __func__, __LINE__, loss_rate);

	return loss_rate;
} 

/*
 * cal_chksum - ����У���
 *
 * ���ݰ����ݼ���������
 * 
 * ������
 *	addr		-	�����ݵĵ�ַ
 *	len		-	������
 *
 * ����ֵ��
 *	answer		-	У���
 *
 */
unsigned short cal_chksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	/*��ICMP��ͷ������������2�ֽ�Ϊ��λ�ۼ�����*/
	while (nleft > 1)
	{
		sum +=  *w++;
		nleft -= 2;
	}
	/*��ICMP��ͷΪ�������ֽڣ���ʣ�����һ�ֽڡ������һ���ֽ���Ϊһ��2�ֽ����ݵĸ��ֽڣ����2�ֽ����ݵĵ��ֽ�Ϊ0�������ۼ�*/
	if (nleft == 1)
	{
		*(unsigned char*)(&answer) = *(unsigned char*)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum &0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

/*
 * pack - ���
 *
 * ���ݰ��źͽ���id���
 * 
 * ������
 *	pack_no		-	������
 *	sendpacket	-	������
 *	pid		-	���̺�
 *
 * ����ֵ��
 *	packsize	-	��װ���İ��ĳ���
 *
 */
static int pack(int pack_no, char *sendpacket, int pid)
{
	int packsize;
	struct icmp *icmph;

	icmph = (struct icmp*) sendpacket;
	icmph->icmp_type = ICMP_ECHO;
	icmph->icmp_code = 0;
	icmph->icmp_cksum = 0;
	icmph->icmp_seq = htons(pack_no);
	icmph->icmp_id = htons(pid);
	packsize = 8 + DATA_LEN;
	icmph->icmp_cksum = cal_chksum((unsigned short*)icmph, packsize); /*У���㷨*/

	return packsize;
}

/*
 * send_packet - ����
 *
 * ����socket���������Ͱ�
 * 
 * ������
 *	sockfd		-	socket������
 *	nsend		-	�ѷ��Ͱ������ĵ�ַ
 *	interval	-	���Ͱ����
 *	dstip		-	���Ͱ���Ŀ��ip
 *	pid		-	���̺�
 *
 * ����ֵ��
 *	ret		-	���ͳɹ�����ʧ��
 *	0		-	����ʧ��
 *	1		-	���ͳɹ�
 *
 */
static int send_packet(int sockfd, int nsend, struct sockaddr_in *dest_addr, int pid)
{
	char sendpacket[PACKET_SIZE];
	int packetsize, ret = 1;

	SWORD_PRINTF("%s-%d, begin send, pid = %d\n", __func__, __LINE__, pid);
	packetsize = pack(nsend, sendpacket, pid); /*����ICMP��ͷ*/
	if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*)
				dest_addr, sizeof(*dest_addr)) < 0) {
		ret = 0;
	} 
	return ret;
}

/*
 * unpack - ���
 *
 * ������жϴ˰��Ƿ����Լ��������ķ��ذ�
 * 
 * ������
 *	buf	-	���հ�����
 *	len	-	���հ��ĳ���
 *	pid	-	���̺�p
 *
 * ����ֵ��
 *	ret	-	�Ƿ����Լ��������ķ��ذ�
 *	0	-	�����Լ��������ķ��ذ�
 *	1	-	���Լ��������ķ��ذ�
 *
 */
static int unpack(char *buf, int len, int pid)
{
	int ret = -1;
	int iphdrlen;
	struct ip *iph;
	struct icmp *icmph;
	double rtt;

	iph = (struct ip*) buf;
	iphdrlen = iph->ip_hl << 2; /*��ip��ͷ����,��ip��ͷ�ĳ��ȱ�־��4*/
	icmph = (struct icmp*)(buf + iphdrlen); /*Խ��ip��ͷ,ָ��ICMP��ͷ*/
	len -= iphdrlen; /*ICMP��ͷ��ICMP���ݱ����ܳ���*/

	/*С��ICMP��ͷ�����򲻺���*/
	if (len < 8) {
		SWORD_PRINTF("ICMP packets\'s length is less than 8\n");
	} else { 

		SWORD_PRINTF("%s-%d, icmph->icmp_type = %d, ICMP_ECHOREPLY = %d, htons(icmph->icmp_id) = %d, pid = %d\n", __func__, __LINE__, icmph->icmp_type, ICMP_ECHOREPLY, htons(icmph->icmp_id), pid);
		/*ȷ�������յ����������ĵ�ICMP�Ļ�Ӧ*/
		if ((icmph->icmp_type == ICMP_ECHOREPLY) && (htons(icmph->icmp_id) == pid))
			ret = 0;
	}

	SWORD_PRINTF("%s-%d, ret = %d\n", __func__, __LINE__, ret);
	return ret;
}

/*
 * recv_packet - ����icmp��
 *
 * ͨ��socket����������icmp��
 * 
 * ������
 *	sockfd		-	socket������
 *	nreceived	-	�ѽ��յİ�����
 *	pid		-	���̺�p
 *
 * ����ֵ:
 *	ret		-	�����Ƿ�ɹ�
 *	1		-	����ʧ��
 *	0		-	���ճɹ�
 *
 */
static int recv_packet(int sockfd, int *nreceived, int pid)
{
	struct sockaddr_in from;
	int n, fromlen, ret = 0;
	char recvpacket[PACKET_SIZE];
	extern int errno;

	fromlen = sizeof(from);
	/* 
	 * recvfrom�յ�����ip�����ϵ����ݣ�recvֻ���յ�Ӧ�ò�����ݣ����ܷ���
	 * �Ƿ�����ȷ�ķ��ذ�,��˴˴�������recvfrom
	 */
	if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0,
					(struct sockaddr*) &from, &fromlen)) < 0) {
		ret = 1;
	} 
	/*����ж��Ƿ�����ȷ�ķ��ذ�*/
	if (unpack(recvpacket, n, pid) ==  0)
		++(*nreceived);
	SWORD_PRINTF("%s-%d, *nreceived = %d\n", __func__, __LINE__, *nreceived);
	return ret;
}

/*
 * utt_ping - ping�����·
 *
 * ����icmp��Ȼ����գ������㶪����
 * 
 * ������
 *	sockfd		-	socket������
 *	pid		-	���̺�
 *	interval	-	�����
 *	count		-	������
 *	dstip		-	���ip
 *
 * ����ֵ��
 *	loss_rate	-	��·������
 *
 */
int utt_ping(int sockfd, int pid, long interval, long count, unsigned int dstip)
{
	struct sockaddr_in dest_addr;
	int nsend = 0, nreceived = 0, loss_rate = 0;
	int flags;
	long timeout;

	/*����Ŀ�ĵ�ַ*/
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = dstip;

	/*�����׽���Ϊ������ģʽ*/
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	while (nsend < count) {
		nsend += send_packet(sockfd, nsend, &dest_addr, pid); /*��������ICMP����*/
		sleep(interval); /*ÿ��interval�뷢��һ��ICMP����*/
		recv_packet(sockfd, &nreceived, pid); /*��������ICMP����*/
		SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
				__func__, __LINE__, nsend, nreceived);
	}
	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
			__func__, __LINE__, nsend, nreceived);

	/*�����׽���Ϊ����ģʽ*/
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);

	/*���ó�ʱʱ��*/
	timeout = interval * count;
	/*���ȴ�MAX_WAIT_TIME�룬������Ϊ��Զ����Ӧ��*/
	if (timeout > MAX_WAIT_TIME)
		timeout = MAX_WAIT_TIME;
	while ((nreceived < count) && timeout) {
		SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d, timeout = %d\n",
				__func__, __LINE__, nsend, nreceived, timeout);
		timeout -= recv_packet(sockfd, &nreceived, pid); /*��������ICMP����*/
	}

	loss_rate = statistics(nsend, nreceived); /*����ͳ��*/
	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d, loss_rate = %d\n",
			__func__, __LINE__, nsend, nreceived, loss_rate);

	return loss_rate;
}
