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

#if (EXCEPT_QQ == FYES)
#ifdef CONFIG_UTT_NETFILTER
#include <linux/netfilter_ipv4/utt_netfilter.h>
extern int spdNfNlSock;
static int uttNfNlQQDelAll(void);
static void uttNfNlQQAdd(void);
static void QQChainAOrD(SystemProfile *prof,char action);

#ifdef SWORD_DEBUG
#define SWORD_PRINTF(fmt, args...)		printf(fmt, ## args)
#else
#define SWORD_PRINTF(fmt, args...)
#endif

static void exceptqqProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptQQProfile *oldProfileDataPtr, const ExceptQQProfile *newProfileDataPtr );
static void exceptqqglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr );
static void exceptqqInit(void);
static void exceptqqUpdatePortMap(void); 
static void uttNfNlenterpriseQQDelAll();
static void uttNfNlenterpriseQQAdd();

/********************************************************************
 * 函数： uttNfNlenterpriseQQAdd
 * 功能： 读取策略,发消息把策略传递给内核
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/
static void uttNfNlenterpriseQQAdd()
{
    MibProfileType mibType = MIB_PROF_PDB_GLOBAL;
    PdbProfile *prof= NULL;
    char temp[MAX_PDB_LINE_LENGTH + 1],*temp1;
    char *ptr;
    char slipchar[]=",";
    UttNlNfEnterpriseQQConf  nlConf;
    int min, max, index, i, j;
    unsigned short Num = 0;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_ENTERPRISE_QQ;
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;

    /*遍历策略库*/
    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) {
        prof = (PdbProfile*)ProfGetInstPointByIndex(mibType, i);
        if (ProfInstIsFree(prof) == 1) {
            continue;
        }

        if (prof->type == EnterpriseQQPdb) {
#if 0
            for(j=0;j<MAX_PDB_LINE_COUNT;j++){
                if (strlen(prof->line[j]) > 0){

                    SWORD_PRINTF("%s-%d ===================line[%d]: =%s=\n",__func__, __LINE__,j,prof->line[j]);
                    Num = 0;
                    Num = (unsigned short)strtoul(prof->line[j],NULL,16);

                    if(Num == 0)
                        continue;

                    nlConf.entryVal.qqValue = Num;
                    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*发送消息给内核*/
                }
            }
#endif
            /*遍历企业QQ策略*/
            for(i=0;i<MAX_PDB_LINE_COUNT;i++){
                if (strlen(prof->line[i]) > 0) {
                    strcpy(temp,prof->line[i]);
                    temp1 = strtok_r(temp,slipchar,&ptr);
                    while(temp1){
                        if(strlen(temp1) == 4 ){
                            SWORD_PRINTF("%s-%d line: =%s=\n",__func__, __LINE__,temp1);
                            unsigned short Num =(unsigned short)strtoul(temp1,NULL,16);
                            if(Num == 0)
                                continue;
                            nlConf.entryVal.qqValue = Num;
                            uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0 ,0 );/*发送消息给内核*/
                        }
                        temp1=strtok_r(NULL,slipchar,&ptr);
                    }
                }
            }
            break ;
        }
    }
    return;
}
/********************************************************************
 * 函数： uttNfNlenterpriseQQDelAll
 * 功能： 发消息给内核，删除全部策略
 * 创建： 2013-09-20
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： xu.jian
 ********************************************************************/
