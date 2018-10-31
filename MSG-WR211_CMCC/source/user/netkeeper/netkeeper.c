#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "netkeeper.h"

static  char realNetKeeperName[10+MAX_UNIT_NAME_LEN + 1];

/*
    ��ѹ����16���������ַ���תΪ�����ַ�����
    length Ϊ��ת���Ŀ����ַ����ĳ��ȡ�
    tag:0-��bcd��1-�ҿ�bcd����lengthΪż��ʱ,�˲�����Ч��
    str����Ӧ����Ϊlength+1��
    ����������strĩβ�ӽ�������
    str������Ϊ'0'-'9','a'-'f'�ĸ�ʽ��
    ���磺
    memcpy( hex, "\x01\x23\x45\x6a\xbc\xde", 6 );
    swBcdhex2Strabc( hex, str, 11, 0 );
    �� str Ϊ "0123456abcd"��
    ��
    swBcdhex2Strabc( hex, str, 11, 1 );
    �� str Ϊ "123456abcde"��
*/
char *swBcdhex2Strabc(const unsigned char *hex, unsigned char *str, int length, int tag)
/* tag:0-��bcd��1-�ҿ�bcd */
{
        int     i;
        unsigned char ch;
        int check;

        if ( length % 2 )
                check=1;
        else
                check=0;

        for( i=0;i<length;i++ )
        {
            if ( check==0 || tag==0 ) {
                if( i%2 == 0 )
                    ch = ( hex[i/2] & 0xf0 ) >> 4;
                else
                    ch = hex[i/2] & 0x0f;
            }
            else {  //����Ϊ�����������ҿ�
                if( i%2 )
                    ch = ( hex[i/2+1] & 0xf0 ) >> 4;
                else
                    ch = hex[i/2] & 0x0f;
            }
            if ( ch <= 9 )
                str[i]=ch+'0';
            else
                str[i]=ch+'a'-10;
        }
        str[i]=0x0;
        return str;
}

int main(int argc, char** argv)
{
    int where;
    char* name;
    int i = 0;
    char buf[0x10];
    if(argc < 3) {
        printf("sample: netkeeper argv[1] argv[2]\n");
        printf("argv[1]: SpMode1 jiangxi, SpMode2 hunanhubei, SpMode3 henan\n");
        printf("argv[2]: username\n");
        exit(0);
    }

    for(i = 1; i < 20; i++) {
        sprintf(buf, "SP%d", i);
        if(strcmp(argv[1], buf) == 0) {
            where = i;
            break;
        }
    }
    switch(where) {
        case NETKEEPER_HENAN:
            name = henan_netKeeper_main(argv[2]);
            break;
        case NETKEEPER_HUNANHUBEI:
            name = hunan_netKeeper_main(argv[2]);
            break;
        case NETKEEPER_JIANGXI:
            name = jiangxi_netKeeper_main(argv[2]);
            break;
        default:
	    name = argv[2];
            break;
    }
    /* add by lei */
    char strabc[(10+MAX_UNIT_NAME_LEN)*2+1];
    memset(strabc, 0x00, sizeof(strabc));
    swBcdhex2Strabc(name, strabc, 2*strlen(name),0);
    /* end of add */
    printf("%s",strabc);
    exit(0);
}
