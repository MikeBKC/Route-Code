
#include <stdlib.h>
#include <stdio.h>
#include "netkeeper.h"

/**************************************************************************
 * �����ˣ�         ������
 * ά���ˣ�         ������
 * ����ʱ�䣺       2008-11-27
 * ����޸��ˣ�     ������
 * ����޸�ʱ�䣺   2008-11-27
 * �ļ���Ҫ���ܣ�   �ƽ������ͨ���������ͻ��˲���
 ***************************************************************************/

char realNetKeeperName[10+MAX_UNIT_NAME_LEN + 1];

/***********************************************************************
 * �� �� ����	henan_netKeeper_main
 * ����������   �ƽ���������ͻ��˲��ţ����û�������ת��
 * �������:    ��ͨ����pppoe�û���
 * ���������	ת������û���
 * ����ֵ��     ת������û���
 * �������ڣ�	2008-11-27 
 * �޸����ڣ�
 * ���ߣ�       ������	
 * ����˵����   
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
