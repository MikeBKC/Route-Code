#ifndef UTT_SEM_H
#define UTT_SEM_H

#include "uttMachine.h"
typedef enum{
    SEM_COM_NO = 0,/*������ �����������̫����������goahead������speedweb�Ļ���*/
    SEM_PPPOE_NO,/*ʹ����pppoe*/
    SEM_UTTNF_NO, /*utt nefitler*/
    SEM_WEBAUTH_NO,/*������web��֤*/
#if (EASY_TASK == FYES)
    SEM_CONNSTA_NO,/*ʹ�����׹���WAN������״̬*/
#endif
#if (WIRELESS == FYES)
    SEM_WIFI_NO,/*ʹ��������*/
#endif
#if (FEATURE_AC == FYES)
    SEM_AP_LOG,    /*AP��־��Ϣ*/
#endif
#if (FEATURE_AP_MAIL == FYES)
    SEM_AP_MAIL,    /*��ʱ����AP���ߺ�������Ϣ���ʼ�*/
#endif
#if SZONE_AUTO_UPDATA
    SEM_SZONE_UPDATA,
#endif
    SEM_PHYDETECT_NO,/*������·���*/
    SEM_VPN_NO, /*VPN����*/
#if (ADFILTER == FYES)
    SEM_ADFILTER,/*������*/
#endif
#if(UTT_IPAUTOREP == FYES)
    SEM_IPAUTOREP,
#endif
#if (MAKE_MONEY == FYES)
    SEM_SALE_WIFI,
#endif
#if (CLOUD_CONFIG == FYES)
    SEM_CLOUD_CONFIG,   /*����֪ͨ*/
#endif
#if (UTT_KNOWIFI == FYES)
    SEM_KNOWIFI,
#endif
    UTT_SEM_COUNT/*�������ź��������ź���������*/
}UTT_SEM_NO;

extern int uttSemInit(void);
extern int uttSemP(UTT_SEM_NO sem_no, int no_wait);
extern int uttSemV(UTT_SEM_NO sem_no, int no_wait);
extern void uttWaitZero(UTT_SEM_NO sem_no);
extern void uttDelSem(void);
#endif
