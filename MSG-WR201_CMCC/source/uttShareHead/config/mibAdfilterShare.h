#ifndef MIB_ADFILTER_SHARE_H_
#define MIB_ADFILTER_SHARE_H_
#include "mibunionShare.h"
#include "mibtypeShare.h"

#define HOST_LEN 35
#define URI_LEN 27
#define CONFIG_LEN 20
#if 1
#define SECONDS_OF_DAY (24*60*60)
#else
#define SECONDS_OF_DAY (5*60)/*测试用*/
#endif
#define APP_DATA_JSON_TYPE 1
#define ADFILTER_VERSION 1/*应用版本，用于策略更新控制*/

typedef struct st_Adfilter_Profile {
    mibHead head;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    int policyVersion;/*用于检测更新*/
    int updateType;/*更新类型,0-自动；1-手动*/
    unsigned int checkCycle;/*检测周期，默认出厂状态下为一天检测一次,单位是天*/
    unsigned int oldVersion;/*检测版本,路由器现有配置的版本*/
    unsigned int newVersion;/*检测版本,从远程服务器获取到的值*/
#endif
    char originalHost[CONFIG_LEN][HOST_LEN+1];
    char originalUri[CONFIG_LEN][URI_LEN+1];
    char host[CONFIG_LEN][HOST_LEN+1];
    char uri[CONFIG_LEN][URI_LEN+1];
    char normalHostIp[CONFIG_LEN][16];
} AdfilterProfile;
#endif
