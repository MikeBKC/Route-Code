#include "uttMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include "typedef.h"
#include "mib.h"
#include "profacce.h"
#include "daughter.h"
#include "cliConvert.h"


static int cliCTSpliterRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP);
static int  cliConfTreeRlback(CliConfBuildAss *curNodeP, CliConfSTree *strTree);
static int cliConfTreeANode(CliConfSTree *strTree, CliConfBuildAss *curNodeP);
static int cliConfTreeAStr(CliConfSTree *strTree, char *addStr);
static int cliConfTreeAIndent(CliConfSTree *strTree, CliConfBuildAss *curNodeP);
static indentFlag;

extern CliConfig cliRootConfig[];
/**
 * 往配置中写入新的命令
 * 每次最多CLI_MAX_WRITE_BUF个长度
 */
extern int cliConfWrite(CliConfOp *cliOp, const char* _Format, ...)
{
    char cmdstr[CLI_MAX_WRITE_BUF]; 
    va_list   args; 

    cmdstr[CLI_MAX_WRITE_BUF - 1] = '\0';
    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, CLI_MAX_WRITE_BUF-1, _Format,   args   ); 
    va_end(args);

    /*根据indentFlag的情况添加缩进*/
    cliConfTreeAIndent(cliOp->strTree, cliOp->curNodeP);

    if(index(cmdstr, '\n'))
    {/*命令键入了回车*/
	indentFlag = 0;/*换行后 新的命令才会有缩进*/
    }

    /*添加命令内容到配置树*/
    cliConfTreeAStr(cliOp->strTree, cmdstr);
    return;
}
/*先relloac再strcat
 * strBufP: 指向原来字符串的空间指针。该值可能改变
 * addStr ： 要添加的字串
 * curStrLenP： 原来字串的长度。若为NULL。则直接strlen计算原来字串长度
 *            不为NULL则本函数结束后 会修改 其值
 *            当strBufP 中的字串过长时。直接指定其值可能带来高效
 
*  retrun :
*         < 0 分配失败
*         >=0 空闲空间的长度（为了性能我们有分配空间的最小限制）
 * */
extern int reallocStrCat(char **strBufP, char *addStr, int *curStrLenP)
{
    int oldStrLen , idleLen, addStrLen, newStrLen;
    char *q;
    if(curStrLenP != NULL)
    {/*直接取原有值*/
	oldStrLen = (*curStrLenP);
    }
    else
    {/*计算长度*/
	oldStrLen = strlen(*strBufP);
    }
    addStrLen = strlen(addStr);

    /*不停的分配会带来性能影响 所以我们规定最小分配粒度*/
    newStrLen = (addStrLen < 1024? 1024:addStrLen);
    newStrLen += oldStrLen;/*新字串总长度*/

    /*空间扩到 到 字符串长度加 接结束符*/
    q = (char*)realloc(*strBufP, newStrLen + 1);
    if (NULL != q) 
    {/*分配成功*/   
	*strBufP = q;/*这里注意relloac可能导致内存移动*/
	strcpy(q + oldStrLen, addStr);
	if(curStrLenP != NULL)
	{/*原有字串长度改变*/
	    *curStrLenP = addStrLen + oldStrLen;
	}
	/*空闲没使用的长度*/
	idleLen = (addStrLen >= 1024? 0 : (1024-addStrLen));
    }
    return idleLen;
}
/**
 * 创建cli配置 并返回指向配置的指针
 * return:
 *       非NULL cli配置字串指针
 *       NULL  配置生成失败
 *
 * 注意: 返回的指针指向的内存 使用完 必须cliConfFree 来释放
 */
extern char* cliConfCreat(void)
{
    CliConfBuildAss curNode ;
    CliConfSTree retTree;
    memset(&curNode, 0, sizeof(curNode));
    memset(&retTree, 0, sizeof(retTree));
    /*这里必须先分配点空间
     * 这样后面的函数 就可以省去很多 检查*/

    retTree.strBuf = (char *)malloc(1024);
    if(retTree.strBuf != NULL)
    {/*分配成功*/
	retTree.idleLen = 1024;
	curNode.cliConfig = cliRootConfig;
        /*构建配置*/
	cliConfBuild(&curNode, &retTree);
    }
    /*配置指针*/
    return retTree.strBuf;
}
/*释放配置内存*/
extern void cliConfFree(char* buf)
{
    if(buf != NULL)
    {
	free(buf);
    }
    return;
}

