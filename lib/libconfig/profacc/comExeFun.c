/**
 * 共用生效函数集合
 * 增加此文件是为了避免消息泛滥。或进程加锁导致的其他进程异常
 *
 * 如pppd加锁更新计费profilfe 并 发消息 speedweb更新规则时，可能由于pppd操作
 * profile数量过多，或时间过长。导致 speedweb消息缓冲区溢出，或无法即时处理其他消息
 */
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/autoconf.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "ipsetApi.h"
#include "comExeFun.h"
#include <time.h>
#include <../../libusb-user/usb-user.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include "base64.h"

//#define SWORD_DEBUG
#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

#if (PPPOE_ACCOUNT == FYES) 

#if 0
#if (NOTICE == FYES)
static void upPoeExpiringRule(PPPoESrvAccProfile *prof , char addOrDel);
#endif
static void upPoeExpiredRule(PPPoESrvAccProfile *prof , char addOrDel);
#else
#if (NOTICE == FYES)
static void upPoeExpiringRule(SessionList prof , char addOrDel);
#endif
static void upPoeExpiredRule(SessionList prof , char addOrDel);
#endif
int uttGlTime = 0;
extern int Base64Encode(char *OrgString, char *Base64String, int OrgStringLen );
/********************************************************************
 * 函数： upPppAccStatus 
 * 功能： 获取用户账号状态
 * 创建： 2012-04-01
 * 参数： PPPoE计费用户的profile
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
void upPppAccStatus(PPPoESrvAccProfile *prof)
{
#if (NOTICE == FYES)
    MibProfileType mibType = MIB_PROF_NOTICE;
    NoticeProfile *profA = NULL;
#endif

    int tmCurrent = 0;
    if(uttGlTime != 0) 
    {
	tmCurrent = uttGlTime;
    }
    else
    {
	tmCurrent = time(NULL);
    }

    if((prof->tmStart < tmCurrent) && (tmCurrent < prof->tmStop))
    {
	prof->expired = PPPOE_ACC_NORMAL;
 #if (NOTICE == FYES) 
	profA = (NoticeProfile*)ProfGetInstPointByIndex(mibType, 1);
	if((profA != NULL) && (ProfInstIsFree(profA) == 0))
	{
	    if(((prof->tmStop - (profA->remainDays) * 86400) < tmCurrent) && (tmCurrent < prof->tmStop))
	    {
		prof->expired = PPPOE_ACC_EXPIRING;
	    }
	}
#endif
    }
    else if((tmCurrent >= DELIVERY_START_TIME) && (tmCurrent <= DELIVERY_END_TIME))
    {/*若时间为出厂时间到出厂时间后的2小时则不判断是否过期*/
	prof->expired = PPPOE_ACC_NORMAL;
    }
    else if((tmCurrent < prof->tmStart) || (tmCurrent > prof->tmStop))
    {
	prof->expired = PPPOE_ACC_EXPIRED;
    }
#if 0
    printf("%s %d %d\n", __func__, __LINE__, prof->expired);
#endif

    return;
}

#if 0
#if (NOTICE == FYES)
/**
 * 更pppoe将过期规则
 */
static void upPoeExpiringRule(PPPoESrvAccProfile *prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRING_ACC, prof->ip, addOrDel);/*清将过期组*/
    if(addOrDel == IPSET_MEM_DEL)
    {
	ipsetAOrDIpStr(POE_NT_OK, prof->ip, addOrDel);/*清已通告组。则再连线，再次通告*/
    }
    return;
}
#endif
/**
 * 更新过期账号规则
 */
static void upPoeExpiredRule(PPPoESrvAccProfile *prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRED_ACC, prof->ip, addOrDel);
    return;
}
/**
 * 更新pppoe用户相关规则
 * prof:    pppoe用户profile指针
 * status : pppoe用户状态
 * isAdd  ：1 添加规则 ， 0 删除规则
 */
void upPppAccRule(PPPoESrvAccProfile *prof, emPppoeAccStatus status, int isAdd)
{
    char  ipsetAct = IPSET_MEM_ADD;
    if((strcmp(prof->device, (char *)getLanIfName()) == 0))
	/*只是针对lan口pppoe用户*/
    {
	if(isAdd == 0) 
	{
	    ipsetAct = IPSET_MEM_DEL;
	}

	switch(status)
	{
	    case PPPOE_ACC_EXPIRED:
		upPoeExpiredRule(prof, ipsetAct);
		break;
#if (NOTICE == FYES)
	    case PPPOE_ACC_EXPIRING:
		upPoeExpiringRule(prof, ipsetAct);
		break;
#endif
	    default:
		break;

	}
    }
    return;
}
#else
#if (NOTICE == FYES)
/**
 * 更pppoe将过期规则
 */
static void upPoeExpiringRule(SessionList prof , char addOrDel)
{
    ipsetAOrDIpStr(POE_EXPIRING_ACC, prof.ip, addOrDel);/*清将过期组*/
    if(addOrDel == IPSET_MEM_DEL)
    {
	ipsetAOrDIpStr(POE_NT_OK, prof.ip, addOrDel);/*清已通告组。则再连线，再次通告*/
    }
    return;
}
#endif
/**
 * 更新过期账号规则
 */
static void upPoeExpiredRule(SessionList prof , char addOrDel)
{
    SWORD_PRINTF("ipset -q -%c %s %s\n", addOrDel, POE_EXPIRED_ACC, prof.ip);
    ipsetAOrDIpStr(POE_EXPIRED_ACC, prof.ip, addOrDel);
    return;
}
/**
 * 更新pppoe用户相关规则
 * prof:    会话指针
 * status : pppoe用户状态
 * isAdd  ：1 添加规则 ， 0 删除规则
 */
void upPppAccRule(SessionList prof, emPppoeAccStatus status, int isAdd)
{
    char  ipsetAct = IPSET_MEM_ADD;
    if((strcmp(prof.device, (char *)getLanIfName()) == 0))
	/*只是针对lan口pppoe用户*/
    {
	if(isAdd == 0) 
	{
	    ipsetAct = IPSET_MEM_DEL;
	}

	switch(status)
	{
	    case PPPOE_ACC_EXPIRED:
		upPoeExpiredRule(prof, ipsetAct);
		break;
#if (NOTICE == FYES)
	    case PPPOE_ACC_EXPIRING:
		upPoeExpiringRule(prof, ipsetAct);
		break;
#endif
	    default:
		break;

	}
    }
    return;
}
#endif
#if 0
 /********************************************************************
 * 函数： upPppoeAcc
 * 功能： 统计单个pppoe用户的状态，并更新相关规则
 * 创建： 2012-08-24
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
void upPppoeAcc(PPPoESrvAccProfile *prof, SessionList Session)
{
    emPppoeAccStatus oldSta = prof->expired;/*备份老的状态*/
    upPppAccStatus(prof);/*更新现有状态*/
    SWORD_PRINTF("%s------%d, oldSta = %d, prof->expired = %d\n", __func__, __LINE__, oldSta, prof->expired);
    if(oldSta != prof->expired)
    {
	upPppAccRule(Session, oldSta, 0);/*删除原有规则*/
	upPppAccRule(Session, prof->expired, 1);/*添加新规则*/
    }

    return;
}
#endif
 /********************************************************************
 * 函数： pppoeSesCheck
 * 功能： 循环遍历账号对应的所有会话，更新计费信息
 * 创建： 2012-08-24
 * 参数： 
 *	  prof     -    账号对应profile
 * 返回： 略
 * 输出： 略
 * return : 1 计费状态变化 0 计费状态无变化
 * 作者： Jianqing.Sun 
 ********************************************************************/
