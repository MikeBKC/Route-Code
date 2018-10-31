/*
 *urcp debug 处理程序
 *
 *2012-12-06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include "../urcp/urcp_debug.h"

#ifdef FEATURE_AP
#include "../urcp/ap_msg.h"
#endif

#ifdef FEATURE_AC
#include "sw_cfg.h"
#include "urcp_ipc.h"
#endif
#include "../urcp/debug_module.c"
#include "version.h"

static int msg_id = -1;

void debug_help(void);

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
int urcp_ipc_init(void)
{
    msg_id = sw_msg_init();

    return 0;
}

/*判断用户输入是否合法
 *返回值 magic的 id，-1表示不合法
 * */
int isMagicInList(char *magic)
{
	int ret = -1;
	urcp_db_unit *pm = NULL;

	if(strcmp(magic,"all") == 0)
	{
		ret = URCP_MODULE_ID_ALL;
	} else if (strcmp(magic, "device") == 0) {
		ret = URCP_MSG_CLIENT;
	}
	else
	{
	    pm = &(urcp_debug_module[0]);
	    while(pm->id < URCP_MODULE_ID_MAX)
	    {
		    if(!strcmp(magic,pm->magic))
		    {
			    ret = pm->id;
			    break;
		    }
		    pm++;
	    }
	}

	return ret;
}

int main(int argc, char *argv[])
{
    int id,l,len,dlevel,llevel;
    struct msgbuf_t webmsg;
    urcp_db_t  cmd_msg;

    memset(&webmsg, 0, sizeof(struct msgbuf_t));

    urcp_ipc_init();

    if (argc >= 3 ) {
	/*类型*/
	webmsg.mtype = SW_MSG_TYPE;
	webmsg.sw_msg.msg_type = MSG_TYPE_URCP_DB;
	if(strcmp(argv[1],"set") == 0)
	{
	    webmsg.sw_msg.msg_code = URCP_DB_SET;
	    if(argc<5)
	    {
		debug_help();
		return -1;
	    }
	}else if(strcmp(argv[1],"get") == 0)
	{
	    webmsg.sw_msg.msg_code = URCP_DB_GET;
	    if(argc>3)
	    {
		debug_help();
		return -1;
	    }
	}else{
	    debug_help();
	    return -1;
	}
	id = isMagicInList(argv[2]);
	if( id == -1)
	{
	    debug_help();
	    return -1;
	}
	cmd_msg.id = id;
	if(argc > 3){
	    /*set*/
	    dlevel = strtoul(argv[3], NULL, 10);
	    llevel = strtoul(argv[4], NULL, 10);
	    if((dlevel >=0) &&(dlevel <=5))
	    {
		cmd_msg.dlevel = dlevel;
	    }else{
		debug_help();
		return -1;
	    }
	    if((llevel >=0) &&(llevel <=5))
	    {
		cmd_msg.llevel = llevel;
	    }else{
		debug_help();
		return -1;
	    }
	    if(argc == 6){
		if((strlen(argv[5]) < 32)&&(strlen(argv[5]) > 0))
		{
		    strcpy(cmd_msg.logfile,argv[5]);
		}
	    }else if(argc == 5){
		strcpy(cmd_msg.logfile,URCP_TRACE_LOG);
	    }
	}
	/*buffer*/
	len=sizeof(cmd_msg);
	if(len<=MAX_MSG_SIZE)
	{
	    memcpy(webmsg.sw_msg.buffer, (char *)&cmd_msg, len);
	    l = (int)(((struct msgbuf_t*)0)->sw_msg.buffer);
	    l = l + len;
	    /*发送消息*/
	    if(msgsnd(msg_id, &webmsg, l, 0)==-1)
	    {
		/*error*/
		perror("urcpDb: message send error");
	    }
	}
	else
	{
	    printf("urcpDb: length error\n");
	}
    }
    else if((argc == 2) && (strcmp(argv[1],"version") == 0))
    {
	printf("path    : %s\n", PATH);
	printf("cvs tag : %s\n", TAG);
	printf("by      : %s\n", USER);
	printf("date    : %s\n", TIME);
    }
    else {
	debug_help();
    }

    return 0;
}
void debug_help()
{
    urcp_db_unit *pm = NULL;

    printf("Usage:\n\
    urcpDb [method(set/get/version)] [module] [debug level] [log level] [logfile path]\n\
    method : set, get or show the version\n\
    module : 'all' means set all of the modules' level\n\
    module name :\n");
    pm = &(urcp_debug_module[0]);
    while(pm->id < URCP_MODULE_ID_MAX)
    {
	printf("        %s\n", pm->magic);
	pm++;
    }
    printf("    level : used to set the debug or log level\n\
        0 - off,  1 - error,  2 - waring,  3 - trace,  4 - information,  5 - loud , 0xff - no change\n\
    logfile path : It will write the debug informations to a specitied file,null means set nothing\n\
    for example:\n\
    1: urcpDb set urcp 0 1 /tmp/log\n\
    2: urcpDb get all\n");
}
