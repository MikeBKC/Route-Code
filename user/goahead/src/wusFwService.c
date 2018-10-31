#include <stdio.h>
#include <string.h>
#include <mib.h>
#include "typedef.h"
#include "wsIntrn.h"
#include "um.h"
#include "utils.h"
#include "uttfunction.h"
#include "profacce.h"
#include "uttMachine.h"
#include "comExeFun.h"
#include "ipsetApi.h"
#include "translate.h"

#if (PORT_GRP== FYES)
#define MSG_MAXSIZE  100
#define TYPE_PORT   "portGroup"
#define TYPE_URL    "urlGroup"
#define TYPE_DNS    "dnsGroup"
#define TYPE_KEY    "keyGroup"
#define TYPE_MAC    "macGroup"

//#define  PORT_GRP_DEBUG 1
#ifdef  PORT_GRP_DEBUG
#define PORT_GRP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define PORT_GRP_PRINTF(fmt, args...)
#endif

static void printTree();
static bool savePortRange(PortGrpProfile *tmp, char *str, int index);
static int grpDelCheck(PortGrpProfile *repProf);
static int portGrpCheckAdd(PortGrpProfile *repProf);
int portGrpCheckAddTree(PortGrpProfile* curGrp);
static int portGrpCheckEdit(PortGrpProfile* oldProf, PortGrpProfile* newProf);
int portGrpCheckEditTree(PortGrpProfile* curGrp, int fatherDep);

/********************************************************************
 * ������ getFwGrpList
 * ���ܣ� ��ҳ�����������
 * ������ 
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� 
 ********************************************************************/ 
extern int getFwGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile *prof = NULL;
    char type[20] = {0};

    int min = 0, max = 0;
    int i = 0;
    int num = 0;
    websWrite(wp,"var fwGrpEn = 1;");

    websWrite(wp,"var fwGroupNames = new Array();");
    websWrite(wp,"var fwGroupTypes = new Array();");
    websWrite(wp,"var definedServ = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
        memset(type,0,sizeof(type));
	    websWrite(wp,"fwGroupNames[%d] = \"%s\";",num,prof->head.name);
	    websWrite(wp,"fwGroupTypes[%d] = %d;",num,prof->grpType);
	    num++;
	}
    }
    websWrite(wp,"\n");
    for(i=0;i<STATIC_SER_NUM;i++)
    {
	    websWrite(wp,"definedServ[%d] = \"%s\";",i,tcpserver[i]);
    }
    return 0;
}

/********************************************************************
 * ������ portGrpCheckEditTree
 * ���ܣ� �޸�ʱ�Ƿ�·�Լ�����ж�
 * ������ 
 * ������ ���жϵ�������profile
 * ���أ� 
 *	 1 û�л�·
 *	 0 ���ڻ�·
 * ����� ��
 * ���ߣ� 
 ********************************************************************/
int portGrpCheckEditTree(PortGrpProfile* curGrp, int fatherDep)
{
	int flag = 1, break_flag = 0;
	int i = 0, maxEntrys = 0;
	int subDep = 0;
	char referedMsg[MSG_MAXSIZE];
	PortGrpProfile* subProf = NULL;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

    maxEntrys = PORTGRP_ENTRYS ;

	PORT_GRP_PRINTF("%s------%d,maxEntrys = %d\n", __func__, __LINE__, maxEntrys);
	for(i = 0; i < maxEntrys; i++)
	{
		if(curGrp->portEntry[i].entryType == PORTENT_GRP)
		{
			subProf = (PortGrpProfile*)
				ProfGetInstPointByName(MIB_PROF_PORTGRP,curGrp->portEntry[i].entryVal.grpName);
			if(ProfInstIsFree(subProf) != 0)/*����δʹ��*/
			{
				flag = 0;
				PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
				break_flag = 1;
			}
			else
			{
				subDep = portGrpDepSearch(subProf,curGrp->head.name);/*�����Ƿ��и���*/
				PORT_GRP_PRINTF("%s------%d,subDep = %d\n", __func__, __LINE__, subDep);
				if(subDep == 0)
				{/*��·*/
                    ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_CANT_CONTAIN_OTHERS);
                    ErrMsg2 = getTransDataForC(C_LANG_INDEX_CONTAIN);
                    snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s!", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name);
					setTheErrorMsgOnFree(referedMsg);/*ֻ�������ͬ���͵�����*/
					flag = 0;
					PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break_flag = 1;
				}
				else if((subDep + fatherDep + 1) >  MAX_PORTGRP_DEP)
				{/*��ȹ���*/
					ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_DEPTH_CANT_MORE_THAN);
					ErrMsg2 = getTransDataForC(C_LANG_INDEX_CANT_ADD_GROUP);
					snprintf(referedMsg, sizeof(referedMsg), "%s%d%s%s!", ErrMsg, MAX_PORTGRP_DEP, ErrMsg2, subProf->head.name);
					setTheErrorMsgOnFree(referedMsg);
					flag = 0;
					PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break_flag = 1;
				}
				else
				{
					/*test*/
				}
			}/*end ����ʹ��*/
		}/*end ����ΪPORTENT_GRP*/
        else if(curGrp->portEntry[i].entryType == PORTENT_SYS)
        {
            if((1 + fatherDep + 1) >  MAX_PORTGRP_DEP)
            {/*��ȹ���*/
                ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_DEPTH_CANT_MORE_THAN);
                ErrMsg2 = getTransDataForC(C_LANG_INDEX_CANT_ADD_GROUP);
                snprintf(referedMsg, sizeof(referedMsg), "%s%d%s%s!", ErrMsg, MAX_PORTGRP_DEP, ErrMsg2, curGrp->head.name);
                setTheErrorMsgOnFree(referedMsg);
                flag = 0;
                PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
                break_flag = 1;
            }
        }
        if(break_flag == 1)
        {
            break;
        }
	}/*end forѭ��*/

	return flag;
}
/**
 * ���ҷ������� rootPorf������subName��������
 * rootProf: ָ��Ҫ���ҵĸ��ڵ�
 * subName:  Ҫ���ҵ�����
 *           ��ΪNULL����������������
 * return: 
 *         -1  rootProf������subName
 *        >=0 rootProf��subName�������ȣ�������rootProf��
 *
 */