int pppoeSesCheck(PPPoESrvAccProfile* prof)
{
    MibProfileType mibTypeS = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *profS = NULL;
    int i = 0, j = 0, ret = 0;

    profS = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibTypeS, 0);
    if(profS != NULL)
    {
	emPppoeAccStatus oldSta = prof->expired;/*备份老的状态*/
	upPppAccStatus(prof);/*更新现有状态*/
	SWORD_PRINTF("%s------%d, oldSta = %d, prof->expired = %d\n", __func__, __LINE__, oldSta, prof->expired);
	if(oldSta != prof->expired)
	{  
            ret = 1;
	    j = prof->sesIndex;
	    if(SesIndexIsOk(j) == 1)
	    {
		for(i = 0; i < prof->actualSession; i++)
		{
		    SWORD_PRINTF("%s------%d, nvramProfile->Session[%d].ConTime = %d, nvramProfile->Session[%d].ip = %s\n", __func__, __LINE__, j, nvramProfile->Session[j].ConTime, j, nvramProfile->Session[j].ip);
		    if((nvramProfile->Session[j].ConTime == 0U) && (prof->chargeType !=  NONE_CHARG_MODE))/*在线并且开启了计费*/
		    {		   
			upPppAccRule(nvramProfile->Session[j], oldSta, 0);/*删除原有规则*/
			upPppAccRule(nvramProfile->Session[j], prof->expired, 1);/*添加新规则*/
		    }
		    j = nvramProfile->Session[j].nextIndex;
		    if(SesIndexIsOk(j) == 0)
		    {
			break;
		    }
		}
	    }
	}
    }
    return ret;
}
#endif

#if (IP_GRP == FYES)
/**
 * 在指定的ip组中查找 指定的用户
 *
 * grpProfIndex : ip组profile索引
 * userName     : 要查找的用户名
 * userType     : 用户类型 
 * searched     : 辅助查找数组。用户保证 被查找过的组不再被重复查找
 *                数组值 0 该组未被查找过  -1 在组中找不到
 *                1 在该组中找得到
 *
 * return: -1 ip组中不含有该用户
 *         1   ip组中含有该用户
 */
int ipGrpSearchUser(int grpProfIndex, char*userName, ipGrpEntryType userType, int*searched)
{
    int i = 0, ret = -1, subIndex = 0;
    IpGrpProfile* prof = NULL;
    MibProfileType mibType = MIB_PROF_IPGRP;

    ret = searched[grpProfIndex];/*该组是否已经进行了查找*/
    if(ret == 0)/*未查找过*/
    {
	ret = -1;/*查找不到*/
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, grpProfIndex);
	if(ProfInstIsFree(prof) == 0)
	{
	    for(i=0; i< IPGRP_USER_ENTRYS; i++)
	    {/*遍历所有成员*/
		if(prof->ipEntry[i].entryType == userType)
		{	
		    if(strcmp(userName, prof->ipEntry[i].entryVal.name) == 0)
		    {
			ret = 1;/*找到*/
		    }
		}
		else 
		{

		    if(prof->ipEntry[i].entryType == IPENT_GRP)
		    {/*在子组中查找*/
			subIndex = ProfGetInstIndexByName(mibType, prof->ipEntry[i].entryVal.name);
			if(subIndex != PROFFAIL)/*该组使用了*/
			{
			    ret = ipGrpSearchUser(subIndex, userName, userType, searched);
			}
		    }

		}
		if(ret == 1)
		{/*找到则退出 */
		    break;
		}
	    }
	}
	searched[grpProfIndex] = ret;/*记录到已查找*/
    }

    return ret;
}
/**
用户上下线时更行对应的ip到相应的ipset组
Username： 用户名
Ip： 用户对应的ip
UserType：  pppoe账号 或 web认证用户或其他
AddorDel:   IPSET_MEM_ADD 用于上线时添加
IPSET_MEM_DEL  用于下线时删除
**/
void userUpToIpGrp(char* username,char *ip, ipGrpEntryType userType, char addOrDel)
{
    MibProfileType mibType = MIB_PROF_IPGRP;
    IpGrpProfile* prof = NULL;
    int i = 0, min = 0, max = 0;
    int searched[MAX_IPGRP_PROFILES];/*辅助空间。保证每个组只被查找一次。提高效率*/
    memset(searched, 0,sizeof(searched));/*初始化为每个组都未查找过*/

    ProfInstLowHigh(mibType, &max, &min);
    SWORD_PRINTF("%s------%d, user is %s,ip = %s\n", __func__, __LINE__, username, ip);
    /*遍历整个profile*/
    for(i = min; i < max; i++) 
    {
	prof = (IpGrpProfile*)ProfGetInstPointByIndex(mibType, i);
	if(ProfInstIsFree(prof) == 0 && prof->grpType == IPGRP_USER)/*只查找用户性ip组 */
	{
	    if(searched[i] == 0)
	    {/*该组未被查找过*/
		ipGrpSearchUser(i, username, userType, searched);
	    }
	    SWORD_PRINTF("%s-----%d,searched[i]\n", __func__, __LINE__);
	    if(searched[i] == 1)
	    {/*在该组中查找到了grpName*/
		SWORD_PRINTF("%s------%d, prof->head.name is %s\n", __func__, __LINE__, prof->head.name);
		ipsetAOrDIpStr(prof->head.name, ip, addOrDel);/*更新相应的ipset组*/
	    }
	}
    }

    return;
}
/**
 * 查找ip地址组树 rootPorf到子组subName的最大深度
 * rootProf: 指向要查找的根节点
 * subName:  要查找的子树
 *           若为NULL则查找整棵树的深度
 * return: 
 *         -1  rootProf不包含subName
 *        >=0 rootProf到subName的最大深度（不包含rootProf）
 *
 */
