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
 * ConnStatus -  ����״̬
 * wanMode - WAN�����ӷ�ʽ,DHCP,PPPOE,
 * wanLinkStates - WAN������״̬��0Ϊ����WAN�ڶ��Ͽ�
 * easyTaskNoticeEn - �׹�����߲�����ʾ�Ŀ���
 * safeToNet - ��ɫ����
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
