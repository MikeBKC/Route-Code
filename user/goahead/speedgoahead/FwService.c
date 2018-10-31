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
#if (PORT_GRP == FYES)
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif
static void exceptPortGroupProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PortGrpProfile *oldProfileDataPtr, const PortGrpProfile *newProfileDataPtr );
static void portGrpEdit(PortGrpProfile *curGrp);

/**
 * ������ fatherName����ȣ��������ڵ㣩
 * fatherName: ָ��Ҫ��������
 * subName : ������ʱ�� �����Ƿ��и��������.
 * return��subNameΪNUll�򷵻�fatherName���
 *         subNae��ΪNULL��������fatherName�Լ��������ҵ�subName������0.���򷵻�rootProf���
 */
extern int portGrpDepSearchName(char* fatherName, char *subName)
{
    PortGrpProfile *subGrp = NULL;
    PortGrpProfile *rootProf = NULL;
    int i = 0, dep = 1, subDep = 0, maxEntrys = 0;

    maxEntrys = PORTGRP_ENTRYS ;
    if(fatherName == NULL)
        return 0;
    rootProf = (PortGrpProfile*)ProfGetInstPointByName(MIB_PROF_PORTGRP, fatherName);
    if(rootProf == NULL)
        return 0;

    if((subName != NULL) && (strcmp(subName, rootProf->head.name) == 0))
    {/*������ͬ�����ҵ�����*/
	dep = 0;
    }
    else
    {
	/*ѭ�����������������Ŀ*/
	for(i = 0; i < maxEntrys; i++)
	{
	    if(rootProf->portEntry[i].entryType == PORTENT_GRP)
        {/*����Ŀ����Ϊ����������*/

            /*���������Ƿ����*/
            subGrp = (PortGrpProfile*)ProfGetInstPointByName(MIB_PROF_PORTGRP, rootProf->portEntry[i].entryVal.grpName);
            if(ProfInstIsFree(subGrp) == 0)/*����ʹ����*/
            {
                subDep = portGrpDepSearchName(subGrp->head.name, subName);/*�����Ƿ������������*/
                if(subDep == 0)
                {/*����������򷵻�0*/
                    dep = 0;
                }
                else 
                {
                    subDep = subDep + 1;/*������ȼ�������*/
                    if(dep < subDep)
                    {/*���µ�ǰ���� ���*/
                        dep = subDep;
                    }
                }
            }

        }
        else if(rootProf->portEntry[i].entryType == PORTENT_GRP)
        {
            dep = 0; 
        }
        if(dep == 0)
        {/*�ڸ�����������ҵ����ҵ���*/
            break;
        }

    }
    }
    return dep;

}

extern void getPortFwGrp(char* fwGrpName,char* str,char* proto)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile* prof = NULL;
    int i = 0,j = 0, min = 0, max = 0,s = 0;
    char* dot = NULL;
    char temp[30]={0};

    if(fwGrpName == NULL)
        return NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((ProfInstIsFree(prof) == 0)
             && (strcmp(fwGrpName,prof->head.name) == 0)
             && (prof->grpType == PORTGRP_PORT)
          )
        {
            for(j = 0;j < PORTGRP_ENTRYS; j++ )
            {
             memset(temp,0,sizeof(temp));
             if(prof->portEntry[j].entryType == PORTENT_GRP)
             {
                 getPortFwGrp(prof->portEntry[j].entryVal.grpName,str,proto);
#if 0
                 strcat(proto,",");
                 strcat(str,",");
#endif
             }
             else
             if(prof->portEntry[j].entryType == PORTENT_TCP)
             {
             sprintf(temp,"%sT-%d:%d,%d:%d;",temp
                     ,prof->portEntry[j].entryVal.portRange.ssport
                     ,prof->portEntry[j].entryVal.portRange.seport
                     ,prof->portEntry[j].entryVal.portRange.dsport
                     ,prof->portEntry[j].entryVal.portRange.deport);
             if(strstr(str,temp) == NULL)
             {
                 strcat(str,temp);
             }
             }
             else
             if(prof->portEntry[j].entryType == PORTENT_UDP)
             {
             sprintf(temp,"%sU-%d:%d,%d:%d;",temp
                     ,prof->portEntry[j].entryVal.portRange.ssport
                     ,prof->portEntry[j].entryVal.portRange.seport
                     ,prof->portEntry[j].entryVal.portRange.dsport
                     ,prof->portEntry[j].entryVal.portRange.deport);
             if(strstr(str,temp) == NULL)
             {
                 strcat(str,temp);
             }
             }
             else
             if(prof->portEntry[j].entryType == PORTENT_OTHER)
             {
             sprintf(proto,"%s%d;",proto,prof->portEntry[j].entryVal.protocol);
             }
             else
             if(prof->portEntry[j].entryType == PORTENT_SYS)
             {
                 for(s=0;s<STATIC_SER_NUM;s++)
                 {
                     if(strcmp(prof->portEntry[j].entryVal.sysGrp.sysGrpName,tcpserver[s])==0)
                     {
                         if(s<STATIC_TCPSERV_NUM) {
                             sprintf(temp,"T-1:65535,%d:%d;",port_start[s],port_end[s]);
                             if(strstr(str,temp) == NULL)
                             {
                                 strcat(str,temp);
                             }
                         }else if(s<(STATIC_TCPSERV_NUM+STATIC_UDPSERV_NUM)){
                             sprintf(temp,"U-1:65535,%d:%d;",port_start[s],port_end[s]);
                             if(strstr(str,temp) == NULL)
                             {
                                 strcat(str,temp);
                             }
                         }else{
                             sprintf(proto,"%s%d;",proto,prof->portEntry[j].entryVal.sysGrp.protocol);
                         }
                         break;
                     }
                 }
             }
            }
#if 0
            dot = strrchr(str,',');        if(dot != NULL) *dot = '\0';
            dot = strrchr(proto,',');      if(dot != NULL) *dot = '\0';
#endif
#if 0
            if(proto[strlen(proto) - 1]==',') proto[strlen(proto) - 1]='\0';
            if(str[strlen(str) - 1]==',')     str[strlen(str) - 1]='\0';
#endif
        }
    }
}