int ipGrpDepToSub(IpGrpProfile *rootProf, char* subName)
{
    IpGrpProfile *subGrp = NULL;
    int i = 0, dep = -1, subDep = 0 , maxEntrys = 0;

    if(rootProf->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }


    if(subName == NULL)
    {
	dep = 1;/*不查找子组。则最小深度为1*/
    }
    else 
    {
	if((strcmp(subName, rootProf->head.name) == 0))
	{/*自己到自己的深度为0*/	
	    dep = 0;
	}
    }

    if(dep != 0)/*自己不等于子组*/
    {
	/*循环搜索该组的所有条目*/
	for(i = 0; i < maxEntrys; i++)
	{
	    if(rootProf->ipEntry[i].entryType == IPENT_GRP)
	    {/*该条目类型为其他地址组*/
		/*查找子组是否包含*/
		subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, rootProf->ipEntry[i].entryVal.name);
		if(ProfInstIsFree(subGrp) == 0)/*该组使用了*/
		{
		    subDep = ipGrpDepToSub(subGrp, subName);/*子树到subName的最大深度*/
		    if(subDep >= 0 )
		    {
			subDep = subDep + 1;/*子树包含根节点到subNma的深度*/
			if(dep < subDep)
			{/*各深度中的最大值*/
			    dep = subDep;
			}

		    }
		}

	    }

	}
    }
    return dep;

}
/**
 * 遍历组 rootProf的深度（不含根节点）
 * rootProf: 指向要遍历的组
 * subName : 遍历的时候 查找是否有该组或子组.
 * return：subName为NUll则返回rootProf深度
 *         subNae不为NULL。则若在rootProf以及子组中找到subName。返回0.否则返回rootProf深度
 */
int ipGrpDepSearch(IpGrpProfile *rootProf, char *subName)
{
    IpGrpProfile *subGrp = NULL;
    int i = 0, dep = 1, subDep = 0, maxEntrys = 0;

    if(rootProf->grpType == IPGRP_IP)
    {
	maxEntrys =  IPGRP_IP_ENTRYS;
    }
    else
    {
	maxEntrys = IPGRP_USER_ENTRYS;
    }


    if((subName != NULL) && (strcmp(subName, rootProf->head.name) == 0))
    {/*组名相同。即找到子树*/
	dep = 0;
    }
    else
    {
	/*循环搜索该组的所有条目*/
	for(i = 0; i < maxEntrys; i++)
	{
	    if(rootProf->ipEntry[i].entryType == IPENT_GRP)
	    {/*该条目类型为其他地址组*/

		/*查找子组是否包含*/
		subGrp = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP, rootProf->ipEntry[i].entryVal.name);
		if(ProfInstIsFree(subGrp) == 0)/*该组使用了*/
		{
		    subDep = ipGrpDepSearch(subGrp, subName);/*查找是否子组包含该组*/
		    if(subDep == 0)
		    {/*子组包含。则返回0*/
			dep = 0;
		    }
		    else 
		    {
			subDep = subDep + 1;/*子树深度加子树根*/
			if(dep < subDep)
			{/*更新当前树的 深度*/
			    dep = subDep;
			}
		    }
		}
		if(dep == 0)
		{/*在该组或子组中找到查找的组*/
		    break;
		}

	    }

	}
    }
    return dep;

}
#if (FIREWALL == FYES)
/**
 * 判断ip组是否被访问控制策略引用了
 * return :
 *        1 被引用了
 *        0 未被引用
 */
int isIpGrpReredByFireWall(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i); 
	if(ProfInstIsFree(prof) == 0) {
	    if((strcmp(ipProf->head.name, prof->ipGrpName) == 0)
	      ||(strcmp(ipProf->head.name, prof->destIpGrpName) == 0))
	    {/*被引用了*/
		ret = 1;
		break;
	    }
	}
    }
    return ret;
}
#endif
#if (GROUP_BASE == FYES)
/**
 * 判断ip组是否被行为管理引用了
 * return :
 *        1 被引用了
 *        0 未被引用
 */
int isIpGrpReredByNf(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_GROUP_BASE;
    GroupBaseProfile *prof = NULL;
    int ret = 0;
    int max, min, i;
    ProfInstLowHigh(mibType, &max, &min);
    for(i = min; i < max; i++) {
	prof = (GroupBaseProfile*)ProfGetInstPointByIndex(mibType, i); 
	if(ProfInstIsFree(prof) == 0) {
	    if(prof->groupType == GBT_ACT_CONTROL)
	    {/*上网行为管理*/
		if(strcmp(ipProf->head.name, prof->ipGrpName) == 0)
		{/*被引用了*/
		    ret = 1;
		    break;
		}
	    }
	}
    }
    return ret;
}
#endif
#if (WEB_AUTH == FYES)
/***********************************************************8
 *
 *判断是否被web认证引用为例外地址组
 *return 1 被引用
 *		0 未被引用
 * *********************************************************/
int isIpGrpReredByWebAuth(IpGrpProfile *ipProf)
{
   MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
   WebAuthGlobalProfile *prof = NULL;
	int ret = 0;
	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, 0); 
	if(ProfInstIsFree(prof) == 0) {
		if(strcmp(ipProf->head.name, prof->exceptIpGroup) == 0)
		{/*被引用了*/
		    ret = 1;
		}
	    }
    return ret;
}
/*************************
 *
 *功能：删除OK组中旧的例外地址组内的成员
 *参数：WebAuthGlobalProfile *prof
 *时间：20120327
 * *************************/
void webauthDelexceptIpGroup(WebAuthGlobalProfile *prof)
{

	IpGrpProfile* profs1 = NULL;
	IpGrpProfile* profs2 = NULL;
	int i=0,j=0;
	IPADDR ipfrom,ipto;

	if(prof->exceptIpGroup[0] != '\0')
	{
printf("%s,%d,prof->exceptIpGroup=%s\n",__func__,__LINE__,prof->exceptIpGroup);
	profs1 =(IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,prof->exceptIpGroup); /*获取实例*/


	while( i < IPGRP_ENTRYS )
			{
				if(profs1->ipEntry[i].entryType == IPENT_GRP)
				{
					profs2 = (IpGrpProfile*)ProfGetInstPointByName(MIB_PROF_IPGRP,profs1->ipEntry[i].entryVal.name);

					while(j<IPGRP_ENTRYS){
					if(profs2->ipEntry[j].entryType == IPENT_RANGE){
					ipfrom = profs2->ipEntry[j].entryVal.range.ipFrom;
					ipto = profs2->ipEntry[j].entryVal.range.ipTo;
					SWORD_PRINTF("%s,%d,IPfrom=%x,ipto=%x\n",__func__,__LINE__,ipfrom,ipto);
					/*删除地址段*/
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_DEL);
					}
					j++;
					}
				}
				else if (profs1->ipEntry[i].entryType == IPENT_RANGE) {
					ipfrom =  profs1->ipEntry[i].entryVal.range.ipFrom;
					ipto =  profs1->ipEntry[i].entryVal.range.ipTo;
					SWORD_PRINTF("%s,%d,IPfrom=%x,ipto=%x\n",__func__,__LINE__,ipfrom,ipto);
					ipsetAOrDIpRange(SYS_WEB_AUTH_OK, ipfrom, ipto, IPSET_MEM_DEL);
				}
				i++;
			}
		}
