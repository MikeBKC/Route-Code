/*
 * 文件名:
 *     wusIpGroup.c
 *
 * 描述:
 *    用户组交互文件
 *    
 * 作者：
 *     孙剑卿
 *
 * 维护人：
 *     孙剑卿
 *
 * 变更：
 *     1. 初始版本:2012-05-07
 *
 */
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

#if (IP_GRP== FYES)
#define CONTENT_LENGTH	(UTT_IPGRP_NAME_LEN+5)*IPGRP_ENTRYS
#define MSG_MAXSIZE  100
#define TYPE_IP	    "groupAddr"
#define TYPE_USER   "groupUser"
#ifdef  IP_GRP_DEBUG
#define IP_GRP_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define IP_GRP_PRINTF(fmt, args...)
#endif

static void ipGrpDel(IpGrpProfile *delProf);
extern IpGrpProfile* userRefByIpGrp(char *username, ipGrpEntryType userType);
static int ipGrpCheckAdd(IpGrpProfile *repProf);
static int ipGrpCheckEdit(IpGrpProfile* oldProf, IpGrpProfile* newProf);
static int grpDelCheck(IpGrpProfile *repProf);
int ipGrpCheckAddTree(IpGrpProfile* curGrp);
int ipGrpCheckEditTree(IpGrpProfile* curGrp, int fatherDep);
unsigned int ipGrpCountIp(IpGrpProfile* prof, IpGrpProfile* repProf);
#if (POLICY_ROUTE == FYES)
extern int isIpGrpReredByPolicyRoute(IpGrpProfile *ipProf);
#endif

/********************************************************************
 * 函数： getIpGrpList
 * 功能： 向页面输出用户组
 * 创建： 2012-05-10
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern int getIpGrpList(int eid, webs_t wp, int argc, char_t **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;

    int min = 0, max = 0;
    int i = 0;
    int num = 0;

    websWrite(wp,"var addGroupNames = new Array();");

    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)) 
	{
	    websWrite(wp,"addGroupNames[%d] = \"%s\";",num,prof->head.name);
	    num++;
	}
    }
    return 0;
}
/********************************************************************
 * 函数： ipGrpDel
 * 功能： ip组删除
 * 创建： 2012-05-07
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void ipGrpDel(IpGrpProfile *delProf)
{
	ipsetDelGrp(delProf->head.name, TRUE);
	return;
}
/********************************************************************
 * 函数： exceptIpGroupProfileChange
 * 功能： 判断用户是否被组引用了
 * 创建： 2012-05-04
 * 参数： 
 *	 username: 要删除 或 修改的用户名
 *	 userType：pppoe账号 或 web认证用户名 或其他
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
extern IpGrpProfile* userRefByIpGrp(char *username, ipGrpEntryType userType)
{
	MibProfileType mibType = MIB_PROF_IPGRP;
	IpGrpProfile* retProf = NULL;
	IpGrpProfile* prof = NULL;

	int i = 0, min = 0, max = 0;
	int searched[MAX_IPGRP_PROFILES];/*辅助空间。保证每个组只被查找一次。提高效率*/
	memset(searched, 0,sizeof(searched));/*初始化为 每个组都未查找过*/

	IP_GRP_PRINTF("%s------%d, username = %s\n", __func__, __LINE__, username);
	ProfInstLowHigh(mibType, &max, &min);
	/*遍历整个profile*/
	for(i = min; i < max; i++) 
	{
		prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
		if((ProfInstIsFree(prof) == 0) && (prof->grpType == IPGRP_USER))/*只查找用户性组*/
		{
			if(searched[i] == 0)
			{/*该组未被查找过*/
				ipGrpSearchUser(i, username, userType, searched);
			}
			IP_GRP_PRINTF("%s------%d, searched[%d] = %d\n", __func__, __LINE__, i, searched[i]);
			if(searched[i] == 1)
			{/*在该组中查找到了grpName*/
				IP_GRP_PRINTF("%s------%d, searched[%d] = %d\n", __func__, __LINE__, i, searched[i]);
				retProf = prof; 
			}
		} 
	}

	return retProf;
}

