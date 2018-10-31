/*
 * donot kill progress. 
 *
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdint.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include <signal.h>
#include "md5.h"
#include <time.h>
#include "log.h"
#include "uttMachine.h"
#include "mib.h"
#include "profacce.h"
#include "ifmisc.h"
#define CBC 1
#define ECB 1

#include "aes.h"

#define DEBUG 0

#define IP_LEN 15
#if DEBUG
#define MIN_SEC 1
#else
#define MIN_SEC 300
#endif
#define PAUSE_SEC 3
#define SN_LEN 15
#define SENDSTR_LEN (IP_LEN + SN_LEN + 1)
extern unsigned int getSystemSn(void);

/* error return -1 */
static int getDomain(char *str, char *sn) {
    char tmp1[TMP_LEN] = {0};
    char tmp2[TMP_LEN] = {0};
    MD5_CONTEXT md5ctx;
    unsigned char       hash[MD5_LEN];
    int i;
    time_t now;
    struct tm *tm_now;
    char date[TMP_LEN + 1] = {0};

    time(&now);
    tm_now = localtime((const time_t *)&now);

    snprintf(date, TMP_LEN, "%d%02d", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon);
#if 1
    if (strlen(date) != 6) {
	printf("get date error[%s]\n", date);
	return -1;
    }
#if 0
    if (strlen(sn) < 6) {
	printf("get sn error[%s]\n", sn);
	return -1;
    }
#endif
#endif
    strncpy(tmp1, sn, BATCH_LEN);
    snprintf(tmp2, TMP_LEN - 1, "%s%s", tmp1, date);
    MD5Init(&md5ctx);
    MD5Update(&md5ctx, tmp2, strlen(tmp2));
    MD5Final(hash, &md5ctx);
    for (i = 0; i < MD5_DOMAIN_LEN; i++) {
	sprintf(str, "%02x", hash[i]);
	str += 2;
    }
    strcat(str, ".f3322.net");
    return 0;

}

/* 加密字符串,aes一次加密16字符,分段加密 */
static void encrypt_str(unsigned char *out, int *outLen, char *in, int inLen) {
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };

    unsigned char decrypt[AES_CRYPT_LEN + 1]= {0};    
    char tmp[AES_CRYPT_LEN] = {0};
    char *inStr = in;
    unsigned char *outS = out;
    int i;
    *outLen = 0;
    for (i = 0; i < inLen; i += AES_CRYPT_LEN) {
	memset(tmp, 0, AES_CRYPT_LEN);
	memset(decrypt, 0, AES_CRYPT_LEN);
	strncpy(tmp, inStr, AES_CRYPT_LEN);
	AES128_ECB_encrypt((uint8_t *)tmp, key, decrypt);
	memcpy(outS, decrypt, AES_CRYPT_LEN);
	outS += AES_CRYPT_LEN;
	inStr += AES_CRYPT_LEN;
	*outLen += AES_CRYPT_LEN;
    }
}

/* 获取WAN口IP地址，按照1 ~ 4顺序，获取第一个正在连接的IP */
static int getWANIP(char *ipStr) {
    int wanCount = getWanSlotCount();	
    int i;
    char ip[IP_LEN + 1] = {0};
    for (i = 1; i <= wanCount; i++) {
	if (get_phylink_status(i) == 0) {
	    printf("wan[%d] donot link\n", i);
	    continue;
	}

	if (getIfIp(getWanIfNamePPP(i), ip) == 0) {
	    strcpy(ipStr, ip);
	    return 0;
	}
    }
    return -1;
}

int main(int argc, char** argv)
{
    int sockfd = -1;
    unsigned char sendline[MAXLINE];
    int sendLen = 0;
    struct sockaddr_in servaddr;
    char domain[DOMAIN_LEN + 1] = {0};
    struct hostent *host = NULL;
    char ip[IP_LEN + 1];
    unsigned int sn = 0;
    char snStr[SN_LEN + 1] = {0};
    char sendstr[SENDSTR_LEN + 1];
    int i, len;

    ProfInit();
    sn = getSystemSn();
    snprintf(snStr, SN_LEN, "%u", sn);
    /* init socket fd */
    while (1) {
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
	    sleep(PAUSE_SEC);
	} else {
	    break;
	}
    }

    /* 根据域名获得IP */
    while (1) {
	sleep(MIN_SEC);

	/* 获取WAN口IP */
	memset(ip, 0, IP_LEN + 1);
	if (getWANIP(ip) == -1) {
#if DEBUG
	    printf("cant get wan ip\n");
#endif
	    continue;
	}

	/* 根据序列号和时间计算域名 */
	memset(domain, 0, DOMAIN_LEN + 1);
	getDomain(domain, snStr);
#if 0
	strcpy(domain, "www.bidu.com");
#endif
#if DEBUG
	printf("domain[%s]\n", domain);
#endif

	host = gethostbyname(domain);
	if (host == NULL) {
#if DEBUG
	    printf("host[NULL]\n");
#endif
	    continue;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT_NUM);
#if 1
	servaddr.sin_addr = *((struct in_addr *)(host->h_addr));
#else
	inet_pton(AF_INET, "200.200.202.150", &servaddr.sin_addr);
#endif

	/* 将ip地址和序列号加密 */
	memset(sendstr, 0, SENDSTR_LEN + 1);
	strcat(sendstr, ip);
	strcat(sendstr, ";");
	strcat(sendstr, snStr);
	len = strlen(sendstr);
	if (len >= MAXLINE - AES_CRYPT_LEN) {
#if DEBUG
	    printf("sendstr too long[%s]\n", sendstr);
#endif
	    continue;
	}
	    
	encrypt_str(sendline, &sendLen, sendstr, len);
	if (sendLen == 0) {
#if DEBUG
	    printf("encrypt error\n");
#endif
	    continue;
	}
#if 0
	for (i = 0; i < sendLen; i++) {
	    printf("%02x",sendline[i]);
	}
	printf("\n");
#endif

	if( sendto(sockfd, sendline, sendLen, 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
#if DEBUG
	    printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
#endif
	}
    }
    close(sockfd);
    ProfDetach();
    return 0;
}
