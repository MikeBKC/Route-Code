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
 * ��������д���µ�����
 * ÿ�����CLI_MAX_WRITE_BUF������
 */
extern int cliConfWrite(CliConfOp *cliOp, const char* _Format, ...)
{
    char cmdstr[CLI_MAX_WRITE_BUF]; 
    va_list   args; 

    cmdstr[CLI_MAX_WRITE_BUF - 1] = '\0';
    /*���������ַ���*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, CLI_MAX_WRITE_BUF-1, _Format,   args   ); 
    va_end(args);

    /*����indentFlag������������*/
    cliConfTreeAIndent(cliOp->strTree, cliOp->curNodeP);

    if(index(cmdstr, '\n'))
    {/*��������˻س�*/
	indentFlag = 0;/*���к� �µ�����Ż�������*/
    }

    /*����������ݵ�������*/
    cliConfTreeAStr(cliOp->strTree, cmdstr);
    return;
}
/*��relloac��strcat
 * strBufP: ָ��ԭ���ַ����Ŀռ�ָ�롣��ֵ���ܸı�
 * addStr �� Ҫ��ӵ��ִ�
 * curStrLenP�� ԭ���ִ��ĳ��ȡ���ΪNULL����ֱ��strlen����ԭ���ִ�����
 *            ��ΪNULL�򱾺��������� ���޸� ��ֵ
 *            ��strBufP �е��ִ�����ʱ��ֱ��ָ����ֵ���ܴ�����Ч
 
*  retrun :
*         < 0 ����ʧ��
*         >=0 ���пռ�ĳ��ȣ�Ϊ�����������з���ռ����С���ƣ�
 * */
extern int reallocStrCat(char **strBufP, char *addStr, int *curStrLenP)
{
    int oldStrLen , idleLen, addStrLen, newStrLen;
    char *q;
    if(curStrLenP != NULL)
    {/*ֱ��ȡԭ��ֵ*/
	oldStrLen = (*curStrLenP);
    }
    else
    {/*���㳤��*/
	oldStrLen = strlen(*strBufP);
    }
    addStrLen = strlen(addStr);

    /*��ͣ�ķ�����������Ӱ�� �������ǹ涨��С��������*/
    newStrLen = (addStrLen < 1024? 1024:addStrLen);
    newStrLen += oldStrLen;/*���ִ��ܳ���*/

    /*�ռ����� �� �ַ������ȼ� �ӽ�����*/
    q = (char*)realloc(*strBufP, newStrLen + 1);
    if (NULL != q) 
    {/*����ɹ�*/   
	*strBufP = q;/*����ע��relloac���ܵ����ڴ��ƶ�*/
	strcpy(q + oldStrLen, addStr);
	if(curStrLenP != NULL)
	{/*ԭ���ִ����ȸı�*/
	    *curStrLenP = addStrLen + oldStrLen;
	}
	/*����ûʹ�õĳ���*/
	idleLen = (addStrLen >= 1024? 0 : (1024-addStrLen));
    }
    return idleLen;
}
/**
 * ����cli���� ������ָ�����õ�ָ��
 * return:
 *       ��NULL cli�����ִ�ָ��
 *       NULL  ��������ʧ��
 *
 * ע��: ���ص�ָ��ָ����ڴ� ʹ���� ����cliConfFree ���ͷ�
 */
extern char* cliConfCreat(void)
{
    CliConfBuildAss curNode ;
    CliConfSTree retTree;
    memset(&curNode, 0, sizeof(curNode));
    memset(&retTree, 0, sizeof(retTree));
    /*��������ȷ����ռ�
     * ��������ĺ��� �Ϳ���ʡȥ�ܶ� ���*/

    retTree.strBuf = (char *)malloc(1024);
    if(retTree.strBuf != NULL)
    {/*����ɹ�*/
	retTree.idleLen = 1024;
	curNode.cliConfig = cliRootConfig;
        /*��������*/
	cliConfBuild(&curNode, &retTree);
    }
    /*����ָ��*/
    return retTree.strBuf;
}
/*�ͷ������ڴ�*/
extern void cliConfFree(char* buf)
{
    if(buf != NULL)
    {
	free(buf);
    }
    return;
}

