#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <stdint.h>
#include "md5.h"

#define CBC 1
#define ECB 1

#include "aes.h"
#include "log.h"

static void usage(void) {
    printf("usage:./logMonitor sn date\n");
    printf("      sn    - utt Serial Number\n");
    printf("      date  - the current date, only year and month.eg: 201502\n");
}

/* 
 * descript: 输入输出不能超过MAXLINE, AES分16字符一段来加密和解密 
 * para:
 *  out - 解密之后字符串
 *  in	- 输入加密的16进制数组
 *  inLen - 输入数据大小
 * attention:
 *  inLen <= outLen - 16
 */
static void decrypt_str(char *out, unsigned char *in, int inLen) {
    /* mantis 17335 大小端问题 */
#if 0
    int key[32] = {0x1,0x2,0x3,0x4d,0x56,0x10,0x11,0x12,
		0x1,0x2,0x3,0x4d,0x56,0x10,0x11,0x12,
		0x1,0x2,0x3,0x4d,0x56,0x10,0x11,0x12,
		0x1,0x2,0x3,0x4d,0x56,0x10,0x11,0x12};
    struct crypto_aes_ctx ctx;
#else
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
#endif
    unsigned char decrypt[AES_CRYPT_LEN + 1];    
    char tmp[AES_CRYPT_LEN + 1] = {0};
    char deStr[MAXLINE] = {0};
    unsigned char *inS = in;
    int i;
#if 0
    crypto_aes_expand_key(&ctx,(u8 *)key,32);
#endif
    for (i = 0; i < inLen; i += AES_CRYPT_LEN) {
	memset(decrypt, 0, AES_CRYPT_LEN + 1);
	memset(tmp, 0, AES_CRYPT_LEN + 1);
	memcpy(decrypt, inS, AES_CRYPT_LEN);	
#if 0
	aes_decrypt(&ctx, tmp,decrypt);
#else
	AES128_ECB_decrypt((uint8_t *)decrypt, key, (uint8_t *)tmp);
#endif
	strcat(deStr, tmp);
	inS += AES_CRYPT_LEN;
    }
    strcpy(out, deStr);
}

/* 根据序列号批次和当前年月生成监听域名, 错误返回-1 */
static int getDomain(char *str, char *sn, char *date) {
    char tmp1[TMP_LEN] = {0};
    char tmp2[TMP_LEN] = {0};
    MD5_CONTEXT md5ctx;
    unsigned char hash[MD5_LEN] = {0};
    int i;
    if (strlen(date) != 6) {
	printf("date input error[%s]\n", date);
	usage();
	return -1;
    }
    if (strlen(sn) < 6) {
	printf("sn too short[%s]\n", sn);
	return -1;
    }
    strncpy(tmp1, sn, BATCH_LEN);
    /* 根据批次 + 年月通过MD5生成域名 */
    snprintf(tmp2, TMP_LEN - 1, "%s%s", tmp1, date);
    MD5Init(&md5ctx);
    MD5Update(&md5ctx, tmp2, strlen(tmp2));
    MD5Final(hash, &md5ctx);
    for (i = 0; i < MD5_DOMAIN_LEN; i++) {
	sprintf(str, "%02x", hash[i]);
	str += 2;
    }
    return 0;
}

int main(int argc, char** argv)
{
    int sockFd;
    struct sockaddr_in servaddr;
    unsigned char buff[MAXLINE] = {0};
    char deStr[MAXLINE] = {0};
    int     n, i;
    int sock_size = sizeof(struct sockaddr);
    char domain[DOMAIN_LEN + 1] = {0};
    if (argc != 3) {
	usage();
	return 0;
    }
    if (getDomain(domain, argv[1], argv[2]) == -1) {
	printf("get domain name error\n");
	return 0;
    }
    printf("Please register %s.f3322.net for monitor.\n", domain);
    if( (sockFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){
	printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_NUM);

    if( bind(sockFd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
	printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }


    printf("======log monitor======\n");
    while(1){
	n = recvfrom(sockFd, buff, MAXLINE, 0, (struct sockaddr*)&servaddr, &sock_size);
	/* AES加密一定是16整数倍 */
	if (n % AES_CRYPT_LEN != 0) {
	    printf("recv error encrypt msg:\n");
	    for (i = 0; i < n; i++) {
		printf("%02x", buff[i]);
	    }
	    printf("\n");
	    continue;
	}
	decrypt_str(deStr, buff, n);
	printf("log:ip and sn[%s]\n", deStr);
    }

    close(sockFd);
}