/**
 * 配置树字串生成
 * curNodeP : 当前辅助节点指针
 * retTree  : 结果树
 *
 * return:
 *       1 成功
 *       0 失败
 */
extern int cliConfBuild(CliConfBuildAss *curNodeP, CliConfSTree *retTree)
{
    int ret = 1;
    CliConfBuildAss subNode;
    while(curNodeP->cliConfig != NULL)
    {
        /*该条配置扩展 索引从0 开始*/
	curNodeP->curSimBrother = 0;
        /*默认该条配置不会扩展*/
	curNodeP->nextSimBrother = -1;
	/*未添加过分隔符*/
	curNodeP->simSpliter = -1;

	/*配置扩展。注意配置扩展时。每一个扩展的命令都将有子命令*/
	while(curNodeP->curSimBrother != -1)
	{
            /*当前节点内容 添加到配置树字串
	     * 以下语句会修改nextSimBrother的值*/
	    ret &= cliConfTreeANode(retTree, curNodeP);
	    if(curNodeP->cliConfig->child != NULL)
	    {/*有子配置*/
		memset(&subNode, 0, sizeof(subNode));
		subNode.father = curNodeP;/*孩子的父亲就是当前节点*/
		subNode.cliConfig = curNodeP->cliConfig->child;/*指向配置*/
		subNode.depth = curNodeP->depth;
		if((curNodeP->cliConfig->buildTag & CMD_SUB_DEP_NADD) == 0)
		{/*如没有显示指定孩子的深度不增加。则孩子的深度将比当前多1*/
		    subNode.depth += 1;
		}
                /*构建子节点内容到配置树*/
		ret &= cliConfBuild(&subNode, retTree);

                /*当前节点内容的添加 可能依靠 子节
		 * 所以子节点构建完成后 需修复当前配置树内容*/
		ret &= cliConfTreeRlback(curNodeP, retTree);
	    }
	    else
	    {
		ret &= cliCTSpliterRlback(retTree, curNodeP);
	    }
            /*处理下一个命令扩展*/
	    curNodeP->curSimBrother = curNodeP->nextSimBrother;
	}
	if((curNodeP->cliConfig->buildTag & CMD_LAST_BROTHER) != 0)
	{/*最后一个兄弟*/
	    curNodeP->cliConfig = NULL;
	}
	else
	{/*还有其他兄弟需处理*/
	    (curNodeP->cliConfig)++;
	}

    }
    return ret;
}
/**
 * 往配置树字串中添加一个 字串
 *  return: 
 *        1  成功 
 *        0  失败
 */
static int cliConfTreeAStr(CliConfSTree *strTree, char *addStr)
{
    int addLen = strlen(addStr);
    int curStrLen = strTree->strLen;
    int ret = 1;
    if(addLen <= strTree->idleLen)
    {/*空闲空间够用*/
	strcpy((strTree->strBuf + curStrLen), addStr);
	strTree->strLen += addLen;
	strTree->idleLen -= addLen;
    }
    else
    {/*空闲空间不够用*/
	/*先用空闲空间*/
	strncpy((strTree->strBuf + curStrLen), addStr, strTree->idleLen);
	/*当前空闲空闲用完。全部填充字串*/
	curStrLen += strTree->idleLen;
	/*这里注意strncpy可能不在结尾放结束符*/
	(strTree->strBuf)[curStrLen] = '\0';
	/*分配空间存储剩下的字串*/
	ret = reallocStrCat(&(strTree->strBuf), addStr + (strTree->idleLen), &curStrLen);
	if(ret < 0)
	{/*添加失败*/
              /*失败需保证本次添加 无影响
	       * 即本次添加不做任何动作*/
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    ret = 0;
	}
	else
	{/*成功*/
            /*更新添加后的状态*/
	    strTree->idleLen = ret;
	    strTree->strLen = curStrLen;
	    ret = 1;
	}
    }
    return ret;
}
/**
 * 根据命令生成函数 生成命令内容
 * 并添加到配置树字串
 *
 * return:
 *       1  成功
 *       0  失败
 */
