#include	<string.h>
#include	<time.h>
#if ((EASY_TASK == FYES) && (WIRELESS == FYES))

#define DELIVERY_END_TIME 1293847200u

/*
 *  main	-	检测时间是否已更新，若更新则触发与时间相关操作
 */
int main(int argc, char *argv[]) 
{
	unsigned int tmCurrent = 0u;

	while (1) {

		tmCurrent = time(NULL);
		/*若时间大于出厂时间后的2小时则发消息给speedweb进行处理*/
		if(tmCurrent > DELIVERY_END_TIME) {
			system("snd2speed wireless date");
			exit(1);
		}

		/*每次循环睡眠10s*/ 
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
