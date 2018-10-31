#include "stdio.h"
#include "stdlib.h"
#include "uttMachine.h"
#define CNT(a) sizeof(a)/sizeof(*a)

/*用uttMachine.h里面的宏对内核宏添加进行控制*/
const char *linux_configs[] = {
#if (PPPOE_LIMIT == FYES)
    "CONFIG_UTT_NF_PPPOE_LIMIT=y",/*PPPOE限速内核控制宏*/
#endif
#if (DNS_FILTER == FYES)
    "CONFIG_UTT_NF_DNS_FILTER=y",/*域名过滤内核控制宏*/
#endif
#if (TIME_GRP == FYES)
    "CONFIG_UTT_TIME_GRP=y",/*时间组内核控制宏*/
#endif
	"null"
};
/********************************************************************
 * 函数： main
 * 功能： 生成页面控制
 * 创建： 2013-03-18
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int main(int argc, char **argv) 
{
    FILE *fp = NULL;
    int i = 0;

    fp = fopen("linux-config", "w");/*打开文件，不存在则创建*/
    if (fp == NULL) {
	return 0;
    }
    fseek(fp, 0, SEEK_END);

    /*将页面写入文件中，以空格隔开*/
    for (i = 0; i < (CNT(linux_configs) - 1); i++) {
	fputs(linux_configs[i], fp);
	fputs("\n", fp);
    }
    fclose(fp);/*关闭文件*/

    return 0;
}
