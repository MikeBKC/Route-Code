/**
 * ipset命令接口文件
 * 作用：ipset版本升级时更好的兼容新的ipset命令
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "ipsetApi.h"
#include <stdarg.h>

#if CONFIG_IP_NF_SET
#define IP_SET_DEBUG 0

/* 某些操作需要高效的 ipset操作（http包触发的通告等 程序级自动针对组的操作）
 * 如 用socket 直接和内核通信 代替 用system函数调用
 * */
#define IPSET_OP_EFF  1/*efficient*/

#if IPSET_OP_EFF
#include <include/linux/netfilter_ipv4/ip_set.h>
#endif

#if IP_SET_DEBUG
#define IPSET_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define IPSET_DEBUG(fmt, argx...)
#endif

static void doSystem(const char* _Format, ...);
/**
 * ipset命令运行
 * 注意：该函数应尽应用于ipset。若有时间可以把 goahead以及speedweb中的doSystem移到库中
 * bhou
 */
static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*应该命令主要针对ipset故不会太大*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*生成命令字符串*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    IPSET_DEBUG(cmdstr);
    /*运行命令*/
    system(cmdstr);
    return;
}
#if IPSET_OP_EFF
/**
 * 向ipset的内核发包 做操作
 * isGet:1 调用内核注册的setsockopt
 *       0 调用内核注册的getsockopt
 */
static int ipset_skop(void *data, socklen_t* size, int isGet)
{
    int res = -1;
    int sockfd = -1;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd != -1)
    {
	if(isGet > 0)
	{
	    /* Send! */
	    res = getsockopt(sockfd, SOL_IP, SO_IP_SET, data, size);
	}
	else
	{
	    /*get*/
	    res = setsockopt(sockfd, SOL_IP, SO_IP_SET, data, *size);
	}
	close(sockfd);
#if 0
/*we biuild ipset in kernel*/

/*if build ipset ad a kernel module*/
	if (res != 0 
		&& errno == ENOPROTOOPT 
		&& ipset_insmod("ip_set", "/sbin/modprobe") == 0)
	    res = setsockopt(sockfd, SOL_IP, SO_IP_SET, data, size);
	DP("res=%d errno=%d", res, errno);
#endif
    }

    return res;
}
#define ALIGNED(len)	IPSET_VALIGN(len, 0)
/**
 * ipset ipmap 和 iphash 类型set的操作
 * 支持 add del 和 test 一个ip地址
 *
 * test 同时支持ipteemap 因为该类型其实 test时 只能测试首个ip
 *
 * return :
 *        0 成功
 *        -1 失败
 */
static int ipsetSkAdtIp(char *groupName, unsigned op, unsigned long ipaddr )
{
    struct ip_set_req_adt_get req_adt_get;
    struct ip_set_req_adt *req_adt;
   /*因为 ip_set_req_ipmap 与 ip_set_req_iphash 其实结构完全一样。本函数适用iphash 和 ipmap的单个ip*/
    size_t size;
    char   data[(ALIGNED(sizeof(struct ip_set_req_adt)) + (2*sizeof(unsigned long)))];
    int res = 0;

    req_adt_get.op = IP_SET_OP_ADT_GET;
    req_adt_get.version = IP_SET_PROTOCOL_VERSION;
    strcpy(req_adt_get.set.name, groupName);
    size = sizeof(struct ip_set_req_adt_get);

    ipset_skop((void *) &req_adt_get, &size, 1);

#if 0
    /* Alloc memory for the data to send */
    size = ALIGNED(sizeof(struct ip_set_req_adt)) + set->settype->adt_size ;
    data = malloc(size);
#endif

    size = sizeof(data);

    ipaddr = ntohl(ipaddr);
    if(strcmp(req_adt_get.typename,"iptreemap") != 0)
    {/*集合类型为 iphash 和 ipmap
    因为ipset采用类型注册的方式完成很多操作。所以不同的头文件包含相同的宏名。我们无法直接用这个头文件
    只能直接算大小*/
	size -= sizeof(unsigned long);
    }
    else
    {/*结束ip地址。若不为一个ip段。必须跟起始一致*/
	*((unsigned long *) (data + ALIGNED(sizeof(struct ip_set_req_adt)) + sizeof(unsigned long))) 
	    = ipaddr;
    }

    /* Fill out the request */
    req_adt = (struct ip_set_req_adt *) data;
    req_adt->op = op;
    req_adt->index =  req_adt_get.set.index;
#if 0
    memcpy(data + ALIGNED(sizeof(struct ip_set_req_adt)),
	    set->settype->data, set->settype->adt_size);
#endif
    /*若要支持其他类型的操作。需修改data中的此填充段*/
    /*ip set use host byte order*/
    *((unsigned long *) (data + ALIGNED(sizeof(struct ip_set_req_adt)))) = ipaddr;

    res = ipset_skop(data, &size, 0);
    return res;
}

