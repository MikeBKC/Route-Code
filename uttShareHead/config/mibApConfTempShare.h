#ifndef MIBAPCONFTEMP_SHARE_H
#define MIBAPCONFTEMP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"


typedef struct st_apConfTemp_profile {
    mibHead head;
    char tempName[UTT_INST_NAME_LEN + 1];   /* ģ������ */
 //   char suitType[UTT_INST_NAME_LEN + 1];   /* �����ͺ� */
 //   char firmware[UTT_INST_NAME_LEN + 1];   /* Ӳ���汾 */
 //   ProfileEnableEnum def;	/* Ĭ��ģ�� */
    ProfileEnableEnum wireless;	/* �������߹��� */
    ProfileEnableEnum sgi;  /* short guard interval */
    ProfileEnableEnum preamble;	/* short preamble */
    ProfileEnableEnum wmm;  /* ����WMM */
    
    WlanRate rate;  /* �������� */
    Uint32 wlFre;   /* ����Ƶ��  0: 2.4GHz; 1: 5GHz*/
    Uint32 bw;	/* Ƶ������ */
    Uint32 channel; /* �ŵ� */
    Uint32 power;   /* ���ߴ��书�� */
    WlanPowerManual manPower;	/* ���ߴ��书�� �ֶ����� */
    Uint32 mode;    /* ����ģʽ */
    Uint32 BeaconPeriod; /**/
    Uint32 VChanWidth;	/*VHT BW*/
}ApConfTempProfile;

#endif
