#include	<string.h>
#include	<time.h>
#if ((EASY_TASK == FYES) && (WIRELESS == FYES))

#define DELIVERY_END_TIME 1293847200u

/*
 *  main	-	���ʱ���Ƿ��Ѹ��£��������򴥷���ʱ����ز���
 */
int main(int argc, char *argv[]) 
{
	unsigned int tmCurrent = 0u;

	while (1) {

		tmCurrent = time(NULL);
		/*��ʱ����ڳ���ʱ����2Сʱ����Ϣ��speedweb���д���*/
		if(tmCurrent > DELIVERY_END_TIME) {
			system("snd2speed wireless date");
			exit(1);
		}

		/*ÿ��ѭ��˯��10s*/ 
		sleep(10);
	}

	return 0;
}

#else
int main(int argc, char **argv) 
{
    return 0;
}
#endif
