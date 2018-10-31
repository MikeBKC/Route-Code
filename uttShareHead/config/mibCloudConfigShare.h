
#ifndef MIBCLOUDCONFIG_SHARE_H
#define MIBCLOUDCONFIG_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"



typedef enum{
    UTT_ACCESS_NEW = 0 , /*新用户接入*/
    UTT_ACCESS_AGAIN = 1,/*短期内再次接入*/
    UTT_ACCESS_WIRED =2,/*有线接入用户*/
    UTT_ACCESS_WIRELESS = 3, /*无线接入用户*/
    UTT_ACCESS_WHITE = 4,/*白名单用户*/
    UTT_ACCESS_REMARK = 5,/*做过备注的用户*/
    UTT_ACCESS_MACLIMIT = 6,/*做过mac地址限速的用户*/
    UTT_ACCESS_MONEY = 7,/*赚钱wifi接入的用户*/

}UTT_ACCESS_WARN_TYPE;/*注意：以上最大为31。且因为需要和云同步。所以值不能改变*/


typedef  struct st_noticeMac{
    MacAddr  macaddr;
    Uint32   sendFlag;
    Uint32   accessType;/*接入类型*/
    char     name[MAX_NAME_LEN+1];
}NoticeMac;

typedef struct st_CloudConfig_Profile{
    mibHead head;
    Uint32  noticeBanMask;/*屏蔽的提醒类型*/
    MacAddr friendMac[MAX_FRIEND_NUM];/*做过备注的mac*/
    char    friendName[MAX_FRIEND_NUM][MAX_NAME_LEN+1];

}CloudConfigProfile;

#endif