return ;
}
/*删除例外地址组*/
void IPGrpDelWebExIp(char* grpName)
{

	MibProfileType mibType = MIB_PROF_WEB_AUTH_GLOBAL;
	WebAuthGlobalProfile *prof = NULL;
	int i=0;

	prof = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibType, i);
	if((prof != NULL) && (ProfInstIsFree(prof) == 0)&&(strcmp(prof->exceptIpGroup,grpName) == 0))
	{
	SWORD_PRINTF("%s--------%d, del webauth ipgroup\n", __func__, __LINE__); 
	webauthDelexceptIpGroup(prof);
	}
	return;
}

#if (WEB_AUTH_SHARE == FYES)
/*
 * checkSessionImdex()
 * 检测web认证账号共享，会话的索引是否正确
 * 1 -- 会话索引正确
 * 0 -- 会话索引错去
 * */
int checkSessionIndex(int index)
{ 
    int flag = 0;        
    if((index >= 0)&&(index < MAX_WEB_AUTH_SESSION))
    {
	flag = 1;        
    }
    return flag;
} 
#endif
#endif
#if (P2P_LIMIT == FYES)
/**
 * 判断ip组是否被P2P限速引用了
 * return :
 *        1 被引用了
 *        0 未被引用
 */
int isIpGrpReredByP2PLimit(IpGrpProfile *ipProf)
{
    MibProfileType mibType = MIB_PROF_P2P_LIMIT;
    P2PLimitProfile *prof = NULL;
    int ret = 0;
    prof = (P2PLimitProfile*)ProfGetInstPointByIndex(mibType, 0); 
    if(ProfInstIsFree(prof) == 0) {
	if(strcmp(ipProf->head.name, prof->ipGrpName) == 0)
	{/*被引用了*/
	    ret = 1;
	}
    }
    return ret;
}
#endif
#endif
#if (PPPOE_SERVER == FYES)
/********************************************************************
 * 函数： SesIndexIsOk
 * 功能： 判断会话数组下标是否正确
 * 创建： 2012-08-23
 * 参数： 会话数组下标
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int SesIndexIsOk(int i)
{
    MibProfileType mibType = MIB_PROF_PPPOE_SRV_GLO;
    PPPoESrvGloProfile *prof = NULL;

    int flag = 0;

    prof = (PPPoESrvGloProfile*)ProfGetInstPointByIndex(mibType, 0);

    if(prof != NULL)
    {
#if (ENABLE_PPPOE_SRV_GLO_IPCOUNT == FYES)
	if((i >= 0) && (i < prof->ipcount))
#else
	if((i >= 0) && (i < prof->maxSessCnt))
#endif
	{/*下标应大于零，小于系统最大会话数*/
	    flag = 1;
	}
    }
    return flag;
}
#endif
#if (ARP_BINDS == FYES)
/**
 * 功能: 从arp静态绑定中匹配 ip，mac并返回匹配状态
 * 参数:
 *       ip：欲匹配的ip地址
 *       mac:欲匹配的mac地址
 *       arpIndex：若不为NULL则 输入时指向不查找的索引。输出时内容为匹配到的索引
 * 作者: bhou
 * 时间: 2011-10-26
 */
extern ARP_MATCH matchFromArpBinds(IPADDR ipAddr, MacAddr macAddr, int* arpIndex)
{
    int min = 0, max = 0;
    int i = 0, tmpIndex = 0;

    ARP_MATCH ret = MATCH_NONE;/*初始化为什么都不匹配*/

    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);/*获取最大最小索引*/

    if(arpIndex != NULL)
    {
        tmpIndex = *arpIndex;/*记录查找时不查找的索引*/
    }
    else
    {
        tmpIndex = PROF_UNUSED_INDEX;/*查找所有索引*/
    }
    /*遍历查找*/
    for(i = min; i < max; i++) 
    {
        if(i != tmpIndex)/*若有排除索引，应排除*/
        {
            prof = (UserProfile*)ProfGetInstPointByIndex(mibType, i);/*获取实例*/
            if((prof != NULL) && (ProfInstIsFree(prof) == 0)) /*配置已用*/
            {
                if(prof->ip == ipAddr)
                {
                    ret = MATCH_IP_ONLY;/*仅仅匹配IP地址*/
                }
                if(macAddrCmp(&macAddr, &(prof->mac)) == 0)
                {
#if (DHCP_POOL == FYES)
                    DhcpPoolProfile *profPool= NULL;
                    profPool = (DhcpPoolProfile *)ProfGetInstPointByName(MIB_PROF_DHCP_POOL, prof->dhcpPoolName);
                    if ( (ntohl(ipAddr) & ntohl(profPool->netmask)) == (ntohl(profPool->ipStart) & ntohl(profPool->netmask)) ){
#endif
                        if(ret == MATCH_NONE)/*ip地址无匹配*/
                        {
                            ret = MATCH_MAC_ONLY;/*仅仅匹配MAC地址*/
                        }
                        else
                        {
                            ret = MATCH_ALL;/*匹配所有*/
                        }
#if (DHCP_POOL == FYES)
                    }
#endif
                }
                if(ret != MATCH_NONE)
                {
                    tmpIndex = i;/*记录找到的匹配项目索引*/
                    /*查找到一条匹配信息则查找结束*/
                    break;
                }
            }
        }
    }

    if(arpIndex != NULL)
    {/*记录匹配到的索引*/
        *arpIndex = tmpIndex;
    }
    return ret;
}
#endif