/********************************************************************
 * 函数： ipGrpCheckAdd
 * 功能： 组管理添加判断
 * 创建： 2012-05-08
 * 参数： 添加的组所属profile
 * 返回： 
 *	 1 可以添加
 *	 0 不能添加
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static int ipGrpCheckAdd(IpGrpProfile *repProf)
{
	int flag = 1;
	char referedMsg[MSG_MAXSIZE];
	const char *MsgStr = NULL, *MsgStr2 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

	if(ipGrpCheckAddTree(repProf) == 1)/*环路以及深度判断*/
	{
		if(repProf->grpType == IPGRP_IP)
		{
			if(ipGrpCountIp(repProf, NULL) > MAX_IPGRP_IP_NUM)/*ip数判断*/
			{
				MsgStr = getTransDataForC(C_LANG_INDEX_GROUP);
				MsgStr2 = getTransDataForC(C_LANG_INDEX_IS_IP_NUM_CANT_MORE_THAN);
				snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%d", MsgStr, repProf->head.name, MsgStr2, MAX_IPGRP_IP_NUM);
				setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
				flag = 0;
				IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
			}
		}
	}
	else
	{
		flag = 0;
		IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
	}
	return flag; 
}
/********************************************************************
 * 函数： ipGrpCheckEdit
 * 功能： 组管理修改判断
 * 创建： 2012-05-08
 * 参数： 修改的组所属profile
 * 返回： 
 *	 1 可以修改
 *	 0 不能修改
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static int ipGrpCheckEdit(IpGrpProfile* oldProf, IpGrpProfile* newProf)
{
	MibProfileType mibType = MIB_PROF_IPGRP;
	IpGrpProfile* prof = NULL;

	int flag = 1;
	int i = 0, min = 0, max = 0;
	int fatherDep = 0;
	char referedMsg[MSG_MAXSIZE];
	const char *MsgStr = NULL, *MsgStr2 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

	ProfInstLowHigh(mibType, &max, &min);
	/*遍历所有组查找newProf的祖先*/
	for(i = min; i < max; i++) 
	{
		prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
		if((ProfInstIsFree(prof) == 0) && (prof->grpType == newProf->grpType))
		{/*不同类型的组不会相互包含 */
			/*newGrpName的祖先中到newGrpName的最大深度*/
			fatherDep = ipGrpDepToSub(prof, newProf->head.name);
			IP_GRP_PRINTF("%s------%d,fatherDep = %d\n", __func__, __LINE__, fatherDep);
			if(fatherDep != -1)
			{/*prof是newGrpName的祖先*/
				if(ipGrpCheckEditTree(newProf, fatherDep) == 1)/*环路以及深度判断*/
				{
					if((prof->grpType == IPGRP_IP) && 
							(ipGrpCountIp(prof, newProf) > MAX_IPGRP_IP_NUM))
					{
						MsgStr = getTransDataForC(C_LANG_INDEX_GROUP);
						MsgStr2 = getTransDataForC(C_LANG_INDEX_IS_IP_NUM_CANT_MORE_THAN);
						snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%d", MsgStr, prof->head.name, MsgStr2, MAX_IPGRP_IP_NUM);
						setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
						flag = 0;
						IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
						break;
					}
				}
				else
				{
					flag = 0;
					IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break;
				}
			}
		}/*end  不同类型的组不会相互包含 */
	}/*end for循环*/

	return flag;
}
/********************************************************************
 * 函数： grpDelCheck
 * 功能： 组管理删除判断
 * 创建： 2012-05-08
 * 参数： 删除的组所属profile
 * 返回： 
 *	 1 可以删除
 *	 0 不能删除
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static int grpDelCheck(IpGrpProfile *repProf)
{
	MibProfileType mibType = MIB_PROF_IPGRP;
	IpGrpProfile* prof = NULL;

	int flag = 1;
	char referedMsg[MSG_MAXSIZE];
	int i = 0, min = 0, max = 0;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL, *ErrMsg3 = NULL;
	memset(referedMsg, 0, sizeof(referedMsg));
	if(ipsetGetRefer(repProf->head.name) != 0)
	{/*判断是否被规则引用*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
		IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
	}
#if (GROUP_BASE == FYES)
	else if(isIpGrpReredByNf(repProf) == 1)
	{/*被上网行为管理规则引用的组无法删除*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
	}
#endif
#if (WEB_AUTH == FYES)
	else if(isIpGrpReredByWebAuth(repProf) == 1)
	{
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
	}
#endif
#if (POLICY_ROUTE == FYES)
	else if(isIpGrpReredByPolicyRoute(repProf) == 1)
	{/*被上网行为管理规则引用的组无法删除*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
	}
#endif
#if (P2P_LIMIT == FYES)
	else if(isIpGrpReredByP2PLimit(repProf) == 1)
	{/*被P2P限速引用的组无法删除*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
	}
#endif
#if (FIREWALL == FYES)
	else if(isIpGrpReredByFireWall(repProf) == 1)
	{/*被访问控制策略引用的组无法删除*/
		ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
		ErrMsg2 = getTransDataForC(C_LANG_INDEX_USED_RULE_CANT_DEL);
		snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, repProf->head.name, ErrMsg2);
		setTheErrorMsgOnFree(referedMsg);
		flag = 0;
	}
