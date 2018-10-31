#ifndef MIBAPCONFTEMP_SHARE_H
#define MIBAPCONFTEMP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"


typedef struct st_apConfTemp_profile {
    mibHead head;
    char tempName[UTT_INST_NAME_LEN + 1];   /* 模版名称 */
 //   char suitType[UTT_INST_NAME_LEN + 1];   /* 适用型号 */
 //   char firmware[UTT_INST_NAME_LEN + 1];   /* 硬件版本 */
 //   ProfileEnableEnum def;	/* 默认模版 */
    ProfileEnableEnum wireless;	/* 启用无线功能 */
    ProfileEnableEnum sgi;  /* short guard interval */
    ProfileEnableEnum preamble;	/* short preamble */
    ProfileEnableEnum wmm;  /* 启用WMM */
    
    WlanRate rate;  /* 无线速率 */
    Uint32 wlFre;   /* 无线频段  0: 2.4GHz; 1: 5GHz*/
    Uint32 bw;	/* 频道带宽 */
    Uint32 channel; /* 信道 */
    Uint32 power;   /* 无线传输功率 */
    WlanPowerManual manPower;	/* 无线传输功率 手动设置 */
    Uint32 mode;    /* 无线模式 */
    Uint32 BeaconPeriod; /**/
    Uint32 VChanWidth;	/*VHT BW*/
}ApConfTempProfile;

#endif
