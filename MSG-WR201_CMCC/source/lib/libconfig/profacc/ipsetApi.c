/**
 * ipset����ӿ��ļ�
 * ���ã�ipset�汾����ʱ���õļ����µ�ipset����
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

/* ĳЩ������Ҫ��Ч�� ipset������http��������ͨ��� �����Զ������Ĳ�����
 * �� ��socket ֱ�Ӻ��ں�ͨ�� ���� ��system��������
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
 * ipset��������
 * ע�⣺�ú���Ӧ��Ӧ����ipset������ʱ����԰� goahead�Լ�speedweb�е�doSystem�Ƶ�����
 * bhou
 */
static void doSystem(const char* _Format, ...)
{
    char cmdstr[128];/*Ӧ��������Ҫ���ipset�ʲ���̫��*/
    va_list   args; 

    memset(cmdstr, 0, sizeof(cmdstr));

    /*���������ַ���*/
    va_start(args,_Format); 
    vsnprintf(cmdstr, sizeof(cmdstr)-1, _Format,   args   ); 
    va_end(args);

    IPSET_DEBUG(cmdstr);
    /*��������*/
    system(cmdstr);
    return;
}
#if IPSET_OP_EFF
/**
 * ��ipset���ں˷��� ������
 * isGet:1 �����ں�ע���setsockopt
 *       0 �����ں�ע���getsockopt
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
 * ipset ipmap �� iphash ����set�Ĳ���
 * ֧�� add del �� test һ��ip��ַ
 *
 * test ͬʱ֧��ipteemap ��Ϊ��������ʵ testʱ ֻ�ܲ����׸�ip
 *
 * return :
 *        0 �ɹ�
 *        -1 ʧ��
 */
static int ipsetSkAdtIp(char *groupName, unsigned op, unsigned long ipaddr )
{
    struct ip_set_req_adt_get req_adt_get;
    struct ip_set_req_adt *req_adt;
   /*��Ϊ ip_set_req_ipmap �� ip_set_req_iphash ��ʵ�ṹ��ȫһ��������������iphash �� ipmap�ĵ���ip*/
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
    {/*��������Ϊ iphash �� ipmap
    ��Ϊipset��������ע��ķ�ʽ��ɺܶ���������Բ�ͬ��ͷ�ļ�������ͬ�ĺ����������޷�ֱ�������ͷ�ļ�
    ֻ��ֱ�����С*/
	size -= sizeof(unsigned long);
    }
    else
    {/*����ip��ַ������Ϊһ��ip�Ρ��������ʼһ��*/
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
    /*��Ҫ֧���������͵Ĳ��������޸�data�еĴ�����*/
    /*ip set use host byte order*/
    *((unsigned long *) (data + ALIGNED(sizeof(struct ip_set_req_adt)))) = ipaddr;

    res = ipset_skop(data, &size, 0);
    return res;
}

/**
 * ֱ�Ӻ��ں�ͨ�� ������system�����Ч��
 * ɾ�� ��flushһ��ipset�� ����������κ�����
 * op: IP_SET_OP_DESTROY or IP_SET_OP_FLUSH
 * 
 * return 
 *       -1 ʧ��
 *       0   �ɹ�
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
 * �ж���ӵ������Ƿ��ϵͳ Ĭ����ӵ�������ͬ
 * return:  1 ��ͬ 0��ͬ
 */
int ipsetGrpNameIsSys(char *groupName)
{
    int ret = 0;
    if((strncmp(groupName, IPSET_SYS_GNAME_PRE, strlen(IPSET_SYS_GNAME_PRE)) == 0)
	    ||(strncmp(groupName, IPSET_TSYS_GNAME_PRE, strlen(IPSET_TSYS_GNAME_PRE)) == 0))
    {
	ret = 1;/*��ϵͳĬ������ǰ׺��ͬ*/
    }

    return ret;
}

#if (NOTICE == FYES)
/**
 * ͨ��ok���Ա��ӣ�������¼�Ѿ�ͨ������û�ip
 * return : 0 �ɹ� -1 ʧ��
 */
int noticeOkGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
/**
 * ͨ��web����ӡ�������¼��Ҫͨ����û�ip
 * return : 0 �ɹ� -1 ʧ��
 */
int noticeWebGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}