#if (DHCP_AUTO_BIND == FYES)
/********************************************************************
 * 函数： DhcpArpBindConfig
 * 功能： 绑定ip和mac,由于关闭自动绑定并不删除绑定，故关闭后再次开启
 *	  的时候已绑定过的还是不绑定，但是不能报错
 * 创建： 2012-09-13
 * 参数： 
 *	  ip	      绑定的ip
 *	  macAddr     绑定的mac
 *	  userName    绑定的用户名
 * 返回：
 *        0           绑定失败
 *        1           绑定成功
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/ 
int DhcpArpBindConfig(IPADDR ip, MacAddr macAddr,char* userName)
{
    MibProfileType mibType = MIB_PROF_USER;
    UserProfile *prof = NULL;
    int instIndex = 0;
    const char *errMsg = NULL;
    struProfAlloc *profhead = NULL;
    int flag = 0;

    if(ProfGetInstIndexByName(mibType, userName) == PROFFAIL)
    {/*此实例名是否已使用判断*/
	if(matchFromArpBinds(ip, macAddr, NULL) == MATCH_NONE)
	{/*ip，mac是否已绑定判断*/
	    instIndex = ProfNewInst(mibType, userName, FALSE);/*新建实例*/

	    if(instIndex != PROFFAIL) /*因以作重复性判断。故这里只可能时达到最大条目*/
	    {
		flag = 1;
		ProfBackupByIndex(mibType, PROF_CHANGE_ADD, instIndex, &profhead);/*备份新创建的空实例*/
		prof = (UserProfile*)ProfGetInstPointByIndex(mibType, instIndex);/*获取实例指针*/
		prof->bindEn = FUN_ENABLE;/*默认为允许访问*/
		prof->ip = ip;/*静态绑定ip地址*/
		prof->mac = macAddr;/*静态绑定mac地址*/
		prof->bindType = AUTO_BIND;
		ProfUpdate(profhead);/*发消息生效*/
		ProfFreeAllocList(profhead);/*释放备份控件*/
	    }
	}
    }
    SWORD_PRINTF("%s------%d, flag = %d\n", __func__, __LINE__, flag);

    return flag;
}
#endif

#if (FIREWALL == FYES)
/********************************************************************
 * 函数： websDefineWebAuth
 * 功能： 把精细化限速新增组移动至最前
 * 创建： 2012-08-08
 * 参数： name 新增组的组名
 * 返回： 1 - 做策略移动
 *        0 - 未移动
 * 输出： 略
 * 作者： Jianqing.Sun 
 *********************************************************************/
extern int moveToLast(char* name)
{
    int i = 0, min = 0, max = 0;
    int rst = 0;
    MibProfileType mibType = MIB_PROF_FIRE_WALL;
    FireWallProfile* prof = NULL;

    ProfInstLowHigh(mibType, &max, &min);
    for(i = (max - 1); i >= min; i--)
    {
        prof = (FireWallProfile*)ProfGetInstPointByIndex(mibType, i);
        if(prof != NULL && ProfInstIsFree(prof) == 0)
        {/*找出索引最大的组,把新增组移动至它之后*/
            if(strcmp(prof->head.name, name) != 0)/*确保本身不是最大*/
            {
                moveInstToLast(name, prof->head.name, mibType);
		/*FW_PRINTF("%s-----%d, prof->head.name = %s\n", __func__, __LINE__, prof->head.name);*/
                rst = 1;
            } else {
                rst = 0;
            }
            break;
        }
    }
    return rst;
}

/***********************************************************************
 * 功能描述：将要移动的实例号移动到目的实例号之后
 * 输入参数：
 *          sID:        要移动的实例号。
 *          tID:        目的实例号。
 *          proftype:   实例类型。
 * 输出参数：   0 移动失败（已在后面）
 *              1  移动成功
 * 创建日期：   2012-08-09
 * 作者：       Jianqing.Sun
 ***********************************************************************/
extern int moveInstToLast(char *sID, char * tID, MibProfileType proftype)
{
    int tmp = 0, min = 0, max = 0, i = 0, tagIndex = 0, srcIndex = 0;
    struProfAlloc *proflist  = NULL;
    struProfAlloc *proflist2  = NULL;

    tagIndex = ProfGetInstIndexByName(proftype,tID);
    srcIndex = ProfGetInstIndexByName(proftype,sID);
    ProfInstLowHigh(proftype, &max, &min);
    /*FW_PRINTF("%s------%d,srcIndex = %d, tagIndex = %d\n", __func__, __LINE__, srcIndex, tagIndex);*/
    if(tagIndex != (srcIndex - 1))
    {     /*需要调整位置*/
        for(i = min; i < max; i++)
        {                                     /*  从头开始备份  */
            /* 假如找到目标位置， 就将源实例备份插    入到目标实例位置   随后也需要备份 目标实例，整条练>
 * 就完成的连接起来了 */
            if(i == (tagIndex + 1))
            {
                ProfBackupByIndex(proftype,PROF_CHANGE_EDIT,srcIndex, &proflist);
            }
            if(i==srcIndex) continue;     /*源实例位置 在tagIndex位置已经插>
入，现在不需要插入了*/
            ProfBackupByIndex(proftype,100,i, &proflist);  /*100代表备份，但是不触发profilechange*/
        }
        tmp = 1;
    }
    tagIndex=0;    /*恢复位置从头开始*/
    proflist2 = proflist;
    while(proflist!= NULL)
    {     /*恢复  从开始位置恢复表*/
        _ProfRestore(proflist->proftype, tagIndex++, proflist->oldProf);
        proflist = proflist->next;
    }

    ProfUpdate(proflist2);

    ProfFreeAllocList(proflist);
    ProfFreeAllocList(proflist2);
    return tmp;
}
#endif
/***********************************************************************
 * 函 数 名：   ip2int
 * 创建日期：   2010-3-17
 * 修改日期：
 * 作者：       柏德秀
 * 附加说明：	无
 ***********************************************************************/
extern unsigned int ip2int(char *ip)
{
    char *tmp,ipbak[16]={0},*ptr;
    unsigned int ret=0;
    unsigned int value;
    strcpy(ipbak,ip);
    tmp=strtok_r(ipbak,".",&ptr);
    while(tmp!=NULL)
    {
	// DBGPRINT("ip2int:%s\n",tmp);
        value=atoi(tmp);
        ret=ret<<8;
        ret|=value;
        tmp=strtok_r(NULL,".",&ptr);
    }
        //DBGPRINT("ip2int:%x\n",ret);
    return ret;
}

/******************************************************************
 * 函数功能：使用ioctl查询arp，得到已知ip对应mac
 * 创建时间：2013-02-17
 ****************************************************************/
char* getMacByIpIoctl(struct arpreq *parp)
{
    char *ret = NULL;
    int fd = 0;
    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
	if (!(ioctl (fd, SIOCGARP, parp)))/*查询arp表得到当前ip对应的mac地址*/
	{
	    if (parp->arp_flags != 0)/*查到arp表中Flags等于0,即mac地址等于0，web认证计费时不判断mac地址*/
	    {
		ret = parp->arp_ha.sa_data;
	    }
	}
	close(fd);
    }
    return ret;
}