/**
 * �������ִ�����
 * curNodeP : ��ǰ�����ڵ�ָ��
 * retTree  : �����
 *
 * return:
 *       1 �ɹ�
 *       0 ʧ��
 */
extern int cliConfBuild(CliConfBuildAss *curNodeP, CliConfSTree *retTree)
{
    int ret = 1;
    CliConfBuildAss subNode;
    while(curNodeP->cliConfig != NULL)
    {
        /*����������չ ������0 ��ʼ*/
	curNodeP->curSimBrother = 0;
        /*Ĭ�ϸ������ò�����չ*/
	curNodeP->nextSimBrother = -1;
	/*δ��ӹ��ָ���*/
	curNodeP->simSpliter = -1;

	/*������չ��ע��������չʱ��ÿһ����չ���������������*/
	while(curNodeP->curSimBrother != -1)
	{
            /*��ǰ�ڵ����� ��ӵ��������ִ�
	     * ���������޸�nextSimBrother��ֵ*/
	    ret &= cliConfTreeANode(retTree, curNodeP);
	    if(curNodeP->cliConfig->child != NULL)
	    {/*��������*/
		memset(&subNode, 0, sizeof(subNode));
		subNode.father = curNodeP;/*���ӵĸ��׾��ǵ�ǰ�ڵ�*/
		subNode.cliConfig = curNodeP->cliConfig->child;/*ָ������*/
		subNode.depth = curNodeP->depth;
		if((curNodeP->cliConfig->buildTag & CMD_SUB_DEP_NADD) == 0)
		{/*��û����ʾָ�����ӵ���Ȳ����ӡ����ӵ���Ƚ��ȵ�ǰ��1*/
		    subNode.depth += 1;
		}
                /*�����ӽڵ����ݵ�������*/
		ret &= cliConfBuild(&subNode, retTree);

                /*��ǰ�ڵ����ݵ���� �������� �ӽ�
		 * �����ӽڵ㹹����ɺ� ���޸���ǰ����������*/
		ret &= cliConfTreeRlback(curNodeP, retTree);
	    }
	    else
	    {
		ret &= cliCTSpliterRlback(retTree, curNodeP);
	    }
            /*������һ��������չ*/
	    curNodeP->curSimBrother = curNodeP->nextSimBrother;
	}
	if((curNodeP->cliConfig->buildTag & CMD_LAST_BROTHER) != 0)
	{/*���һ���ֵ�*/
	    curNodeP->cliConfig = NULL;
	}
	else
	{/*���������ֵ��账��*/
	    (curNodeP->cliConfig)++;
	}

    }
    return ret;
}
/**
 * ���������ִ������һ�� �ִ�
 *  return: 
 *        1  �ɹ� 
 *        0  ʧ��
 */
static int cliConfTreeAStr(CliConfSTree *strTree, char *addStr)
{
    int addLen = strlen(addStr);
    int curStrLen = strTree->strLen;
    int ret = 1;
    if(addLen <= strTree->idleLen)
    {/*���пռ乻��*/
	strcpy((strTree->strBuf + curStrLen), addStr);
	strTree->strLen += addLen;
	strTree->idleLen -= addLen;
    }
    else
    {/*���пռ䲻����*/
	/*���ÿ��пռ�*/
	strncpy((strTree->strBuf + curStrLen), addStr, strTree->idleLen);
	/*��ǰ���п������ꡣȫ������ִ�*/
	curStrLen += strTree->idleLen;
	/*����ע��strncpy���ܲ��ڽ�β�Ž�����*/
	(strTree->strBuf)[curStrLen] = '\0';
	/*����ռ�洢ʣ�µ��ִ�*/
	ret = reallocStrCat(&(strTree->strBuf), addStr + (strTree->idleLen), &curStrLen);
	if(ret < 0)
	{/*���ʧ��*/
              /*ʧ���豣֤������� ��Ӱ��
	       * ��������Ӳ����κζ���*/
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    ret = 0;
	}
	else
	{/*�ɹ�*/
            /*������Ӻ��״̬*/
	    strTree->idleLen = ret;
	    strTree->strLen = curStrLen;
	    ret = 1;
	}
    }
    return ret;
}
/**
 * �����������ɺ��� ������������
 * ����ӵ��������ִ�
 *
 * return:
 *       1  �ɹ�
 *       0  ʧ��
 */
