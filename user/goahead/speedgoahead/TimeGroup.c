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
#if (TIME_GRP == FYES)
#if (PARENTS_CONTROL == FYES)
extern void parentsControlOnTimeGrp(TimeGrpProfile *oldProf, TimeGrpProfile *newProf);
#endif
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
extern int profChangeNfNlSock;
extern void sendMsgToKernel(int socket, MibProfileType proftype, int instIndex, Pointer oldDataPtr, int profsize, ProfChangeType changeType);
#endif
/**
 * ����ʱ����ı仯����dns���˹���
 */
extern void dnsFilterTime(TimeGrpProfile *timeGrpProf, char action, char *fir_chain, char *pre_chain, char *chain);
static void dnsFilterOnTimeGrp(TimeGrpProfile *oldProf, TimeGrpProfile *newProf)
{
    SystemProfile *profSys = NULL;
    profSys = (SystemProfile *)ProfGetInstPointByIndex(MIB_PROF_SYSTEM, 0); 
    if (profSys->dnsFilterEn == FUN_ENABLE)/*dns���˿���*/
    {
	if(strcmp(profSys->dnsFilterControl.timeGrpName,
		    newProf->head.name) == 0 )/*�����˸�ʱ����*/
	{
	    /*���³�ʼ��*/
	    dnsFilterTime(oldProf, Iptables_DEL, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	    dnsFilterTime(oldProf, Iptables_DEL, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
	    dnsFilterTime(newProf, Iptables_ADD, PRE_DNS_INPUT_CHAIN, DNS_PRE_INPUT_CHAIN, DNS_INPUT_CHAIN);
	    dnsFilterTime(newProf, Iptables_ADD, PRE_DNS_FORWARD_CHAIN, DNS_PRE_FORWARD_CHAIN, DNS_FORWARD_CHAIN);
	}

    }
    return;
}
/*
 * �������ø�ʱ����Ĺ�������
 * */
static void upRuleOnTimeGrp(TimeGrpProfile *oldProf, TimeGrpProfile *newProf)
{

    dnsFilterOnTimeGrp(oldProf, newProf);/*����dns����*/
#if (PARENTS_CONTROL == FYES)
    parentsControlOnTimeGrp(oldProf, newProf);
#endif

}
static void timeGrpProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const TimeGrpProfile *oldProfileDataPtr, const TimeGrpProfile *newProfileDataPtr )
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    sendMsgToKernel(profChangeNfNlSock, profileType,index, (Pointer)oldProfileDataPtr,sizeof(*oldProfileDataPtr),changeType);
#else
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
	    upRuleOnTimeGrp((TimeGrpProfile *)oldProfileDataPtr, (TimeGrpProfile *)newProfileDataPtr);/*�������ʱ������صĹ���*/
	    break;
        case PROF_CHANGE_DEL:
	    break;
        case PROF_CHANGE_DELALL:
        default:
            break;
    };
    return;
#endif
}
/**
 * ʱ������ں���
 */
void funInitTimeGrp(void)
{
#ifdef CONFIG_PERFORMANCE_OPTIMIZATION
    int min = 0, max = 0,i;
    TimeGrpProfile *prof = NULL;
    MibProfileType mibType = MIB_PROF_TIMEGRP;
    
    ProfInstLowHigh(mibType, &max, &min);

    for (i = min; i < max; i++) {
	/*get TImeGroup inst*/
	prof = (TimeGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE)) {
	    sendMsgToKernel(profChangeNfNlSock, mibType,i, (Pointer)NULL,0,PROF_CHANGE_ADD);
	}
    }
#endif
    registerForProfileUpdates(MIB_PROF_TIMEGRP, (Pointer)NULL, (ProfileUpdateFnc)timeGrpProfileChange);/*��������*/
    return;
}
#endif
