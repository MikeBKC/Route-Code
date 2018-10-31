#ifndef UTT_SEM_H
#define UTT_SEM_H

#include "uttMachine.h"
typedef enum{
    SEM_COM_NO = 0,/*适用于 互斥操作不是太多的情况。如goahead交互与speedweb的互斥*/
    SEM_PPPOE_NO,/*使用于pppoe*/
    SEM_UTTNF_NO, /*utt nefitler*/
    SEM_WEBAUTH_NO,/*适用于web认证*/
#if (EASY_TASK == FYES)
    SEM_CONNSTA_NO,/*使用于易管理WAN口连接状态*/
#endif
#if (WIRELESS == FYES)
    SEM_WIFI_NO,/*使用于无线*/
#endif
#if (FEATURE_AC == FYES)
    SEM_AP_LOG,    /*AP日志信息*/
#endif
#if (FEATURE_AP_MAIL == FYES)
    SEM_AP_MAIL,    /*定时发送AP上线和下线信息的邮件*/
#endif
#if SZONE_AUTO_UPDATA
    SEM_SZONE_UPDATA,
#endif
    SEM_PHYDETECT_NO,/*物理线路检测*/
    SEM_VPN_NO, /*VPN并发*/
#if (ADFILTER == FYES)
    SEM_ADFILTER,/*广告过滤*/
#endif
#if(UTT_IPAUTOREP == FYES)
    SEM_IPAUTOREP,
#endif
#if (MAKE_MONEY == FYES)
    SEM_SALE_WIFI,
#endif
#if (CLOUD_CONFIG == FYES)
    SEM_CLOUD_CONFIG,   /*上线通知*/
#endif
#if (UTT_KNOWIFI == FYES)
    SEM_KNOWIFI,
#endif
    UTT_SEM_COUNT/*创建的信号量集中信号量的总数*/
}UTT_SEM_NO;

extern int uttSemInit(void);
extern int uttSemP(UTT_SEM_NO sem_no, int no_wait);
extern int uttSemV(UTT_SEM_NO sem_no, int no_wait);
extern void uttWaitZero(UTT_SEM_NO sem_no);
extern void uttDelSem(void);
#endif