int portGrpDepToSub(PortGrpProfile *rootProf, char* subName)
{
    PortGrpProfile *subGrp = NULL;
    int i = 0, dep = -1, subDep = 0 , maxEntrys = 0;

    maxEntrys = PORTGRP_ENTRYS ;

    if(subName == NULL)
    {
        dep = 1;/*���������顣����С���Ϊ1*/
    }
    else 
    {
        if((strcmp(subName, rootProf->head.name) == 0))
        {/*�Լ����Լ������Ϊ0*/	
            dep = 0;
        }
    }

    if(dep != 0)/*�Լ�����������*/
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
                    subDep = portGrpDepToSub(subGrp, subName);/*������subName��������*/
                    if(subDep >= 0 )
                    {
                        subDep = subDep + 1;/*�����������ڵ㵽subNma�����*/
                        if(dep < subDep)
                        {/*������е����ֵ*/
                            dep = subDep;
                        }
                    }
                }
            }
        }
    }
    return dep;

}
/********************************************************************
 * ������ portGrpCheckEdit
 * ���ܣ� ������޸��ж�
 * ������ 
 * ������ �޸ĵ�������profile
 * ���أ� 
 *	 1 �����޸�
 *	 0 �����޸�
 * ����� ��
 * ���ߣ� 
 ********************************************************************/
static int portGrpCheckEdit(PortGrpProfile* oldProf, PortGrpProfile* newProf)
{
	MibProfileType mibType = MIB_PROF_PORTGRP;
	PortGrpProfile* prof = NULL;

	int flag = 1;
	int i = 0, min = 0, max = 0;
	int fatherDep = 0;

	ProfInstLowHigh(mibType, &max, &min);
	/*�������������newProf������*/
	for(i = min; i < max; i++) 
	{
		prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
		if((ProfInstIsFree(prof) == 0) && (prof->grpType == newProf->grpType))
		{/*��ͬ���͵��鲻���໥����*/
			/*newGrpName�������е�newGrpName��������*/
			fatherDep = portGrpDepToSub(prof, newProf->head.name);
			PORT_GRP_PRINTF("%s------%d,fatherDep = %d\n", __func__, __LINE__, fatherDep);
			if(fatherDep != -1)
			{/*prof��newGrpName������*/
				if(portGrpCheckEditTree(newProf, fatherDep) != 1)/*��·�Լ�����ж�*/
				{
					flag = 0;
					PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break;
				}
			}
		}/*end  ��ͬ���͵��鲻���໥���� */
	}/*end forѭ��*/

	return flag;
}
/**
 * ������ rootProf����ȣ��������ڵ㣩
 * rootProf: ָ��Ҫ��������
 * subName : ������ʱ�� �����Ƿ��и��������.
 * return��subNameΪNUll�򷵻�rootProf���
 *         subNae��ΪNULL��������rootProf�Լ��������ҵ�subName������0.���򷵻�rootProf���
 */
