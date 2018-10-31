#include <stdio.h>
#include <string.h>
#include "typedef.h"
#include "base64.h"

int Base64Encode(char *OrgString,char *Base64String,int OrgStringLen)
{
    /* OrgString �������ַ���ָ��
     * Base64String ����������ַ���ָ��
     * OrgStringLen �������ַ�������*/
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
/*Base64 ����*/
char GetBase64Value(char ch) //�õ�����ֵ
{
    if ((ch >= 'A') && (ch <= 'Z'))   // A ~ Z
        return ch - 'A';
    if ((ch >= 'a') && (ch <= 'z'))   // a ~ z
        return ch - 'a' + 26;
    if ((ch >= '0') && (ch <= '9'))   // 0 ~ 9
        return ch - '0' + 52;
    switch (ch)    // �����ַ�
    {
        case '+':
            return 62;
        case '/':
            return 63;
        case '=':   //Base64 ����ַ�
            return 0;
        default:
            return 0;
    }
}
/* ���뺯��*/
int Base64Decode(char *OrgString, char *Base64String, int Base64StringLen, bool bForceDecode )   //���뺯��
{
    /* OrgString ����������ַ���ָ��
     *          Base64String �������ַ���ָ��
     *                   Base64StringLen �������ַ�������
     *                            bForceDecode ���������ַ������ȴ���ʱ,�Ƿ�ǿ�ƽ���
     *                                         true   ǿ�ƽ���
     *                                                      false ��ǿ�ƽ���*/
    if(Base64StringLen % 4 && !bForceDecode ) //������� 4 �ı���,�� Base64 ����������
    {
        OrgString[0] = '0';
        return -1;
    }
    unsigned char Base64Encode[4];
    int OrgStringLen=0;

    while(Base64StringLen > 2 )   //���������������3��ʱ,��������(ǿ�ƽ���ʱ��Ч)
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
