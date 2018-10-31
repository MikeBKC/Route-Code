#include "stdio.h"
#include "stdlib.h"
#include "uttMachine.h"
#define CNT(a) sizeof(a)/sizeof(*a)

/*��uttMachine.h����ĺ���ں˺���ӽ��п���*/
const char *linux_configs[] = {
#if (PPPOE_LIMIT == FYES)
    "CONFIG_UTT_NF_PPPOE_LIMIT=y",/*PPPOE�����ں˿��ƺ�*/
#endif
#if (DNS_FILTER == FYES)
    "CONFIG_UTT_NF_DNS_FILTER=y",/*���������ں˿��ƺ�*/
#endif
#if (TIME_GRP == FYES)
    "CONFIG_UTT_TIME_GRP=y",/*ʱ�����ں˿��ƺ�*/
#endif
	"null"
};
/********************************************************************
 * ������ main
 * ���ܣ� ����ҳ�����
 * ������ 2013-03-18
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/ 
int main(int argc, char **argv) 
{
    FILE *fp = NULL;
    int i = 0;

    fp = fopen("linux-config", "w");/*���ļ����������򴴽�*/
    if (fp == NULL) {
	return 0;
    }
    fseek(fp, 0, SEEK_END);

    /*��ҳ��д���ļ��У��Կո����*/
    for (i = 0; i < (CNT(linux_configs) - 1); i++) {
	fputs(linux_configs[i], fp);
	fputs("\n", fp);
    }
    fclose(fp);/*�ر��ļ�*/

    return 0;
}
