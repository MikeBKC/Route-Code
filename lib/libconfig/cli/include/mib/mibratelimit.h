/*
 * 文件名:
 *     mibratelimit.h
 *
 * 描述:
 *    限速策略成员以及数据结构定义文件
 *    
 * 作者：
 *     刘亮亮
 *
 * 时间:
 *     2009-6-14
 *
 * 维护人：
 *     刘亮亮
 *
 */
#if !defined(MIBRATELIMIT_H)
#define MIBRATELIMIT_H


/*
 * 标志带宽使用级别的默认参数
 *
 * - RL_HOST_LOW_LEVEL			带宽空闲，在线主机数是带机量的0%-50%
 *
 * - RL_HOST_MID_LEVEL			带宽适中，在线主机数是带机量的50%-95%
 * 
 * - RL_HOST_HIGH_LEVEL			带宽不足，在线主机数是带机量的95%-100%
 *
 *
 *
 */
#define RL_HOST_LOW_LEVEL		50
#define RL_HOST_MID_LEVEL		95
#define RL_HOST_HIGH_LEVEL		100


/*
 * 限速策略的优先级：
 *
 * fbm(Flex Bandwidth Management)
 *
 * RL_PRIO_LOW:		    低优先级
 *
 * RL_PRIO_MID:		    中优先级
 *
 * RL_PRIO_HIGH:	    高优先级
 *
 */
typedef enum {
    RL_PRIO_MIN,
    
    RL_PRIO_LOW,	    /* 必须从1开始，否则会运算出错 */
    RL_PRIO_MID,
    RL_PRIO_HIGH,

    RL_PRIO_MAX
} fbmPriority;

#define	RL_ANY			"rlAny"


/*
 * 限速策略PROFILE结构：
 *
 * indexName:	    		实例名，由系统按照一定顺序默认生成
 *
 * enable:			为TRUE时，此条配置生效；为FALSE时，此条配置不生效
 *
 * srcIpGroupName:		源地址组
 *
 * dstIpGroupName:		目的地址组
 *
 * portGroupName:		服务组
 *
 * guaranteeRxBandwidth:	保证上行带宽
 *
 * guaranteeTxBandwidth:	保证下行带宽
 *
 * maxRxBandwidth:		最大上行带宽
 *
 * maxTxBandwidth:		最大下行带宽
 *
 * outputIfName:		输出接口名, 如果引用 PPPoE 拨号可能会存在长度不够的现象，
 *				还好目前的系统设置最大为dial_54或者 dial_017位，没有超过 
 *				11位（MAX_PROFILE_NAME_LENGTH），对于以太网取值为 eth1
 *
 * tmrgName:			生效的时间段
 *
 * priority:			此条配置的优先级
 * 
 * share:			为TRUE表明是共享，否则是独享
 *
 * remark:			备注
 *
 */

typedef struct {
    char		indexName[MAX_INDEX_NAME_LEN + 1];
    Boolean		active;
    Boolean		enable;
    char		srcIpGroupName[MAX_IPGROUP_NAME_LENGTH + 1];
    char		dstIpGroupName[MAX_IPGROUP_NAME_LENGTH + 1];
    char		portGroupName[MAX_PORTGROUP_NAME_LENGTH + 1];
    Uint32		guaranteeRxBandwidth;
    Uint32		guaranteeTxBandwidth;
    Uint32		maxRxBandwidth;
    Uint32		maxTxBandwidth;
    char		outputIfName[MAX_PROFILE_NAME_LENGTH + 1];
    char		tmrgName[MAX_PROFILE_NAME_LENGTH + 1];
    fbmPriority		priority;
    Boolean		share;
    char		remark[MAX_REMARK_LENGTH + 1];
} RateLimitProfile;

#endif

