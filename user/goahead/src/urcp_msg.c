#include "urcp_msg.h"

//#if ((UTT_NV_1800N == FYES) || (UTT_NV_510V2 == FYES))
#if (FEATURE_AP == FYES)

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

void request_ac_send_msg(webs_t wp, uint32 request_type, uint32 u_misc_type)
{
	extern unsigned int productSn;
	request_cmd_msg_t request_cmd_msg;
	SlotNumber slot;
	Pointer prof;
	int ret;
	char xml_buf[MAX_VALUE_LEN + 1];
	MibProfileType mibType;

	slot.slotNumber = SLOT_NUMBER_ANY;
	request_cmd_msg.opt_type = u_misc_type;
	request_cmd_msg.sn = productSn;

	request_cmd_msg.name_len = 0;
	request_cmd_msg.xml_len = 0;
	memset(request_cmd_msg.content, '\0', sizeof(request_cmd_msg.content));
	memset(xml_buf, '\0', sizeof(xml_buf));
	if (u_misc_type == REQUEST_CHANGE_CONF) {
		DBCK();
		request_cmd_msg.name_len = strlen("apEffectConf");
		strncat(request_cmd_msg.content, "apEffectConf", request_cmd_msg.name_len);
		mibType = MIB_PROF_AP_EFFECT_CONF;
		BuildDefaultFactoryValue(mibType, 0, slot, &prof);
		ret = parseProfileOneInstToXml(mibType, prof, xml_buf, sizeof(xml_buf) - 1);
		request_cmd_msg.xml_len = strlen(xml_buf);
		strncat(request_cmd_msg.content, xml_buf, request_cmd_msg.xml_len);

	} else if (u_misc_type == REQUEST_FAT_TO_FIT){
		DBCK();
		request_cmd_msg.name_len = strlen("fatFitSwitch");
		strncat(request_cmd_msg.content, "fatFitSwitch", request_cmd_msg.name_len);
		mibType = MIB_PROF_FAT_FIT_SWITCH;
		BuildDefaultFactoryValue(mibType, 0, slot, &prof);
		ret = parseProfileOneInstToXml(mibType, prof, xml_buf, sizeof(xml_buf) - 1);
		request_cmd_msg.xml_len = strlen(xml_buf);
		strncat(request_cmd_msg.content, xml_buf, request_cmd_msg.xml_len);
	} else {
		DBCK("to do nothing...........");
	}

	DBCK("sn = %d, name_len = %d, xml_len = %d, content = %s\n",
			request_cmd_msg.sn, request_cmd_msg.name_len,
			request_cmd_msg.xml_len, request_cmd_msg.content);

	if (ret > 0) {
		sendwebmsgbuffer(MSG_TYPE_URCP, request_type, (char *)(&request_cmd_msg),
						sizeof(struct _request_cmd_msg_t));
	} else {
		setTheErrorMsg(T("操作失败"));
	}

	return;
}

/*  func：AP接收从URCP发状态的函数
 *	return：通过参数返回urcp返回的状态，返回值为0代表收消息成功，为-1表示未收到urcp的消息
 *	author：CK
 * */
int reply_ap_revice_msg(int *status)
{
	int len;
	struct msgbuf_t msgbuf;
	int ret = -1;

	memset(&msgbuf, 0, sizeof(struct msgbuf_t));

	// sleep 1秒，防止urcp进程发包过程未完成
	usleep(1000 * 1000);
	len = msgrcv(msgId, &msgbuf, sizeof(msgbuf.sw_msg), 0, MSG_NOERROR | IPC_NOWAIT);
	if (len > 0) {
		ret = 0;

		if ((msgbuf.sw_msg.msg_type == REQUEST_CHANGE_CONF) || (msgbuf.sw_msg.msg_type == REQUEST_SEND_FILE)) {
			*status = msgbuf.sw_msg.msg_code;
		}
	} else {
		// 未收到消息的情况
		ret = -1;
	}

	return ret;
}
#endif
