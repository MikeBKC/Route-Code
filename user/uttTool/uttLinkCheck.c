#include	<string.h>
#include	"uttMachine.h"
#include	"mib.h"
#include	"profacce.h"
#include	"comExeFun.h"
#if (EASY_TASK == FYES)

#define SWORD_DEBUG 0
#if SWORD_DEBUG 
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#define MAX_COMMAND_LEN 50
#define MAX_STR_LEN 256

/*
 * linkCheckHelp	-	外网线路检测帮助信息
 */
static void linkCheckHelp(void)
{
	printf("Usage:\n");
	printf(" uttLinkCheck eth1	-	eth1 is name of wan interface\n");
}
/*
 *  test_server	-	测试外网是否有服务器
 *
 *  输入参数：
 *	command		-	需要执行命令
 *	feature		-	所查服务器特征值
 *
 *  返回值：
 *	0		-	外网无服务器
 *	1		-	外网有服务器
 *
 */
static int test_server(char *command, char *feature) 
{
	FILE *fp = NULL;
	char buffer[MAX_STR_LEN + 1];
	int ret = 0;

	/*初始化*/
	memset(buffer, 0, MAX_STR_LEN);

	fp = popen(command, "r");/*执行命令*/
	if (fp != NULL) {/*执行成功*/
		/*从字符流中读取一行*/
		while (fgets(buffer, MAX_STR_LEN, fp) != NULL) {
			SWORD_PRINTF("%s-%d: buffer = %s\n", __func__, __LINE__, buffer);
			/*判断是否有回应*/
			if (strstr(buffer, feature) != NULL) { 
				SWORD_PRINTF("%s-%d:外网有服务器\n", __func__, __LINE__);
				ret = 1;
				break;
			}
		}
		pclose(fp);
	}
	return ret;
}

/*
 *  main	-	测试外网是否有PPPoE或DHCP服务器
 *
 * 检测顺序：
 *	首先检测是否有PPPoE服务器，若有则返回结果不再检测，
 *	若无则再检测是否有DHCP服务器
 *  输出：
 *	用printf打印出测试结果
 *
 */
int main(int argc, char *argv[]) 
{
	MibProfileType mibType = MIB_PROF_EASY_TASK;
	EasyTaskProfile *prof = NULL;
	char pppoe_command[MAX_COMMAND_LEN + 1];
	char dhcp_command[MAX_COMMAND_LEN + 1];

	if (argc < 2) {
		linkCheckHelp();
		return -1;
	}

	/*共享内存初始化*/
	if(ProfInit() == PROFFAIL) {
		return -1;
	}
    
	wanConnStatsChange(CHECKING);

	sprintf(pppoe_command, "pppoe -I %s -A", argv[1]);
	sprintf(dhcp_command, "udhcpc -i %s -n", argv[1]);
	SWORD_PRINTF("%s-%d: pppoe_command = %s, dhcp_command = %s\n",
			__func__, __LINE__, pppoe_command, dhcp_command);

	prof = (EasyTaskProfile*)ProfGetInstPointByIndex(mibType, 0);
	if (test_server(pppoe_command, "Access-Concentrator") == 1){
		prof->wanMode = ETH_CONN_PPPOE;
	}
	else if (test_server(dhcp_command, "Sending select") == 1){
		prof->wanMode = ETH_CONN_DHCP;
	}
	else {
		prof->wanMode = ETH_CONN_STATIC;
	}

	wanConnStatsChange(CHECK_COMPLETE);

	/*共享内存分离*/
	ProfDetach();
	return 0;
}

#else
int main(int argc, char **argv) 
{
    return 0;
}
#endif