static void uttNfNlenterpriseQQDelAll()
{
    UttNlNfQQConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_ENTERPRISE_QQ;
    /*删除所有*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;

    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
    return 0;
}
/*删除内核对应的所有策略*/
static int uttNfNlQQDelAll(void)
{
    UttNlNfQQConf  nlConf;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_QQ;
    /*删除所有*/
    nlConf.confType = UTT_NFCONF_DEL_ALL;

    uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
    return 0;
}

/********************************************************************
 * 函数： exceptqqProfileChange
 * 功能： 新建QQ白名单
 * 创建： 2012-06-19
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void uttNfNlQQAdd(void) 
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    UttNlNfQQConf  nlConf;
    int min, max, index;

    memset(&nlConf, 0, sizeof(nlConf));
    nlConf.msgHead.nlType = UTT_NLNF_QQ;
    /*增加*/
    nlConf.confType = UTT_NFCONF_ADD;

    ProfInstLowHigh(mibType, &max, &min);
    for (index = min; index < max; index++) 
    {
        prof = (ExceptQQProfile*)ProfGetInstPointByIndex(mibType, index);
        if((prof != NULL) && (ProfInstIsFree(prof) == 0) 
                && (prof->head.active == TRUE))
        {
            sprintf(nlConf.entryVal.qqNumber, "%u", prof->qqNumber);/*QQ号码*/
            SWORD_PRINTF("%s------%d, prof->qqNumber = %d, nlConf.entryVal.qqNumber = %s\n", __func__, __LINE__, prof->qqNumber, nlConf.entryVal.qqNumber);
            uttNfNlSend(spdNfNlSock,(char*)&nlConf, sizeof(nlConf), 0U ,0U );/*发送消息给内核*/
        }
    }
    return;
}
/********************************************************************
 * 函数： exceptqqProfileChange
 * 功能： QQ白名单配置变化 生效函数
 * 创建： 2012-06-19
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void exceptqqProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const ExceptQQProfile *oldProfileDataPtr, const ExceptQQProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
        case PROF_CHANGE_EDIT:
        case PROF_CHANGE_DEL:
        case PROF_CHANGE_DELALL:
            uttNfNlQQDelAll();
            uttNfNlQQAdd();
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
 * 函数： exceptqqglobalProfileChange
 * 功能： QQ白名单开关变化 生效函数
 * 创建： 2012-06-19
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void exceptqqglobalProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const SystemProfile *oldProfileDataPtr, const SystemProfile *newProfileDataPtr )
{
    switch(changeType) {
        case PROF_CHANGE_ADD:
            break;
        case PROF_CHANGE_EDIT:
            QQChainAOrD((SystemProfile *)oldProfileDataPtr, Iptables_DEL);
            QQChainAOrD((SystemProfile *)newProfileDataPtr, Iptables_ADD);
            /* 开关变化 */
            if ((oldProfileDataPtr->exceptQQEn != newProfileDataPtr->exceptQQEn) || 
                    (oldProfileDataPtr->exceptEnterpriseQQEn != newProfileDataPtr->exceptEnterpriseQQEn)) {
                reNewPdbChainJumpRule();
            }
            /*企业QQ: 开启->关闭*/
            if((oldProfileDataPtr->exceptEnterpriseQQEn  == FUN_ENABLE) && (newProfileDataPtr->exceptEnterpriseQQEn == FUN_DISABLE)){
                uttNfNlenterpriseQQDelAll();
            }
            /*企业QQ: 关闭->开启*/
            if((oldProfileDataPtr->exceptEnterpriseQQEn  == FUN_DISABLE) && (newProfileDataPtr->exceptEnterpriseQQEn == FUN_ENABLE)){
                uttNfNlenterpriseQQAdd();
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
 * 函数： exceptqqProfileChange
 * 功能： 增加或删除QQ白名单规则链到iptables规则
 * 创建： 2012-06-19
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void QQChainAOrD(SystemProfile *prof,char action)
{
    int qqSwitch = 0;
    if(prof != NULL)
    {
        qqSwitch = (prof->exceptQQEn << 0) | (prof->exceptEnterpriseQQEn << 1);
        SWORD_PRINTF("%s-%d, qqSwitch = %d\n",__func__, __LINE__, qqSwitch);
        if(qqSwitch != 0)
        {
            doSystem("iptables -t mangle -%c %s -j UTTEXCEPTQQ --config 0x%x", action, IM_EXCEPT_CHAIN, qqSwitch);
            SWORD_PRINTF("iptables -t mangle -%c %s -j UTTEXCEPTQQ --config 0x%x\n", action, IM_EXCEPT_CHAIN, qqSwitch);
        }
    }
    return;
}

/********************************************************************
 * 函数： exceptqqInit
 * 功能： 认证初始化函数 - 先清空现有规则链表，再重新添加
 * 创建： 2012-04-27
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void exceptqqInit(void)
{
    MibProfileType mibTypeSys = MIB_PROF_SYSTEM;
    SystemProfile *profSys = NULL;
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);

    profSys = (SystemProfile *)ProfGetInstPointByIndex(mibTypeSys, 0);
    if(profSys->exceptEnterpriseQQEn == FUN_ENABLE) 
    {
        uttNfNlenterpriseQQDelAll();
        uttNfNlenterpriseQQAdd();
    }
    QQChainAOrD(profSys, Iptables_ADD);
    uttNfNlQQDelAll();
    uttNfNlQQAdd();
    return;
}
/********************************************************************
 * 函数： exceptqqUpdatePortMap
 * 功能： 更新QQ白名单账号记录map
 * 创建： 2012-06-19
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
static void exceptqqUpdatePortMap(void) 
{
    MibProfileType mibType = MIB_PROF_EXCEPT_QQ;
    ExceptQQProfile *prof = NULL;
    int i = 0, min = 0, max = 0;
    Uint16 webInstIndex;

    ProfInstLowHigh(mibType, &max, &min);
    for (i = min; i < max; i++) 
    {
        prof = (ExceptQQProfile *)ProfGetInstPointByIndex(mibType, i);
        if ((ProfInstIsFree(prof) == 0) && (strncmp(prof->head.name, "ID", 2) == 0)) 
        {/*取出id后面的数字*/
            webInstIndex = (Uint16)strtol(&(prof->head.name[2]), NULL, 10);/*取配置中的id号*/
            if (webInstIndex != 0U) 
            {
                setPortMap(&(nvramProfile->exceptqqInstRec.instRecMap), webInstIndex);/*赋值给实例号*/
            }
        }
    }

    return;
}
/********************************************************************
 * 函数： funInitExceptQQ
 * 功能： 认证功能 初始化入口函数
 * 创建： 2012-04-27
 * 参数： 略
 * 返回： 略
 * 输出： 略
 * 作者： Jianqing.Sun 
 ********************************************************************/
void funInitExceptQQ(void)
{
    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    registerForProfileUpdates(MIB_PROF_SYSTEM, (Pointer)NULL, (ProfileUpdateFnc)exceptqqglobalProfileChange);/*触发函数*/
    registerForProfileUpdates(MIB_PROF_EXCEPT_QQ, (Pointer)NULL, (ProfileUpdateFnc)exceptqqProfileChange);/*触发函数*/

    SWORD_PRINTF("%s-%d\n",__func__, __LINE__);
    exceptqqInit();/*初始化*/
    /*初始化记录map */
    exceptqqUpdatePortMap();
    return;
}
#endif
#endif
