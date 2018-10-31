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
#include "pdb.h"

#if (EXCEPT_MSN == FYES)
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
static int uttNfNlMSNDelAll(void);
static void uttNfNlMSNAdd(void);
static void MSNChainAOrD(char action);
static void exceptmsnUpdatePortMap(void); 

#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void exceptmsnProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptMSNProfile *oldProfileDataPtr, const ExceptMSNProfile *newProfileDataPtr );
static void exceptmsnglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void exceptmsnInit(void);

/*ɾ���ں˶�Ӧ�����в���*/
static int uttNfNlMSNDelAll(void)
{
    UttNlNfMsnConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_MSN;
    /*ɾ������*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;

    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*������Ϣ���ں�*/
    return 0;
}

/********************************************************************
* ������ exceptmsnProfileChange
* ���ܣ� �½�MSN������
* ������ 2012-06-19
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void uttNfNlMSNAdd(void) 
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    UttNlNfMsnConf  nlConf;
    int min, max, index;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_MSN;
    /*����*/
    nlConf.confType = UTT_NFCONF_ADD;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
	prof = (ExceptMSNProfile*)ProfGetInstPointByIndex(mibType, index);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
		&& (prof->head.active == TRUE))
	{
		strcpy(nlConf.entryVal.msnNumber, prof->msnNumber);/*MSN����*/
		SWORD_PRINTF("%s------%d, prof->msnNumber = %s, nlConf.entryVal.msnNumber = %s\n", __func__, __LINE__, prof->msnNumber, nlConf.entryVal.msnNumber);
		uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*������Ϣ���ں�*/
	}
    }
    return;
}
/********************************************************************
* ������ exceptmsnProfileChange
* ���ܣ� MSN���������ñ仯 ��Ч����
* ������ 2012-06-19
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void exceptmsnProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptMSNProfile *oldProfileDataPtr, const ExceptMSNProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
	    uttNfNlMSNDelAll();
	    uttNfNlMSNAdd();
	    break;
        default:/*default*/
            break;
    }
    return;
}
#ifdef CONFIG_UTT_NETFILTER
extern void reNewPdbChainJumpRule(void);
#endif
/********************************************************************
* ������ exceptmsnglobalProfileChange
* ���ܣ� MSN���������ر仯 ��Ч����
* ������ 2012-06-19
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void exceptmsnglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    if((oldProfileDataPtr->exceptMSNEn == FUN_ENABLE) && (newProfileDataPtr->exceptMSNEn == FUN_DISABLE))
	    {/*������Ϊ�ر�*/
#ifdef CONFIG_UTT_NETFILTER
		/* �ؽ�mangle FORWARD�й��� */
		reNewPdbChainJumpRule();
#endif

		MSNChainAOrD(Iptables_DEL);
		uttNfNlMSNDelAll();
	    }
	    else if((oldProfileDataPtr->exceptMSNEn == FUN_DISABLE) && (newProfileDataPtr->exceptMSNEn == FUN_ENABLE))
	    {/*�رձ�Ϊ����*/
#ifdef CONFIG_UTT_NETFILTER
		/* �ؽ�mangle FORWARD�й��� */
		reNewPdbChainJumpRule();
#endif

		MSNChainAOrD(Iptables_ADD);
		uttNfNlMSNAdd();
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
* ������ exceptmsnProfileChange
* ���ܣ� ���ӻ�ɾ��MSN��������������iptables����
* ������ 2012-06-19
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void MSNChainAOrD(char action)
{
    doSystem("iptables -t mangle -%c %s -j UTTEXCEPTMSN", action, IM_EXCEPT_CHAIN);
    SWORD_PRINTF("iptables -t mangle -%c %s -j UTTEXCEPTMSN\n", action, IM_EXCEPT_CHAIN);
    return;
}

/********************************************************************
* ������ exceptmsnInit
* ���ܣ� ��֤��ʼ������ - ��������й����������������
* ������ 2012-04-27
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void exceptmsnInit(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys->exceptMSNEn == FUN_DISABLE) 
    {
	MSNChainAOrD(Iptables_DEL);
    }
    else
    {
	MSNChainAOrD(Iptables_ADD);
    }
    uttNfNlMSNDelAll();
    uttNfNlMSNAdd();
    return;
}
/********************************************************************
* ������ exceptmsnUpdatePortMap
* ���ܣ� ����MSN�������˺ż�¼map
* ������ 2012-06-19
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
static void exceptmsnUpdatePortMap(void) 
{
    MibProfileType mibType = MIB_PROF_EXCEPT_MSN;
    ExceptMSNProfile *prof = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ExceptMSNProfile *)ProfGetInstPointByIndex(mibType, i);
	if ((ProfInstIsFree(prof) == 0) && (strncmp(prof->head.name, "ID", 2) == 0)) 
	{/*ȡ��id���������*/
	    webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);/*ȡ�����е�id��*/
	    if (webInstIndex != 0U) 
	    {
		setPortMap(&(nvramProfile->exceptmsnInstRec.instRecMap), webInstIndex);/*��ֵ��ʵ����*/
	    }
	}
    }

    return;
}
/********************************************************************
* ������ funInitExceptMSN
* ���ܣ� ��֤���� ��ʼ����ں���
* ������ 2012-04-27
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� Jianqing.Sun 
********************************************************************/
void funInitExceptMSN(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)exceptmsnglobalProfileChange);/*��������*/
    registerForProfileUpdates(MIB_PROF_EXCEPT_MSN, (Pointer)NULL, (ProfileUpdateFnc)exceptmsnProfileChange);/*��������*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    exceptmsnInit();/*��ʼ��*/
    /*��ʼ����¼map */
    exceptmsnUpdatePortMap();
    return;
}
#endif
#endif