static int cliConfTreeANByFun(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
#if 0
    int ret = 1;

    /* 先生成命令内容
     * 这里命令内容指向cmdstr 且必须动态分配*/
    curNodeP->cliConfig->cmdContGetFunP(curNodeP);
    if(curNodeP->cmdstr != NULL)
    {   
	/*生成内容添加到配置树*/
	ret &= cliConfTreeAStr(strTree, curNodeP->cmdstr);
	/*释放掉malloc的内容*/
	free(curNodeP->cmdstr);
    }
    return ret;
#endif
    CliConfOp cliOp;/*cli操作结构体*/
    cliOp.strTree = strTree;
    cliOp.curNodeP = curNodeP;
    /*生成命令内容到配置树*/
    return curNodeP->cliConfig->cmdContGetFunP(&cliOp);
}
#ifdef CLI_CONF_RELY_PROF
/**
 * 将profile中的值 转换为 命令
 * 这里只有跟默认配置不同的值才会进行转化
 */
static void parseProfValToCmd(CliConfBuildAss *curNodeP, char *buf)
{
    CliConfig *curConfig = curNodeP->cliConfig;
    SlotNumber slot; 
    slot.slotNumber = SLOT_NUMBER_ANY; 
    Pointer profdef;
    /*出厂配置*/
    BuildDefaultFactoryValue(curConfig->mibtype, curNodeP->curSimBrother, slot, &profdef);
    if((profdef != NULL) &&
	    memcmp((Byte*)(curNodeP->prof) + (curConfig->offset), (Byte*)profdef + (curConfig->offset), curConfig->size)) 
    {/*不是默认配置*/
	if(curConfig->itype == TENUM)
	{/*枚举*/
	    profGetEnumToStr2(curNodeP->prof,
		    curConfig->offset,
		    curConfig->size,
		    buf,
		    (LcdParamEnumValue *)curConfig->enumP
		    );
	}
	else
	{/*其他类型的转换*/
	    parseProfileValue2(curConfig->offset, 
		    curConfig->size,
		    curConfig->itype,
		    buf,
		    curNodeP->prof);
	}
    }
    return;
}
/**
 * 通过本节点的mibtype 指定相关profile
 * 并转换其中相关内容为命令 最后写到配置树中
 */
static int cliConfTreeANByProf(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    SlotNumber slot; 
    CliConfig *curConfig = curNodeP->cliConfig;
    Uint profSize;
    char buf[MAX_VALUE_LEN] = {0};
    int instMin, instMax;
    int ret = 1;

    slot.slotNumber = SLOT_NUMBER_ANY; 
    ProfInstLowHigh(curConfig->mibtype, &instMax, &instMin);
   /*查找对应的profile*/
    curNodeP->prof = _getProfilePointer(slot,curConfig->mibtype, curNodeP->curSimBrother, &profSize, NULL, NULL);
    /*生成命令内容*/
    parseProfValToCmd(curNodeP, buf);
    /*添加到配置树字串*/
    ret = cliConfTreeAStr(strTree, buf);
    /*下一个待处理的profile*/
    curNodeP->nextSimBrother = curNodeP->curSimBrother + 1;
    if(curNodeP->nextSimBrother >= instMax)
    {/*已经到达最大索引*/
	curNodeP->nextSimBrother = -1;
    }
    return ret;
}
/**
 * 向上查找祖父。找到第一个成员prof不为null的节点
 * 然后把该prof信息复制过来
 * 根据此prof指针 和当前节点的 offset  size等生成命令内容
 * 并添加到配置树 字串
 *
 * return: 
 *        1 成功
 *        0 失败
 */