#endif
	else
	{/*判断是否被组引用*/
		ProfInstLowHigh(mibType, &max, &min);
		for(i = min; i < max; i++) 
		{
			prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i); 
			/*只可能被同类型的组包含*/
			if((ProfInstIsFree(prof) == 0) && (prof->grpType == repProf->grpType)) 
			{
				if(prof != repProf)/*不判断被自己引用*/
				{
					if(ipGrpDepSearch(prof, repProf->head.name) == 0)
					{
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_BE_GROUP);
						ErrMsg3 = getTransDataForC(C_LANG_INDEX_USING_CANT_DEL);
						snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s%s", ErrMsg, repProf->head.name, ErrMsg2, prof->head.name, ErrMsg3);
						setTheErrorMsgOnFree(referedMsg);
						flag = 0;
						IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
						break;
					}
				}
			}/*end 只可能被同类型的组包含*/
		}/*end for循环*/
	}/*end 判断是否被组引用*/
	return flag;
}
/********************************************************************
 * 函数： ipGrpCheckAddTree
 * 功能： 添加时是否环路以及深度判断
 * 创建： 2012-05-08
 * 参数： 需判断的组所属profile
 * 返回： 
 *	 1 没有环路
 *	 0 存在环路
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
int ipGrpCheckAddTree(IpGrpProfile* curGrp)
{
	int flag = 1;
	int i = 0, maxEntrys = 0;
	int subDep = 0;
	char referedMsg[MSG_MAXSIZE];
	IpGrpProfile* subProf = NULL;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL, *ErrMsg3 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

	if(curGrp->grpType == IPGRP_IP)
	{
		maxEntrys =  IPGRP_IP_ENTRYS;
	}
	else
	{
		maxEntrys = IPGRP_USER_ENTRYS;
	}
	IP_GRP_PRINTF("%s------%d,maxEntrys = %d\n", __func__, __LINE__, maxEntrys);
	for(i = 0; i < maxEntrys; i++)
	{
		if(curGrp->ipEntry[i].entryType == IPENT_GRP)
		{/*ip组条目类型为子ip组*/
			subProf = (IpGrpProfile*)
				ProfGetInstPointByName(MIB_PROF_IPGRP,curGrp->ipEntry[i].entryVal.name);
			if(ProfInstIsFree(subProf) == 0)/*该组使用了*/
			{   
				if(subProf->grpType != curGrp->grpType)
				{/*页面上已有判断，一般情况不会走的*/
					ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
					ErrMsg2 = getTransDataForC(C_LANG_INDEX_AND_GROUP);
					ErrMsg3 = getTransDataForC(C_LANG_INDEX_TYPE_DIFF);
					snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s%s", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name, ErrMsg3);
					setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
					flag = 0;
					IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
				}
				else
				{
					subDep = ipGrpDepSearch(subProf,curGrp->head.name);/*子组是否含有父组*/
					IP_GRP_PRINTF("%s------%d,subDep = %d\n", __func__, __LINE__, subDep);
					if(subDep == 0)
					{/*环路(这里应该是不会走的，添加时不可能有环路，只有修改时才有)*/
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_CANT_CONTAIN_OTHERS);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_CONTAIN);
						snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s!", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name);
						setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
						flag = 0;
						IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					}
					else if((subDep + 1) >  MAX_IPGRP_DEP)
					{/*深度过大*/
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_DEPTH_CANT_MORE_THAN);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_CANT_ADD_GROUP);
						snprintf(referedMsg, sizeof(referedMsg), "%s%d%s%s!", ErrMsg, MAX_IPGRP_DEP, ErrMsg2, curGrp->head.name);
						setTheErrorMsgOnFree(referedMsg);
						flag = 0;
						IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					}
					else
					{
						/*test*/
					}
				}/*end 组类型相同*/
			}/*end 该组使用了*/
			else
			{
				ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP);
				ErrMsg2 = getTransDataForC(C_LANG_INDEX_NEGATION);
				snprintf(referedMsg, sizeof(referedMsg), "%s%s%s", ErrMsg, subProf->head.name, ErrMsg2);
				setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
				flag = 0;
				IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
			}
		}/*end 类型为IPENT_GRP*/
	}/*end for循环*/

	return flag;
}
/********************************************************************
 * 函数： ipGrpCheckEditTree
 * 功能： 修改时是否环路以及深度判断
 * 创建： 2012-05-08
 * 参数： 需判断的组所属profile
 * 返回： 
 *	 1 没有环路
 *	 0 存在环路
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
int ipGrpCheckEditTree(IpGrpProfile* curGrp, int fatherDep)
{
	int flag = 1, break_flag = 0;
	int i = 0, maxEntrys = 0;
	int subDep = 0;
	char referedMsg[MSG_MAXSIZE];
	IpGrpProfile* subProf = NULL;
	const char *ErrMsg = NULL, *ErrMsg2 = NULL;

	memset(referedMsg, 0, sizeof(referedMsg));

	if(curGrp->grpType == IPGRP_IP)
	{
		maxEntrys =  IPGRP_IP_ENTRYS;
	}
	else
	{
		maxEntrys = IPGRP_USER_ENTRYS;
	}
	IP_GRP_PRINTF("%s------%d,maxEntrys = %d\n", __func__, __LINE__, maxEntrys);
	for(i = 0; i < maxEntrys; i++)
	{
		if(curGrp->ipEntry[i].entryType == IPENT_GRP)
		{
			subProf = (IpGrpProfile*)
				ProfGetInstPointByName(MIB_PROF_IPGRP,curGrp->ipEntry[i].entryVal.name);
			if(ProfInstIsFree(subProf) != 0)/*该组未使用*/
			{
				flag = 0;
				IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
				break_flag = 1;
			}
			else
			{
				subDep = ipGrpDepSearch(subProf,curGrp->head.name);/*子组是否含有父组*/
				IP_GRP_PRINTF("%s------%d,subDep = %d\n", __func__, __LINE__, subDep);
				if(subDep == 0)
				{/*环路*/
						ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_CANT_CONTAIN_OTHERS);
						ErrMsg2 = getTransDataForC(C_LANG_INDEX_CONTAIN);
						snprintf(referedMsg, sizeof(referedMsg), "%s%s%s%s!", ErrMsg, subProf->head.name, ErrMsg2, curGrp->head.name);
					setTheErrorMsgOnFree(referedMsg);/*只能添加相同类型的子组*/
					flag = 0;
					IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break_flag = 1;
				}
				else if((subDep + fatherDep + 1) >  MAX_IPGRP_DEP)
				{/*深度过大*/
					ErrMsg = getTransDataForC(C_LANG_INDEX_GROUP_DEPTH_CANT_MORE_THAN);
					ErrMsg2 = getTransDataForC(C_LANG_INDEX_CANT_ADD_GROUP);
					snprintf(referedMsg, sizeof(referedMsg), "%s%d%s%s!", ErrMsg, MAX_IPGRP_DEP, ErrMsg2, subProf->head.name);
					setTheErrorMsgOnFree(referedMsg);
					flag = 0;
					IP_GRP_PRINTF("%s------%d,flag = %d\n", __func__, __LINE__, flag);
					break_flag = 1;
				}
				else
				{
					/*test*/
				}
			}/*end 该组使用*/
			if(break_flag == 1)
			{
				break;
			}
		}/*end 类型为IPENT_GRP*/
	}/*end for循环*/

	return flag;
}
/********************************************************************
 * 函数： ipGrpCountIp
 * 功能： 计算一个组的ip数量
 * 创建： 2012-05-08
 * 参数： 
 *	 prof    要计算组的profile
 *	 repProf 被修改的组的profile，若是添加则这里为NULL
 * 返回： ip数量
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
unsigned int ipGrpCountIp(IpGrpProfile* prof, IpGrpProfile* repProf)
{
	unsigned int num = 0;
	int i = 0, maxEntrys = 0;
	IpGrpProfile* subGrp = NULL;
	if(prof->grpType == IPGRP_IP)
	{
		maxEntrys =  IPGRP_IP_ENTRYS;
	}
	else
	{
		maxEntrys = IPGRP_USER_ENTRYS;
	}
	IP_GRP_PRINTF("%s------%d,maxEntrys = %d\n", __func__, __LINE__, maxEntrys);
	if((repProf != NULL) && (strcmp(prof->head.name, repProf->head.name) == 0))
	{/*组本身修改了*/
		prof = repProf;
		IP_GRP_PRINTF("%s------%d,prof has been modified!\n", __func__, __LINE__);
	}
	for(i = 0; i < maxEntrys; i++)
	{
		if(prof->ipEntry[i].entryType == IPENT_RANGE)
		{/*该条目类型为ip范围*/
			num += ntohl(prof->ipEntry[i].entryVal.range.ipTo) - ntohl(prof->ipEntry[i].entryVal.range.ipFrom) + 1;
			IP_GRP_PRINTF("%s------%d, ntohl(prof->ipEntry[i].entryVal.range.ipTo) = %x, ntohl(prof->ipEntry[i].entryVal.range.ipFrom) = %x\n", __FUNCTION__, __LINE__, ntohl(prof->ipEntry[i].entryVal.range.ipTo), ntohl(prof->ipEntry[i].entryVal.range.ipFrom));
			IP_GRP_PRINTF("%s------%d,num = %x\n", __func__, __LINE__, num);

		}
		else if(prof->ipEntry[i].entryType == IPENT_GRP)
		{/*该条目类型为其他地址组*/
			if((repProf != NULL) && (strcmp(prof->ipEntry[i].entryVal.name, repProf->head.name) == 0))
			{/*此子组修改过，计算修改后的ip个数*/
				num += ipGrpCountIp(repProf, NULL);
				IP_GRP_PRINTF("%s------%d,num = %d\n", __func__, __LINE__, num);
			}
			else
			{ 
				subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, prof->ipEntry[i].entryVal.name);
				if(ProfInstIsFree(subGrp) == 0)/*该组使用了*/
				{
					num += ipGrpCountIp(subGrp, repProf);/*子组ip个数*/
					IP_GRP_PRINTF("%s------%d,num = %d\n", __func__, __LINE__, num);
				}
			}
		}
		else
		{
			/*test*/
		}
	}
	IP_GRP_PRINTF("%s------%d,num = %x\n", __func__, __LINE__, num);
	return num;
}
/********************************************************************
 * 函数： getIpGroupListData
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static int getIpGroupListData(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;
    struct in_addr from,end;
    char *ipFrom=NULL,*ipTo=NULL;
    char iptmp[UTT_IPGRP_NAME_LEN]={0};
    char ipStr[UTT_IPGRP_NAME_LEN]={0},ipGrpNameStr[UTT_IPGRP_NAME_LEN]={0};
    char webAuthStr[UTT_IPGRP_NAME_LEN]={0},pppoeStr[UTT_IPGRP_NAME_LEN]={0},otherStr[UTT_IPGRP_NAME_LEN]={0};
    char pContent[CONTENT_LENGTH]={0};
    int min = 0, max = 0;
    int i = 0, j=0;
    int num = 0;


    /*define web varliable*/
    websWrite(wp,"var groupNames=new Array();");
    websWrite(wp,"var groupType=new Array();");
    websWrite(wp,"var ipEntrynum=new Array();");
    websWrite(wp,"var userEntrynum=new Array();");
    websWrite(wp,"var groupContent=new Array();");
    websWrite(wp, T("var totalrecs;"));
    websWrite(wp, T("\n"));
    /*get inst max num and min num*/
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0) && (prof->head.active == TRUE))
	{
	    websWrite(wp,"groupNames[%d] = \"%s\";", num, prof->head.name);
	    if(prof->grpType == IPGRP_IP)
	    {
		memset(pContent,0,sizeof(pContent));
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		websWrite(wp, "ipEntrynum[%d]=%d;",num,IPGRP_IP_ENTRYS);
		websWrite(wp,"groupType[%d]=\"%s\";", num, "ipgroup");
		for(j=0;j<IPGRP_IP_ENTRYS;j++)
		{
		    if(prof->ipEntry[j].entryType!=IPENT_NONE)
		    {
			/*ipRange*/
			if(prof->ipEntry[j].entryType==IPENT_RANGE)
			{
			    IP_GRP_PRINTF("%s-%d:ipfrom=%x,ipTo=%x\n",__FUNCTION__,__LINE__,prof->ipEntry[j].entryVal.range.ipFrom,prof->ipEntry[j].entryVal.range.ipTo);
			    from.s_addr=prof->ipEntry[j].entryVal.range.ipFrom;
			    ipFrom=inet_ntoa(from);
			    IP_GRP_PRINTF("%s-%d:ipfrom=%s\n",__FUNCTION__,__LINE__,ipFrom);
			    strcpy(iptmp,ipFrom);
			    end.s_addr=prof->ipEntry[j].entryVal.range.ipTo;
			    ipTo=inet_ntoa(end);
			    IP_GRP_PRINTF("%s-%d:ipfrom=%s,ipTo=%s\n",__FUNCTION__,__LINE__,iptmp,ipTo);
			    if(ipFrom && ipTo)
			    {
				sprintf(ipStr,"P(%s-%s)",iptmp,ipTo);
			    }
			    strcat(pContent,ipStr);
			}
			else if(prof->ipEntry[j].entryType==IPENT_GRP)	/*other ip group*/
			{
			    sprintf(ipGrpNameStr,"G(%s)",prof->ipEntry[j].entryVal.name);
			    strcat(pContent,ipGrpNameStr);
			}
		    }
		    if(strlen(pContent) > CONTENT_LENGTH - 40U)
		    {
			IP_GRP_PRINTF("%s-%d: pContent=%s\n",__FUNCTION__,__LINE__,pContent);
			break;
		    }
		}
		websWrite(wp,"groupContent[%d]=\"%s\";",num,pContent);
	    }
	    else if(prof->grpType == IPGRP_USER) /*user group*/
	    {
		memset(pContent,0,sizeof(pContent));
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		websWrite(wp,"groupType[%d] = \"%s\";", num, "usergroup");
		for(j=0;j<IPGRP_USER_ENTRYS;j++)
		{
		    if(prof->ipEntry[j].entryType!=IPENT_NONE)
		    {
#if (WEB_AUTH == FYES)
			if(prof->ipEntry[j].entryType==IPENT_WEB_AUTH)	/*web user*/
			{
			    IP_GRP_PRINTF("%s-%d:j=%d, webName=%s\n",__FUNCTION__,__LINE__,j,prof->ipEntry[j].entryVal.name);
			    sprintf(webAuthStr,"W(%s)",prof->ipEntry[j].entryVal.name);
			    strcat(pContent,webAuthStr);
			}
			else
#endif
#if (PPPOE_SERVER == FYES)
			    if(prof->ipEntry[j].entryType==IPENT_POE_ACC)  /*pppoe user*/
			    {
				IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
				sprintf(pppoeStr,"P(%s)",prof->ipEntry[j].entryVal.name);
				strcat(pContent,pppoeStr);
			    }
			    else
#endif
				if(prof->ipEntry[j].entryType==IPENT_GRP)	/*other user group*/
				{
				    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
				    sprintf(otherStr,"G(%s)",prof->ipEntry[j].entryVal.name);
				    strcat(pContent,otherStr);
				}

		    }
		    if(strlen(pContent) > CONTENT_LENGTH - 40U)
		    {
			IP_GRP_PRINTF("%s-%d: pContent=%s\n",__FUNCTION__,__LINE__,pContent);
			break;
		    }
		}
		websWriteBigBlock(wp,"groupContent[%d]=\"%s\";",num,pContent);
	    }
	    num++;
	}
    }
    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    getTheErrorMsg(eid, wp, argc, argv);	/*get system error msg*/
    websWrite(wp,"var totalrecs=%d;",num);	/*output inst num*/
    websWrite(wp,"var max_totalrecs =%d;",max);	/*output max inst num*/