int portGrpDepSearch(PortGrpProfile *rootProf, char *subName)
{
    PortGrpProfile *subGrp = NULL;
    int i = 0, dep = 1, subDep = 0, maxEntrys = 0;

    maxEntrys = PORTGRP_ENTRYS ;

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
                subDep = portGrpDepSearch(subGrp, subName);/*�����Ƿ������������*/
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
/********************************************************************
 * ������ portGrpCheckAddTree
 * ���ܣ� ���ʱ�Ƿ�·�Լ�����ж�
 * ������ 
 * ������ ���жϵ�������profile
 * ���أ� 
 *	 1 û�л�·
 *	 0 ���ڻ�·
 * ����� ��
 * ���ߣ� 
 ********************************************************************/
int portGrpCheckAddTree(PortGrpProfile* curGrp)
{
	int flag = 1;
	int i = 0, maxEntrys = 0;
	int subDep = 0;
	char referedMsg[MSG_MAXSIZE];
	PortGrpProfile* subProf = NULL;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL, *ErrMsg3 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

    maxEntrys = PORTGRP_ENTRYS ;
	PORT_GRP_PRINTF("%s------%d,maxEntrys = %d\n", __func__, __LINE__, maxEntrys);

	for(i = 0; i < maxEntrys; i++)
	{
		if(curGrp->portEntry[i].entryType == PORTENT_GRP)
		{/*��������Ŀ����Ϊ�ӷ�����*/
			subProf = (PortGrpProfile*)
				ProfGetInstPointByName(MIB_PROF_PORTGRP,curGrp->portEntry[i].entryVal.grpName);
			if(ProfInstIsFree(subProf) == 0)/*����ʹ����*/
			{   
				if(subProf->grpType != curGrp->grpType)
				{/*ҳ���������жϣ�һ����������ߵ�*/
					ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
					ErrMsg2 = getTransDataForC(C_LANG_INDEX_AND_GROUP);
					ErrMsg3 = getTransDataForC(C_LANG_INDEX_TYPE_DIFF);
					snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s%s", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name, ErrMsg3);
					setTheErrorMsgOnFree(referedMsg);/*ֻ�������ͬ���͵�����*/
					flag = 0;
					PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
				}
				else
				{
					subDep = portGrpDepSearch(subProf,curGrp->head.name);/*�����Ƿ��и���*/
					PORT_GRP_PRINTF("%s------%d,subDep = %d\n", __func__, __LINE__, subDep);
					if(subDep == 0)
					{/*��·(����Ӧ���ǲ����ߵģ����ʱ�������л�·��ֻ���޸�ʱ����)*/
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_CANT_CONTAIN_OTHERS);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_CONTAIN);
						snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s!", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name);
						setTheErrorMsgOnFree(referedMsg);/*ֻ�������ͬ���͵�����*/
						flag = 0;
						PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					}
					else if((subDep + 1) >  MAX_PORTGRP_DEP)
					{/*��ȹ���*/
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_DEPTH_CANT_MORE_THAN);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_CANT_ADD_GROUP);
						snprintf(referedMsg, sizeof(referedMsg), "%s%d%s%s!", ErrMsg, MAX_PORTGRP_DEP, ErrMsg2, curGrp->head.name);
						setTheErrorMsgOnFree(referedMsg);
						flag = 0;
						PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					}
					else
					{
						/*test*/
					}
				}/*end ��������ͬ*/
			}/*end ����ʹ����*/
			else
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
				ErrMsg2 = getTransDataForC(C_LANG_INDEX_NEGATION);
				snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, subProf->head.name, ErrMsg2);
				setTheErrorMsgOnFree(referedMsg);/*ֻ�������ͬ���͵�����*/
				flag = 0;
				PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
			}
		}/*end ����ΪPORTENT_GRP*/
	}/*end forѭ��*/

	return flag;
}
/********************************************************************
 * ������ portGrpCheckAdd
 * ���ܣ� ���������ж�
 * ������ 
 * ������ ��ӵ�������profile
 * ���أ� 
 *	 1 �������
 *	 0 �������
 * ����� ��
 * ���ߣ� Jianqing.Sun 
 ********************************************************************/
static int portGrpCheckAdd(PortGrpProfile *repProf)
{
	int flag = 1;

	if(portGrpCheckAddTree(repProf) == 1)/*��·�Լ�����ж�*/
	{
        flag = 1;
		PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
	}
	else
	{
		flag = 0;
		PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
	}
	return flag; 
}
/*******************************************************************
 * ������ formPortGroupListDelAll
 * ���ܣ� 
 * ������ 
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static void formPortGroupListDelAll(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType=MIB_PROF_PORTGRP;
	PortGrpProfile *prof=NULL;
	struProfAlloc *profhead = NULL;

	int i=0;
	int min,max;
	/*get inst max num and min num*/
	ProfInstLowHigh(mibType,&max,&min);
	for(i=min;i<max;i++)
	{
		prof=(PortGrpProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
		if((prof!=NULL)&&(ProfInstIsFree(prof)==0))
		{
			PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
			if(grpDelCheck(prof))	/*check inst is references or not*/
			{
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profhead);
				PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
				ProfDelInstByIndex(mibType, i);
			}
		}
	}
	PORT_GRP_PRINTF("%s-%d:-----\n",__FUNCTION__,__LINE__);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	websRedirect(wp, "FwService.asp");	/*ҳ����ת*/
	return;
}
extern bool checkFwGrpExist(char* groupName)
{
    bool flag = FALSE;
    if(checkFwGrpInFireWall(groupName))
    {
        flag = TRUE; 
    }
    return flag;
}
/********************************************************************
 * ������ grpDelCheck
 * ���ܣ� �����ɾ���ж�
 * ������ 
 * ������ ɾ����������profile
 * ���أ� 
 *	 1 ����ɾ��
 *	 0 ����ɾ��
 * ����� ��
 * ���ߣ� 
 ********************************************************************/
