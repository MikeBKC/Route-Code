#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <linux/autoconf.h>
#include <uttMachine.h>

#if defined CONFIG_SWITCH_EXT || (FEATURE_AC == FYES)
#include <sw_cfg.h>
#include <urcp_ipc.h>
#endif

#if SW_FEATURE_URCP || (FEATURE_AC == FYES)
static int msg_id=-1;
static int sem_id=-1;

void pend_sem(int no_wait)
{
    sw_sem_v(sem_id, no_wait);
}

/*
 *
 * msg shm sem initial
 * KeHuatao
 */
int urcp_ipc_init(void)
{
    sw_shm_init();
    msg_id = sw_msg_init();
    sem_id = sw_sem_init();

    return 0;
}
/*
 *send message
 *KeHuatao
 */
int sendwebmsg(int msg_type, int web_id)
{
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    webmsg.sw_msg.msg_code = web_id;
    if(msgsnd(msg_id, &webmsg, sizeof(webmsg.sw_msg), 0)==-1)
    {
	/*error*/
	perror("goahead: message send error:");
    }
    return 0;
}


/*
 *发送带参数的消息
 *len  参数长度
 *KeHuatao
 */
int sendwebmsgbuffer(int msg_type, int web_id, char* msg_buffer, unsigned int len)
{
    int l;
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));

    /*类型*/
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    /*页面id*/
    webmsg.sw_msg.msg_code = web_id;
    /*buffer*/
    if(len<=MAX_MSG_SIZE)
    {
	memcpy(webmsg.sw_msg.buffer, msg_buffer, len);
	l = (int)(((struct msgbuf_t*)0)->sw_msg.buffer);
	l = l + len;
	/*发送消息*/
	if(msgsnd(msg_id, &webmsg, l, 0)==-1)
	{
	    /*error*/
	    perror("goahead: message send error");
	}
    }
    else
    {
	printf("sendwebmsgbuffer: length error\n");
    }

    return 0;
}
#endif
