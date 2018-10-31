#include "ap_msg.h"
#include "urcp_debug.h"

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
static int msg_id = -1;
static int sem_id = -1;

static int msgId = -1;

int ap_msg_handle(void)
{
    int len;
    struct msgbuf_t msgbuf;

    len = msgrcv(msg_id, &msgbuf, sizeof(msgbuf.sw_msg), 0, MSG_NOERROR | IPC_NOWAIT);
    if(len!=-1)
    {
        if(msgbuf.mtype == SW_MSG_TYPE)
        {
                if(msgbuf.sw_msg.msg_type == MSG_TYPE_URCP)
		{
			switch(msgbuf.sw_msg.msg_code)
			{
#if 0
					case REQUEST_AC_CHANGE:
						urcp_msg_change_config(&msgbuf);
						break;
#endif
			case REQUEST_SEND_FILE_TO_AC:
				urcp_msg_send_prof_to_ac(&msgbuf);
				break;
			default:
				break;
			}
		}
                else if(msgbuf.sw_msg.msg_type == MSG_TYPE_URCP_DB)
		{
		    switch(msgbuf.sw_msg.msg_code)
		    {
#ifndef URCP_REL
		     case URCP_DB_SET:
		         urcp_set_debug(&msgbuf);
		         break;
		     case URCP_DB_GET:
		         urcp_get_debug(&msgbuf);
		         break;
#endif
		     default:
			 break;
		     }
	        }
	}
    }

    return 0;
}

int sw_msg_init(void)
{
    int msg_id;

    /*create the msg queue*/
    msg_id = msgget((key_t)SW_MSG_KEY, IPC_CREAT | 0666);
    if(msg_id == -1)
    {
	perror("switch msgget error");
    }

    return msg_id;
}

int re_msg_init(void)
{
    int msgId;

    /*create the msg queue*/
    msgId = msgget((key_t)RQ_MSG_KEY, IPC_CREAT | 0666);
    if(msgId == -1)
    {
	perror("switch msgget error");
    }

    return msgId;
}

int urcp_ipc_init(void)
{
    msg_id = sw_msg_init();
	msgId  = re_msg_init();

    return 0;
}

int sendwebmsg(int msg_type, int web_id)
{
    struct msgbuf_t webmsg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));
    webmsg.mtype = SW_MSG_TYPE;
    webmsg.sw_msg.msg_type = msg_type;
    webmsg.sw_msg.msg_code = web_id;
    if(msgsnd(msgId, &webmsg, sizeof(webmsg.sw_msg), 0)==-1)
    {
	/*error*/
	perror("urcp: message send error");
    }
    return 0;
}

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
	if(msgsnd(msgId, &webmsg, l, 0)==-1)
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

