
#ifndef MIBCLOUDCONFIG_SHARE_H
#define MIBCLOUDCONFIG_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"



typedef enum{
    UTT_ACCESS_NEW = 0 , /*���û�����*/
    UTT_ACCESS_AGAIN = 1,/*�������ٴν���*/
    UTT_ACCESS_WIRED =2,/*���߽����û�*/
    UTT_ACCESS_WIRELESS = 3, /*���߽����û�*/
    UTT_ACCESS_WHITE = 4,/*�������û�*/
    UTT_ACCESS_REMARK = 5,/*������ע���û�*/
    UTT_ACCESS_MACLIMIT = 6,/*����mac��ַ���ٵ��û�*/
    UTT_ACCESS_MONEY = 7,/*׬Ǯwifi������û�*/

}UTT_ACCESS_WARN_TYPE;/*ע�⣺�������Ϊ31������Ϊ��Ҫ����ͬ��������ֵ���ܸı�*/


typedef  struct st_noticeMac{
    MacAddr  macaddr;
    Uint32   sendFlag;
    Uint32   accessType;/*��������*/
    char     name[MAX_NAME_LEN+1];
}NoticeMac;

typedef struct st_CloudConfig_Profile{
    mibHead head;
    Uint32  noticeBanMask;/*���ε���������*/
    MacAddr friendMac[MAX_FRIEND_NUM];/*������ע��mac*/
    char    friendName[MAX_FRIEND_NUM][MAX_NAME_LEN+1];

}CloudConfigProfile;

#endif
