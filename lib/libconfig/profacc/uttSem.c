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
 * ��ʼ���ź��������������ź�����ֵ
 * */
static void uttInitSemVal(void)
{
    struct semun sem;
    int i = 0;
    sem.val = 1;/*��ʼ��Ϊ1.����ʾ��Դ��Ϊ1*/
    for(i = 0; i < UTT_SEM_COUNT; i++)
    {/*��ʼ��Ϊ0*/
	semctl(semid, i, SETVAL, sem);
    }

    /*������Ҫ���ź���ֵ�������ʼ��*/
    /*end*/
    return;
}
/*
 *
 * initial sem
 * return : �ɹ��򷵻� �ź�����id ��Ϊ����0������
 * 
 **/
extern int uttSemInit(void)
{
    
    key_t key;
    if(semid <= 0) /*δ��ʼ��*/
    {
	key = ftok(UTT_SEM_SPEED_FILE, 0);
	if(key == -1)
	{
	    perror("utt create semaphore id error\n");
	}
	else
	{
	    semid = semget(key, UTT_SEM_COUNT, IPC_EXCL|IPC_CREAT|0x1B6u);/*����Ϊ����һ������һ���ź������ź�������*/
	    if (semid == -1)
	    {
		if(errno == EEXIST)
		{/*�ź����Ѵ���*/
		    semid = semget(key, UTT_SEM_COUNT, IPC_CREAT|0x1B6u);/*��ȡ�Ѵ������ź���id*/
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
		uttInitSemVal();/*�״δ�����ʼ��*/
	    }
	}
    }

    return semid;
}



/*
 * ����һ����Դ���ź�����1
 * sem_no : �ź��������е� �ź�����š���0��ʼ
 * no_wait: 1 ���ź���ֵС��1 ֱ���˳�
 *          0 ���ź���ֵС��1 �ȴ�
 **/ 
int uttSemP(UTT_SEM_NO sem_no, int no_wait)
{
    struct sembuf sops = {0, -1, 0};
    sops.sem_num = sem_no;

    sops.sem_flg = SEM_UNDO;/*�����쳣��ֹʱ�ָ��ź�������*/
    if(no_wait > 0){
	sops.sem_flg |= IPC_NOWAIT;
    }
   
    return (semop(semid, &sops, 1));                                                                                         
}

/*
 *  �ͷ�һ����Դ���ź�����1
 *  sem_no:�ź������
 *  no_wait: �����÷� ������Ϊ����ʵ��Ϊ�ź���Ϊ0ʱ��������
 **/
int uttSemV(UTT_SEM_NO sem_no, int no_wait)
{
    struct sembuf sops = {0, +1, 0};
    sops.sem_num = sem_no;
    sops.sem_flg = SEM_UNDO;/*�����쳣��ֹʱ�ָ��ź�������*/

#if 0
    /*���²����޷�ʵ�֡��ȴ���0ʱ�ټӵĻ����������Ϊ���벻��ԭ����*/
    if(no_wait > 0){
        sops.sem_flg |= IPC_NOWAIT;
    }
    else
    {/*�ȸ��ź���ֵΪ0*/
	uttWaitZero(sem_no);
    }
#endif
    return (semop(semid, &sops, 1));      
}

/*
 *
 * �ȴ��ź������0
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
 * ɾ���ź���
 *
 **/
void uttDelSem(void)
{
    struct semun sem;

    sem.val = 0;
    semctl(semid,0,IPC_RMID,sem);
    return;
}




