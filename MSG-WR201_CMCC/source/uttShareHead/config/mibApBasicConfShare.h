#ifndef MIBAPBASICCONF_SHARE_H
#define MIBAPBASICCONF_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define MAX_DEV_NAME_LEN          24u  /*�豸��*/
#define MAX_USER_NAME_LEN         12u  /*�û���*/
#define MAX_PASSWORD_LEN          18u
typedef struct st_ap_basic_conf_profile {
    mibHead head;
//    Uint32 work_mode;    /* ����ģʽ 1-fit 2-fat*/
    char dev_name[MAX_DEV_NAME_LEN];	/* AP �豸�� */
    Uint32 ip_addr; /* IP��ַ */
    Uint32 net_mask; /* �������� */
    //char user_name[MAX_USER_NAME_LEN];   /* AP�û��� */
    char password[MAX_PASSWORD_LEN];	/* ���� */
//    Uint32 max_clt;  /* ���ͻ����� */
    Uint32 channel;  /* �ŵ� */
	Uint16            dbm;  /* ������ֵ */
#if defined(WIRELESS_5G) && (WIRELESS_5G==FYES)
    Uint16            dbm_5g;  /* ������ֵ */
#endif
 //   Uint32 effect_conf;	/* ��Ч���� 1-local ����ֵ��ΪAC�·�*/
	Uint32 dhcp_en;
}ApBasicConfProfile;

#endif
