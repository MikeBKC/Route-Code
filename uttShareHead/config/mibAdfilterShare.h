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
#define SECONDS_OF_DAY (5*60)/*������*/
#endif
#define APP_DATA_JSON_TYPE 1
#define ADFILTER_VERSION 1/*Ӧ�ð汾�����ڲ��Ը��¿���*/

typedef struct st_Adfilter_Profile {
    mibHead head;
#if (APP_POLICY_REMOTE_UPDATE == FYES)
    int policyVersion;/*���ڼ�����*/
    int updateType;/*��������,0-�Զ���1-�ֶ�*/
    unsigned int checkCycle;/*������ڣ�Ĭ�ϳ���״̬��Ϊһ����һ��,��λ����*/
    unsigned int oldVersion;/*���汾,·�����������õİ汾*/
    unsigned int newVersion;/*���汾,��Զ�̷�������ȡ����ֵ*/
#endif
    char originalHost[CONFIG_LEN][HOST_LEN+1];
    char originalUri[CONFIG_LEN][URI_LEN+1];
    char host[CONFIG_LEN][HOST_LEN+1];
    char uri[CONFIG_LEN][URI_LEN+1];
    char normalHostIp[CONFIG_LEN][16];
} AdfilterProfile;
#endif
