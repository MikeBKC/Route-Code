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
 * linkCheckHelp	-	������·��������Ϣ
 */
static void linkCheckHelp(void)
{
	printf("Usage:\n");
	printf(" uttLinkCheck eth1	-	eth1 is name of wan interface\n");
}
/*
 *  test_server	-	���������Ƿ��з�����
 *
 *  ���������
 *	command		-	��Ҫִ������
 *	feature		-	�������������ֵ
 *
 *  ����ֵ��
 *	0		-	�����޷�����
 *	1		-	�����з�����
 *
 */
static int test_server(char *command, char *feature) 
{
	FILE *fp = NULL;
	char buffer[MAX_STR_LEN + 1];
	int ret = 0;

	/*��ʼ��*/
	memset(buffer, 0, MAX_STR_LEN);

	fp = popen(command, "r");/*ִ������*/
	if (fp != NULL) {/*ִ�гɹ�*/
		/*���ַ����ж�ȡһ��*/
		while (fgets(buffer, MAX_STR_LEN, fp) != NULL) {
			SWORD_PRINTF("%s-%d: buffer = %s\n", __func__, __LINE__, buffer);
			/*�ж��Ƿ��л�Ӧ*/
			if (strstr(buffer, feature) != NULL) { 
				SWORD_PRINTF("%s-%d:�����з�����\n", __func__, __LINE__);
				ret = 1;
				break;
			}
		}
		pclose(fp);
	}
	return ret;
}

/*
 *  main	-	���������Ƿ���PPPoE��DHCP������
 *
 * ���˳��
 *	���ȼ���Ƿ���PPPoE�������������򷵻ؽ�����ټ�⣬
 *	�������ټ���Ƿ���DHCP������
 *  �����
 *	��printf��ӡ�����Խ��
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

	/*�����ڴ��ʼ��*/
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

	/*�����ڴ����*/
	ProfDetach();
	return 0;
}

#else
int main(int argc, char **argv) 
{
    return 0;
}
#endif
