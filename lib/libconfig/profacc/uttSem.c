#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <errno.h>


#include "uttSem.h"
struct semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
static int semid = 0;

#define UTT_SEM_SPEED_FILE "/tmp/uttSemForSpeedweb"

static void uttInitSemVal(void);
/*
 * 初始化信号量集的中所有信号量的值
 * */
static void uttInitSemVal(void)
{
    struct semun sem;
    int i = 0;
    sem.val = 1;/*初始化为1.即表示资源量为1*/
    for(i = 0; i < UTT_SEM_COUNT; i++)
    {/*初始化为0*/
	semctl(semid, i, SETVAL, sem);
    }

    /*特殊需要的信号量值在这里初始化*/
    /*end*/
    return;
}
/*
 *
 * initial sem
 * return : 成功则返回 信号量集id 且为大于0的整数
 * 
 **/
extern int uttSemInit(void)
{
    
    key_t key;
    if(semid <= 0) /*未初始化*/
    {
	key = ftok(UTT_SEM_SPEED_FILE, 0);
	if(key == -1)
	{
	    perror("utt create semaphore id error\n");
	}
	else
	{
	    semid = semget(key, UTT_SEM_COUNT, IPC_EXCL|IPC_CREAT|0x1B6u);/*这里为创建一个包含一个信号量的信号量集合*/
	    if (semid == -1)
	    {
		if(errno == EEXIST)
		{/*信号量已创建*/
		    semid = semget(key, UTT_SEM_COUNT, IPC_CREAT|0x1B6u);/*获取已创建的信号量id*/
		    if (semid == -1)
		    {
			perror("utt get semaphore id error\n");
		    }	
		}
		else
		{
		    perror("utt creat semaphore error\n");
		}

	    }
	    else
	    {
		uttInitSemVal();/*首次创建初始化*/
	    }
	}
    }

    return semid;
}



/*
 * 申请一个资源。信号量减1
 * sem_no : 信号量集合中的 信号量编号。从0开始
 * no_wait: 1 则信号量值小于1 直接退出
 *          0 则信号量值小于1 等待
 **/ 
int uttSemP(UTT_SEM_NO sem_no, int no_wait)
{
    struct sembuf sops = {0, -1, 0};
    sops.sem_num = sem_no;

    sops.sem_flg = SEM_UNDO;/*进程异常终止时恢复信号量操作*/
    if(no_wait > 0){
	sops.sem_flg |= IPC_NOWAIT;
    }
   
    return (semop(semid, &sops, 1));                                                                                         
}

/*
 *  释放一个资源。信号量加1
 *  sem_no:信号量编号
 *  no_wait: 暂无用法 。考虑为后期实现为信号量为0时。才增加
 **/
int uttSemV(UTT_SEM_NO sem_no, int no_wait)
{
    struct sembuf sops = {0, +1, 0};
    sops.sem_num = sem_no;
    sops.sem_flg = SEM_UNDO;/*进程异常终止时恢复信号量操作*/

#if 0
    /*以下操作无法实现。等待到0时再加的互斥操作。因为代码不具原子性*/
    if(no_wait > 0){
        sops.sem_flg |= IPC_NOWAIT;
    }
    else
    {/*等该信号量值为0*/
	uttWaitZero(sem_no);
    }
#endif
    return (semop(semid, &sops, 1));      
}

/*
 *
 * 等待信号量变成0
 */
void uttWaitZero(UTT_SEM_NO sem_no)
{
    struct sembuf sops={0,0,0};
    sops.sem_num = sem_no;

    semop(semid,&sops,1);
    return ;
}
/*
 *
 * 删除信号量
 *
 **/
void uttDelSem(void)
{
    struct semun sem;

    sem.val = 0;
    semctl(semid,0,IPC_RMID,sem);
    return;
}