static int cliConfTreeANByFun(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
#if 0
    int ret = 1;

    /* ��������������
     * ������������ָ��cmdstr �ұ��붯̬����*/
    curNodeP->cliConfig->cmdContGetFunP(curNodeP);
    if(curNodeP->cmdstr != NULL)
    {   
	/*����������ӵ�������*/
	ret &= cliConfTreeAStr(strTree, curNodeP->cmdstr);
	/*�ͷŵ�malloc������*/
	free(curNodeP->cmdstr);
    }
    return ret;
#endif
    CliConfOp cliOp;/*cli�����ṹ��*/
    cliOp.strTree = strTree;
    cliOp.curNodeP = curNodeP;
    /*�����������ݵ�������*/
    return curNodeP->cliConfig->cmdContGetFunP(&cliOp);
}
#ifdef CLI_CONF_RELY_PROF
/**
 * ��profile�е�ֵ ת��Ϊ ����
 * ����ֻ�и�Ĭ�����ò�ͬ��ֵ�Ż����ת��
 */
static void parseProfValToCmd(CliConfBuildAss *curNodeP, char *buf)
{
    CliConfig *curConfig = curNodeP->cliConfig;
    SlotNumber slot; 
    slot.slotNumber = SLOT_NUMBER_ANY; 
    Pointer profdef;
    /*��������*/
    BuildDefaultFactoryValue(curConfig->mibtype, curNodeP->curSimBrother, slot, &profdef);
    if((profdef != NULL) &&
	    memcmp((Byte*)(curNodeP->prof) + (curConfig->offset), (Byte*)profdef + (curConfig->offset), curConfig->size)) 
    {/*����Ĭ������*/
	if(curConfig->itype == TENUM)
	{/*ö��*/
	    profGetEnumToStr2(curNodeP->prof,
		    curConfig->offset,
		    curConfig->size,
		    buf,
		    (LcdParamEnumValue *)curConfig->enumP
		    );
	}
	else
	{/*�������͵�ת��*/
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
 * ͨ�����ڵ��mibtype ָ�����profile
 * ��ת�������������Ϊ���� ���д����������
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
   /*���Ҷ�Ӧ��profile*/
    curNodeP->prof = _getProfilePointer(slot,curConfig->mibtype, curNodeP->curSimBrother, &profSize, NULL, NULL);
    /*������������*/
    parseProfValToCmd(curNodeP, buf);
    /*��ӵ��������ִ�*/
    ret = cliConfTreeAStr(strTree, buf);
    /*��һ���������profile*/
    curNodeP->nextSimBrother = curNodeP->curSimBrother + 1;
    if(curNodeP->nextSimBrother >= instMax)
    {/*�Ѿ������������*/
	curNodeP->nextSimBrother = -1;
    }
    return ret;
}
/**
 * ���ϲ����游���ҵ���һ����Աprof��Ϊnull�Ľڵ�
 * Ȼ��Ѹ�prof��Ϣ���ƹ���
 * ���ݴ�profָ�� �͵�ǰ�ڵ�� offset  size��������������
 * ����ӵ������� �ִ�
 *
 * return: 
 *        1 �ɹ�
 *        0 ʧ��
 */
static int cliConfTreeANByFProf(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    CliConfBuildAss *tempNodeP = curNodeP;
    char buf[MAX_VALUE_LEN] = {0};
    int ret = 1;

    curNodeP->prof = NULL;

    /*���ϲ����游 prof��Աָ��*/
    while((curNodeP->prof == NULL) && (tempNodeP->father != NULL))
    {
	tempNodeP = tempNodeP->father;
	curNodeP->prof = tempNodeP->prof;
    }
    if(curNodeP->prof != NULL)
    {/*���ҵ���ΪNULL��*/
        /*��ǰ�ڵ����û�и�ֵmibtype����ʱֱ�Ӱ��ҵ��Ľڵ� ��mibtype������*/
	curNodeP->cliConfig->mibtype = tempNodeP->cliConfig->mibtype;
	/*������������*/
	parseProfValToCmd(curNodeP, buf);
	/*��ӵ��������ִ�*/
	ret = cliConfTreeAStr(strTree, buf);
    }

    return ret;

}
#endif
/**
 *  Ϊ��ǰ�������������depthΪ���پ���Ӽ���������
 *  return:
 *       1 �ɹ�
 *       0 ʧ��
 */
static int cliConfTreeAIndent(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    char indentStr[MAX_CLI_TREE_DEPTH + 1];
    int ret = 1;

    /*���������Ż�������
     * ��ֻ�����뻻�к� �Ż��ٴ��������*/
    if(0 == indentFlag)
    {
	if(curNodeP->depth > 0 )
	{/*depth������*/
	    memset(indentStr , ' ', curNodeP->depth);
	    indentStr[curNodeP->depth] = '\0';
	    /*���������������*/
	    ret =  cliConfTreeAStr(strTree, indentStr);
            /*����һ���������� �����������ǰ���ٴ�����*/
	    indentFlag = 1;
	}
    }
    return ret;
}
/**
 * ��ʱ����������
 * ��ɾ�����ڵ���ӵ�����
 */
static void cliCTIndentRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int curIndentLen = curNodeP->depth;/*Ŀǰ�����������*/
    if(curIndentLen > 0 )
    {/*���������*/
	strTree->strLen -= curIndentLen;
	strTree->idleLen += curIndentLen;
	(strTree->strBuf)[strTree->strLen] = '\0';
        /*���������������*/
	indentFlag = 0;
    }

    return ;
}
/**
 * ������������ ������ �������Ƿ���Ҫ��ʾ
 * �û�����������Ӻ󡣷�����������Ϊ�յ����֮��
 */
static void cliCTCmdNameRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int delLen = 0;
    CliConfig *curConfig = curNodeP->cliConfig;


    if(curConfig->cmdName != NULL )
    {/*�������������*/

	/*��ɾ�����ֺ���Ŀո�*/
	strTree->strLen -= 1;
	strTree->idleLen += 1;
	(strTree->strBuf)[strTree->strLen] = '\0';

	if(((curConfig->buildTag & CMD_DEPEND_SUB) == 0)  && /*�����ӽڵ���������һ������*/
		(curConfig->cmdContGetFunP != NULL) || (curConfig->itype != TNONE))/*���ӡ�������ݵ� �������ſ��ܲ�����*/
	{
	    /*�������Ƴ���*/
	    delLen = strlen(curConfig->cmdName);
	    strTree->strLen -= delLen;
	    strTree->idleLen += delLen;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	}
    }
    return ;
}
/**
 * �ָ������
 */
