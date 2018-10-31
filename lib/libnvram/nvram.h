#ifndef _NVRAM_H
#define _NVRAM_H 	1

#include <linux/autoconf.h>

#if 0
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"        
#include "mibunion.h"   
#include "profacce.h"
#endif

#ifdef CONFIG_DUAL_IMAGE

#define UBOOT_NVRAM	0
#define RT2860_NVRAM    1
#define RTINIC_NVRAM    2
#define RT2561_NVRAM    3
#else
#define FATAL_NVRAM     0
#define RT2860_NVRAM    1
#if 1
#define RTINIC_NVRAM    3
#define RT2561_NVRAM    3
#endif
#endif
#define UTT_MSG_NVRAM_FILE "/etc/utt-speedmsg"
//#define UTT_MSG_NVRAM_FILE "/tmp/uttMsgforRoute"
#define UTT_SHM_NVRAM_FILE "/etc/utt-nvramshm"
#define UTT_MAX_NVRAM_SEG 0xd000
#define UTT_MAX_NVRAM_FATAL 0x2000
#define UTT_SPEED_WEB 44
typedef enum en_init_type{
    INIT_LAN = 0,
    INIT_WAN,
    INIT_INTERNET,
    INIT_DHCP,
    NVRAM_WRITE,
    INIT_WLAN,
    WLAN_SEC_INIT,
    WAN_LINK_UP,
    WAN_LINK_DOWN,
    DOUBLE_WAN_CONF_CHG,
    ISP_PDB_CHANGE,
    MP_COMB_CHANGE,
    EDIT_STATIC_ROUTE,
    INIT_STATIC_NAT,
    EDIT_NAT_RULE,
    INIT_DMZ,
    INIT_PPPOE_SERVER,
    INIT_PPPOE_SERVER_USER_FILE,
    INIT_SEC_DEFENSE,
    INIT_DDNS,
    INIT_GLOBAL_BASE,
    INIT_GROUP
}speedInitType;

struct st_speed_msg_para{
    speedInitType initType;
    int wanIndex;
    int action;
    char data[128];
    int offset;
    int len;
    int crc;
};
typedef struct st_speed_msg{
    long mtype;
    struct st_speed_msg_para speedPara;
}speed_msg;

int nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
char *nvram_get(int index, char *name);
int nvram_bufset(int index, char *name, char *value);
char *nvram_bufget(int index, char *name);

void nvram_buflist(int index);
int nvram_commit(int index);
int nvram_clear(int index);
int nvram_erase(int index);

int getNvramNum(void);
unsigned int getNvramOffset(int index);
unsigned int getNvramBlockSize(int index);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);
void toggleNvramDebug(void);
extern void sendMsgForSpeed(unsigned int initType, int wanIndex, int action, char* data, int size);
#endif
