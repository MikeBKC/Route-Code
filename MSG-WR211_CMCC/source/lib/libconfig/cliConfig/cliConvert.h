#ifndef _CLI_CONVERT_H
#define _CLI_CONVERT_H

#include <typedef.h>
#define CLI_MAX_WRITE_BUF 1024
#define CLI_CONF_RELY_PROF 1
/*最大配置树深度
 * 这里仅仅提供一个最大值
 * 以防止每次都malloc 而影响性能*/
#define MAX_CLI_TREE_DEPTH 31
#define CLICONFEND {NULL, CMD_LAST_BROTHER, NULL, NULL}
typedef enum
{
    /*普通命令不做任何特殊处理*/
    CMD_LINE_NORMAL = 0x0,

    /*若子命令内容为空。则此命令将不显示*/
    CMD_DEPEND_SUB = 0x1,
    /*在此命令行前打印分隔符（默认！）*/
    CMD_LINE_SPLITER = 0x2,
    /*对于同类型的此命令只添加一个分隔符*/
    CMD_SIMILAR_SPLITER = 0x4,
    /*此命令没有兄弟节点了.每一层配置中的最后一条配置必需加此标志*/
    CMD_LAST_BROTHER = 0x8,
    /*子节点深度不增加*/
    CMD_SUB_DEP_NADD = 0x10,
   
    /*该条命名还没终结。即命令内容生成后不自动添加回车*/
    CMD_LINE_NEND = 0x20,

#ifdef CLI_CONF_RELY_PROF
    /*此命令将向上查找其祖父的prof指针
     * 若找到不为null的， 则根据该指针以及配置转换中指定的
     * offset size等 自动转换为 命令行内容*/
    CMD_USE_FATHER_PROF = 0x40,
    /*根据mibtype自动生成profile
     * 该节点将 根据profile的实例数 扩展为多个兄弟*/
    CMD_CPROF_BY_MIBTYPE = 0x80
#endif
    
}CMD_BUILD_TAG;

struct st_cliConfBuildAss;
struct st_cliConfOp;

/*配置树节点结构*/
typedef struct st_cliConfig{
    /*名称不为null 将会在名称后面添加空格。为null则不加*/
    char *cmdName;/*命令名称（可以为空）*/

    unsigned int buildTag;/*标志*/

    /* 命令打印函数指针（先打印命令名称，后该函数在命令名称后面打印）
     * 并通过改变curNodeP->nextSimBrother的值来决定是否该节点需扩展为多个兄弟
     * 当本函数依赖祖父节点时 还可以通过 curNodeP->father 来逆向查找其所有祖父 */
    int (*cmdContGetFunP)(struct st_cliConfOp *cliOp);

    struct st_cliConfig* child;/*孩子节点指针*/

#ifdef CLI_CONF_RELY_PROF
    /*以下完全同st_xmlConfig*/
    IType           itype;
    Uint32          offset;
    Uint32          size;
    Pointer         enumP;
    MibProfileType  mibtype;
#endif
}CliConfig;

/*配置树生成辅助结构体*/
typedef struct st_cliConfBuildAss
{
    /*这里的类似兄弟。指的是 类似的操作集合
     * 如配置接口的命令 不同的接口配置命令 作为一个类似兄弟集合*/
    int curSimBrother;/*该节点类似命令兄弟 0起始。当直接操作profile时。做profile index使用*/
    int nextSimBrother;/*下一个类似兄弟。-1结束*/
#if 0
    char *cmdstr; /*当前节点转换的命令内容字串。必须malloc分配*/
#endif

    int depth;/*该节点在配置中的深度.深度为多少，则在左端产生几个缩进*/
    struct st_cliConfBuildAss *father;/*父亲。用于生成树build时的逆向查找*/
    CliConfig *cliConfig;/*配置结构体指针*/

#ifdef CLI_CONF_RELY_PROF
    Pointer prof;/*profile指针*/
#endif

    /*添加本节点内容前 配置树字串的长度*/
    int preSTreeStrLen;
    /*添加本节点后 配置树字串的长度*/
    int curSTreeStrLen;
    /*哪个同类型命令添加了分隔符*/
    int simSpliter ;
}CliConfBuildAss;

/*配置字串树*/
typedef struct st_cliConfSTree{
    char *strBuf;/*结果字串指针*/
    int strLen;/*结果字串长度*/
    int idleLen;/*空闲内存长度*/
}CliConfSTree;

/*配置操作结构体*/
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