static int grpDelCheck(PortGrpProfile *repProf)
{
	MibProfileType mibType = MIB_PROF_PORTGRP;
	PortGrpProfile* prof = NULL;

	int flag = 1;
	char referedMsg[MSG_MAXSIZE];
	int i = 0, min = 0, max = 0;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL, *ErrMsg3 = NULL;
	memset(referedMsg, 0, sizeof(referedMsg));
    /*
     * �жϷ������Ƿ���������
     * */
    if(checkFwGrpExist(repProf->head.name))
	{/*�ж��Ƿ񱻹�������*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
		PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
	}else{
    /*�ж��Ƿ�������*/
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i); 
        /*ֻ���ܱ�ͬ���͵������*/
        if((ProfInstIsFree(prof) == 0) && (prof->grpType == repProf->grpType)) 
        {
            if(prof != repProf)/*���жϱ��Լ�����*/
            {
                if(portGrpDepSearch(prof, repProf->head.name) == 0)
                {
                    ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
                    ErrMsg2 = getTransDataForC(C_LANG_INDEX_BE_GROUP);
                    ErrMsg3 = getTransDataForC(C_LANG_INDEX_USING_CANT_DEL);
                    snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s%s", ErrMsg, repProf->head.name, ErrMsg2, prof->head.name, ErrMsg3);
                    setTheErrorMsgOnFree(referedMsg);
                    flag = 0;
                    PORT_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
                    break;
                }
            }
        }/*end ֻ���ܱ�ͬ���͵������*/
    }/*end forѭ��*/
    }

	return flag;
}
/*******************************************************************
 * ������ formPortGroupListDel
 * ���ܣ� 
 * ������ 
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static void formPortGroupListDel(webs_t wp, char_t *path, char_t *query)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
	PortGrpProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	char_t *groupName = NULL; 
	char_t *tmp = NULL;
	const char *ErrMsg = NULL;

	a_assert(wp);
	/*
	 * ɾ������ѡ���PROFILE
	 */
	groupName = websGetVar(wp,T("delstr"),T("")); 
	PORT_GRP_PRINTF("%s-%d, get delstr = %s\n", __FUNCTION__, __LINE__, groupName);

	if (strcmp(groupName,"")==0) 
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_NAME_NOT_FIND);
		setTheErrorMsg(ErrMsg);
	}
	else
	{
		tmp = strtok(groupName, UTT_SPLIT_SIGN_STR);
		while(tmp != NULL) 
		{
			prof=(PortGrpProfile *)ProfGetInstPointByName(mibType, tmp);
			if(prof)
			{
				if(grpDelCheck(prof))	/*delete check*/
				{
					PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
					ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
					ProfDelInstByName(mibType, tmp); 
				}
			}
			PORT_GRP_PRINTF("%s-%d:del str=%s\n",__FUNCTION__,__LINE__,tmp);
			tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
		}
		PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfUpdate(profhead);   /*update*/
		PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfFreeAllocList(profhead);
		PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		nvramWriteCommit(); /*save*/
		PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	}
	websRedirect(wp, "FwService.asp");
	return;	   
}
/********************************************************************
 * ������ PortGroupConfigModify
 * ���ܣ� ������޸�������
 * ������ 
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/ 
static void PortGroupConfigModify(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile tmpProf;
    PortGrpProfile *curProf=NULL;
    struProfAlloc *profhead = NULL;

    char_t *groupName = NULL, *groupNameOld = NULL, *content = NULL,*groupType = NULL;
    char_t *pList1=NULL, *pList2=NULL,*temp = NULL;
    int i = 0,num=0;

    groupName = websGetVar(wp, T("groupName"), T(""));
    groupNameOld = websGetVar(wp, T("groupNameOld"), T("")); 
    content = websGetVar(wp, T("temp1"), T(""));
    groupType = websGetVar(wp, T("groupType"), T(""));
    PORT_GRP_PRINTF("%s_____%d ; groupType : %s\n",__func__,__LINE__,groupType);
    PORT_GRP_PRINTF("%s-%d:groupName=%s,groupNameOld=%s,content=%s\n",__FUNCTION__,__LINE__,groupName,groupNameOld,content);
	curProf = ProfGetInstPointByName(mibType, groupNameOld);
	memcpy(&tmpProf, curProf, sizeof(tmpProf));
	PORT_GRP_PRINTF("%s-%d:tmpProf.grpType=%d\n",__FUNCTION__,__LINE__,tmpProf.grpType);
	if(strcmp(content, "")!=0)
	{
        for(num=0;num<PORTGRP_ENTRYS;num++)
        {
            tmpProf.portEntry[num].entryType=PORTENT_NONE;
        }
        if(strcmp(groupType,TYPE_PORT)==0)
        {
            if(strcmp(content, "")!=0)
            {
                tmpProf.grpType=PORTGRP_PORT;
                pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
                PORT_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                while(pList1!=NULL)
                {
                    if(savePortRange(&tmpProf, pList1, i) == FALSE)
                    {
                        return;
                    }
                    pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                    i++;
                    PORT_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                }
            }
        }
        else
        {
            if(strcmp(groupType,TYPE_URL)==0) {
                tmpProf.grpType=PORTGRP_URL;
            }else if(strcmp(groupType,TYPE_DNS)==0) {
                tmpProf.grpType=PORTGRP_DNS;
            }else if(strcmp(groupType,TYPE_KEY)==0) {
                tmpProf.grpType=PORTGRP_KEY;
            }else if(strcmp(groupType,TYPE_MAC)==0) {
                tmpProf.grpType=PORTGRP_MAC;
            }else{
                return ; 
            }

            pList2=strtok(content, UTT_SPLIT_SIGN_RECORDS);
            PORT_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,i,pList2);
            while(pList2!=NULL)
            {
                temp = strstr(pList2,")");
                if(temp == NULL)
                    break;
                *temp = '\0';
                if((strstr(pList2, "C(")!=NULL) || (strstr(pList2, "c(")!=NULL))
                {
                    tmpProf.portEntry[i].entryType = PORTENT_NORMAL;
                    strcpy(tmpProf.portEntry[i].entryVal.content,pList2 + 2);
                    PORT_GRP_PRINTF("%s-%d, content=%s\n",__FUNCTION__,__LINE__,tmpProf.portEntry[i].entryVal.content);
                }
                else if((strstr(pList2, "G(")!=NULL) || (strstr(pList2, "g(")!=NULL))
                {
                    tmpProf.portEntry[i].entryType = PORTENT_GRP;
                    strcpy(tmpProf.portEntry[i].entryVal.grpName,pList2 + 2);
                    PORT_GRP_PRINTF("%s-%d, grpName = %s\n",__FUNCTION__,__LINE__,tmpProf.portEntry[i].entryVal.grpName);
                }
                pList2=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                i++;
                PORT_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,i,pList2);
            }
        }
        if(portGrpCheckEdit(curProf, &tmpProf)==1)
        {
            ProfBackupByName(mibType, PROF_CHANGE_EDIT, groupName, &profhead);
            memcpy(curProf, &tmpProf, sizeof(tmpProf));
            PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            ProfUpdate(profhead);/*����*/
            ProfFreeAllocList(profhead);
            PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
            nvramWriteCommit(); /*save*/
        }
    }