#if (EASY_TASK == FYES)
/*
 * 功能：返回SystemProfile中成员的值
 * 创建时间：2013-6-7
 * 输入：num -- 每一种序号对应一个成员
 * 输出：int -- 对应成员的值
 * 备注：只能返回int型成员的值
 */
int getValueByProfSys(int num)
{
    int retVal = -1;
    MibProfileType mibType = MIB_PROF_SYSTEM;
    SystemProfile *prof = NULL;

    prof = (SystemProfile*)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == 0)
    {
	switch(num)
	{
	    case 1:
		retVal = prof->isDefaultConf;/*出厂配置*/
		break;
	    default:
		/*null*/
		break;
	}
    }
    return retVal;
}

/*
 * 功能：改变易管理中WAN口的状态，非WAN口实际状态(无pv锁)
 * 创建时间：2013-6-7
 * 输入：status -- 改变后的状态
 * 输出：0 -- 成功  1 -- 失败
 */
int wanConnStatsChangeNPV(int status)
{
    int retVal = 0;
    MibProfileType mibType = MIB_PROF_EASY_TASK;
    EasyTaskProfile *prof = NULL;

    prof = (EasyTaskProfile *)ProfGetInstPointByIndex(mibType, 0);
    if (ProfInstIsFree(prof) == 0)
    {
#if 0
	if (((prof->ConnStatus == CONNECTED) || (prof->ConnStatus == CONNECTING)) && ((status < CHECKING) || (status > CHECK_COMPLETE)))
#endif
	{
	    prof->ConnStatus = status;
	}
    }    

    return retVal;

}
/*
 * 功能：改变易管理中WAN口的状态，非WAN口实际状态
 * 创建时间：2013-6-7
 * 输入：status -- 改变后的状态
 * 输出：0 -- 成功  1 -- 失败
 */
int wanConnStatsChange(int status)
{
    int retVal = 0;

    uttSemP(SEM_CONNSTA_NO, 0);/*加锁*/
    retVal = wanConnStatsChangeNPV(status);
    uttSemV(SEM_CONNSTA_NO, 0);/*解锁*/
    return retVal;
}
#endif
#if (WEB_AUTH == FYES)
#if (WEBAUTH_AUTO == FYES)
#define WEBAUTH_MAX_LOG_LEN 128
#define WEBAUTH_MAX_LOG_NUM 2
int isStorageExist(char usb_mount_path[][MAX_PATH_LEN],int *count,unsigned int flag)
{
    char buf[256];
    char device_path[256];
    char mount_path[256];
    FILE *fp=NULL, *fp2=NULL;
    int i = 0;
    int device_exist = 0;

    fp = fopen("/proc/mounts", "r");
    if(!fp){
	perror(__FUNCTION__);
	return 0;
    }

    while(fgets(buf, sizeof(buf), fp)){
        device_exist = 0;

        if((flag & USB_EXIST) &&(strstr(buf, USB_STORAGE_SIGN)) ){
            device_exist = 1;
        }
        if((flag & SD_EXIST) &&(strstr(buf, SD_STORAGE_SIGN)) ){
            device_exist = 1;
        }

        if(device_exist){
            sscanf(buf, "%s %s", device_path, mount_path);
            fp2 = fopen(device_path, "r");
            if(fp2!=NULL)
            {
                if(usb_mount_path[i] != NULL)
                {
                    strcpy(usb_mount_path[i], mount_path);
                    strcat(usb_mount_path[i], "/");
                    i++;
                }
                fclose(fp2);
            }
        }
    }

    *count = i;
    fclose(fp);

    if(*count>0){
        return 1;
    }else{
        return 0;
    }
}
/*
*计算文件系统剩余空间
*KeHuatqo
*2011-04-29
*/
int storageFree(const char * path, long long *disk_size, long long *disk_free)
{
    struct statfs buf;
    int ret=0;

    /*初始化*/
    memset(&buf, 0, sizeof(struct statfs));
    if(statfs(path, &buf)==0)
    {
#if 0
	/*每个block里面包含的字节数*/
	printf("block size   = %ld\n", buf.f_bsize);
	printf("total blocks = %ld\n", buf.f_blocks);
	printf("free  blocks = %ld\n", buf.f_bfree);
	printf("total size   = %ld MB\n", (buf.f_bsize * buf.f_blocks)>>20); 
	printf("free size    = %ld MB\n", (buf.f_bsize * buf.f_bfree)>>20);
#endif
	if(disk_size!=NULL)
	{
	    *disk_size = (long long)(buf.f_bsize) * (long long)(buf.f_blocks);
	}
	/*计算剩余空间*/
	if(disk_free!=NULL)
	{
	    *disk_free = (long long)(buf.f_bfree) * (long long)(buf.f_bsize);
	}
#if 0
	printf("disk free size = %lld B\n", freeDisk);
#endif
    }
    else
    {
	/*出错*/
	ret = -1;
    }

    return ret;
}

#define  SERVER_PORT 12345
#define  SERVERIP    "116.236.120.162"
#define  KEY_LEN     10

struct  LOG_MSG{
    char key[KEY_LEN + 2];
    char productSn[KEY_LEN + 2];
    char data[0];
};

/***********************************************************************
 ** 函 数 名：   ConnectNonb
 ** 创建日期：   2014-2-22
 ** 修改日期：
 ** 作者：       xu.jian
 ** 附加说明：	解决连接远端服务器超时问题
 ************************************************************************/
int ConnectNonb(int sockfd, struct sockaddr *saptr, socklen_t salen, int nsec)
{
    int flags, n, error;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    errno = 0;

    if ((n = connect(sockfd, saptr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            return EXIT_FAILURE;
        }
    }
    /*可以做任何其它的操作*/
    if (n == 0)
    {
        goto done; // 一般是同一台主机调用，会返回 0
    }
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset; // 这里会做 block copy
    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    /*如果nsec 为0，将使用缺省的超时时间，即其结构指针为 NULL*/
    /*如果tval结构中的时间为0，表示不做任何等待，立刻返回*/
    if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0)
    {
        close(sockfd);
        errno = 10;
        return EXIT_FAILURE;
    }
    if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
    {
        len = sizeof(error);/* 如果连接成功，此调用返回 0*/
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR,&error,&len))
                return EXIT_FAILURE;
    }else{
        printf("select error: sockfd not set");
    }

