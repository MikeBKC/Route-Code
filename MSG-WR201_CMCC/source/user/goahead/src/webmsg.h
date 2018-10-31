#ifndef __WEB_MSG_H__
#define __WEB_MSG_H__


#define WEB_DEBUG 0 

#if (WEB_DEBUG)
#define WEB_PRINTF(fmt, args...)  do {\
        printf("WEB_PRINTF: " fmt, ## args);    \
} while(0)
#else
#define WEB_PRINTF(fmt, args...)
#endif

#define DELSEPARATE T(",")

extern int urcp_init_ipc(void);

extern int sendwebmsg(int msg_type, int web_id);

extern int sendwebmsgbuffer(int msg_type, int web_id, char* msg_buffer, unsigned int len);

extern void pend_sem(int no_wait);
#endif