#if 0
    printTree();
#endif
    websRedirect(wp, "FwService.asp");
    return;
}
/*******************************************************************
 * ������ getPortGroupOneInfo
 * ���ܣ� 
 * ������ 
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static int getPortGroupOneInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile *prof = NULL;
    int min = 0, max = 0;
    int i = 0,j = 0;
    char buf[100] = {0};
    char type[10] = {0};


    char *editName = NULL;
    const char *ErrMsg = NULL;
    /*define web varliable*/
    websWrite(wp, "var curgroupNames = \"\";");
    websWrite(wp, "var curgroupType = \"\";");
    websWrite(wp, "var curContent = new Array();");
    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*�鿴ʵ�����Ƿ����*/ 
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	PORT_GRP_PRINTF("%s-%d:editName=%s\n",__FUNCTION__,__LINE__,editName);
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
	    prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);	/*get inst pointer by index*/
	    if((prof != NULL) && (strcmp(editName, prof->head.name) == 0))
	    {
            websWrite(wp,"curgroupNames = \"%s\";",prof->head.name);

            memset(type,0,sizeof(type));
            if(prof->grpType == PORTGRP_PORT) {
                strcpy(type,TYPE_PORT);
            }else if(prof->grpType == PORTGRP_URL){
                strcpy(type,TYPE_URL);

            }else if(prof->grpType == PORTGRP_DNS){
                strcpy(type,TYPE_DNS);

            }else if(prof->grpType == PORTGRP_KEY){
                strcpy(type,TYPE_KEY);
            }
            websWrite(wp,"curgroupType = \"%s\";",type);

            for(j = 0;j <PORTGRP_ENTRYS ; j++ )
            {
                memset(buf,0,sizeof(buf));
                if(prof->portEntry[j].entryType==PORTENT_GRP) {
                    sprintf(buf,"G(%s)",prof->portEntry[j].entryVal.grpName);
                } 
                else if(prof->portEntry[j].entryType==PORTENT_SYS) {
                    sprintf(buf,"%s",prof->portEntry[j].entryVal.sysGrp.sysGrpName);
                }
                else if(prof->portEntry[j].entryType==PORTENT_OTHER){ 
                    sprintf(buf,"O(%d)",prof->portEntry[j].entryVal.protocol);
                }
                else if(prof->portEntry[j].entryType==PORTENT_TCP){ 
                    sprintf(buf,"T(%d-%d,%d-%d)",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_UDP){ 
                    sprintf(buf,"U(%d-%d,%d-%d)",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_NORMAL){
                    sprintf(buf,"C(%s)",prof->portEntry[j].entryVal.content);
                }else{
                    break;
                }
                websWrite(wp,"curContent[%d] = \"%s\";",j,buf);
            }
	    }
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/
    return 0;
}

static unsigned char nucstrIsNum(char* str)
{ 
    if(str == NULL) return 0;
	if((strlen(str)>5)||(strlen(str)==0))return 0;
	while((*str)!='\0')
	{
		if('0'<=(*str)&&(*str)<='9'){
			str++;
		}else{
			return 0;
		} 
	} 
	return 1;
}
/********************************************************************
 * ������ savePortRange
 * ���ܣ� ���������������
 * ������ 
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ� 
 ********************************************************************/ 
static bool savePortRange(PortGrpProfile *tmp, char *str, int index)
{
	char *temp1,*temp;
    int i,sta_num=-1;
	unsigned short srcportfrom,srcportto,destportto,destportfrom,protocol = 0;

    temp = str;

    if((temp[0]=='U'||temp[0]=='T'||temp[0]=='u'||temp[0]=='t')&&(temp[1]=='('))
    {
        temp += 2;
        temp1 = strstr(temp,"-");
        *temp1 = '\0';
        if((nucstrIsNum(temp)==0)||(atoi(temp)<0)||(atoi(temp)>65535)) return FALSE;
        srcportfrom=atoi(temp);
        temp = temp1 + 1;

        temp1 = strstr(temp, ",");
        *temp1 = '\0';
        if((nucstrIsNum(temp)==0)||(atoi(temp)<0)||(atoi(temp)>65535)) return FALSE;
        srcportto=atoi(temp);
        temp = temp1 + 1;
        if(srcportto<srcportfrom) return FALSE;

        temp1 = strstr(temp, "-");
        *temp1 = '\0';
        if((nucstrIsNum(temp)==0)||(atoi(temp)<0)||(atoi(temp)>65535)) return FALSE;
        destportfrom=atoi(temp);
        temp = temp1 + 1;

        temp1 = strstr(temp, ")");
        *temp1 = '\0';
        if((nucstrIsNum(temp)==0)||(atoi(temp)<0)||(atoi(temp)>65535)) return FALSE;
        destportto=atoi(temp);
        temp1 = temp1 + 1;
        if(destportto<destportfrom) return FALSE;
        PORT_GRP_PRINTF("%s____%d ; srcfrom : %d ; srcto : %d ; destfrom : %d ; destto : %d \n",__func__,__LINE__,srcportfrom ,srcportto ,destportfrom ,destportto);

        if(str[0] == 'T' || str[0] == 't'){
            tmp->portEntry[index].entryType = PORTENT_TCP;
        }else{
            tmp->portEntry[index].entryType = PORTENT_UDP;
        }
        tmp->portEntry[index].entryVal.portRange.ssport = srcportfrom;
        tmp->portEntry[index].entryVal.portRange.seport = srcportto;
        tmp->portEntry[index].entryVal.portRange.dsport = destportfrom; 
        tmp->portEntry[index].entryVal.portRange.deport = destportto; 
        PORT_GRP_PRINTF("%s____%d ; srcfrom : %d ; srcto : %d ; destfrom : %d ; destto : %d \n",__func__,__LINE__,
                tmp->portEntry[index].entryVal.portRange.ssport,
                tmp->portEntry[index].entryVal.portRange.seport,
                tmp->portEntry[index].entryVal.portRange.dsport,
                tmp->portEntry[index].entryVal.portRange.deport);
    } else if(((temp[0]=='O')||(temp[0]=='o'))&&(temp[1]=='('))	{
        tmp->portEntry[index].entryType = PORTENT_OTHER;
        
        temp += 2;
        temp1 = strstr(temp,")");
        *temp1 = '\0';
        if((nucstrIsNum(temp)==0)||(atoi(temp)<0)||(atoi(temp)>65535)) return FALSE;
        tmp->portEntry[index].entryVal.protocol = atoi(temp);
        PORT_GRP_PRINTF("%s____%d ; protocol : %d ;\n",__func__,__LINE__,tmp->portEntry[index].entryVal.protocol);
    } else if(((temp[0]=='G')||(temp[0]=='g'))&&(temp[1]=='('))	{
        tmp->portEntry[index].entryType = PORTENT_GRP;

        temp += 2;
        temp1 = strstr(temp,")");
        *temp1 = '\0';
        strcpy(tmp->portEntry[index].entryVal.grpName,temp);
        PORT_GRP_PRINTF("%s____%d ; grpName : %s ;\n",__func__,__LINE__,tmp->portEntry[index].entryVal.grpName);
    }else{//ϵͳԤ��
        tmp->portEntry[index].entryType = PORTENT_SYS;

        for(i=0;i<STATIC_SER_NUM;i++)
        {
            if(strcmp(str,tcpserver[i])==0)
            {
                if(i<STATIC_TCPSERV_NUM) {
                    if(i==0)	{
                        destportfrom = 20;
                    }else{
                        destportfrom = port_start[i];
                    }
                    destportto = port_end[i];
                }else if(i<(STATIC_TCPSERV_NUM+STATIC_UDPSERV_NUM)){
                    destportfrom = port_start[i];
                    destportto = port_end[i];
                }else{
                    protocol=port_start[i];
                }

                tmp->portEntry[index].entryVal.sysGrp.portRange.ssport = 0;
                tmp->portEntry[index].entryVal.sysGrp.portRange.seport = 65535;
                tmp->portEntry[index].entryVal.sysGrp.portRange.dsport = destportfrom;
                tmp->portEntry[index].entryVal.sysGrp.portRange.deport = destportto;
                tmp->portEntry[index].entryVal.sysGrp.protocol = protocol;
                break;
            }
        }
        PORT_GRP_PRINTF("%s________%d ; name : %s,%d-%d,%d-%d\n",
                __func__,__LINE__, str,
                tmp->portEntry[index].entryVal.sysGrp.portRange.ssport,
                tmp->portEntry[index].entryVal.sysGrp.portRange.seport,
                tmp->portEntry[index].entryVal.sysGrp.portRange.dsport,
                tmp->portEntry[index].entryVal.sysGrp.portRange.deport);

        strcpy(tmp->portEntry[index].entryVal.sysGrp.sysGrpName,str);
    }
    return TRUE;
}
//new
static void printTree()
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int j = 0;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
        {
            for(j = 0;j <PORTGRP_ENTRYS ; j++ )
            {
                if(prof->portEntry[j].entryType==PORTENT_GRP) {
                    PORT_GRP_PRINTF("\tGRP(%s)\n",prof->portEntry[j].entryVal.grpName);
                } 
                else if(prof->portEntry[j].entryType==PORTENT_SYS) {
                    PORT_GRP_PRINTF("\tSYS(%s)\n",prof->portEntry[j].entryVal.sysGrp.sysGrpName);
                }
                else if(prof->portEntry[j].entryType==PORTENT_OTHER){ 
                    PORT_GRP_PRINTF("\tOTH(%d)\n",prof->portEntry[j].entryVal.protocol);
                }
                else if(prof->portEntry[j].entryType==PORTENT_TCP){ 
                    PORT_GRP_PRINTF("\tTCP(%d-%d,%d-%d)\n",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_UDP){ 
                    PORT_GRP_PRINTF("\tUDP(%d-%d,%d-%d)\n",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_NORMAL){
                    PORT_GRP_PRINTF("\tCON(%s)\n",prof->portEntry[j].entryVal.content);
                }
            }
        }
    }

}
/********************************************************************
 * ������ getPortGrpListData
 * ���ܣ� ��ҳ�����������
 * ������ 
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� xu.jian
 ********************************************************************/ 
static int getPortGrpListData(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile *prof = NULL;

    int min = 0, max = 0; int i = 0; int num = 0; int j = 0;
    char  buf[50] = {0};
    char  type[10] = {0};

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
        prof = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, i);
        
        if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
        {
            websWrite(wp,"groupNames[%d] = \"%s\";",num,prof->head.name);

            memset(type,0,sizeof(type));
            if(prof->grpType == PORTGRP_PORT) {
                strcpy(type,TYPE_PORT);
            }else if(prof->grpType == PORTGRP_URL){
                strcpy(type,TYPE_URL);

            }else if(prof->grpType == PORTGRP_DNS){
                strcpy(type,TYPE_DNS);

            }else if(prof->grpType == PORTGRP_KEY){
                strcpy(type,TYPE_KEY);
            }
            websWrite(wp,"type[%d] = \"%s\";",num,type);

            //websWrite(wp,"remark[%d] = \"%s\";",i,prof->head.name);
            for(j = 0;j <PORTGRP_ENTRYS ; j++ )
            {
                memset(buf,0,sizeof(buf));
                if(prof->portEntry[j].entryType==PORTENT_GRP) {
                    sprintf(buf,"G(%s)",prof->portEntry[j].entryVal.grpName);
                } 
                else if(prof->portEntry[j].entryType==PORTENT_SYS) {
                    sprintf(buf,"%s",prof->portEntry[j].entryVal.sysGrp.sysGrpName);
                }
                else if(prof->portEntry[j].entryType==PORTENT_OTHER){ 
                    sprintf(buf,"O(%d)",prof->portEntry[j].entryVal.protocol);
                }
                else if(prof->portEntry[j].entryType==PORTENT_TCP){ 
                    sprintf(buf,"T(%d-%d,%d-%d)",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_UDP){ 
                    sprintf(buf,"U(%d-%d,%d-%d)",prof->portEntry[j].entryVal.portRange.ssport,prof->portEntry[j].entryVal.portRange.seport,prof->portEntry[j].entryVal.portRange.dsport,prof->portEntry[j].entryVal.portRange.deport);
                }
                else if(prof->portEntry[j].entryType==PORTENT_NORMAL){
                    sprintf(buf,"C(%s)",prof->portEntry[j].entryVal.content);
                }
                websWrite(wp,"line%d[%d] = \"%s\";",j+1,num,buf);
            }
            //websWrite(wp,"line[%d] = new Array(line1[%d],line2[%d],line3[%d],line4[%d],line5[%d],line6[%d],line7[%d],line8[%d],line9[%d],line10[%d]);\n",i,i,i,i,i,i,i,i,i,i,i);
            websWrite(wp,"line[%d] = new Array(line1[%d],line2[%d],line3[%d],line4[%d],line5[%d],line6[%d],line7[%d],line8[%d],line9[%d],line10[%d]);\n",num,num,num,num,num,num,num,num,num,num,num);
            num++;
        }
    }
#if 0
    printTree();
#endif
    websWrite(wp,"totalrecs = %d;",num);
    websWrite(wp,"var max_totalrecs =%d;",max);	/*output max inst num*/
    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/

    return 0;
}
/********************************************************************
 * ������ PortGroupConfigAdd
 * ���ܣ� ���������
 * ������ 
 * ������ �����̶��������
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/ 
static void PortGroupConfigAdd(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_PORTGRP;
    PortGrpProfile tmpProf;
    PortGrpProfile *curProf=NULL;
    struProfAlloc  *profhead = NULL;

    char_t *groupName = NULL, *groupType = NULL, *content = NULL;
    char *pList1=NULL, *pList2=NULL,*temp = NULL;

    int i = 0, num = 0;
    int instIndex = 0;
    const char *ErrMsg = NULL;

    instIndex = ProfGetInstIndexFree(mibType);/*��ȡһ������ʵ��*/
    PORT_GRP_PRINTF("%s-%d:instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
    if(instIndex == PROFFAIL)
    {
        ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
        setTheErrorMsgOnFree((char *)ErrMsg);
    }
    else
    {
        PORT_GRP_PRINTF("%s-%d , get free instIndex = %d\n", __func__, __LINE__, instIndex);
        curProf = (PortGrpProfile*)ProfGetInstPointByIndex(mibType, instIndex);
        profileBuildFactory(mibType, (Pointer *)&tmpProf);

        /*��ҳ��ռ��ȡֵ*/
        groupName = websGetVar(wp, T("groupName"), T(""));
        groupType = websGetVar(wp, T("groupType"), T(""));
        content = websGetVar(wp, T("temp1"), T(""));
        if(strcmp(groupName,"")!=0)
        {
	    if(ProfGetInstIndexByName(mibType,groupName) != PROFFAIL)
	    {
		ErrMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
		setTheErrorMsgOnFree((char *)ErrMsg);
		websRedirect(wp,T("FwService.asp"));
		return;

	    }	
            strcpy(tmpProf.head.name,groupName);
            tmpProf.head.active = 1;
            for(num=0;num<PORTGRP_ENTRYS;num++)
            {
                tmpProf.portEntry[num].entryType=PORTENT_NONE;
            }
            if(strcmp(groupType,TYPE_PORT)==0)
            {
                if(strcmp(content, "")!=0)
                {
                    tmpProf.grpType=PORTGRP_PORT;
                    pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
                    PORT_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                    while(pList1!=NULL)
                    {
                        if(savePortRange(&tmpProf, pList1, i) == FALSE)
                        {
                            return;
                        }
                        pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                        i++;
                        PORT_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
                    }
                }
            }
            else
            {
                if(strcmp(groupType,TYPE_URL)==0) {
                    tmpProf.grpType=PORTGRP_URL;
                }else if(strcmp(groupType,TYPE_DNS)==0) {
                    tmpProf.grpType=PORTGRP_DNS;
                }else if(strcmp(groupType,TYPE_KEY)==0) {
                    tmpProf.grpType=PORTGRP_KEY;
                }else if(strcmp(groupType,TYPE_MAC)==0) {
                    tmpProf.grpType=PORTGRP_MAC;
                }else{
                    return ; 
                }

                pList2=strtok(content, UTT_SPLIT_SIGN_RECORDS);
                PORT_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,i,pList2);
                while(pList2!=NULL)
                {
                    temp = strstr(pList2,")");
                    if(temp != NULL)
                        *temp = '\0';

                    if((strstr(pList2, "C(")!=NULL) || (strstr(pList2, "c(")!=NULL))
                    {
                        tmpProf.portEntry[i].entryType = PORTENT_NORMAL;
                        strcpy(tmpProf.portEntry[i].entryVal.content,pList2 + 2);
                        PORT_GRP_PRINTF("%s-%d, tmpProf.Content=%s\n",__FUNCTION__,__LINE__,tmpProf.portEntry[i].entryVal.content);
                    }
                    if((strstr(pList2, "G(")!=NULL) || (strstr(pList2, "g(")!=NULL))
                    {
                        tmpProf.portEntry[i].entryType = PORTENT_GRP;
                        strcpy(tmpProf.portEntry[i].entryVal.grpName,pList2 + 2);
                        PORT_GRP_PRINTF("%s-%d, tmpProf.grpName=%s\n",__FUNCTION__,__LINE__,tmpProf.portEntry[i].entryVal.grpName);
                    }


                    pList2=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
                    i++;
                    PORT_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,i,pList2);
                }
            }

            if(portGrpCheckAdd(&tmpProf)==1)
            {
                ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
                memcpy(curProf, &tmpProf, sizeof(tmpProf));
                PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);    
                ProfUpdate(profhead);
                ProfFreeAllocList(profhead);
                PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);    
                nvramWriteCommit();
            }
        }
    }