static int cliConfTreeANByFProf(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    CliConfBuildAss *tempNodeP = curNodeP;
    char buf[MAX_VALUE_LEN] = {0};
    int ret = 1;

    curNodeP->prof = NULL;

    /*向上查找祖父 prof成员指针*/
    while((curNodeP->prof == NULL) && (tempNodeP->father != NULL))
    {
	tempNodeP = tempNodeP->father;
	curNodeP->prof = tempNodeP->prof;
    }
    if(curNodeP->prof != NULL)
    {/*若找到不为NULL的*/
        /*当前节点可能没有赋值mibtype。这时直接把找到的节点 的mibtype拿来用*/
	curNodeP->cliConfig->mibtype = tempNodeP->cliConfig->mibtype;
	/*生成命令内容*/
	parseProfValToCmd(curNodeP, buf);
	/*添加到配置树字串*/
	ret = cliConfTreeAStr(strTree, buf);
    }

    return ret;

}
#endif
/**
 *  为当前命令添加缩进（depth为多少就添加几个缩进）
 *  return:
 *       1 成功
 *       0 失败
 */
static int cliConfTreeAIndent(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    char indentStr[MAX_CLI_TREE_DEPTH + 1];
    int ret = 1;

    /*新起的命令才会有缩进
     * 即只有输入换行后 才会再次添加缩进*/
    if(0 == indentFlag)
    {
	if(curNodeP->depth > 0 )
	{/*depth个缩进*/
	    memset(indentStr , ' ', curNodeP->depth);
	    indentStr[curNodeP->depth] = '\0';
	    /*添加缩进到配置树*/
	    ret =  cliConfTreeAStr(strTree, indentStr);
            /*打了一个缩进则在 本行命令结束前不再打缩进*/
	    indentFlag = 1;
	}
    }
    return ret;
}
/**
 * 有时候多加了缩进
 * 需删除本节点添加的缩进
 */
static void cliCTIndentRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int curIndentLen = curNodeP->depth;/*目前缩进等于深度*/
    if(curIndentLen > 0 )
    {/*有添加缩进*/
	strTree->strLen -= curIndentLen;
	strTree->idleLen += curIndentLen;
	(strTree->strBuf)[strTree->strLen] = '\0';
        /*缩进可以再添加了*/
	indentFlag = 0;
    }

    return ;
}
/**
 * 根据命令内容 来决定 命令名是否需要显示
 * 用户命令内容添加后。发现命令内容为空的情况之后
 */
static void cliCTCmdNameRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int delLen = 0;
    CliConfig *curConfig = curNodeP->cliConfig;


    if(curConfig->cmdName != NULL )
    {/*有添加命令名称*/

	/*先删除名字后面的空格*/
	strTree->strLen -= 1;
	strTree->idleLen += 1;
	(strTree->strBuf)[strTree->strLen] = '\0';

	if(((curConfig->buildTag & CMD_DEPEND_SUB) == 0)  && /*依靠子节点则命令名一定存在*/
		(curConfig->cmdContGetFunP != NULL) || (curConfig->itype != TNONE))/*需打印命令内容的 命令名才可能不存在*/
	{
	    /*命令名称长度*/
	    delLen = strlen(curConfig->cmdName);
	    strTree->strLen -= delLen;
	    strTree->idleLen += delLen;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	}
    }
    return ;
}
/**
 * 分隔符添加
 */
static int cliConfTreeASpliter(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int ret = 1;
    if( ((curNodeP->cliConfig->buildTag & CMD_LINE_SPLITER) != 0) 
	    || (((curNodeP->cliConfig->buildTag & CMD_SIMILAR_SPLITER) != 0)  && (curNodeP->simSpliter == -1))
      )
    {/*本命令前添加分隔符号*/
	ret &= cliConfTreeAStr(strTree, "!\n");
	curNodeP->simSpliter = curNodeP->curSimBrother;
    }
    return ret;
}
/********
 * 分隔符修复
 */
static int cliCTSpliterRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    /*若最后为分隔符*/
    if(strTree->strLen > 2)
    {
	if (strcmp(strTree->strBuf + strTree->strLen - 3, "!\n") == 0)
	{ 
	    /*字串长度恢复*/
	    strTree->strLen -=  2;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    /*多分配的空间我们不会释放。这样效率更高点*/
	    strTree->idleLen += 2;
	    if(curNodeP->simSpliter == curNodeP->curSimBrother)
	    {/*本节点添加的分隔符被删掉了*/
		curNodeP->simSpliter = -1;
	    }
	}
    }
    return 1;
}
/**
 * 往配置树字串中添加节点内容
 * strTree : 配置树
 * curNodeP: 欲添加的节点
 *
 * return :
 *          1   成功
 *          0   失败
 */
