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

/*删除内核对应的所有策略*/
static int uttNfNlAliDelAll(void)
{
    UttNlNfAliConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_ALI;
    /*删除所有*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;

    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
    return 0;
}

/********************************************************************
 * 函数： uttNfNlAliAdd
 * 功能： 新建Ali白名单
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
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
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
        prof = (ExceptAliProfile*)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
                && (prof->head.active == TRUE))
        {
            strcpy(nlConf.entryVal.aliNumber, prof->aliNumber);/*Ali号码*/
            SWORD_PRINTF("%s------%d, prof->aliNumber = %s, nlConf.entryVal.aliNumber = %s\n", __func__, __LINE__, prof->aliNumber, nlConf.entryVal.aliNumber);
            uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
        }
    }
    return;
}
/********************************************************************
 * 函数： exceptaliProfileChange
 * 功能： Ali白名单配置变化 生效函数
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
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
 * 函数： exceptaliglobalProfileChange
 * 功能： Ali白名单开关变化 生效函数
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
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
            {/*开启变为关闭*/
#ifdef CONFIG_UTT_NETFILTER
                /* 重建mangle FORWARD中规则 */
                reNewPdbChainJumpRule();
#endif

                AliChainAOrD(Iptables_DEL);
                uttNfNlAliDelAll();
            }
            else if((oldProfileDataPtr->exceptAliEn == FUN_DISABLE) && (newProfileDataPtr->exceptAliEn == FUN_ENABLE))
            {/*关闭变为开启*/
#ifdef CONFIG_UTT_NETFILTER
                /* 重建mangle FORWARD中规则 */
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
 * 函数： AliChainAOrD
 * 功能： 增加或删除Ali白名单规则链到iptables规则
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/
static void AliChainAOrD(char action)
{
    doSystem("iptables -t mangle -%c %s -j UTTEXCEPTALIIM", action, IM_EXCEPT_CHAIN);
    SWORD_PRINTF("iptables -t mangle -%c %s -j UTTEXCEPTALIIM\n", action, IM_EXCEPT_CHAIN);
    return;
}

/********************************************************************
 * 函数： exceptaliInit
 * 功能： 认证初始化函数 - 先清空现有规则链表，再重新添加
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
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
 * 函数： exceptaliUpdatePortMap
 * 功能： 更新阿里旺旺白名单账号记录map
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
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
        {/*取出id后面的数字*/
            webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);/*取配置中的id号*/
            if (webInstIndex != 0U) 
            {
                setPortMap(&(nvramProfile->exceptaliInstRec.instRecMap), webInstIndex);/*赋值给实例号*/
            }
        }
    }

    return;
}
/********************************************************************
 * 函数： funInitExceptAli
 * 功能： 认证功能 初始化入口函数
 * 创建： 2013-9-21
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/
void funInitExceptAli(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)exceptaliglobalProfileChange);/*触发函数*/
    registerForProfileUpdates(MIB_PROF_EXCEPT_ALI, (Pointer)NULL, (ProfileUpdateFnc)exceptaliProfileChange);/*触发函数*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    exceptaliInit();/*初始化*/
    /*初始化记录map */
    exceptaliUpdatePortMap();
    return;
}
#endif
#endif
