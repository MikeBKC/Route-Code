/*
 * �ļ���:
 *     mibratelimit.h
 *
 * ����:
 *    ���ٲ��Գ�Ա�Լ����ݽṹ�����ļ�
 *    
 * ���ߣ�
 *     ������
 *
 * ʱ��:
 *     2009-6-14
 *
 * ά���ˣ�
 *     ������
 *
 */
#if !defined(MIBRATELIMIT_H)
#define MIBRATELIMIT_H


/*
 * ��־����ʹ�ü����Ĭ�ϲ���
 *
 * - RL_HOST_LOW_LEVEL			������У������������Ǵ�������0%-50%
 *
 * - RL_HOST_MID_LEVEL			�������У������������Ǵ�������50%-95%
 * 
 * - RL_HOST_HIGH_LEVEL			�����㣬�����������Ǵ�������95%-100%
 *
 *
 *
 */
#define RL_HOST_LOW_LEVEL		50
#define RL_HOST_MID_LEVEL		95
#define RL_HOST_HIGH_LEVEL		100


/*
 * ���ٲ��Ե����ȼ���
 *
 * fbm(Flex Bandwidth Management)
 *
 * RL_PRIO_LOW:		    �����ȼ�
 *
 * RL_PRIO_MID:		    �����ȼ�
 *
 * RL_PRIO_HIGH:	    �����ȼ�
 *
 */
typedef enum {
    RL_PRIO_MIN,
    
    RL_PRIO_LOW,	    /* �����1��ʼ�������������� */
    RL_PRIO_MID,
    RL_PRIO_HIGH,

    RL_PRIO_MAX
} fbmPriority;

#define	RL_ANY			"rlAny"


/*
 * ���ٲ���PROFILE�ṹ��
 *
 * indexName:	    		ʵ��������ϵͳ����һ��˳��Ĭ������
 *
 * enable:			ΪTRUEʱ������������Ч��ΪFALSEʱ���������ò���Ч
 *
 * srcIpGroupName:		Դ��ַ��
 *
 * dstIpGroupName:		Ŀ�ĵ�ַ��
 *
 * portGroupName:		������
 *
 * guaranteeRxBandwidth:	��֤���д���
 *
 * guaranteeTxBandwidth:	��֤���д���
 *
 * maxRxBandwidth:		������д���
 *
 * maxTxBandwidth:		������д���
 *
 * outputIfName:		����ӿ���, ������� PPPoE ���ſ��ܻ���ڳ��Ȳ���������
 *				����Ŀǰ��ϵͳ�������Ϊdial_54���� dial_017λ��û�г��� 
 *				11λ��MAX_PROFILE_NAME_LENGTH����������̫��ȡֵΪ eth1
 *
 * tmrgName:			��Ч��ʱ���
 *
 * priority:			�������õ����ȼ�
 * 
 * share:			ΪTRUE�����ǹ��������Ƕ���
 *
 * remark:			��ע
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