/**
 * ͨ��http redirect ��¼����ӡ�������¼����GET / HTTP/1.1��http����
 * return : 0 �ɹ� -1 ʧ��
 */
int noticeRGrpAdd(char* groupName, char *ipFrom, char *ipTo)
{
    doSystem("ipset -N %s ipportiphash --from %s --to %s", groupName, ipFrom, ipTo);
    return 0;
}
/**
 * ͨ��http redirect ��¼��༭
 * �����޸ļ�¼��ķ�Χ����������Ŀ����Ϊ��ʹ���ø���Ĺ��򲻱�
 * * return : 0 �ɹ� -1 ʧ��
 */
int noticeRGrpEdit(char* groupName, char *ipFrom, char *ipTo)
{
    char tmpName[32];
    int preLen = strlen(IPSET_TSYS_GNAME_PRE);
    
    /*������ʱ����*/
    memset(tmpName, 0, sizeof(tmpName));
    strncpy(tmpName, IPSET_TSYS_GNAME_PRE, sizeof(tmpName) - 1);
    strncpy(&tmpName[preLen], groupName, sizeof(tmpName) - preLen - 1);
    
    /*��һ��ͬ������ʱ��*/
    doSystem("ipset -N %s ipportiphash --from %s --to %s", tmpName, ipFrom, ipTo);
    /*�������齻�����ơ�*/
    doSystem("ipset -W %s %s", tmpName, groupName);
    /*ɾ����������������ʱ��*/
    doSystem("ipset -X %s", tmpName);

    return 0;
}
#endif
/**
 * �˺�����ӵ��顣���ڶ�̬�滻WEB��֤��ͨ����response��sportƥ������
 * return : 0 �ɹ� -1 ʧ��
 */
int httpPortGrpAdd(char* groupName)
{
    doSystem("ipset -N %s portmap --from 1 --to 65535", groupName);
    return 0;
}
/**
 * �˺�����ӵ��顣���ڶ�̬�滻WEB��֤��ͨ����response��sportƥ������
 * return : 0 �ɹ� -1 ʧ��
 */
int httpPortAdd(char* groupName,Uint32 port)
{
    doSystem("ipset -q -A %s  %u",groupName, port);
    return 0;
}
#if (PPPOE_ACCOUNT == FYES)
/**
 * �˺�����ӵ��顣ֻ�������洢 pppoe�˺ţ� ���˺������Ը���pppoe�˺ŷ�������Ż�
 * return : 0 �ɹ� -1 ʧ��
 */
int poeAccGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
#endif
#if (WEB_AUTH == FYES)
/**
 * ��֤ok���Ա��ӣ�������¼�Ѿ���֤�����û�ip
 * return : 0 �ɹ� -1 ʧ��
 */
int webauthOkGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iptreemap", groupName);
    return 0;
}
/**
 * ��֤web����ӡ�������¼��Ҫ��֤���û�ip
 * return : 0 �ɹ� -1 ʧ��
 */
int webauthWebGrpAdd(char* groupName)
{
    doSystem("ipset -N %s iphash", groupName);
    return 0;
}
#endif
#if (IP_GRP == FYES)
/**
 * ip��ַ�����
 * �������ӵ���ip����Χip�Լ�����
 *
 * return : 0 �ɹ� -1 ʧ��
 */
int ipGrpIpAddSet(char* groupName)
{
    doSystem("ipset -q -N %s iptreemap", groupName);
    return 0;
}
/**
 * �û������
 * return : 0 �ɹ� -1 ʧ��
 */
int ipGrpUserAddSet(char* groupName)
{
    doSystem("ipset -q -N %s iphash", groupName);
    return 0;
}
#endif
#if 0


/**
 * ��pppoe��ַ�ط�Χ�仯ʱ
 * �����û���
 * �ò���������û��������
 */
int userGrpEdit(char *groupName, char* ipFrom, char* ipTo)
{
    char *tmpName = "tmpGroup";/*������������ �û����鲻��ʹ�õ�����*/
    doSystem("ipset -N %s ipmap --from %s --to %s", tmpName, ipFrom, ipTo);
    doSystem("ipset -W %s %s", tmpName, groupName);
    doSystem("ipset -X %s", tmpName);
}



