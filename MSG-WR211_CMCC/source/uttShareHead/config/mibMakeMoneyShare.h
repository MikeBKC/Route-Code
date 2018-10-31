#ifndef MIBMAKEMONEY_SHARE_H
#define MIBMAKEMONEY_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#if (WIFIDOG_MAKE_MONEY == FYES)
#include "mibWebAuthShare.h"
#ifndef MAKE_MONEY_HEARTBEAT
#define MAKE_MONEY_HEARTBEAT FNO
#endif
#endif
typedef struct st_FreeExp{
    MacAddr mac;
    u_long time;
}FreeExp;/*赚钱wifi免费体验*/




typedef struct st_MakeMoney_Profile {
    mibHead head;
    ProfileEnableEnum MakeMoneyEn;
    ProfileEnableEnum SaftyEn;
    char phoneNumber[UTT_PHONE_LEN+1];
    int isBind;
    char TimeStart[UTT_TIME_LEN];
    char TimeStop[UTT_TIME_LEN];
    char charge[5];
    unsigned int FreeTime;
    char ssid[UTT_SSID_LENTH];
    char ssid2[UTT_SSID_LENTH];
    char ssid2_5g[UTT_SSID_LENTH];
    char ssid_5g[UTT_SSID_LENTH];
#if (WLAN_MULTI_SSID == FYES)
    char encodeType[7];/*ssid编码方式*/
    char encodeType_5g[7];/*ssid_5g编码方式*/
    char encodeType2[7];/*ssid编码方式,对应ssid2*/
    char encodeType2_5g[7];/*ssid编码方式,对应ssid2_5g*/
#endif
    unsigned long dRate;
    unsigned long uRate; 
    int wifitime_active;
//    FreeExp freeExpList[MAX_FREE_EXP_LIST];
} MakeMoneyProfile;
#endif
