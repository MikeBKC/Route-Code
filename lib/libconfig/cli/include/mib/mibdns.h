
#if !defined(MIBDNS_H)
#define MIBDNS_H


#define MAX_DOMAIN_NAME_LINE_COUNT 10 

/*
 *DnsResolveProfile
 *
 * dns自助解析
 *
 *   indexName	系统自定义的index
 *  
 *   enable	
 *  
 *   lively    启用/禁用
 *
 *   domainName	    域名 
 *  
 *   ip		    域名对应的ip
 *  
 *   remark	备注
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


