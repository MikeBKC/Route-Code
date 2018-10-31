#ifndef _CLI_CONVERT_H
#define _CLI_CONVERT_H

#include <typedef.h>
#define CLI_MAX_WRITE_BUF 1024
#define CLI_CONF_RELY_PROF 1
/*������������
 * ��������ṩһ�����ֵ
 * �Է�ֹÿ�ζ�malloc ��Ӱ������*/
#define MAX_CLI_TREE_DEPTH 31
#define CLICONFEND {NULL, CMD_LAST_BROTHER, NULL, NULL}
typedef enum
{
    /*��ͨ������κ����⴦��*/
    CMD_LINE_NORMAL = 0x0,

    /*������������Ϊ�ա�����������ʾ*/
    CMD_DEPEND_SUB = 0x1,
    /*�ڴ�������ǰ��ӡ�ָ�����Ĭ�ϣ���*/
    CMD_LINE_SPLITER = 0x2,
    /*����ͬ���͵Ĵ�����ֻ���һ���ָ���*/
    CMD_SIMILAR_SPLITER = 0x4,
    /*������û���ֵܽڵ���.ÿһ�������е����һ�����ñ���Ӵ˱�־*/
    CMD_LAST_BROTHER = 0x8,
    /*�ӽڵ���Ȳ�����*/
    CMD_SUB_DEP_NADD = 0x10,
   
    /*����������û�սᡣ�������������ɺ��Զ���ӻس�*/
    CMD_LINE_NEND = 0x20,

#ifdef CLI_CONF_RELY_PROF
    /*��������ϲ������游��profָ��
     * ���ҵ���Ϊnull�ģ� ����ݸ�ָ���Լ�����ת����ָ����
     * offset size�� �Զ�ת��Ϊ ����������*/
    CMD_USE_FATHER_PROF = 0x40,
    /*����mibtype�Զ�����profile
     * �ýڵ㽫 ����profile��ʵ���� ��չΪ����ֵ�*/
    CMD_CPROF_BY_MIBTYPE = 0x80
#endif
    
}CMD_BUILD_TAG;

struct st_cliConfBuildAss;
struct st_cliConfOp;

/*�������ڵ�ṹ*/
typedef struct st_cliConfig{
    /*���Ʋ�Ϊnull ���������ƺ�����ӿո�Ϊnull�򲻼�*/
    char *cmdName;/*�������ƣ�����Ϊ�գ�*/

    unsigned int buildTag;/*��־*/

    /* �����ӡ����ָ�루�ȴ�ӡ�������ƣ���ú������������ƺ����ӡ��
     * ��ͨ���ı�curNodeP->nextSimBrother��ֵ�������Ƿ�ýڵ�����չΪ����ֵ�
     * �������������游�ڵ�ʱ ������ͨ�� curNodeP->father ����������������游 */
    int (*cmdContGetFunP)(struct st_cliConfOp *cliOp);

    struct st_cliConfig* child;/*���ӽڵ�ָ��*/

#ifdef CLI_CONF_RELY_PROF
    /*������ȫͬst_xmlConfig*/
    IType           itype;
    Uint32          offset;
    Uint32          size;
    Pointer         enumP;
    MibProfileType  mibtype;
#endif
}CliConfig;

/*���������ɸ����ṹ��*/
typedef struct st_cliConfBuildAss
{
    /*����������ֵܡ�ָ���� ���ƵĲ�������
     * �����ýӿڵ����� ��ͬ�Ľӿ��������� ��Ϊһ�������ֵܼ���*/
    int curSimBrother;/*�ýڵ����������ֵ� 0��ʼ����ֱ�Ӳ���profileʱ����profile indexʹ��*/
    int nextSimBrother;/*��һ�������ֵܡ�-1����*/
#if 0
    char *cmdstr; /*��ǰ�ڵ�ת�������������ִ�������malloc����*/
#endif

    int depth;/*�ýڵ��������е����.���Ϊ���٣�������˲�����������*/
    struct st_cliConfBuildAss *father;/*���ס�����������buildʱ���������*/
    CliConfig *cliConfig;/*���ýṹ��ָ��*/

#ifdef CLI_CONF_RELY_PROF
    Pointer prof;/*profileָ��*/
#endif

    /*��ӱ��ڵ�����ǰ �������ִ��ĳ���*/
    int preSTreeStrLen;
    /*��ӱ��ڵ�� �������ִ��ĳ���*/
    int curSTreeStrLen;
    /*�ĸ�ͬ������������˷ָ���*/
    int simSpliter ;
}CliConfBuildAss;

/*�����ִ���*/
typedef struct st_cliConfSTree{
    char *strBuf;/*����ִ�ָ��*/
    int strLen;/*����ִ�����*/
    int idleLen;/*�����ڴ泤��*/
}CliConfSTree;

/*���ò����ṹ��*/
typedef struct st_cliConfOp{
   CliConfBuildAss *curNodeP;

   CliConfSTree *strTree;
}CliConfOp;

extern int reallocStrCat(char **strBufP, char *addStr, int *curStrLenP);
extern int cliConfBuild(CliConfBuildAss *curNodeP, CliConfSTree *retTree);
extern int cliConfWrite(CliConfOp *cliOp, const char* _Format, ...);
extern char* cliConfCreat(void);
extern void cliConfFree(char* buf);
#endif