done:
         fcntl(sockfd, F_SETFL, flags); // 恢复socket 属性
    if (error)
    {
    close(sockfd);
    errno = error;
    return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int getSmsServerIp(char *ip)
{
    struct hostent *h;
    struct in_addr *p;
    MibProfileType mibTypeG = MIB_PROF_WEB_AUTH_GLOBAL;
    WebAuthGlobalProfile *profG = NULL;

    printf("%s %d.........\n", __func__, __LINE__);
    profG = (WebAuthGlobalProfile*)ProfGetInstPointByIndex(mibTypeG, 0);
    if ((profG == NULL) || (ProfInstIsFree(profG)) || (profG->head.active == FALSE))
    {
	printf("%s %d: cannot get global parameters.\n", __func__, __LINE__);
	return 0;
    }

    printf("%s %d.........get host name: %s\n", __func__, __LINE__, profG->logServer);
    h = gethostbyname(profG->logServer);                            
    if (h != NULL)   
	strcpy(ip, inet_ntoa(*(struct in_addr *)h->h_addr));
    else                                                  
	return 0;                                          

    printf("%s %d.........get host ip: %s\n", __func__, __LINE__, ip);
    return 1;
}
/***********************************************************************
 ** 函 数 名：   sendMsg2UTT
 ** 创建日期：   2014-2-22
 ** 修改日期：
 ** 作者：       xu.jian
 ** 附加说明：	发送web认证上网日志发送到远端UTT服务器
 ************************************************************************/

int sendMsg2UTT(char *key,unsigned int productSn,char* data)
{
    int    sockfd;
    char   buffer[1024] = {0};
    struct sockaddr_in server_addr;
    int    nbytes;
    int    opt = SO_REUSEADDR;
    struct LOG_MSG  *sms;
    char   Base64String[1000] = {0};
    int    Base64StringLen = 0;
    sms =  (struct LOG_MSG*)buffer;
    char   serverIP[256] = {0};

    if(strlen(key) > KEY_LEN )
    {
        perror("argv error.\n");
        return 0;
    }
    memset(buffer,0,sizeof(buffer));

    memcpy(sms->key,key,strlen(key));
    sprintf(sms->productSn,"%d",productSn);
    memcpy(sms->data,data,strlen(data));

    Base64StringLen = Base64Encode(sms,Base64String,strlen(data) + sizeof(struct LOG_MSG));
    memcpy(buffer,Base64String,Base64StringLen);

    SWORD_PRINTF("%s------%dsizeof(struct LOG_MSG) :%d;strlen(data): %d ; Base64StringLen : %d\r\n", __func__, __LINE__,sizeof(struct LOG_MSG),strlen(data),Base64StringLen);

    if(!getSmsServerIp(serverIP))
    {
        memcpy(serverIP,SERVERIP,sizeof(SERVERIP));
    }
    if (inet_aton(serverIP, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "the hostip is not right!");
        return 0;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket Error:%s\n\a", strerror(errno));
        return 0;
    }
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(SERVER_PORT);
    if (ConnectNonb(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr),2) == -1) {
        fprintf(stderr, "Connect Error:%s\n\a", strerror(errno));
        return 0;
    }
    if(write(sockfd,buffer,Base64StringLen) == -1)
    {
        perror("send error.\n");
        return 0;
    }
    close(sockfd);
    return 0;
}

/***********************************************************************
 *  * 函 数 名：   wevAuth2UTT
 *   * 创建日期：   2014-2-13
 *    * 修改日期：
 *     * 作者：       李文昌
 *      * 附加说明：	web认证上网日志发送到远端UTT服务器
 *       ***********************************************************************/
static void webAuth2UTT(int authType, char *authName, char *authMac, unsigned long authStartTime, unsigned long authStopTime)
{
#define MAX_AUTH_SEND_LEN 480
    static int sentFlag = 1;
    static unsigned char sendBuf[MAX_AUTH_SEND_LEN+100];

    if (sentFlag) {
	memset(sendBuf, 0, sizeof(sendBuf));
	sentFlag = 0;
    }

    sprintf(sendBuf, "%s%1d%02d%s%02X%02X%02X%02X%02X%02X%08X%08X",
	    sendBuf, authType, strlen(authName), authName, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], (unsigned char)authMac[2], 
	    (unsigned char)authMac[3], (unsigned char)authMac[4], (unsigned char)authMac[5], 
	    authStartTime, authStopTime);
#if 0
    printf("%s*******%d;%s%1d%02d%s%02X%02X%02X%02X%02X%02X%08X%08X\r\n",
        __func__,__LINE__,
	    sendBuf, authType, strlen(authName), authName, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], (unsigned char)authMac[2], 
	    (unsigned char)authMac[3], (unsigned char)authMac[4], (unsigned char)authMac[5], 
	    authStartTime, authStopTime);
#endif

    /* send out to UTT server */
#if 1
    SWORD_PRINTF("%s------%d\n,len:%d;sendBuf:%s\r\n", __func__, __LINE__,strlen(sendBuf),sendBuf);
#endif
    if (strlen(sendBuf) >= MAX_AUTH_SEND_LEN)
    {
	sentFlag = 1;
	sendMsg2UTT("UTT20",getSystemSn(),sendBuf);
	//printf("%s send: \n%s\n", __func__, sendBuf);
    }

    return;
}

/***********************************************************************
 *  * 函 数 名：   webAuthLogWrite
 *   * 创建日期：   2010-3-17
 *    * 修改日期：
 *     * 作者：       李文昌
 *      * 附加说明：	web认证上网日志写入文件
 *       ***********************************************************************/
static int webAuthLogWrite(int authType, char *logPath, char *logPathName, char *data, int dataLen)
{
    FILE *fp;
    long long total_space = 0, unused_space = 0;
    char  buf[WEBAUTH_MAX_LOG_LEN];

    /* 检查磁盘剩余空间是否足够 */
    storageFree(logPath, &total_space, &unused_space);  
    if (unused_space < 10*1024)
    {
	printf("%s %d: %s not enough server storage is available.\n", 
		__func__, __LINE__, logPath); 
	return 0;
    }

    /* 检查文件是否存在，若不存在，则新建 */
    fp = fopen(logPathName, "r");
    if(fp == NULL)
    {
	printf("%s %d: %s not exist, create it.\n", __func__, __LINE__, logPathName);
	fp = fopen(logPathName, "w");
	if (fp == NULL)
	{
	    printf("can't create file %s.\n", logPathName);
	    return 0;
	}

	if (authType == IDCARD_METHOD)
	    fprintf(fp, "%-18s\t%-17s\t%-19s\t%-19s\t%-8s\n------------------", "account", "mac", "startTime", "stopTime", "onlineTime");
	else if (authType == SMS_METHOD)
	    fprintf(fp, "%-11s\t%-17s\t%-19s\t%-19s\t%-8s\n-----------", "account", "mac", "startTime", "stopTime", "onlineTime");
	else if (authType == WECHAT_METHOD)
	    fprintf(fp, "%-30s\t%-17s\t%-19s\t%-19s\t%-8s\n------------------------------", "account", "mac", "startTime", "stopTime", "onlineTime");
	fprintf(fp, "------------------------------------------------------------------------------------\n");
    }
    fclose(fp);

    /* 日志信息存入文件 */
    fp = fopen(logPathName, "a");
    if (fp == NULL)
    {
	printf("%s %d: can't open file %s.\n", __func__, __LINE__, logPathName);
	return 0;
    }
    if(fwrite(data, dataLen, 1, fp) != 1)
    {
	printf("write log error!\n");
    }

    fclose(fp);

    return 1;
}