#if (PPPOE_SERVER == FYES)
    websWrite(wp,"PPPoEFlag = 1;");	/*output max inst num*/
#endif
#if (WEB_AUTH == FYES)
    websWrite(wp,"WebAuthFlag = 1 ;");	/*output max inst num*/
#endif

    return 0;
}
/*******************************************************************
 * 函数： getIpGroupOneInfo
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static int getIpGroupOneInfo(int eid, webs_t wp, int argc, char **argv)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile *prof = NULL;
    struct in_addr from,end;
    char *ipFrom=NULL,*ipTo=NULL;
    char iptmp[UTT_IPGRP_NAME_LEN]={0};
    char ipStr[UTT_IPGRP_NAME_LEN]={0},ipGrpNameStr[UTT_IPGRP_NAME_LEN]={0};
    char webAuthStr[UTT_IPGRP_NAME_LEN]={0},pppoeStr[UTT_IPGRP_NAME_LEN]={0},otherStr[UTT_IPGRP_NAME_LEN]={0};
    int min = 0, max = 0;
    int i = 0,j = 0;

    char *editName = NULL;
    const char *ErrMsg = NULL;
    /*define web varliable*/
    websWrite(wp, "var curgroupNames = \"\";");
    websWrite(wp, "var curgroupType = \"\";");
    websWrite(wp, "var curContent = new Array();");
    if ((ejArgs(argc, argv, T("%s"), &editName) < 1) || (editName == NULL))/*查看实例名是否存在*/ 
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_READ_DATA_ERR);
	setTheErrorMsg(ErrMsg);
    }
    else
    {
	IP_GRP_PRINTF("%s-%d:editName=%s\n",__FUNCTION__,__LINE__,editName);
	ProfInstLowHigh(mibType, &max, &min);
	for (i = min; i < max; i++) 
	{
	    prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);	/*get inst pointer by index*/
	    if((prof != NULL) && (strcmp(editName, prof->head.name) == 0))
	    {	/* find inst
		 * write inst info
		 */
		websWrite(wp, "curgroupNames = \"%s\";", prof->head.name);
		if(prof->grpType==IPGRP_IP)
		{
		    websWrite(wp, "curgroupType = \"%s\";", "ipgroup");
		}
		else if(prof->grpType==IPGRP_USER)
		{
		    websWrite(wp, "curgroupType = \"%s\";", "usergroup");
		}
		if(prof->grpType == IPGRP_IP)	/*inst is ip group*/
		{
		    for(j=0;j<IPGRP_IP_ENTRYS;j++)
		    {
			if(prof->ipEntry[j].entryType!=IPENT_NONE)
			{

			    if(prof->ipEntry[j].entryType==IPENT_RANGE)
			    {
				IP_GRP_PRINTF("%s-%d:ipfrom=%x,ipTo=%x\n",__FUNCTION__,__LINE__,prof->ipEntry[j].entryVal.range.ipFrom,prof->ipEntry[j].entryVal.range.ipTo);
				from.s_addr=prof->ipEntry[j].entryVal.range.ipFrom;
				ipFrom=inet_ntoa(from);
				IP_GRP_PRINTF("%s-%d:ipfrom=%s\n",__FUNCTION__,__LINE__,ipFrom);
				strcpy(iptmp,ipFrom);
				end.s_addr=prof->ipEntry[j].entryVal.range.ipTo;
				ipTo=inet_ntoa(end);
				IP_GRP_PRINTF("%s-%d:ipfrom=%s,ipTo=%s\n",__FUNCTION__,__LINE__,iptmp,ipTo);
				if(ipFrom && ipTo)
				{
				    sprintf(ipStr,"P(%s-%s)",iptmp,ipTo);
				}
				websWrite(wp,"curContent[%d]=\"%s\";",j,ipStr);
			    }
			    else if(prof->ipEntry[j].entryType==IPENT_GRP)
			    {
				sprintf(ipGrpNameStr,"G(%s)",prof->ipEntry[j].entryVal.name);
				websWrite(wp,"curContent[%d]=\"%s\";",j,ipGrpNameStr);
			    }
			}
		    }
		}
		else if(prof->grpType == IPGRP_USER)	/*inst is user group*/
		{
		    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		    for(j=0;j<IPGRP_USER_ENTRYS;j++)
		    {
			if(prof->ipEntry[j].entryType!=IPENT_NONE)
			{
#if (WEB_AUTH == FYES)
			    if(prof->ipEntry[j].entryType==IPENT_WEB_AUTH)
			    {
				sprintf(webAuthStr,"W(%s)",prof->ipEntry[j].entryVal.name);
				websWrite(wp,"curContent[%d]=\"%s\";",j,webAuthStr);
			    }
			    else 
#endif
#if (PPPOE_SERVER == FYES)
				if(prof->ipEntry[j].entryType==IPENT_POE_ACC)
				{
				    sprintf(pppoeStr,"P(%s)",prof->ipEntry[j].entryVal.name);
				    websWrite(wp,"curContent[%d]=\"%s\";",j,pppoeStr);
				}
				else
#endif
				    if(prof->ipEntry[j].entryType==IPENT_GRP)
				    {
					sprintf(otherStr,"G(%s)",prof->ipEntry[j].entryVal.name);
					websWrite(wp,"curContent[%d]=\"%s\";",j,otherStr);
				    }
			}
		    }
		}
	    }
	}
    }
    getTheErrorMsg(eid, wp, argc, argv);    /*get system error msg*/
    return 0;
}
/*******************************************************************
 * 函数： formIpGroupListDelAll
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formIpGroupListDelAll(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType=MIB_PROF_IPGRP;
	IpGrpProfile *prof=NULL;
	struProfAlloc *profhead = NULL;

	int i=0;
	int min,max;
	/*get inst max num and min num*/
	ProfInstLowHigh(mibType,&max,&min);
	for(i=min;i<max;i++)
	{
		prof=(IpGrpProfile *)ProfGetInstPointByIndex(mibType,i);    /*get inst pointer*/
		if((prof!=NULL)&&(ProfInstIsFree(prof)==0))
		{
			IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
			if(grpDelCheck(prof))	/*check inst is references or not*/
			{
				ProfBackupByIndex(mibType, PROF_CHANGE_DELALL, i, &profhead);
				IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
				ipGrpDel(prof); /*delete inst*/
				ProfDelInstByIndex(mibType, i);
			}
		}
	}
	IP_GRP_PRINTF("%s-%d:-----\n",__FUNCTION__,__LINE__);
	ProfFreeAllocList(profhead);
	nvramWriteCommit(); /*save*/

	websRedirect(wp, "UserGroup.asp");	/*页面跳转*/
	return;
}
/*******************************************************************
 * 函数： formIpGroupListDel
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formIpGroupListDel(webs_t wp, char_t *path, char_t *query)
{
	MibProfileType mibType = MIB_PROF_IPGRP;
	IpGrpProfile *prof = NULL;
	struProfAlloc *profhead = NULL;

	char_t *groupName = NULL; 
	char_t *tmp = NULL;
	const char *ErrMsg = NULL;

	a_assert(wp);
	/*
	 * 删除所有选择的PROFILE
	 */
	groupName = websGetVar(wp,T("delstr"),T("")); 
	IP_GRP_PRINTF("%s-%d, get delstr = %s\n", __FUNCTION__, __LINE__, groupName);
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
			prof=(IpGrpProfile *)ProfGetInstPointByName(mibType, tmp);
			if(prof)
			{
				if(grpDelCheck(prof))	/*delete check*/
				{
					IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
					ProfBackupByName(mibType, PROF_CHANGE_DEL, tmp, &profhead);
					ipGrpDel(prof);
					ProfDelInstByName(mibType, tmp); 
				}
			}
			IP_GRP_PRINTF("%s-%d:del str=%s\n",__FUNCTION__,__LINE__,tmp);
			tmp=strtok(NULL, UTT_SPLIT_SIGN_STR);
		}
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfUpdate(profhead);   /*update*/
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		ProfFreeAllocList(profhead);
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		nvramWriteCommit(); /*save*/
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	}
	websRedirect(wp, "UserGroup.asp");

	return;	   
}
/********************************************************************
 * 函数： IpGroupConfigAdd
 * 功能： 组管理增加组配置
 * 创建： 2012-05-08
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者： 
 ********************************************************************/ 