#endif

/**
 * ���ipset��
 * return : 0 �ɹ� -1 ʧ��
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
 * ɾ��ipset�顣ע���������Ƕ���δ�� ����ipset�� �� iptables�������õĲſ���ɾ��
 * return : 0 �ɹ� -1 ʧ��
 */
int ipsetDelGrp(char* groupName, bool notWarn)
{
    if(notWarn == TRUE)
    {/*���������󾯸���Ϣ*/
	doSystem("ipset -q -X %s", groupName);
    }
    else
    {/*�������󾯸���Ϣ*/
	doSystem("ipset -X %s", groupName);
    }

    return 0;
}
/**
 * ��ip������� ��ɾ�� ip
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 �ɹ� -1 ʧ��
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
 * ��ip������� ��ɾ�� ip
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 �ɹ� -1 ʧ��
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
 * ��ip������ӻ�ɾ��ip��Χ
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 �ɹ� -1 ʧ��
 */
int ipsetAOrDIpRange(char* groupName, IPADDR ipFrom, IPADDR ipTo, char addOrDel)
{
    char ipFromStr[32];
    char ipToStr[32];
    struct in_addr addrS;
    int ret = -1;

    memset(ipFromStr, 0, sizeof(ipFromStr));
    memset(ipToStr, 0, sizeof(ipFromStr));

    /*ת����ʼipΪ�ִ�*/
    addrS.s_addr = ipFrom;
    strcpy(ipFromStr, inet_ntoa(addrS));

    /*ת������ipΪ�ִ�*/
    addrS.s_addr = ipTo;
    strcpy(ipToStr, inet_ntoa(addrS));

    ret = ipsetAOrDIpStrRange(groupName,ipFromStr, ipToStr ,addOrDel);
    return ret;
}
/**
 * ��ip������ӻ�ɾ��ip��Χ
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 �ɹ� -1 ʧ��
 */
int ipsetAOrDIpStrRange(char* groupName, char* ipFromStr, char* ipToStr, char addOrDel)
{
    doSystem("ipset -q -%c %s %s-%s", addOrDel, groupName, ipFromStr, ipToStr);
    return 0;
}
#if 1
/**
 * ��ip������ӻ�ɾ��ip����
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 * return : 0 �ɹ� -1 ʧ��
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
  * ��ip������ӻ�ɾ��ip����
 * addOrDel: IPSET_MEM_ADD or IPSET_MEM_DEL
 return : 0 �ɹ� -1 ʧ��
 */
int ipsetAOrDNetStr(char* groupName, char* ipStr, Uint32 cidr, char addOrDel)
{
    doSystem("ipset -q -%c %s %s/%d", addOrDel, groupName, ipStr, cidr);
    return 0;
}
#endif
/**
 * ����ip��ַipstr�Ƿ���ָ���� ipset������
 * return : 1 �� 0 ���� 
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

    fp = popen(tmpstr, "r");/*ִ������*/
    if(fp != NULL)
    {
	if(fgets(tmpstr, sizeof(tmpstr) -1 , fp) != NULL)/*��ȡһ��*/
	{
	    IPSET_DEBUG("%s\n", tmpstr);
	    if(strstr(tmpstr,"is in") != NULL )/*the ipstr is in groupName*/
	    {
		ret = 1;
	    }
	}

	pclose(fp);/*�ر�ͨ��*/

    }
#endif

}
/*
 * ��ȡ�鱻�������õĴ���
 groupName ���жϵ�ipset����
 * 0 û�б�����
 * �������� �����õĴ���*/
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

    fp = popen(tmpstr, "r");/*ִ������*/
    if(fp != NULL)
    {
        fgets(linestr, MAX_LINE_LENGTH, fp);
        fgets(linestr, MAX_LINE_LENGTH, fp);
	if(fgets(linestr, MAX_LINE_LENGTH , fp) != NULL)/*��ȡһ��*/
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

	pclose(fp);/*�ر�ͨ��*/

    }

    return ret;
}
#endif