/***********************************************************************
 *  * 函 数 名：   _webAuthLog
 *   * 创建日期：   2010-3-17
 *    * 修改日期：
 *     * 作者：       李文昌
 *      * 附加说明：	web认证上网日志写入文件
 *       ***********************************************************************/
static void _webAuthLog(char *logPath, int authType, char *authName, char *authMac, unsigned long authStartTime, unsigned long authStopTime)
{
    static unsigned long lastWriteTime = 0;
    static int idcardLogNum = 0, smsLogNum = 0, wechatLogNum = 0;
    static char smsLogData[WEBAUTH_MAX_LOG_LEN*WEBAUTH_MAX_LOG_NUM];
    static char wechatLogData[WEBAUTH_MAX_LOG_LEN*WEBAUTH_MAX_LOG_NUM];

    unsigned long usedTime = authStopTime - authStartTime;
    struct tm *tmStart = localtime(&authStartTime);
    struct tm *tmStop;
    struct tm *ptm;
    char logPathName[MAX_PATH_LEN];
    char *pData;

    if (authType == SMS_METHOD)
    {
	if (smsLogNum == 0)
	    memset(smsLogData, 0, sizeof(smsLogData));
	pData = smsLogData;
	sprintf(pData, "%s%-11s\t", pData, authName);
	smsLogNum++;
    }
    else if (authType == WECHAT_METHOD)
    {
	if (wechatLogNum == 0)
	    memset(wechatLogData, 0, sizeof(wechatLogData));
	pData = wechatLogData;
	sprintf(pData, "%s%-28s\t", pData, authName);
	wechatLogNum++;
    }
    else
    {
	printf("%s %d: authType %d error!\n", __func__, __LINE__, authType);
	return;
    }

    sprintf(pData, "%s%02X:%02X:%02X:%02X:%02X:%02X\t", pData, 
	    (unsigned char)authMac[0], (unsigned char)authMac[1], 
	    (unsigned char)authMac[2], (unsigned char)authMac[3], 
	    (unsigned char)authMac[4], (unsigned char)authMac[5]); 
    sprintf(pData, "%s%-04d-%02d-%02d %02d:%02d:%02d\t", pData, 
	    tmStart->tm_year+1900, tmStart->tm_mon+1, tmStart->tm_mday, 
	    tmStart->tm_hour, tmStart->tm_min, tmStart->tm_sec);
    tmStop = localtime(&authStopTime);
    sprintf(pData, "%s%-04d-%02d-%02d %02d:%02d:%02d\t", pData, 
	    tmStop->tm_year+1900, tmStop->tm_mon+1, tmStop->tm_mday, 
	    tmStop->tm_hour, tmStop->tm_min, tmStop->tm_sec);
    sprintf(pData, "%s%02d:%02d:%02d\n", pData, 
	    usedTime/3600, (usedTime%3600)/60, usedTime%60);

    if (idcardLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sIdCardWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	idcardLogNum = 0;
    }
    else if (smsLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sSmsWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	smsLogNum = 0;
    }
    else if (wechatLogNum >= WEBAUTH_MAX_LOG_NUM)
    {
	sprintf(logPathName, "%sWechatWebAuth_%04d%02d.txt", logPath, tmStop->tm_year+1900, tmStop->tm_mon+1);
	wechatLogNum = 0;
    } else {
	printf("%s %d: idcardLogNum: %d; smsLogNum: %d; wechatLogNum: %d\n", __func__, __LINE__, idcardLogNum, smsLogNum, wechatLogNum);
	return;
    }

    webAuthLogWrite(authType, logPath, logPathName, pData, strlen(pData));
    return;

}

/***********************************************************************
 *  * 函 数 名：   webAuthLog
 *   * 创建日期：   2014-2-13
 *    * 修改日期：
 *     * 作者：       李文昌
 *      * 附加说明：	web认证上网日志写入文件
 *       ***********************************************************************/
int webAuthLog(WebAuthProfile *prof, Uint32 ip, char *mac)
{
    char device_path[MAX_STORAGE_NUM][MAX_PATH_LEN];
    int i, count = 0;

    unsigned int flag = 0u;
    unsigned long currentTime = time(0);

    static int firstFlag = 1;
    static char logPath[MAX_PATH_LEN];
    char  username[20];

    /* 首次生成log日志，需要检测存外置储设备是否存在 */
    if (firstFlag)
    {
	flag |= USB_EXIST;
	flag |= SD_EXIST;
	memset(device_path, 0, sizeof(device_path));
	memset(logPath, 0, sizeof(logPath));
	if(isStorageExist(device_path,&count,flag)==1){
	    strcpy(logPath, device_path[0]);
	    printf("%s %d: logPath: %s\n", __func__, __LINE__, logPath);
	} else {
	    printf("%s %d: no usb or sd card found.\n", __func__, __LINE__);
	}
	firstFlag = 0;
    }

    /* 仅保存身份证阅读器、短信、微信生成的账号 */
    memset(username, 0, sizeof(username));
    if (prof->autoAuthMethod == IDCARD_METHOD)
	strcpy(username, prof->idcode);
    else if ((prof->autoAuthMethod == SMS_METHOD) || (prof->autoAuthMethod == WECHAT_METHOD))
	strcpy(username, prof->user);
    else
	return;

#if 1
    printf("%s %d: user %s ip %#X mac: %02X%02X-%02X%02X-%02X%02X\n", 
	    __func__, __LINE__, username, ip, 
	    (unsigned char)mac[0], (unsigned char)mac[1], 
	    (unsigned char)mac[2], (unsigned char)mac[3], 
	    (unsigned char)mac[4], (unsigned char)mac[5]);
#endif
    /* 日志信息写入文件 */
    if (logPath[0])
    {
	_webAuthLog(logPath, prof->autoAuthMethod, username, mac, prof->timeStart, currentTime);
    }

    /* 日志信息发送到UTT服务器 */
    webAuth2UTT(prof->autoAuthMethod, username, mac, prof->timeStart, currentTime);

    return;
}
#else
int webAuthLog(WebAuthProfile *prof, Uint32 ip, char *mac)
{
    return 0;
}
#endif
#endif
