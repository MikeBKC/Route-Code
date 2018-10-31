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

#if (EXCEPT_ALI == FYES)
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int  spdNfNlSock;
static int  uttNfNlAliDelAll(void);
static void uttNfNlAliAdd(void);
static void AliChainAOrD(char action);
static void exceptaliUpdatePortMap(void); 

//#define SWORD_DEBUG
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void exceptaliProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptAliProfile *oldProfileDataPtr, const ExceptAliProfile *newProfileDataPtr );
static void exceptaliglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void exceptaliInit(void);

/*ɾ���ں˶�Ӧ�����в���*/
static int uttNfNlAliDelAll(void)
{
    UttNlNfAliConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_ALI;
    /*ɾ������*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;

    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*������Ϣ���ں�*/
    return 0;
}

/********************************************************************
 * ������ uttNfNlAliAdd
 * ���ܣ� �½�Ali������
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void uttNfNlAliAdd(void) 
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    UttNlNfAliConf  nlConf;
    int min, max, index;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_ALI;
    /*����*/
    nlConf.confType = UTT_NFCONF_ADD;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
        prof = (ExceptAliProfile*)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
                && (prof->head.active == TRUE))
        {
            strcpy(nlConf.entryVal.aliNumber, prof->aliNumber);/*Ali����*/
            SWORD_PRINTF("%s------%d, prof->aliNumber = %s, nlConf.entryVal.aliNumber = %s\n", __func__, __LINE__, prof->aliNumber, nlConf.entryVal.aliNumber);
            uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*������Ϣ���ں�*/
        }
    }
    return;
}
/********************************************************************
 * ������ exceptaliProfileChange
 * ���ܣ� Ali���������ñ仯 ��Ч����
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void exceptaliProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptAliProfile *oldProfileDataPtr, const ExceptAliProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
            uttNfNlAliDelAll();
            uttNfNlAliAdd();
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
 * ������ exceptaliglobalProfileChange
 * ���ܣ� Ali���������ر仯 ��Ч����
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void exceptaliglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
            if((oldProfileDataPtr->exceptAliEn == FUN_ENABLE) && (newProfileDataPtr->exceptAliEn == FUN_DISABLE))
            {/*������Ϊ�ر�*/
#ifdef CONFIG_UTT_NETFILTER
                /* �ؽ�mangle FORWARD�й��� */
                reNewPdbChainJumpRule();
#endif

                AliChainAOrD(Iptables_DEL);
                uttNfNlAliDelAll();
            }
            else if((oldProfileDataPtr->exceptAliEn == FUN_DISABLE) && (newProfileDataPtr->exceptAliEn == FUN_ENABLE))
            {/*�رձ�Ϊ����*/
#ifdef CONFIG_UTT_NETFILTER
                /* �ؽ�mangle FORWARD�й��� */
                reNewPdbChainJumpRule();
#endif

                AliChainAOrD(Iptables_ADD);
                uttNfNlAliAdd();
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
 * ������ AliChainAOrD
 * ���ܣ� ���ӻ�ɾ��Ali��������������iptables����
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void AliChainAOrD(char action)
{
    doSystem("iptables -t mangle -%c %s -j UTTEXCEPTALIIM", action, IM_EXCEPT_CHAIN);
    SWORD_PRINTF("iptables -t mangle -%c %s -j UTTEXCEPTALIIM\n", action, IM_EXCEPT_CHAIN);
    return;
}

/********************************************************************
 * ������ exceptaliInit
 * ���ܣ� ��֤��ʼ������ - ��������й����������������
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void exceptaliInit(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys->exceptAliEn == FUN_DISABLE) 
    {
        AliChainAOrD(Iptables_DEL);
    }
    else
    {
        AliChainAOrD(Iptables_ADD);
    }
    uttNfNlAliDelAll();
    uttNfNlAliAdd();
    return;
}
/********************************************************************
 * ������ exceptaliUpdatePortMap
 * ���ܣ� ���°��������������˺ż�¼map
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
static void exceptaliUpdatePortMap(void) 
{
    MibProfileType mibType = MIB_PROF_EXCEPT_ALI;
    ExceptAliProfile *prof = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ExceptAliProfile *)ProfGetInstPointByIndex(mibType, i);
        if ((ProfInstIsFree(prof) == 0) && (strncmp(prof->head.name, "ID", 2) == 0)) 
        {/*ȡ��id���������*/
            webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);/*ȡ�����е�id��*/
            if (webInstIndex != 0U) 
            {
                setPortMap(&(nvramProfile->exceptaliInstRec.instRecMap), webInstIndex);/*��ֵ��ʵ����*/
            }
        }
    }

    return;
}
/********************************************************************
 * ������ funInitExceptAli
 * ���ܣ� ��֤���� ��ʼ����ں���
 * ������ 2013-9-21
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/
void funInitExceptAli(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)exceptaliglobalProfileChange);/*��������*/
    registerForProfileUpdates(MIB_PROF_EXCEPT_ALI, (Pointer)NULL, (ProfileUpdateFnc)exceptaliProfileChange);/*��������*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    exceptaliInit();/*��ʼ��*/
    /*��ʼ����¼map */
    exceptaliUpdatePortMap();
    return;
}
#endif
#endif
