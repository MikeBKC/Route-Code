#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include "internet.h"
#include "spdComFun.h"
#include "comExeFun.h"
#if 0
/* �����ƶ���speedweb/pdb.c��*/
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void UserStatusChainDel(void);
static void UserStatusChainAdd(void);
static void UserStatusInit(void);
/********************************************************************
* ������ UserStatusProfileChange
* ���ܣ� UserStatus���ر仯 ��Ч����
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void UserStatusProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->UserStatusEn == FUN_ENABLE) && (newProfileDataPtr->UserStatusEn == FUN_DISABLE))
	    {
		UserStatusChainDel();
	    }
	    else if((oldProfileDataPtr->UserStatusEn == FUN_DISABLE) && (newProfileDataPtr->UserStatusEn == FUN_ENABLE))
	    {
		UserStatusChainAdd();
	    }
	    else
	    {
		/*test*/
	    }
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
	    break;
        default:/*default*/
            break;
    }
    return;
}

/********************************************************************
* ������ UserStatusChainDel
* ���ܣ� ɾ��UserStatus����
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void UserStatusChainDel(void)
{
#if 0
    /*ɾ����*/
    doSystem("iptables -t mangle -D POSTROUTING -j UTTHOSTSTA");
#endif
    return;
}

/********************************************************************
* ������ UserStatusChainAdd
* ���ܣ� ���UserStatus����
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void UserStatusChainAdd(void)
{
#if 0
    /*�����*/
    doSystem("iptables -t mangle -I POSTROUTING -j UTTHOSTSTA 1>/dev/null 2>&1");
#endif
    return;
}
/********************************************************************
* ������ UserStatusInit
* ���ܣ� �û�״̬��ʼ������ - ��������й����������������
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void UserStatusInit(void)
{
#if 0
    UserStatusChainDel();
    UserStatusChainAdd();
#endif
    return;
}

/********************************************************************
* ������ funInitUserStatus
* ���ܣ� �û�״̬ ��ʼ����ں���
* ������ 2012-06-10
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
void funInitUserStatus(void)
{
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, UserStatusProfileChange);/*��������*/

    UserStatusInit();/*��ʼ��*/
    return;
}
#endif
