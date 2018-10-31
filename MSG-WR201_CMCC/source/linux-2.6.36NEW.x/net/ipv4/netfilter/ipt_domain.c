#if defined(MODVERSIONS)
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_domain.h>
#include <net/udp.h>
#include <linux/ctype.h>

#define MAXDNAME 256
# define INDIR_MASK     0xc0

typedef  struct {
	__u16	id;		/* query identification number */
# ifdef BIG_ENDIAN
	/* fields in third byte */
	__u8	qr:1;		/* response flag */
	__u8	opcode:4;	/* purpose of message */
	__u8	aa:1;		/* authoritive answer */
	__u8	tc:1;		/* truncated message */
	__u8	rd:1;		/* recursion desired */
	/* fields in fourth byte */
	__u8	ra:1;		/* recursion available */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	unused:2;	/* unused bits */
	__u8	rcode:4;	/* response code */
# else
	/* fields in third byte */
	__u8	rd:1;		/* recursion desired */
	__u8	tc:1;		/* truncated message */
	__u8	aa:1;		/* authoritive answer */
	__u8	opcode:4;	/* purpose of message */
	__u8	qr:1;		/* response flag */
	/* fields in fourth byte */
	__u8	rcode:4;	/* response code */
	__u8	unused:2;	/* unused bits */
	__u8	pr:1;		/* primary server required (non standard) */
	__u8	ra:1;		/* recursion available */
# endif
	/* remaining bytes */
	__u16	qdcount;	/* number of question entries */
	__u16	ancount;	/* number of answer entries */
	__u16	nscount;	/* number of authority entries */
	__u16	arcount;	/* number of resource entries */
}DNS_HDR;

static char *utt_strstri (char *known, char *substr)
{
    if (known == substr)
        return ((char *)known);

    while (*known) {
        /*
         * Check to see if we match on a character yet
         */
        if ( tolower(*known++) == tolower(*substr) ) {
            /*
             * We do, so see if the rest of the substr matches
             */
            char *runner     = (char *)substr + 1;
            char *matchpoint = (char *)known - 1;

            while (*runner) {
                /*
                 * Compare our strings
                 */
                if ( tolower(*known) != tolower(*runner) )
                    break;
                /*
                 * Step to the next comparison
                 */
                known++;
                runner++;
            }

            /*
             * Did we get to the end of the substring?
             */
            if (!(*runner))
                return(matchpoint);
        }
    }

    return(NULL);
}

