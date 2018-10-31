#include <syslog.h>
#include "pppoe-server.h"
#include "utt_conf_define.h"
#include "utt_ip_assign.h"

#ifdef UTT_IP_ASSIGN

#if (SYS_LOG_INFO == FYES)
extern int isPppoeLogEn(void);
#endif
#define UTT_MAXLINE 256

/*
 * 根据ip查找FreeSessions或者BusySessions中对应的节点
 */
static ClientSession *getSessByPeerIp(unsigned int ip, ClientSession *sessList) {
    ClientSession *sess = NULL;
    /* sess中的peerIp是低地址存高字节，直接可转换成网络序 */
    for (sess = sessList; sess != NULL; sess = sess->next) {
	if (ip == *((unsigned int *)(sess->peerip))) {
	    return sess;
	}
    }
    return NULL;
}

/*
 * 输入参数：
 *  msg - 从ppp模块接收到得消息
 * 说明：
 * 根据接收到得msg，调整pppoe server地址池
 * 1 poe_ass_ip 在busy list,ppp_ass_ip在free list - 目前处理得情况
 * 2 poe_ass_ip 在free list,ppp_ass_ip在free list - 不处理
 * 3 poe_ass_ip 在free list,ppp_ass_ip在busy list - 不处理
 * 4 poe_ass_ip 在busy list,ppp_ass_ip在busy list - 暂未考虑此种情况
 */
extern void utt_poesrv_ip_adjust(utt_msg_poesrv *msg) {
    ClientSession *freeSess = NULL;
    ClientSession *busySess = NULL;
    unsigned char bufIp[IPV4ALEN];

    utt_syslog(LOG_INFO, "%s: poe_ass_ip = 0x%x, ppp_ass_ip = 0x%x\n", 
	    __FUNCTION__, msg->msg_data.poe_ass_ip, msg->msg_data.ppp_ass_ip);
    /* pppoe server和ppp分配的ip地址相同，不需要调整 */
    if (msg->msg_data.poe_ass_ip == msg->msg_data.ppp_ass_ip) {
	return;
    }

    /* poe_ass_ip 在 busy list, ppp_ass_ip 在 free list */
    busySess = getSessByPeerIp(msg->msg_data.poe_ass_ip, BusySessions);
    freeSess = getSessByPeerIp(msg->msg_data.ppp_ass_ip, FreeSessions);
    if ((busySess != NULL) && (freeSess != NULL)) {
	/* 交换peer ip地址 */
	memcpy(bufIp, busySess->peerip, IPV4ALEN);
	memcpy(busySess->peerip, freeSess->peerip, IPV4ALEN);
	memcpy(freeSess->peerip, bufIp, IPV4ALEN);
    }
}

/*
 * 是否非绑定IP会话
 * 输入参数：
 *  需要判断的PPPoE会话
 * 返回值：
 *  1	-   是非绑定IP会话
 *  0	-   是绑定IP会话
 */
static int isNonBindSess(ClientSession *sess) {
    FILE *fp = NULL;
    char line[UTT_MAXLINE];
    unsigned char bufIp[IPV4ALEN];
    unsigned int a, b, c, d;
    int rst = 1;

    fp = fopen(PPPOE_SERVER_BIND_IP_FILE, "r");
    if (fp == NULL) {
	utt_syslog(LOG_INFO, "%s: open file %s error\n", __FUNCTION__, PPPOE_SERVER_BIND_IP_FILE);
	return rst;
    }

    while (feof(fp) == 0) {
	if (fgets(line, UTT_MAXLINE, fp) == NULL) {
	    break;
	}
	if ((sscanf(line, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) && 
		(a < 256) && (b < 256) && (c < 256) && (d < 256)) {
	    /* 判断绑定IP与sess中IP是否一致 */
	    bufIp[0] = (unsigned char)a;
	    bufIp[1] = (unsigned char)b;
	    bufIp[2] = (unsigned char)c;
	    bufIp[3] = (unsigned char)d;
	    if (memcmp(bufIp, sess->peerip, IPV4ALEN) == 0) {
		rst = 0;
		break;
	    }
	}
    }
    fclose(fp);
    return rst;
}

/*
 * 在FreeSessions链中查找是否存在非绑定IP会话节点
 *
 * 输入参数：
 *  无
 * 输出参数：
 *  prev    -	非绑定IP会话节点的前一个节点，如果cur节点为第一个节点，则prev为NULL
 *  cur	    -	找到的非绑定IP会话节点
 * 返回值：
 *  1	-   FreeSessions链中存在非绑定IP会话节点
 *  0	-   FreeSessions链中不存在非绑定IP会话节点
 */
extern int uttExistNonBindFreeSess(ClientSession **prev, ClientSession **cur) {
    *prev = NULL;
    *cur = FreeSessions;

    while (*cur != NULL) {
	if (isNonBindSess(*cur) == 1) { /* 找到非PPP绑定IP会话 */
	    return 1;
	}
	*prev = *cur;
	*cur = (*cur)->next;
    }
    return 0;
}
#endif