static bool saveIpRange(IpGrpProfile *tmp, char *List, int index)
{
	IP_GRP_PRINTF("%s-%d, get List=%s,index=%d\n",__FUNCTION__,__LINE__,List,index);
	char *p=NULL;
	struct in_addr fromip, endip;
	p=strstr(List, "-");
	if(p)
	{
		*p='\0';
		p=p+1;
		inet_aton(List, &fromip);
		tmp->ipEntry[index].entryVal.range.ipFrom=fromip.s_addr; /*save from ip*/
		IP_GRP_PRINTF("%s-%d:ipFrom=%s,after = %x\n",__FUNCTION__,__LINE__,List,tmp->ipEntry[index].entryVal.range.ipFrom);
		inet_aton(p, &endip);
		tmp->ipEntry[index].entryVal.range.ipTo=endip.s_addr;
		tmp->ipEntry[index].entryType=IPENT_RANGE;
		IP_GRP_PRINTF("%s-%d:ipTo=%s, after = %x\n",__FUNCTION__,__LINE__,p,tmp->ipEntry[index].entryVal.range.ipTo);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
/*
 * sav other ipgroup name 
 */
static bool saveIpGrpName(IpGrpProfile *tmp, char *List, int index)
{
	IP_GRP_PRINTF("%s-%d, get List=%s,index=%d\n",__FUNCTION__,__LINE__,List,index);
	if(List)
	{
		tmp->ipEntry[index].entryType=IPENT_GRP;
		strcpy(tmp->ipEntry[index].entryVal.name, List);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
#if (PPPOE_SERVER == FYES)
/*
 * save pppoe user 
 */
static bool savePPPoEUser(IpGrpProfile *tmp, char *List, int index)
{
	IP_GRP_PRINTF("%s-%d, get List=%s,index=%d\n",__FUNCTION__,__LINE__,List,index);
	char *ptr=NULL;
	if(List)
	{
		ptr=strstr(List,"-");
		if(ptr)
		{
			tmp->ipEntry[index].entryType=IPENT_POE_ACC;
			strcpy(tmp->ipEntry[index].entryVal.name, ptr+1);
			IP_GRP_PRINTF("%s-%d:poeStr=%s\n",__FUNCTION__,__LINE__,ptr+1);
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
#endif
#if (WEB_AUTH == FYES)
/*
 * save webauth user 
 */
static bool saveWebAuth(IpGrpProfile *tmp, char *List, int index)
{
	IP_GRP_PRINTF("%s-%d, get List=%s,index=%d\n",__FUNCTION__,__LINE__,List,index);
	char *ptr=NULL;
	if(List)
	{
		ptr=strstr(List,"-");
		if(ptr)
		{
			tmp->ipEntry[index].entryType=IPENT_WEB_AUTH;
			strcpy(tmp->ipEntry[index].entryVal.name, ptr+1);
			IP_GRP_PRINTF("%s-%d:webAuthStr=%s\n",__FUNCTION__,__LINE__,ptr+1);
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
#endif
/*
 * save other usergroup name
 */
static bool saveOtherUser(IpGrpProfile *tmp, char *List, int index)
{
	IP_GRP_PRINTF("%s-%d, get List=%s,index=%d\n",__FUNCTION__,__LINE__,List,index);
	char *ptr=NULL;
	if(List)
	{
		ptr=strstr(List,"-");
		if(ptr)
		{
			tmp->ipEntry[index].entryType=IPENT_GRP;
			strcpy(tmp->ipEntry[index].entryVal.name, ptr+1);
			IP_GRP_PRINTF("%s-%d:othGrpStr=%s\n",__FUNCTION__,__LINE__,ptr+1);
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
static void IpGroupConfigAdd(webs_t wp)
{
#if 1
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile tmpProf;
    IpGrpProfile *curProf=NULL;
    struProfAlloc *profhead = NULL;

    char_t *groupName = NULL, *groupType = NULL, *content = NULL;
    char *pList1=NULL, *pList2=NULL;

    int i = 0, j = 0, num = 0;
    int instIndex = 0;
    const char *ErrMsg = NULL;

    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
    instIndex = ProfGetInstIndexFree(mibType);/*获取一个空闲实例*/
    IP_GRP_PRINTF("%s-%d:instIndex=%d\n",__FUNCTION__,__LINE__,instIndex);
    if(instIndex == PROFFAIL)
    {
	ErrMsg = getTransDataForC(C_LANG_INDEX_PPTP_SERVER_MAX_TOTAL_ERR);
	setTheErrorMsgOnFree((char *)ErrMsg);
    }
    else
    {
	IP_GRP_PRINTF("%s-%d , get free instIndex = %d\n", __func__, __LINE__, instIndex);
	curProf = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, instIndex);
	profileBuildFactory(mibType, (Pointer *)&tmpProf);
	
	/*从页面空间获取值*/
	groupName = websGetVar(wp, T("groupName"), T(""));
	groupType = websGetVar(wp, T("groupType"), T(""));
	content = websGetVar(wp, T("temp1"), T(""));
	if(strcmp(groupName,"")!=0)
	{

	if(ProfGetInstIndexByName(mibType,groupName) != PROFFAIL)
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_INST_EXIST);
	    setTheErrorMsgOnFree((char *)ErrMsg);
	    websRedirect(wp,T("UserGroup.asp"));
	    return;

	}	
	if(ipsetGrpNameIsSys(groupName)) /*check groupname*/
	{
	    ErrMsg = getTransDataForC(C_LANG_INDEX_SYSTEM_GROUP_CONFLICT);
	    setTheErrorMsgOnFree((char *)ErrMsg);
	    websRedirect(wp, "UserGroup.asp");
	    return;
	}
	strcpy(tmpProf.head.name,groupName);
	tmpProf.head.active=1;
	for(num=0;num<IPGRP_ENTRYS;num++)
	{
	    tmpProf.ipEntry[num].entryType=IPENT_NONE;
	}
	if(strcmp(groupType,TYPE_IP)==0) /*ipgroup type*/
	{
	tmpProf.grpType=IPGRP_IP;
	if(strcmp(content, "")!=0)
	{
	    pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
	    IP_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
	    while(pList1!=NULL)
	    {
		if(strstr(pList1, "-")!=NULL)  /*ip range*/
		{
		    if(saveIpRange(&tmpProf, pList1, i) == FALSE)
		    {
			return;
		    }
		    IP_GRP_PRINTF("%s-%d, tmpProf.ipRange.formIp=%x,tmpProf.ipRange.endIp=%x\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[i].entryVal.range.ipFrom,tmpProf.ipEntry[i].entryVal.range.ipTo);
		}
		else
		{
		    saveIpGrpName(&tmpProf, pList1, i);
		    IP_GRP_PRINTF("%s-%d, tmpProf.IpGrpName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[i].entryVal.name);
		}
		pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
		i++;
		IP_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
	    }
	}
	}
	else if(strcmp(groupType, TYPE_USER)==0)    /*usergroup type*/
	{
	    tmpProf.grpType=IPGRP_USER;
	    pList2=strtok(content, UTT_SPLIT_SIGN_RECORDS);
	    IP_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,j,pList2);
	    while(pList2!=NULL)
	    {
#if (PPPOE_SERVER == FYES)
		if(strstr(pList2, "P-")!=NULL)	/*pppoe user*/
		{
		    savePPPoEUser(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.POEName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		else
#endif
#if (WEB_AUTH == FYES)
		 if(strstr(pList2, "W-")!=NULL)	/*webauth user*/
		{
		    saveWebAuth(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.WebAuthName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		 else
#endif
		 if(strstr(pList2, "G-")!=NULL)	/*other usergroup*/
		{
		    saveOtherUser(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.OtherName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		else
		{}
		pList2=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
		j++;
		IP_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,j,pList2);
	    }
	}
	else
	{}
	if(ipGrpCheckAdd(&tmpProf)==1)
	{
	    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);
	    memcpy(curProf, &tmpProf, sizeof(tmpProf));
	    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);    
	    ProfUpdate(profhead);/*更新*/
	    ProfFreeAllocList(profhead);
	    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);    
	    nvramWriteCommit(); /*save*/
	}
	}
    }

#endif
    websRedirect(wp, "UserGroup.asp");
    return;
}
/********************************************************************
 * 函数： IpGroupConfigAdd
 * 功能： 组管理修改组配置
 * 创建： 2012-05-08
 * 参数： 交互固定输入参数
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/ 
static void IpGroupConfigModify(webs_t wp)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile tmpProf;
    IpGrpProfile *curProf=NULL;
    struProfAlloc *profhead = NULL;

    char_t *groupName = NULL, *groupNameOld = NULL, *content = NULL;
    char_t *pList1=NULL, *pList2=NULL;
    int i = 0,j=0,num=0;

    groupName = websGetVar(wp, T("groupName"), T(""));
    groupNameOld = websGetVar(wp, T("groupNameOld"), T("")); 
    content = websGetVar(wp, T("temp1"), T(""));
    IP_GRP_PRINTF("%s-%d:groupName=%s,groupNameOld=%s,content=%s\n",__FUNCTION__,__LINE__,groupName,groupNameOld,content);
#if 0
    if (strcmp(groupName, groupNameOld) == 0) 
    { /* 用户名不能修改 */
#endif
	IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	curProf = ProfGetInstPointByName(mibType, groupNameOld);
	memcpy(&tmpProf, curProf, sizeof(tmpProf));
#if (WEB_AUTH == FYES)
	int k=0,min=0,max=0;
    IpGrpProfile *prof=NULL;
    ProfInstLowHigh(mibType, &max, &min);
    for(k = min; k < max; k++)
    {
		prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, k);
		if(ProfInstIsFree(prof) == 0 && curProf->grpType == prof->grpType)/*该组使用了*/
		{/*只遍历同类型的组*/
			if(ipGrpDepSearch(prof, curProf->head.name) == 0)/*prof引用到了组curGrpName*/
			{
				if((isIpGrpReredByWebAuth(prof))== 1)
				{
					IPGrpDelWebExIp(prof->head.name);
				}
			}
		}
	}
#endif
	IP_GRP_PRINTF("%s-%d:tmpProf.grpType=%d\n",__FUNCTION__,__LINE__,tmpProf.grpType);
#if 1
	if(strcmp(content, "")!=0)
	{
	IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    for(num=0;num<IPGRP_ENTRYS;num++)
	    {
		tmpProf.ipEntry[num].entryType=IPENT_NONE;
	    }
	IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	if(tmpProf.grpType==IPGRP_IP)	/*ipgroup type*/
	{
	    pList1=strtok(content, UTT_SPLIT_SIGN_RECORDS);
	    IP_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
	    while(pList1!=NULL)
	    {
		if(strstr(pList1, "-")!=NULL)
		{
		    if(saveIpRange(&tmpProf, pList1, i) == FALSE)
		    {
			return;
		    }
		    IP_GRP_PRINTF("%s-%d, tmpProf.ipRange.formIp=%x,tmpProf.ipRange.endIp=%x\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[i].entryVal.range.ipFrom,tmpProf.ipEntry[i].entryVal.range.ipTo);
		}
		else
		{
		    saveIpGrpName(&tmpProf, pList1, i);
		    IP_GRP_PRINTF("%s-%d, tmpProf.IpGrpName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[i].entryVal.name);
		}
		pList1=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
		i++;
		IP_GRP_PRINTF("%s-%d, pList[%d]=%s\n",__FUNCTION__,__LINE__,i,pList1);
	    }
	}
	else if(tmpProf.grpType==IPGRP_USER)	/*usergroup type*/
	{
	    pList2=strtok(content, UTT_SPLIT_SIGN_RECORDS);
	    IP_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,j,pList2);
	    while(pList2!=NULL)
	    {
#if (PPPOE_SERVER == FYES)
		if(strstr(pList2, "P-")!=NULL)
		{
		    savePPPoEUser(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.POEName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		else 
#endif
#if (WEB_AUTH == FYES)
		if(strstr(pList2, "W-")!=NULL)
		{
		    saveWebAuth(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.WebAuthName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		else
#endif
		 if(strstr(pList2, "G-")!=NULL)
		{
		    saveOtherUser(&tmpProf, pList2, j);
		    IP_GRP_PRINTF("%s-%d, tmpProf.OtherName=%s\n",__FUNCTION__,__LINE__,tmpProf.ipEntry[j].entryVal.name);
		}
		else
		{}
		pList2=strtok(NULL, UTT_SPLIT_SIGN_RECORDS);
		j++;
		IP_GRP_PRINTF("%s-%d, pList2[%d]=%s\n",__FUNCTION__,__LINE__,j,pList2);
	    }
	}
	else
	{}
	if(ipGrpCheckEdit(curProf, &tmpProf)==1)
	{
	    ProfBackupByName(mibType, PROF_CHANGE_EDIT, groupName, &profhead);
	    memcpy(curProf, &tmpProf, sizeof(tmpProf));
	    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    ProfUpdate(profhead);/*更新*/
	    ProfFreeAllocList(profhead);
	    IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
	    nvramWriteCommit(); /*save*/
	}
	}
#endif
#if 0
    }
    else
    {
	setTheErrorMsg(T("不能修改组名。"));
    }
out:
#endif
    websRedirect(wp, "UserGroup.asp");
    return;
}
/*******************************************************************
 * 函数： formIpGroupConfig
 * 功能： 
 * 创建： 2012-05-08
 * 参数： 
 * 返回： 略
 * 输出： 略
 * 作者：  
 ********************************************************************/
static void formIpGroupConfig(webs_t wp, char_t *path, char_t *query)
{

	char_t *action = NULL;


	action = websGetVar(wp,T("Action"), T(""));

	IP_GRP_PRINTF("%s-%d, action = %s\n", __func__, __LINE__, action);
	/*add inst*/
	if (strcmp(action, "add") == 0) 
	{
		IP_GRP_PRINTF("%s-%d:\n",__FUNCTION__,__LINE__);
		IpGroupConfigAdd(wp);
	}
	else    /*modify inst*/
	{
		IpGroupConfigModify(wp);
	}
	return;
}
/********************************************************************
 * 函数： websDefineIpGrp
 * 功能： 页面与交互转换函数
 * 创建： 2012-04-13
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
extern void websDefineIpGrp(void)
{
	websFormDefine(T("formIpGroupConfig"), formIpGroupConfig);
	websFormDefine(T("formIpGroupListDel"), formIpGroupListDel);
	websFormDefine(T("formIpGroupListDelAll"), formIpGroupListDelAll);
	websAspDefine(T("aspOutputIpGroupListData"), getIpGroupListData);
	websAspDefine(T("aspOutputIpGroupOneInfo"), getIpGroupOneInfo);
}
#endif