static int  _compareTwoStr(char *str1,char *str2)
{
    char temp[MAXDNAME + 1];
    char *pointer = str2;
    char *pointer1 = str1;
    int length1 = strlen(str1);
    int num =0;
    int i;

    for(i = 0; i<= length1; i++, str1++) {
        if((*str1 =='*') || (*str1 =='\0')) {
            temp[num] = '\0';               /* mod by llliu@080804 */
            num = 0;

            if(strlen(temp) == 0)
                continue;
            pointer = utt_strstri(pointer,temp);
            if(pointer == NULL) {
                return 0;
            }

           pointer += strlen(temp);
        } else {
            temp[num++] = *str1;
        }
    }

    /*
     * compare the last char 
     */
    if(pointer1[length1 - 1] != '*') {
        if(*pointer == '\0') {
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        return 1;
    }
}

static int expandDnsName(unsigned char	*msg, unsigned char *eomorig, 
		unsigned char *comp_dn, unsigned char *exp_dn, int length)
{
    unsigned char *cp, *dn;
    int		n, c;
    unsigned char     *eom;
    int        len = -1, checked = 0;
    
    dn = exp_dn;
    cp = (unsigned char *) comp_dn;
    eom = exp_dn + length;
    
    /*
     * fetch next label in domain name
     */
    
    while ((n = *cp++) != 0)
      {
	  /*
	   * Check for indirection
	   */
	  switch (n & INDIR_MASK)
	    {
	      case 0:
		if (dn != exp_dn)
		  {
		      if (dn >= eom)
			return (-1);
				*dn++ = '.';
		  }
		if (dn + n >= eom)
		  return (-1);
		checked += n + 1;
		while (--n >= 0)
		  {
		      if ((c = *cp++) == '.')
			{
			    if (dn + n + 2 >= eom)
			      return (-1);
			    *dn++ = '\\';
			}
		      *dn++ = c;
		      if (cp >= eomorig)	/* out of range */
			return (-1);
		  }
		break;
		
	      case INDIR_MASK:
		if (len < 0)
		  len = cp - comp_dn + 1;
		cp = (unsigned char *) msg + (((n & 0x3f) << 8) | (*cp & 0xff));
		if (cp < msg || cp >= eomorig)	/* out of range */
		  return (-1);
		checked += 2;
		/*
		 * Check for loops in the compressed name; if we've
		 * looked at the whole message, there must be a loop.
		 */
		if (checked >= eomorig - msg)
		  return (-1);
		break;
		
	      default:
		return (-1);	/* flag error */
	    }
      }
    *dn = '\0';
    if (len < 0)
      len = cp - comp_dn;
    return (len);
}

static int getDomainName(DNS_HDR *dns_hdr, int dns_len, unsigned char *buffer)
{
    int question_num;
    int len;
    unsigned char *cp;
    unsigned char name[MAXDNAME + 1];
    int cp_len;
    int name_len;

    if (dns_hdr == NULL || dns_len == 0 || buffer == NULL) {
        return 0;
    }
#if 0
    if (dns_len == 0) {
        return 0;
    }

    if (buffer == NULL) {
        return 0;
    }
#endif

    /*
     * 指向dns头
     */
    cp = (unsigned char *)dns_hdr + sizeof(DNS_HDR);


    question_num = ntohs(dns_hdr->qdcount);

    /*
     * 只处理请求数是1的dns数据包
     */
    if (question_num != 1) {
#if 0
        DNS_FILTER_PRINTF("%s-%d. question_num(%d) not support!\n",
                __FUNCTION__,
                __LINE__,
                question_num
                );
        _getDomainNameBadNum++;
#endif
        return 0;
    }



    len = expandDnsName(
            (unsigned char *) dns_hdr,
            (unsigned char *) dns_hdr + dns_len,
            cp,
            name,
            sizeof(name) 
            );

    if (len < 0) {
        return 0;
    }

    name_len = strlen(name);

    if (name_len == 0) {
        return 0;
    }
    
    cp_len = min((MAXDNAME+1), min(name_len, dns_len));

    strncpy(buffer, name, cp_len);

    if (buffer[cp_len] != '\0') {
#if 0
        if (dns_len >= MAXDNAME + 1) {
            _getDomainNameOverflow1++;
        }
        if (name_len >= MAXDNAME + 1) {
            _getDomainNameOverflow2++;
        }

        if (dns_len > name_len) {
            _getDomainNameOverflow3++;

        }
        else {
            _getDomainNameOverflow4++;
        }
        _getDomainNameOverflow++;
#endif
        return 0;
    }

    
//    _getDomainNameSuccess++;
    return 1;
}

static int fast_dns_filter(DNS_HDR *dnsPkt, int pktlen, char *domain)
{
    unsigned char  key[MAXDNAME + 1];
    int rtn;
    memset(key, 0, MAXDNAME + 1);
    rtn = getDomainName(dnsPkt,pktlen,key);
    if(rtn == 0)
        return 0;
    rtn = _compareTwoStr(domain,key);
    return rtn;
}

static int match(const struct sk_buff *skb, const struct xt_match_param *par)
{
	const struct iphdr *iph = ip_hdr(skb);
	struct udphdr *udph;
	const struct ipt_domain_info *info = par->matchinfo;

	if (iph->protocol != IPPROTO_UDP)
		return 0;

	udph = (void *)iph + iph->ihl * 4;

#if 0
	return ( (ntohs(udph->source) == 53 || ntohs(udph->dest) == 53)
				&& (udph->len >= (8 + 12 + info->len + 5))
				&& !strcmp(info->name,
				(char *)((void *)udph + ntohs(udph->len) - info->len - 5))
		);
#else
        if ((ntohs(udph->dest) == 53) && (udph->len >= (8 + 12 + info->len + 5))) {
            return fast_dns_filter(udph+1,ntohs(udph->len) - sizeof(struct udphdr),info->org_name);
        }
        return 0;
#endif

}


static int checkentry(const struct xt_mtchk_param *par)
{

	return 0;
}

static struct xt_match domain_match = { 
	.name		= "domain",
	.family		= AF_INET,
	.match		= &match,
	.matchsize      = sizeof(struct ipt_domain_info),
	.checkentry	= &checkentry,
	.me		= THIS_MODULE,
};

static int __init init(void)
{
	return xt_register_match(&domain_match);
}

static void __exit fini(void)
{
	xt_unregister_match(&domain_match);
}

module_init(init);
module_exit(fini);


MODULE_AUTHOR("Platinum, bbs.chinaunix.net");
MODULE_DESCRIPTION("A module to match DOMAIN. VERSION: 0.0.2");
MODULE_LICENSE("GPL");