extern char* getFwGContentGrp(char* fwGrpName,char* temp)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile* prof = NULL;

    int i = 0,j = 0, min = 0, max = 0;
    char *dot = NULL;
    int  size = 320;

    SWORD_PRINTF("%s_____%d;\n",__func__,__LINE__);
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        SWORD_PRINTF("%s_____%d;\n",__func__,__LINE__);
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((ProfInstIsFree(prof) == 0)
             && (strcmp(fwGrpName,prof->head.name) == 0)
             && ((prof->grpType == PORTGRP_URL) || (prof->grpType == PORTGRP_DNS) || (prof->grpType == PORTGRP_KEY))
            )
        {
            for(j = 0;j < PORTGRP_ENTRYS; j++ )
            {
                if(prof->portEntry[j].entryType == PORTENT_GRP)
                {
                   getFwGContentGrp(prof->portEntry[j].entryVal.grpName,temp);
#if 0
                   strcat(temp,";");
#endif
                }
                else
                if(prof->portEntry[j].entryType == PORTENT_NORMAL)
                {
                   strcat(temp,prof->portEntry[j].entryVal.content);
                   strcat(temp,";");
                }
            }
#if 0
            dot = strrchr(temp,';');
            if(dot != NULL)
                *dot='\0';
#endif
            SWORD_PRINTF("%s_____%d; hanshu temp : %s\n",__func__,__LINE__,temp);
        } 
    }
    return temp;
}
static void portGroupInit(void);
/********************************************************************
* ������ exceptPortGroupProfileChange
* ���ܣ� ��֤���ñ仯 ��Ч����
* ������ 
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� 
********************************************************************/
static void exceptPortGroupProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const PortGrpProfile *oldProfileDataPtr, const PortGrpProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    break;
        case PROF_CHANGE_EDIT:
        portGrpEdit((PortGrpProfile *)newProfileDataPtr);
	    break;
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
        default:/*default*/
            break;
    }
    return;
}
static void PortGrpUpdate(PortGrpProfile *curGrp)
{
   if(checkFwGrpInFireWall(curGrp->head.name))
   {
       updateFireWall();
   }
}
/********************************************************************
* ������ portGrpEdit
* ���ܣ� ���޸�
*	 ��setNameΪ���༭������,��������setName���鱻���¹���
* ������ 2012-05-07
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� 
********************************************************************/
static void portGrpEdit(PortGrpProfile *curGrp)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    IpGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if(ProfInstIsFree(prof) == 0 && curGrp->grpType == prof->grpType && (strcmp(prof->head.name,curGrp->head.name) == 0))/*����ʹ����*/
        {/*ֻ����ͬ���͵���*/
            SWORD_PRINTF("%s------%d, curGrp->head.name = %s\n",__func__, __LINE__, curGrp->head.name);
            PortGrpUpdate(curGrp);
        }
    }
    return;
}

/********************************************************************
* ������ portGroupInit
* ���ܣ� ��֤��ʼ������ 
* ������ 
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� 
********************************************************************/
static void portGroupInit(void)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile* prof = NULL;

    int i = 0, min = 0, max = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if(ProfInstIsFree(prof) == 0)/*����ʹ����*/
        {
        } 
    }

    return;
}
/********************************************************************
* ������ funInitPortGroup
* ���ܣ� ��֤���� ��ʼ����ں���
* ������ 
* ������ ��
* ���أ� ��
* ����� ��
* ���ߣ� 
********************************************************************/
void funInitPortGroup(void)
{
    registerForProfileUpdates(MIB_PROF_PORTGRP, (Pointer)NULL, (ProfileUpdateFnc)exceptPortGroupProfileChange);/*��������*/

    portGroupInit();/*��ʼ��*/
    return;
}
#endif