/**
 * 直接和内核通信 而不用system以提高效率
 * 删除 或flush一个ipset组 该组可以是任何类型
 * op: IP_SET_OP_DESTROY or IP_SET_OP_FLUSH
 * 
 * return 
 *       -1 失败
 *       0   成功
 */
static int ipsetSkSetDestroy(char *groupName, unsigned op)
{
    struct ip_set_req_std req;
    int size = sizeof(struct ip_set_req_std);

    req.op = op;
    req.version = IP_SET_PROTOCOL_VERSION;
    strcpy(req.name, groupName);

    return ipset_skop(&req, &size, 0);
}

#endif
/**
 * 判断添加的组名是否跟系统 默认添加的组名相同
 * return:  1 相同 0不同
 */
int ipsetGrpNameIsSys(char *groupName)
{
    int ret = 0;
    if((strncmp(groupName, IPSET_SYS_GNAME_PRE, strlen(IPSET_SYS_GNAME_PRE)) == 0)
	    ||(strncmp(groupName, IPSET_TSYS_GNAME_PRE, strlen(IPSET_TSYS_GNAME_PRE)) == 0))
    {
	ret = 1;/*跟系统默认组名前缀相同*/
    }

    return ret;
}

#if (NOTICE == FYES)
/**
 * 通告ok组成员添加，用来记录已经通告过的用户ip
 * return : 0 成功 -1 失败
 */
int noticeOkGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
/**
 * 通告web组添加。用来记录需要通告的用户ip
 * return : 0 成功 -1 失败
 */
int noticeWebGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}

/**
 * 通告http redirect 记录组添加。用来记录发出GET / HTTP/1.1的http请求
 * return : 0 成功 -1 失败
 */
int noticeRGrpAdd(char* groupName, char *ipFrom, char *ipTo)
{
    doSystem("ipset -N %s ipportiphash --from %s --to %s", groupName, ipFrom, ipTo);
    return 0;
}
/**
 * 通告http redirect 记录组编辑
 * 用于修改记录组的范围。这样做的目的是为了使引用该组的规则不变
 * * return : 0 成功 -1 失败
 */
int noticeRGrpEdit(char* groupName, char *ipFrom, char *ipTo)
{
    char tmpName[32];
    int preLen = strlen(IPSET_TSYS_GNAME_PRE);
    
    /*生成临时组名*/
    memset(tmpName, 0, sizeof(tmpName));
    strncpy(tmpName, IPSET_TSYS_GNAME_PRE, sizeof(tmpName) - 1);
    strncpy(&tmpName[preLen], groupName, sizeof(tmpName) - preLen - 1);
    
    /*建一个同类型临时组*/
    doSystem("ipset -N %s ipportiphash --from %s --to %s", tmpName, ipFrom, ipTo);
    /*和现有组交换名称。*/
    doSystem("ipset -W %s %s", tmpName, groupName);
    /*删掉被交换过来的临时组*/
    doSystem("ipset -X %s", tmpName);

    return 0;
}
#endif
/**
 * 此函数添加的组。用于动态替换WEB认证和通告中response的sport匹配条件
 * return : 0 成功 -1 失败
 */
int httpPortGrpAdd(char* groupName)
{
    doSystem("ipset -N %s portmap --from 1 --to 65535", groupName);
    return 0;
}
/**
 * 此函数添加的组。用于动态替换WEB认证和通告中response的sport匹配条件
 * return : 0 成功 -1 失败
 */
int httpPortAdd(char* groupName,Uint32 port)
{
    doSystem("ipset -q -A %s  %u",groupName, port);
    return 0;
}
#if (PPPOE_ACCOUNT == FYES)
/**
 * 此函数添加的组。只能用来存储 pppoe账号， 即此函数可以根据pppoe账号分配进行优化
 * return : 0 成功 -1 失败
 */
int poeAccGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
#endif
#if (WEB_AUTH == FYES)
/**
 * 认证ok组成员添加，用来记录已经认证过的用户ip
 * return : 0 成功 -1 失败
 */
int webauthOkGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iptreemap", groupName);
    return 0;
}
/**
 * 认证web组添加。用来记录需要认证的用户ip
 * return : 0 成功 -1 失败
 */
int webauthWebGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
#endif
#if (IP_GRP == FYES)
/**
 * ip地址组添加
 * 该组可添加单个ip，范围ip以及网段
 *
 * return : 0 成功 -1 失败
 */
int ipGrpIpAddSet(char* groupName)
{
    doSystem("ipset -q -N %s iptreemap", groupName);
    return 0;
}
/**
 * 用户组添加
 * return : 0 成功 -1 失败
 */
int ipGrpUserAddSet(char* groupName)
{
    doSystem("ipset -q -N %s iphash", groupName);
    return 0;
}
#endif
#if 0


/**
 * 当pppoe地址池范围变化时
 * 更新用户组
 * 该操作会清空用户组的内容
 */
int userGrpEdit(char *groupName, char* ipFrom, char* ipTo)
{
    char *tmpName = "tmpGroup";/*该组名必须是 用户建组不能使用的组名*/
    doSystem("ipset -N %s ipmap --from %s --to %s", tmpName, ipFrom, ipTo);
    doSystem("ipset -W %s %s", tmpName, groupName);
    doSystem("ipset -X %s", tmpName);
}



#endif

/**
 * 清空ipset组
 * return : 0 成功 -1 失败
 */
int ipsetFlushGrp(char* groupName)
{
#if IPSET_OP_EFF
    return ipsetSkSetDestroy(groupName, IP_SET_OP_FLUSH);
#else
    doSystem("ipset -q -F %s", groupName);
    return 0;
#endif
}

/**
 * 删除ipset组。注意此组必须是独立未被 其他ipset组 和 iptables规则引用的才可以删除
 * return : 0 成功 -1 失败
 */
int ipsetDelGrp(char* groupName, bool notWarn)
{
    if(notWarn == TRUE)
    {/*不产生错误警告信息*/
	doSystem("ipset -q -X %s", groupName);
    }
    else
    {/*产生错误警告信息*/
	doSystem("ipset -X %s", groupName);
    }

    return 0;
}
/**
 * 往ip组里添加 或删除 ip
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 成功 -1 失败
 */
int ipsetAOrDIp(char* groupName, IPADDR ip, char addOrDel)
{
#if IPSET_OP_EFF
    return ipsetSkAdtIp(groupName,
	    ((addOrDel ==IPSET_MEM_ADD) ? IP_SET_OP_ADD_IP: IP_SET_OP_DEL_IP ), 
	    ip) ;
#else
    int ret = -1;
    struct in_addr addr;
    addr.s_addr = ip;
    ret = ipsetAOrDIpStr(groupName,inet_ntoa(addr), addOrDel);
    return ret;
#endif
}
/**
 * 往ip组里添加 或删除 ip
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 成功 -1 失败
 */
int ipsetAOrDIpStr(char* groupName, char* ipStr, char addOrDel)
{
#if IPSET_OP_EFF
    struct in_addr addr;
    int ret = -1;
    if (inet_aton(ipStr, &addr) != 0) {
	ret = ipsetSkAdtIp(groupName,
		((addOrDel ==IPSET_MEM_ADD) ? IP_SET_OP_ADD_IP: IP_SET_OP_DEL_IP ), 
		addr.s_addr) ;
    }
    return ret;
#else
    doSystem("ipset -q -%c %s %s", addOrDel, groupName, ipStr);
    return 0;
#endif
}
/**
 * 往ip组里添加或删除ip范围
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 成功 -1 失败
 */