#if 0
    printTree();
#endif
    websRedirect(wp, "FwService.asp");
    return;
}
/*******************************************************************
 * ������ formPortGroupConfig
 * ���ܣ� 
 * ������
 * ������ 
 * ���أ� ��
 * ����� ��
 * ���ߣ�  
 ********************************************************************/
static void formPortGroupConfig(webs_t wp, char_t *path, char_t *query)
{

    char_t *action = NULL;

    action = websGetVar(wp,T("Action"), T(""));

    PORT_GRP_PRINTF("%s-%d, action = %s\n", __func__, __LINE__, action);
    /*add inst*/
    if (strcmp(action, "add") == 0) 
    {
        PORT_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
        PortGroupConfigAdd(wp);
    }
    else    /*modify inst*/
    {
        PortGroupConfigModify(wp);
    }
    return;
}
/********************************************************************
 * ������ websDefinePortGrp
 * ���ܣ� ҳ���뽻��ת������
 * ������ 2014-08
 * ������ ��
 * ���أ� ��
 * ����� ��
 * ���ߣ� 
 ********************************************************************/ 
extern void websDefinePortGrp(void)
{
    websFormDefine(T("formPortGroupConfig"), formPortGroupConfig);
    websFormDefine(T("formPortGroupListDel"), formPortGroupListDel);
    websFormDefine(T("formPortGroupListDelAll"), formPortGroupListDelAll);
    websAspDefine(T("aspOutputPortGroupListData"), getPortGrpListData);
    websAspDefine(T("aspOutputPortGroupOneInfo"), getPortGroupOneInfo);
}
#else
extern int getFwGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    return 0;
}
#endif