static int cliConfTreeANode(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int ret = 1, preIndentLen = 0, afIndentLen = 0, afNameLen = 0;
    CliConfig *curConfig = curNodeP->cliConfig;

    /*备份添加内容前 配置树字串的长度*/
    curNodeP->preSTreeStrLen = strTree->strLen;

    /*本命令前添加分隔符号*/
    ret &= cliConfTreeASpliter(strTree, curNodeP);
    
    /*备份添加缩进前的配置树字串长度*/
    preIndentLen = strTree->strLen;
    /*为本命令添加缩进*/
    ret &= cliConfTreeAIndent(strTree, curNodeP);
    /*备份添加缩进后的配置树字串长度*/
    afIndentLen = strTree->strLen;

    if(curConfig->cmdName != NULL)
    {/*需添加命令名称*/
	ret &= cliConfTreeAStr(strTree, curConfig->cmdName);
        /*名称后一个空格*/
	ret &= cliConfTreeAStr(strTree, " ");
    }

    /*添加命令名称之后的长度*/
    afNameLen = strTree->strLen;

    if(curConfig->cmdContGetFunP != NULL)
    {/*根据函数 的运行来生成命令内容*/
         ret &= cliConfTreeANByFun(strTree, curNodeP);
    }
#ifdef CLI_CONF_RELY_PROF
    else if((curConfig->buildTag & CMD_CPROF_BY_MIBTYPE) != 0)
    {/*根据cliConfig中的mibtype定位profile 并自动添加命令内容到配置树
       若本函数 仅仅用来改变 profile指针而不生成命令内容
       则itype需指定为TNONE*/
	ret &= cliConfTreeANByProf(strTree, curNodeP);
    }
    else if((curConfig->buildTag & CMD_USE_FATHER_PROF) != 0)
    {/*跟据祖父节点的profile指针。加上本节点的offset size等转换命令
       并添加到配置树*/
         ret &= cliConfTreeANByFProf(strTree, curNodeP);
    }
    else
    {}
#endif

    if(afNameLen == strTree->strLen)
    {/*添加名称之后没再添加其他内容。即命令内容为空*/
	cliCTCmdNameRlback(strTree, curNodeP);
    }

    if(afIndentLen != strTree->strLen)
    {/*除了缩进还加了其他内容*/
	if((curConfig->buildTag & CMD_LINE_NEND) == 0)
	{/*没有强制指定当前生成内容后不换行*/
	    ret &= cliConfTreeAStr(strTree, "\n");
	    indentFlag = 0;
	}
    }
    else
    {/*该命令仅仅添加了缩进*/
	if(preIndentLen != afIndentLen)
	{/*若真的添加了。则需删除此缩进*/
	    cliCTIndentRlback(strTree, curNodeP);
	}
    }

    /*添加本节点内容后 配置树长度*/
    curNodeP->curSTreeStrLen = strTree->strLen;

    return ret;
}
/*当前节点内容的添加 可能依靠 子节
 * 所以子节点构建完成后 需修复当前配置树内容
 * 如本节点内容只是针对子节点的一个说明
 * 那么当子节点内容为空时 本节点就没有存在的必要
 * */
static int  cliConfTreeRlback(CliConfBuildAss *curNodeP, CliConfSTree *strTree)
{
    if((curNodeP->cliConfig->buildTag & CMD_DEPEND_SUB) != 0)
    {/*当前命令的显示 依靠子命令*/
	if(strTree->strLen == curNodeP->curSTreeStrLen)
	{/*子节点没有任何内容添加到配置树字串*/
            /*字串长度恢复*/
	    strTree->strLen = curNodeP->preSTreeStrLen;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    /*多分配的空间我们不会释放。这样效率更高点*/
	    strTree->idleLen += (curNodeP->curSTreeStrLen - curNodeP->preSTreeStrLen);
	    if(curNodeP->simSpliter == curNodeP->curSimBrother)
	    {/*本节点添加的分隔符被删掉了*/
		curNodeP->simSpliter = -1;
	    }
	}
    }
    return 1;

}

