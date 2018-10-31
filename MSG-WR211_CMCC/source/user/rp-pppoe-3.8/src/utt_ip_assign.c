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
 * ����ip����FreeSessions����BusySessions�ж�Ӧ�Ľڵ�
 */
static ClientSession *getSessByPeerIp(unsigned int ip, ClientSession *sessList) {
    ClientSession *sess = NULL;
    /* sess�е�peerIp�ǵ͵�ַ����ֽڣ�ֱ�ӿ�ת���������� */
    for (sess = sessList; sess != NULL; sess = sess->next) {
	if (ip == *((unsigned int *)(sess->peerip))) {
	    return sess;
	}
    }
    return NULL;
}

/*
 * ���������
 *  msg - ��pppģ����յ�����Ϣ
 * ˵����
 * ���ݽ��յ���msg������pppoe server��ַ��
 * 1 poe_ass_ip ��busy list,ppp_ass_ip��free list - Ŀǰ��������
 * 2 poe_ass_ip ��free list,ppp_ass_ip��free list - ������
 * 3 poe_ass_ip ��free list,ppp_ass_ip��busy list - ������
 * 4 poe_ass_ip ��busy list,ppp_ass_ip��busy list - ��δ���Ǵ������
 */
extern void utt_poesrv_ip_adjust(utt_msg_poesrv *msg) {
    ClientSession *freeSess = NULL;
    ClientSession *busySess = NULL;
    unsigned char bufIp[IPV4ALEN];

    utt_syslog(LOG_INFO, "%s: poe_ass_ip = 0x%x, ppp_ass_ip = 0x%x\n", 
	    __FUNCTION__, msg->msg_data.poe_ass_ip, msg->msg_data.ppp_ass_ip);
    /* pppoe server��ppp�����ip��ַ��ͬ������Ҫ���� */
    if (msg->msg_data.poe_ass_ip == msg->msg_data.ppp_ass_ip) {
	return;
    }

    /* poe_ass_ip �� busy list, ppp_ass_ip �� free list */
    busySess = getSessByPeerIp(msg->msg_data.poe_ass_ip, BusySessions);
    freeSess = getSessByPeerIp(msg->msg_data.ppp_ass_ip, FreeSessions);
    if ((busySess != NULL) && (freeSess != NULL)) {
	/* ����peer ip��ַ */
	memcpy(bufIp, busySess->peerip, IPV4ALEN);
	memcpy(busySess->peerip, freeSess->peerip, IPV4ALEN);
	memcpy(freeSess->peerip, bufIp, IPV4ALEN);
    }
}

/*
 * �Ƿ�ǰ�IP�Ự
 * ���������
 *  ��Ҫ�жϵ�PPPoE�Ự
 * ����ֵ��
 *  1	-   �Ƿǰ�IP�Ự
 *  0	-   �ǰ�IP�Ự
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
	    /* �жϰ�IP��sess��IP�Ƿ�һ�� */
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
 * ��FreeSessions���в����Ƿ���ڷǰ�IP�Ự�ڵ�
 *
 * ���������
 *  ��
 * ���������
 *  prev    -	�ǰ�IP�Ự�ڵ��ǰһ���ڵ㣬���cur�ڵ�Ϊ��һ���ڵ㣬��prevΪNULL
 *  cur	    -	�ҵ��ķǰ�IP�Ự�ڵ�
 * ����ֵ��
 *  1	-   FreeSessions���д��ڷǰ�IP�Ự�ڵ�
 *  0	-   FreeSessions���в����ڷǰ�IP�Ự�ڵ�
 */
extern int uttExistNonBindFreeSess(ClientSession **prev, ClientSession **cur) {
    *prev = NULL;
    *cur = FreeSessions;

    while (*cur != NULL) {
	if (isNonBindSess(*cur) == 1) { /* �ҵ���PPP��IP�Ự */
	    return 1;
	}
	*prev = *cur;
	*cur = (*cur)->next;
    }
    return 0;
}
#endif
