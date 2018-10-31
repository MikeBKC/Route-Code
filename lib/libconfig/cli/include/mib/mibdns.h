
#if !defined(MIBDNS_H)
#define MIBDNS_H


#define MAX_DOMAIN_NAME_LINE_COUNT 10 

/*
 *DnsResolveProfile
 *
 * dns��������
 *
 *   indexName	ϵͳ�Զ����index
 *  
 *   enable	
 *  
 *   lively    ����/����
 *
 *   domainName	    ���� 
 *  
 *   ip		    ������Ӧ��ip
 *  
 *   remark	��ע
 *  
 *
 */

typedef struct {
    char		indexName[MAX_INDEX_NAME_LEN+1];
    Boolean 		enable;
    Boolean		lively;
    char		domainName[MAX_DOMAIN_NAME_LINE_COUNT][MAX_DOMAIN_NAME_LENGTH+1];
    IpAddress		ip;
    char		remark[MAX_REMARK_LENGTH+1];
} DnsResolveProfile;



#endif


