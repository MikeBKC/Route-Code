#include <stdio.h>
#include <string.h>
#include "typedef.h"
#include "base64.h"

int Base64Encode(char *OrgString,char *Base64String,int OrgStringLen)
{
    /* OrgString 待编码字符串指针
     * Base64String 保存编码结果字符串指针
     * OrgStringLen 待编码字符串长度*/
    static char Base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int Base64StringLen = 0;

    while(OrgStringLen > 0 )
    {
        *Base64String ++ = Base64Encode[(OrgString[0] >> 2 ) & 0x3f];
        if(OrgStringLen > 2 )
        {
            *Base64String ++ = Base64Encode[((OrgString[0] & 3) << 4) | (OrgString[1] >> 4)];
            *Base64String ++ = Base64Encode[((OrgString[1] & 0xF) << 2) | (OrgString[2] >> 6)];
            *Base64String ++ = Base64Encode[OrgString[2] & 0x3F];
        }
        else
        {
            switch(OrgStringLen )
            {
                case 1:
                    *Base64String ++ = Base64Encode[(OrgString[0] & 3) << 4 ];
                    *Base64String ++ = '=';
                    *Base64String ++ = '=';
                    break;
                case 2:
                    *Base64String ++ = Base64Encode[((OrgString[0] & 3) << 4) | (OrgString[1] >> 4)];
                    *Base64String ++ = Base64Encode[((OrgString[1] & 0x0F) << 2) | (OrgString[2] >> 6)];
                    *Base64String ++ = '=';
                    break;
            }
        }

        OrgString +=3;
        OrgStringLen -=3;
        Base64StringLen +=4;
    }

    *Base64String = 0;
    return Base64StringLen;
}
/*Base64 解码*/
char GetBase64Value(char ch) //得到编码值
{
    if ((ch >= 'A') && (ch <= 'Z'))   // A ~ Z
        return ch - 'A';
    if ((ch >= 'a') && (ch <= 'z'))   // a ~ z
        return ch - 'a' + 26;
    if ((ch >= '0') && (ch <= '9'))   // 0 ~ 9
        return ch - '0' + 52;
    switch (ch)    // 其它字符
    {
        case '+':
            return 62;
        case '/':
            return 63;
        case '=':   //Base64 填充字符
            return 0;
        default:
            return 0;
    }
}
/* 解码函数*/
int Base64Decode(char *OrgString, char *Base64String, int Base64StringLen, bool bForceDecode )   //解码函数
{
    /* OrgString 保存解码结果字符串指针
     *          Base64String 待解码字符串指针
     *                   Base64StringLen 待解码字符串长度
     *                            bForceDecode 当待解码字符串长度错误时,是否强制解码
     *                                         true   强制解码
     *                                                      false 不强制解码*/
    if(Base64StringLen % 4 && !bForceDecode ) //如果不是 4 的倍数,则 Base64 编码有问题
    {
        OrgString[0] = '0';
        return -1;
    }
    unsigned char Base64Encode[4];
    int OrgStringLen=0;

    while(Base64StringLen > 2 )   //当待解码个数不足3个时,将忽略它(强制解码时有效)
    {
        Base64Encode[0] = GetBase64Value(Base64String[0]);
        Base64Encode[1] = GetBase64Value(Base64String[1]);
        Base64Encode[2] = GetBase64Value(Base64String[2]);
        Base64Encode[3] = GetBase64Value(Base64String[3]);

        *OrgString ++ = (Base64Encode[0] << 2) | (Base64Encode[1] >> 4);
        *OrgString ++ = (Base64Encode[1] << 4) | (Base64Encode[2] >> 2);
        *OrgString ++ = (Base64Encode[2] << 6) | (Base64Encode[3]);

        Base64String += 4;
        Base64StringLen -= 4;
        OrgStringLen += 3;
    }

    return OrgStringLen;
}
