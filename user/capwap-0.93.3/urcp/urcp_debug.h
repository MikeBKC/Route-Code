
#ifndef __URCP_DEBUG_H__
#define __URCP_DEBUG_H__
#include "urcp.h"

#ifdef FEATURE_AP
#include "ap_msg.h"
#endif

#define  URCP_DB_OFF    0
#define  URCP_DB_ERROR  1
#define  URCP_DB_WARN   2
#define  URCP_DB_TRACE  3
#define  URCP_DB_INFO   4
#define  URCP_DB_LOUD   5
#define	 URCP_DB_MIN    URCP_DB_OFF
#define	 URCP_DB_MAX    URCP_DB_LOUD

#define  URCP_LOG_OFF    0
#define  URCP_LOG_ERROR  1
#define  URCP_LOG_WARN   2
#define  URCP_LOG_TRACE  3
#define  URCP_LOG_INFO   4
#define  URCP_LOG_LOUD   5
#define	 URCP_LOG_MIN    URCP_LOG_OFF
#define	 URCP_LOG_MAX    URCP_LOG_LOUD

#define URCP_CONSOLE "/dev/console"

typedef enum {
	URCP_MODULE_ID_GLOBAL = 0,
	URCP_MODULE_ID_MAIN = 1,
	URCP_MODULE_ID_URCP = 2,
//#ifdef FEATURE_AC
	URCP_MODULE_ID_HASH = 3,
//#endif
	URCP_MODULE_ID_TRAP_STA = 4,
	URCP_MODULE_ID_USER_LIST = 5,
	URCP_MODULE_ID_APP = 6 ,
        URCP_MODULE_ID_STATE = 7,
#ifdef FEATURE_AC
	URCP_MODULE_ID_STA_LOG = 8,
#endif
	URCP_MODULE_ID_HOST_NAME = 9,
	URCP_MODULE_ID_TFTP = 10,
	URCP_MODULE_ID_MAX = 0xFFFEu,
	URCP_MODULE_ID_ALL = 0xFFFFu,
	URCP_MSG_CLIENT,
} urcp_module_id_t;

#ifdef FEATURE_AP
#define URCP_REL
#endif
#define URCP_TRACE_LOG     "/tmp/urcp/urcp_trace.txt"
#undef URCP_REL
#ifndef URCP_REL

#define db_error(URCP_TRACE_MODULE_ID, format, arg...)  urcp_debug(URCP_TRACE_MODULE_ID, URCP_DB_ERROR, format, ##arg)
#define db_warn(URCP_TRACE_MODULE_ID, format, arg...)	  urcp_debug(URCP_TRACE_MODULE_ID, URCP_DB_WARN, format, ##arg)
#define db_trace(URCP_TRACE_MODULE_ID, format, arg...)  urcp_debug(URCP_TRACE_MODULE_ID, URCP_DB_TRACE, format, ##arg)
#define db_info(URCP_TRACE_MODULE_ID, format, arg...)   urcp_debug(URCP_TRACE_MODULE_ID, URCP_DB_INFO, format, ##arg)
#define db_loud(URCP_TRACE_MODULE_ID, format, arg...)   urcp_debug(URCP_TRACE_MODULE_ID, URCP_DB_LOUD, format, ##arg)
#else
#define db_error(URCP_TRACE_MODULE_ID, format, arg...)  
#define db_warn(URCP_TRACE_MODULE_ID, format, arg...)
#define db_trace(URCP_TRACE_MODULE_ID, format, arg...) 
#define db_info(URCP_TRACE_MODULE_ID, format, arg...) 
#define db_loud(URCP_TRACE_MODULE_ID, format, arg...)
#endif
typedef struct _urcp_db_t {
	int id;
	int dlevel;
	int llevel;
	char logfile[32];
	char io_file[32];
} urcp_db_t;

typedef struct {
	int  id;
	char magic[10];
	int  dlevel;
	int  llevel;
} urcp_db_unit;

#define MAX_MSG_SIZE    1024
struct sw_msg_t{
    int msg_type;
    int msg_code;
    char buffer[MAX_MSG_SIZE];
};

struct msgbuf_t{
    long mtype;
    struct sw_msg_t sw_msg;
};
void ap_ac_debug_init();
void ap_ac_debug_check();
int chang_printf_io(char *io_file);
int urcp_debug(int id, int level, char *format, ...);
int urcp_set_debug(struct msgbuf_t *msgbuf);
int urcp_get_debug(struct msgbuf_t *msgbuf);
int isMagicInList(char *magic);
#endif