int ipsetAOrDIpRange(char* groupName, IPADDR ipFrom, IPADDR ipTo, char addOrDel)
{
    char ipFromStr[32];
    char ipToStr[32];
    struct in_addr addrS;
    int ret = -1;

    memset(ipFromStr, 0, sizeof(ipFromStr));
    memset(ipToStr, 0, sizeof(ipFromStr));

    /*转化起始ip为字串*/
    addrS.s_addr = ipFrom;
    strcpy(ipFromStr, inet_ntoa(addrS));

    /*转化结束ip为字串*/
    addrS.s_addr = ipTo;
    strcpy(ipToStr, inet_ntoa(addrS));

    ret = ipsetAOrDIpStrRange(groupName,ipFromStr, ipToStr ,addOrDel);
    return ret;
}
/**
 * 往ip组里添加或删除ip范围
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 成功 -1 失败
 */
int ipsetAOrDIpStrRange(char* groupName, char* ipFromStr, char* ipToStr, char addOrDel)
{
    doSystem("ipset -q -%c %s %s-%s", addOrDel, groupName, ipFromStr, ipToStr);
    return 0;
}
#if 1
/**
 * 往ip组里添加或删除ip网段
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 成功 -1 失败
 */
int ipsetAOrDNet(char* groupName, IPADDR ip, Uint32 cidr, char addOrDel)
{
    int ret = -1;
    struct in_addr addrS;
    addrS.s_addr = ip;
    ret = ipsetAOrDNetStr(groupName, inet_ntoa(addrS), cidr, addOrDel);
    return ret;
}
/**
  * 往ip组里添加或删除ip网段
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 return : 0 成功 -1 失败
 */
int ipsetAOrDNetStr(char* groupName, char* ipStr, Uint32 cidr, char addOrDel)
{
    doSystem("ipset -q -%c %s %s/%d", addOrDel, groupName, ipStr, cidr);
    return 0;
}
#endif
/**
 * 测试ip地址ipstr是否在指定的 ipset组里面
 * return : 1 在 0 不在 
 */
int ipsetTestIpStr(char* groupName, char* ipStr)
{
#if IPSET_OP_EFF
    int ret = 0;
    struct in_addr addr;
    if (inet_aton(ipStr, &addr) != 0) {
	ret = (ipsetSkAdtIp(groupName, IP_SET_OP_TEST_IP , addr.s_addr) == -1);
    }
    return ret;
#else
    int ret = 0 , i= 0;
    char tmpstr[64];
    char *ptr = NULL;
    FILE *fp = NULL;

    memset(tmpstr, 0, sizeof(tmpstr));
    snprintf(tmpstr, sizeof(tmpstr)-1, "ipset -T %s %s", groupName, ipStr);

    fp = popen(tmpstr, "r");/*执行命令*/
    if(fp != NULL)
    {
	if(fgets(tmpstr, sizeof(tmpstr) -1 , fp) != NULL)/*读取一行*/
	{
	    IPSET_DEBUG("%s\n", tmpstr);
	    if(strstr(tmpstr,"is in") != NULL )/*the ipstr is in groupName*/
	    {
		ret = 1;
	    }
	}

	pclose(fp);/*关闭通道*/

    }
#endif

}
/*
 * 获取组被规则引用的次数
 groupName 待判断的ipset组名
 * 0 没有被引用
 * 其它数字 被引用的次数*/
int ipsetGetRefer(char* groupName)
{
    int ret = 0;
    char tmpstr[64];
    char linestr[MAX_LINE_LENGTH + 1];
    FILE *fp = NULL;
    char number[33];

    memset(tmpstr, 0, sizeof(tmpstr));
    memset(number, 0, sizeof(number));

    sprintf(tmpstr, "ipset -L %s", groupName);

    fp = popen(tmpstr, "r");/*执行命令*/
    if(fp != NULL)
    {
        fgets(linestr, MAX_LINE_LENGTH, fp);
        fgets(linestr, MAX_LINE_LENGTH, fp);
	if(fgets(linestr, MAX_LINE_LENGTH , fp) != NULL)/*读取一行*/
	{
	    IPSET_DEBUG("%s-----%d,%s\n", __func__, __LINE__, linestr);
	    if(strstr(linestr,"References") != NULL )/*the ipstr is in groupName*/
	    {
		sscanf(linestr, "%*s%s", number);
		IPSET_DEBUG("%s-----%d,%s\n", __func__, __LINE__, number);
		if(strcmp(number,"0") != 0)
		{
		    ret = (int)strtol(number, NULL, 10);
		    IPSET_DEBUG("%s-----%d,%d\n", __func__, __LINE__, ret);
		}
	    }
	}

	pclose(fp);/*关闭通道*/

    }

    return ret;
}
#endif