static int cliConfTreeASpliter(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int ret = 1;
    if( ((curNodeP->cliConfig->buildTag & CMD_LINE_SPLITER) != 0) 
	    || (((curNodeP->cliConfig->buildTag & CMD_SIMILAR_SPLITER) != 0)  && (curNodeP->simSpliter == -1))
      )
    {/*������ǰ��ӷָ�����*/
	ret &= cliConfTreeAStr(strTree, "!\n");
	curNodeP->simSpliter = curNodeP->curSimBrother;
    }
    return ret;
}
/********
 * �ָ����޸�
 */
static int cliCTSpliterRlback(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    /*�����Ϊ�ָ���*/
    if(strTree->strLen > 2)
    {
	if (strcmp(strTree->strBuf + strTree->strLen - 3, "!\n") == 0)
	{ 
	    /*�ִ����Ȼָ�*/
	    strTree->strLen -=  2;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    /*�����Ŀռ����ǲ����ͷš�����Ч�ʸ��ߵ�*/
	    strTree->idleLen += 2;
	    if(curNodeP->simSpliter == curNodeP->curSimBrother)
	    {/*���ڵ���ӵķָ�����ɾ����*/
		curNodeP->simSpliter = -1;
	    }
	}
    }
    return 1;
}
/**
 * ���������ִ�����ӽڵ�����
 * strTree : ������
 * curNodeP: ����ӵĽڵ�
 *
 * return :
 *          1   �ɹ�
 *          0   ʧ��
 */
