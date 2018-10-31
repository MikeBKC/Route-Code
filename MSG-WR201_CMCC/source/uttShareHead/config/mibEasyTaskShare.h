#ifndef _EASYTASK_SHARE_H_
#define _EASYTASK_SHARE_H_

#define EASY_TASK_HTTP_CHAIN	"easy_task_http"

typedef enum{
    DISCONNECT = 0,
    CONNECTED,
    CONNECTING,
    CHECKING,
    CHECK_INTERRUPT,
    CHECK_COMPLETE,
    USER_PASS_ERROR,
    CONNECT_ERROR
}emConnStatusType;
 
/*
 * ConnStatus -  连接状态
 * wanMode - WAN口连接方式,DHCP,PPPOE,
 * wanLinkStates - WAN口连接状态，0为所有WAN口都断开
 * easyTaskNoticeEn - 易管理断线不再提示的开关
 * safeToNet - 绿色上网
 */
typedef struct _easy_task_profile{
    mibHead head;
    emConnStatusType ConnStatus;
    enum em_eth_conn_mode wanMode;
    unsigned int wanLinkStates;
    FunEnableEnum easyTaskNoticeEn;
#if (WLAN_WORK_TIME == FYES)
    FunEnableEnum safeToNet;
#endif
}EasyTaskProfile;

#endif
