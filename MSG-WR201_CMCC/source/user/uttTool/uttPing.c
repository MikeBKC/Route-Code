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
 * statistics - 统计丢包率
 *
 * 根据发包数和收包数统计丢包率并将已结束
 * 置为1让收包函数停止收包
 *
 * 输入参数：
 *	nsend		-	发送包数
 *	nreceived	-	接收包数
 *
 * 返回值：
 *	loss_rate	-	丢包率
 * 
 */
static int statistics(int nsend, int nreceived)
{
	int loss_rate;

	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
			__func__, __LINE__, nsend, nreceived);
	/*发送包为0则直接返回*/
	if (!nsend)
		loss_rate = 0;
	else 
		loss_rate = (nsend - nreceived) * 100 / nsend; /*计算丢包率*/

	SWORD_PRINTF("%s-%d, loss_rate = %d\n", __func__, __LINE__, loss_rate);

	return loss_rate;
} 

/*
 * cal_chksum - 计算校验和
 *
 * 根据包内容计算包检验和
 * 
 * 参数：
 *	addr		-	包内容的地址
 *	len		-	包长度
 *
 * 返回值：
 *	answer		-	校验和
 *
 */
unsigned short cal_chksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	/*把ICMP报头二进制数据以2字节为单位累加起来*/
	while (nleft > 1)
	{
		sum +=  *w++;
		nleft -= 2;
	}
	/*若ICMP报头为奇数个字节，会剩下最后一字节。把最后一个字节视为一个2字节数据的高字节，这个2字节数据的低字节为0，继续累加*/
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
 * pack - 组包
 *
 * 根据包号和进程id组包
 * 
 * 参数：
 *	pack_no		-	包号码
 *	sendpacket	-	包内容
 *	pid		-	进程号
 *
 * 返回值：
 *	packsize	-	组装出的包的长度
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
	icmph->icmp_cksum = cal_chksum((unsigned short*)icmph, packsize); /*校验算法*/

	return packsize;
}

/*
 * send_packet - 发包
 *
 * 根据socket描述符发送包
 * 
 * 参数：
 *	sockfd		-	socket描述符
 *	nsend		-	已发送包个数的地址
 *	interval	-	发送包间隔
 *	dstip		-	发送包的目的ip
 *	pid		-	进程号
 *
 * 返回值：
 *	ret		-	发送成功或者失败
 *	0		-	发送失败
 *	1		-	发送成功
 *
 */
static int send_packet(int sockfd, int nsend, struct sockaddr_in *dest_addr, int pid)
{
	char sendpacket[PACKET_SIZE];
	int packetsize, ret = 1;

	SWORD_PRINTF("%s-%d, begin send, pid = %d\n", __func__, __LINE__, pid);
	packetsize = pack(nsend, sendpacket, pid); /*设置ICMP报头*/
	if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*)
				dest_addr, sizeof(*dest_addr)) < 0) {
		ret = 0;
	} 
	return ret;
}

/*
 * unpack - 解包
 *
 * 解包并判断此包是否是自己所发包的返回包
 * 
 * 参数：
 *	buf	-	接收包内容
 *	len	-	接收包的长度
 *	pid	-	进程号p
 *
 * 返回值：
 *	ret	-	是否是自己所发包的返回包
 *	0	-	不是自己所发包的返回包
 *	1	-	是自己所发包的返回包
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
	iphdrlen = iph->ip_hl << 2; /*求ip报头长度,即ip报头的长度标志乘4*/
	icmph = (struct icmp*)(buf + iphdrlen); /*越过ip报头,指向ICMP报头*/
	len -= iphdrlen; /*ICMP报头及ICMP数据报的总长度*/

	/*小于ICMP报头长度则不合理*/
	if (len < 8) {
		SWORD_PRINTF("ICMP packets\'s length is less than 8\n");
	} else { 

		SWORD_PRINTF("%s-%d, icmph->icmp_type = %d, ICMP_ECHOREPLY = %d, htons(icmph->icmp_id) = %d, pid = %d\n", __func__, __LINE__, icmph->icmp_type, ICMP_ECHOREPLY, htons(icmph->icmp_id), pid);
		/*确保所接收的是我所发的的ICMP的回应*/
		if ((icmph->icmp_type == ICMP_ECHOREPLY) && (htons(icmph->icmp_id) == pid))
			ret = 0;
	}

	SWORD_PRINTF("%s-%d, ret = %d\n", __func__, __LINE__, ret);
	return ret;
}

/*
 * recv_packet - 接收icmp包
 *
 * 通过socket描述符接收icmp包
 * 
 * 参数：
 *	sockfd		-	socket描述符
 *	nreceived	-	已接收的包个数
 *	pid		-	进程号p
 *
 * 返回值:
 *	ret		-	接收是否成功
 *	1		-	接收失败
 *	0		-	接收成功
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
	 * recvfrom收到的是ip层以上的数据，recv只能收到应用层的数据，不能分析
	 * 是否是正确的返回包,因此此处必须用recvfrom
	 */
	if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0,
					(struct sockaddr*) &from, &fromlen)) < 0) {
		ret = 1;
	} 
	/*解包判断是否是正确的返回包*/
	if (unpack(recvpacket, n, pid) ==  0)
		++(*nreceived);
	SWORD_PRINTF("%s-%d, *nreceived = %d\n", __func__, __LINE__, *nreceived);
	return ret;
}

/*
 * utt_ping - ping检测线路
 *
 * 发送icmp包然后接收，最后计算丢包率
 * 
 * 参数：
 *	sockfd		-	socket描述符
 *	pid		-	进程号
 *	interval	-	检测间隔
 *	count		-	检测次数
 *	dstip		-	检测ip
 *
 * 返回值：
 *	loss_rate	-	线路丢包率
 *
 */
int utt_ping(int sockfd, int pid, long interval, long count, unsigned int dstip)
{
	struct sockaddr_in dest_addr;
	int nsend = 0, nreceived = 0, loss_rate = 0;
	int flags;
	long timeout;

	/*设置目的地址*/
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = dstip;

	/*设置套接字为非阻塞模式*/
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	while (nsend < count) {
		nsend += send_packet(sockfd, nsend, &dest_addr, pid); /*发送所有ICMP报文*/
		sleep(interval); /*每隔interval秒发送一个ICMP报文*/
		recv_packet(sockfd, &nreceived, pid); /*接收所有ICMP报文*/
		SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
				__func__, __LINE__, nsend, nreceived);
	}
	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d\n",
			__func__, __LINE__, nsend, nreceived);

	/*设置套接字为阻塞模式*/
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);

	/*设置超时时间*/
	timeout = interval * count;
	/*最多等待MAX_WAIT_TIME秒，否则认为永远不会应答*/
	if (timeout > MAX_WAIT_TIME)
		timeout = MAX_WAIT_TIME;
	while ((nreceived < count) && timeout) {
		SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d, timeout = %d\n",
				__func__, __LINE__, nsend, nreceived, timeout);
		timeout -= recv_packet(sockfd, &nreceived, pid); /*接收所有ICMP报文*/
	}

	loss_rate = statistics(nsend, nreceived); /*进行统计*/
	SWORD_PRINTF("%s-%d, nsend = %d, nreceived = %d, loss_rate = %d\n",
			__func__, __LINE__, nsend, nreceived, loss_rate);

	return loss_rate;
}