static int cliConfTreeANode(CliConfSTree *strTree, CliConfBuildAss *curNodeP)
{
    int ret = 1, preIndentLen = 0, afIndentLen = 0, afNameLen = 0;
    CliConfig *curConfig = curNodeP->cliConfig;

    /*�����������ǰ �������ִ��ĳ���*/
    curNodeP->preSTreeStrLen = strTree->strLen;

    /*������ǰ��ӷָ�����*/
    ret &= cliConfTreeASpliter(strTree, curNodeP);
    
    /*�����������ǰ���������ִ�����*/
    preIndentLen = strTree->strLen;
    /*Ϊ�������������*/
    ret &= cliConfTreeAIndent(strTree, curNodeP);
    /*���������������������ִ�����*/
    afIndentLen = strTree->strLen;

    if(curConfig->cmdName != NULL)
    {/*�������������*/
	ret &= cliConfTreeAStr(strTree, curConfig->cmdName);
        /*���ƺ�һ���ո�*/
	ret &= cliConfTreeAStr(strTree, " ");
    }

    /*�����������֮��ĳ���*/
    afNameLen = strTree->strLen;

    if(curConfig->cmdContGetFunP != NULL)
    {/*���ݺ��� ��������������������*/
         ret &= cliConfTreeANByFun(strTree, curNodeP);
    }
#ifdef CLI_CONF_RELY_PROF
    else if((curConfig->buildTag & CMD_CPROF_BY_MIBTYPE) != 0)
    {/*����cliConfig�е�mibtype��λprofile ���Զ�����������ݵ�������
       �������� ���������ı� profileָ�����������������
       ��itype��ָ��ΪTNONE*/
	ret &= cliConfTreeANByProf(strTree, curNodeP);
    }
    else if((curConfig->buildTag & CMD_USE_FATHER_PROF) != 0)
    {/*�����游�ڵ��profileָ�롣���ϱ��ڵ��offset size��ת������
       ����ӵ�������*/
         ret &= cliConfTreeANByFProf(strTree, curNodeP);
    }
    else
    {}
#endif

    if(afNameLen == strTree->strLen)
    {/*�������֮��û������������ݡ�����������Ϊ��*/
	cliCTCmdNameRlback(strTree, curNodeP);
    }

    if(afIndentLen != strTree->strLen)
    {/*����������������������*/
	if((curConfig->buildTag & CMD_LINE_NEND) == 0)
	{/*û��ǿ��ָ����ǰ�������ݺ󲻻���*/
	    ret &= cliConfTreeAStr(strTree, "\n");
	    indentFlag = 0;
	}
    }
    else
    {/*������������������*/
	if(preIndentLen != afIndentLen)
	{/*���������ˡ�����ɾ��������*/
	    cliCTIndentRlback(strTree, curNodeP);
	}
    }

    /*��ӱ��ڵ����ݺ� ����������*/
    curNodeP->curSTreeStrLen = strTree->strLen;

    return ret;
}
/*��ǰ�ڵ����ݵ���� �������� �ӽ�
 * �����ӽڵ㹹����ɺ� ���޸���ǰ����������
 * �籾�ڵ�����ֻ������ӽڵ��һ��˵��
 * ��ô���ӽڵ�����Ϊ��ʱ ���ڵ��û�д��ڵı�Ҫ
 * */
static int  cliConfTreeRlback(CliConfBuildAss *curNodeP, CliConfSTree *strTree)
{
    if((curNodeP->cliConfig->buildTag & CMD_DEPEND_SUB) != 0)
    {/*��ǰ�������ʾ ����������*/
	if(strTree->strLen == curNodeP->curSTreeStrLen)
	{/*�ӽڵ�û���κ�������ӵ��������ִ�*/
            /*�ִ����Ȼָ�*/
	    strTree->strLen = curNodeP->preSTreeStrLen;
	    (strTree->strBuf)[strTree->strLen] = '\0';
	    /*�����Ŀռ����ǲ����ͷš�����Ч�ʸ��ߵ�*/
	    strTree->idleLen += (curNodeP->curSTreeStrLen - curNodeP->preSTreeStrLen);
	    if(curNodeP->simSpliter == curNodeP->curSimBrother)
	    {/*���ڵ���ӵķָ�����ɾ����*/
		curNodeP->simSpliter = -1;
	    }
	}
    }
    return 1;

}

