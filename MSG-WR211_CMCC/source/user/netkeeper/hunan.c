
#include <stdlib.h>
#include <stdio.h>
#include "netkeeper.h"

/***********************************************************************
 * 函 数 名：	hunan_netKeeper_main
 * 功能描述：   破解湖南湖北星空极速客户端拨号，将用户名进行转化
 * 输入参数:    电信给的pppoe用户名
 * 输出参数：	转换后的用户名
 * 返回值：     转换后的用户名
 * 创建日期：	2008-11-27 
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：   
***********************************************************************/

static  char realNetKeeperName[10+MAX_UNIT_NAME_LEN + 1];
const char* hunan_netKeeper_main(char* netKeeperName)
{
    int i,namelen;
    unsigned int sum = 0;
    int tmp1 = 0,tmp2 = 0;
    char rdata[]="NJ3r05t949R9jdkdfo4lDLR2Evzl35Rkdl1tggtjofdKRIOkLH888iJkyUkjNNbVvjU84410Keloekri78DJ490I574RjK96HjJt7676554r5tgjhHhBGY78668754631HIUHUggGgyGFY78684Ffhyj6JJBN464335dfDDXZccblpoppytrdrdfGFtrgjii87pdl545";
    namelen = strlen(netKeeperName);
    /*
     * tmp1 ebp
     * tmp2 eax
     */
    for(i = 0;i < namelen;i++){
        if(netKeeperName[i] == 40)break;
        sum = netKeeperName[i];
        if(netKeeperName[i] < 0)
            sum |= 0xffffff00;
        tmp1 = tmp2;
        tmp1 <<= 5;
        tmp1 += tmp2;
        sum += tmp1;
        tmp2 = sum;
    }

    tmp2 = sum % 0xc8;

    realNetKeeperName[0] = 0x7e;
    realNetKeeperName[1] = rdata[tmp2];
    memcpy( realNetKeeperName+2,netKeeperName,namelen+1);
    return (const char*) realNetKeeperName;
}
