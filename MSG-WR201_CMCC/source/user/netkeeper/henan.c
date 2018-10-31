
#include <stdlib.h>
#include <stdio.h>
#include "netkeeper.h"

/**************************************************************************
 * 创建人：         王炳荣
 * 维护人：         王炳荣
 * 创建时间：       2008-11-27
 * 最后修改人：     王炳荣
 * 最后修改时间：   2008-11-27
 * 文件主要功能：   破解河南网通宽带我世界客户端拨号
 ***************************************************************************/

char realNetKeeperName[10+MAX_UNIT_NAME_LEN + 1];

/***********************************************************************
 * 函 数 名：	henan_netKeeper_main
 * 功能描述：   破解宽带我世界客户端拨号，将用户名进行转化
 * 输入参数:    网通给的pppoe用户名
 * 输出参数：	转换后的用户名
 * 返回值：     转换后的用户名
 * 创建日期：	2008-11-27 
 * 修改日期：
 * 作者：       王炳荣	
 * 附加说明：   
***********************************************************************/


const char* henan_netKeeper_main(char* netKeeperName)
{
    char rdata1[]="9012345678abcdeABCDEFGHIJKLMNfghijklmnUVWXYZxyzuvwopqrstOPQRST";
    int rdata2[]={0x19,0x23,0xB6,0xEC,0x2B,0x29,0x35,0x12,0xE2,0xD7,0x18,0x75,0x23,0xc9,0x34,0x11};

    int find = 0;
    int namelen = 0;
    int i,j,num = 15,tmp1 = 0,tmp4 = 0;
    int var_78 = 0x25,var_7c = 0,var_84,var_80;

    namelen = strlen(netKeeperName);
/*
 * eax tmp1
 * ebx tmp2
 * ecx tmp3
 * edx tmp4
 */
    for(i = 0;i < namelen;i++){
        find = 0;
        for(j = 0;j < 0x3e;j++) {
            if(netKeeperName[i] == rdata1[j]){
                find = 1;
                break;
            }
        }
        if(find == 0){
            realNetKeeperName[i+2] = netKeeperName[i];
            continue;

        }
        if(num < 0)
            num = 15;
        tmp1 = rdata2[num];
        tmp4 = var_78;
        tmp4 *= 3;
        tmp1 ^= tmp4;
        tmp1 ^= var_7c;
        tmp1 += j;
        var_84 = tmp1 / 0x3e;
        var_80 = tmp1 % 0x3e;
        tmp1 = var_80;
        realNetKeeperName[i+2] = rdata1[tmp1];
        tmp4 = var_80;
        tmp4 += 0x24d9;
        var_78 ^= tmp4;
        var_7c += 5;
        num--;
    }
    realNetKeeperName[0] = 0x32;
    realNetKeeperName[1] = 0x3a;
    realNetKeeperName[namelen+2] = 0;
 
    return (const char*) realNetKeeperName;
}
