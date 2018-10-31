/*	by guan  
A. according to RFC2136 RFC1035	RFC2845 and the reason of not using 
related code in dns.c is I think it obey to other obselated RFCs
and not suitable now  
in this version I only support datagram
B. for this version working, u 'd better make sure the time was correctly setup.
time server : 131.107.1.10
				128.250.36.2
a9491c00
*/

#include <stdlib.h>
#include <stdio.h>
#include "config/autoconf.h"
#include "linux/autoconf.h"
#include "ddns.h"
#include "time.h"
#include "../../uttShareHead/config/uttMachine.h"
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hmacmd5.h"
#include "rc4.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory.h>
#include        "net/if.h"
#include        "sys/ioctl.h"
//#include <hmacmd5.h>
# define TIME90_70   	(86400*(365*20+5))  /* added 5 leap years */
# define TIME_19_YEARS	(86400*365*19)
# define TIME_FUDGE	300

# define HQGUAN_BUG_NOT_FIXED

# if defined( HQGUAN_BUG_NOT_FIXED )
# define TIME_OFFSET	(16*3600)
# else
# define TIME_OFFSET	(0*3600)
# endif

#define SERVERS_PER_IP 	2

#define KEYSEEDLEN	44
#define MAX_KEY_LEN     MAX_DDNS_KEY_LEN	


/* per rfc2845 */

#define TSIG_BADKEY	(16)
#define TSIG_BADMAC	(17)
#define TSIG_BADTIME	(18)

#define IPLINK_HOST_DOMAIN "iplink.com.cn"

//enum section_t {ZONE_SEC,PRE_SEC,UP_SEC,AR_SEC};
//enum CONF_OP {add=0,del};


struct ddns_conf{
	char 		*hostname;
	IpAddress 	server_ip[SERVERS_PER_IP];
	int  		server_port[SERVERS_PER_IP];
	int 		servers;
	char 		*keyname;
	char 		*key;
	char 		*enkey;
	int 		update_retry;
	int 		update_timeout;  //unit: second
	int		update_interval;
	Uint8		enable;
	Uint32		offset;
};

static Uint32 _tzOffset;
static  char _keyBuf[MAX_KEY_LEN+1];

static unsigned char _gKeyseed[KEYSEEDLEN+1];

static struct ddns_conf  _ddns_confs[MAXCONFS];

//static const SystemVersion *sv;
static unsigned long long _gshowErr=0,_gDebug=0;

static unsigned long long _gTime=0;   /* GMT TIME since Epoch */


# if defined ( DDNS_DEBUG )

# define PRINTF(x)	if (_gDebug) { printf("%s-%d ", __FUNCTION__, __LINE__); printf x; }

static int test_hmacmd5(Uint8 *,int ,Uint8 *,int,Uint8 *,int );

# else 
# define PRINTF(x)	
# endif



static Uint32	lastnewip=0,lastnewip1;
//static SEM* sem_wk_cf;

static int lk=0,ulk=0, s_flag=1;
#define LOCK(sem) 	{SemaphoreWaitEx( (sem), SEM_WAIT_NONE ); lk++;}
#define UNLOCK(sem) 	{SemaphoreSignal( (sem) );ulk++;}

static Uint8 *pbuf,*answer, *query_digest;
//static int locateSection(Uint8 **pp,Uint8 *msg, section_t t);
//static int locateSection(int **pp,int *msg, section_t t);
#if (DDNS_ADVANCE == FYES)
static int logUpdateTime(char *interface);
static void logSendTime(char *interface);   /* for send req */
static int updateRR_A(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl,char *interface);
#else
static int logUpdateTime(void);
static void logSendTime(void);   /* for send req */
static int updateRR_A(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl);
#endif
static void logUpdateTime_f(void);
#define SSEED_KEY_FMT 	"%x%d%s%10s"

/*
 *follow added by brwang
 */
void putShort(Uint16 sint, Uint8 * msgp)       
{
    msgp[1] = sint;
    msgp[0] = sint >> 8;      
}

Uint16
getShort(Uint8	*msgp)
{
    Uint8 *p = (Uint8 *) msgp;
    register Uint16 u;

    u = *p++ << 8;
    return ((Uint16) (u | *p));
}

Uint32
getLong(Uint8	*msgp)
{
    Uint8 *p = (Uint8 *) msgp;
    Uint32 u;
    
    u = *p++;
    u <<= 8;
    u |= *p++;
    u <<= 8;
    u |= *p++;
    u <<= 8;
    return (u | *p);
}

void
putLong(Uint32		l, 
	Uint8	*msgp)
{
    msgp[3] = l;
    msgp[2] = (l >>= 8);
    msgp[1] = (l >>= 8);
    msgp[0] = l >> 8;
}

Int
expandDnsName(	Uint8	*msg, 
		Uint8	*eomorig, 
		Uint8	*comp_dn, 
		Uint8	*exp_dn, 
		Int		length)
{
    Uint8 	*cp, *dn;
    Int		n, c;
    Uint8      *eom;
    Int         len = -1, checked = 0;
    
    dn = exp_dn;
    cp = (Uint8 *) comp_dn;
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
		cp = (Uint8 *) msg + (((n & 0x3f) << 8) | (*cp & 0xff));
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



static Int
_findName(	Uint8 const	*exp_dn, 
		Uint8		*msg, 
		Uint8		**dnptrs, 
		Uint8		**lastdnptr)
{
    Uint8 const	*dn;
    Uint8	*cp, **cpp;
    Int    	n;
    Uint8   	*sp;
    
    for (cpp = dnptrs; cpp < lastdnptr; cpp++)
      {
	  dn = exp_dn;
	  sp = cp = *cpp;
	  while((n = *cp++) != 0)
	    {
		/*
		 * check for indirection
		 */
		switch (n & INDIR_MASK)
		  {
		    case 0:/* normal case, n == len */
		      while (--n >= 0)
			{
			    if (*dn == '.')
			      goto next;
			    if (*dn == '\\')
			      dn++;
			    if (*dn++ != *cp++)
			      goto next;
			}
		      if ((n = *dn++) == '\0' && *cp == '\0')
			return (sp - msg);
		      if (n == '.')
			continue;
		      goto next;
		      
		    default:	/* illegal type */
		      return (-1);
		      
		    case INDIR_MASK:	/* indirection */
		      cp = msg + (((n & 0x3f) << 8) | *cp);
		  }
	    }
	  if (*dn == '\0')
	    return (sp - msg);
	next:		;
      }	
    return (-1);
}



Int compressDnsName(Uint8	const *exp_dn, 
		Uint8	*comp_dn, 
		Int 	length, 
		Uint8 	**dnptrs, 
		Uint8	**lastdnptr)
{
    Uint8		*cp;
    Uint8 const		*dn;
    Int			c, l;
    Uint8		**cpp, **lpp, *sp, *eob;
    Uint8		*msg;
    
    dn = (Uint8 *) exp_dn;
    cp = comp_dn;
    eob = cp + length;
    cpp = lpp = 0;
    if (dnptrs != NULL)
      {
	  if ((msg = *dnptrs++) != NULL)
	    {
		for (cpp = dnptrs; *cpp != NULL; cpp++)
		  ;
		lpp = cpp;	/* end of list to search */
	    }
      }
    else
      {
	  msg = NULL;
      }
    
    for (c = *dn++; c != '\0';)
      {
	  /* look to see if we can use pointers */
	  if (msg != NULL)
	    {
		if ((l = _findName(dn - 1, msg, dnptrs, lpp)) >= 0)
		  {
		      if (cp + 1 >= eob)
			return (-1);
		      *cp++ = (l >> 8) | INDIR_MASK;
		      *cp++ = l % 256;
		      return (cp - comp_dn);
		  }
		/* not found, save it */
		if (lastdnptr != NULL && cpp < lastdnptr - 1)
		  {
		      *cpp++ = cp;
		      *cpp = NULL;
		  }
	    }
	  sp = cp++;	/* save ptr to length byte */
	  do
	    {
		if (c == '.')
		  {
		      c = *dn++;
		      break;
		  }
		if (c == '\\')
		  {
		      if ((c = *dn++) == '\0')
			break;
		  }
		if (cp >= eob)
		  {
		      if (msg != NULL)
			*lpp = NULL;
		      return (-1);
		  }
		*cp++ = c;
	    } while ((c = *dn++) != '\0');
	  
	  /* catch trailing '.'s but not '..' */
	  if ((l = cp - sp - 1) == 0 && c == '\0')
	    {
		cp--;
		break;
	    }
	  
	  if (l <= 0 || l > MAXLABEL)
	    {
		if (msg != NULL)
		  *lpp = NULL;
		return (-1);
	    }
	  *sp = l;
      }
    
    if (cp >= eob)
      {
	  if (msg != NULL)
	    *lpp = NULL;
	  return (-1);
      }
    *cp++ = '\0';
    
    return (cp - comp_dn);
}


/*
 * end brwang
 */

static Uint32 _tzseed ( Uint32 id ) 
{
  switch ( id ) {
   case DDNS_TZ_GUAN:
   		return (16*3600);

   case DDNS_TZ_NONE:
   		return (0*3600);

   default:
		return  _tzOffset;
 }
}

static const char * _skseed ( Uint32 id ) 
{
  switch ( id ) {
        case DDNS_TZ_GUAN:
	    return "djtn3kpfur9;";
        case DDNS_TZ_NONE:
	    return "ZxxvSlJ2$xZ3bSgjQ2Gdn0Qt2OJxgm0";
        default:
	    return "NepYLZa9$gD0fiOLVMalIMh23Fps.;";
    }
}

static Uint8    *
_skipCompName(	Uint8 		*cp, 
		Uint8 		*msg)
{
    char            name[255];
    Int             n;

    if ((n = expandDnsName((Uint8 *) msg, (Uint8 *) msg + 512, (Uint8 *) cp,
			 (Uint8 *) name, sizeof(name))) < 0)
      return (NULL);
    return (cp + n);
}		

static Uint8    *
_printCompName(	Uint8 		*cp, 
		Uint8 		*msg)
{
    char            name[255];
    Int             n;

    if ((n = expandDnsName((Uint8 *) msg, (Uint8 *) msg + 512, (Uint8 *) cp,
			 (Uint8 *) name, sizeof(name))) < 0)
      return (NULL);

    if (name[0] == '\0')
      {
	  name[0] = '.';
	  name[1] = '\0';
      }

    printf( "%s", name );
    return (cp + n);
}

int printRR(Uint8 **pRR, Uint8 *msg)
{
	Uint8 *p=(*pRR);
	int rdlen,MAClen,otherlen,i;
	Uint16 type;
	char f[20];
	
	printf("RR name: ");
	
	if ((p = _printCompName(p, msg)) == NULL)
      return (-1);	/* compression error */
	printf("TYPE: %d ",type=getShort(p));
	p+=2;
	printf("CLASS: %d ",getShort(p));
	p+=2;		
	printf("TTL: %d ",getLong(p));
	p+=4;	
	rdlen = getShort(p);
	p+=2;	
	printf("RDLENGTH: %d ",rdlen);		
	
	if(rdlen){
		switch(type){
		case T_A:
			printf("ip:%N\n",getLong(p));
			p+=4;
			break;
		case T_TSIG:
			printf("Algorithm Name:");
			if ((p = _printCompName(p, msg)) == NULL)
      			return (-1);	/* compression error */
      		printf(" Time Signed %x",getShort(p));
      		p+=2;
      		printf("%x ",getLong(p));
      		p+=4;
      		printf("Fudge: %d",getShort(p));
      		p+=2;
      		MAClen=getShort(p);
      		printf("MAC Size: %d MAC: ",MAClen);
      		p+=2;
      		//the MAClen should be 16, so
      		for(i=0;i<MAClen/4;i++,p+=4)
      			printf("%x",getLong(p));
      		printf("\n");
      		printf("Original ID %d ",getShort(p));
      		p+=2;
      		printf("Error %d ",getShort(p));
      		p+=2;
      		otherlen = getShort(p);
      		printf("Other len %d\n",otherlen);
      		p+=2;      
      		if(6 == otherlen){
	      		printf(" Other Data %x",getShort(p));
	      		p+=2;
	      		printf("%x ",getLong(p));
	      		p+=4;
      		}else
      			p+=otherlen;      			
			break;
		default:
			sprintf(f,"RD: %%ds\n",rdlen);	
			printf(f,p);
			p += rdlen;
			break;
		}
	}
	else{
		printf("\n");
	}
		
	*pRR = p;
	return 0;
}

int dumpDDNSmessage(Uint8 *msg)
{
	DDNS_HDR *hd=(DDNS_HDR *)msg;
	Uint8 *p=(Uint8 *)(hd+1);
	int i;
	
	printf("header: id %d, qr %d, opcode %d, aa %d, tc %d, rd %d, ra %d, rcode %d\n",
		ntohs(hd->id),hd->qr,hd->opcode,hd->aa,hd->tc,hd->rd,hd->ra,hd->rcode
		);
	printf("zocount %d,prcount %d,upcount %d,arcount %d\n",
		ntohs(hd->zocount),ntohs(hd->prcount),ntohs(hd->upcount),ntohs(hd->arcount)
		);
	for(i=0;i<ntohs(hd->zocount);i++){
		printf("zone name:");
		if ((p = _printCompName(p, msg)) == NULL)
      		return 0;	/* compression error */
		
		printf(" ZTYPE: %d ",getShort(p));
		p+=2;
		printf("ZCLASS: %d\n",getShort(p));
		p+=2;		
	}
	printf("PRE SECTION:");
	for(i=0;i<ntohs(hd->prcount);i++){
		printRR(&p,msg);
	}
	printf("\n");
	printf("UP SECTION:");
	for(i=0;i<ntohs(hd->upcount);i++){
		printRR(&p,msg);
	}
	printf("\n");
	printf("ADD SECTION:");
	for(i=0;i<ntohs(hd->arcount);i++){
		printRR(&p,msg);
	}
	printf("\n");
	return 0;
}

static Uint16	    from_port, wtime, id;
static int	s=-1, nextTimer=0, wtimeTimer=0, tryVal=0, ns=0, timeerr=0;
static DDNS_HDR	*s_hp,*anhp;

static struct commPara{
    Uint8	*buf;
    Int		buflen; 
    Uint8	*answer; 
    Int		anslen;
    int		index;
    Uint8	*query_digest;
    int		query_digest_len;
    //Uint8	time_err_flag;
} cmPa;

static struct updataRR_A_Para{
	Uint32 newip;
	Uint32 oldip;
	char fqdn[MAX_HOST_NAME_LENGTH+1];
	int ttl;
} upPa,*pupPa;

#if (DDNS_ADVANCE == FYES)
static void ddns_cli_receive(int socket, int event_type,char *interface);
#else
static void ddns_cli_receive(int socket, int event_type);
#endif
static void ddns_cli_expire(int nouse,int nouse1);

/*
 * 绑定UDP发送端口。
 */
static int ddns_cli_request_ini(Uint8	*buf, 
		Int	buflen, 
		Uint8	*answer, 
		Int	anslen,
		int index,
		Uint8 *query_digest,
		int query_digest_len)
{

    struct timeval tval;
    cmPa.buf = buf;
    cmPa.buflen = buflen;
    cmPa.answer = answer;
    cmPa.anslen = anslen;
    cmPa.index = index;
    cmPa.query_digest = query_digest;
    cmPa.query_digest_len = query_digest_len;
    //cmPa.time_err_flag = 0;

    tryVal = 0;
    ns = 0;
	
    if (s_flag) {
#if 0
	from_port = UDP_PORT_ANY_NOPRIV;
	s = netbind(&from_port, 0, ddns_cli_receive, 0, GLOBAL_VROUTER_ID);    	    
	if (s < 0) {
	    s = -1;
	    return(-1);
	}
#endif
        if((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
               //perror("socket");
               exit( 1 );
        }
        tval.tv_sec = 5;
        tval.tv_usec = 0;
        setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tval,sizeof(tval)); 
	s_flag = 0;
    }
    return 0;
}

static int
#if (DDNS_ADVANCE == FYES)
ddns_cli_request_send(char *interface)
#else
ddns_cli_request_send(void)
#endif
{
    int ii;
    struct sockaddr_in their_addr;
	
    s_hp = (DDNS_HDR *) cmPa.buf ;
    anhp = (DDNS_HDR *) cmPa.answer;
    id = s_hp->id;
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(_ddns_confs[cmPa.index].server_port[ns]);
//    their_addr.sin_addr.s_addr = htonl((_ddns_confs[cmPa.index].server_ip[ns]));
    their_addr.sin_addr.s_addr = ((_ddns_confs[cmPa.index].server_ip[ns]));
    bzero(&(their_addr.sin_zero),8);
	
    for (ii = 0; ii < 3; ii++) {//if send fail, retry 3 times
	if (!_ddns_confs[cmPa.index].server_ip[ns]) break;
	if(_gDebug) {
	    printf("%s %d: ns = %d, dest ip %N destport %d\n",
		__FUNCTION__,
		__LINE__,
		ns,
		_ddns_confs[cmPa.index].server_ip[ns],
		_ddns_confs[cmPa.index].server_port[ns]
		);
	}
#if 0
	if( netsend(s,
		    cmPa.buf,
		    cmPa.buflen,
		    0,
		    _ddns_confs[cmPa.index].server_ip[ns],
		  htons(_ddns_confs[cmPa.index].server_port[ns]),
		  GLOBAL_VROUTER_ID) == cmPa.buflen) {
	    break;
	}
#else
	if(sendto(s, cmPa.buf, cmPa.buflen, 0, 
                    (struct sockaddr*)&their_addr,
                    sizeof(struct sockaddr))!= -1)
            break;
#endif
    }

    if (3 == ii) {
	if (_gshowErr)
	    printf("%s %d: err: can't send dns request\n",__FUNCTION__,__LINE__);
	return -1;
    }

#if (DDNS_ADVANCE == FYES)
    logSendTime(interface);
    ddns_cli_receive(0,0,interface);
#else
    logSendTime();
    ddns_cli_receive(0,0);
#endif
    return 0;    
}

static char	dateString[512];
static Uint32 succ_up_count=0,fail_up_count=0;

/*
 * 数据包接收函数
 *
 */

#if (DDNS_ADVANCE == FYES)
static void ddns_cli_receive(int socket, int event_type,char *interface)
#else
static void ddns_cli_receive(int socket, int event_type)
#endif
{
    Uint8 packet[1500];
    Uint16	    from_port1;
    Uint32	    from_ip;
    int resplen;
    unsigned long long res_time;
    Uint16	fudge,otherlen;
    Uint16	tsig_RD_Err=0;
    char	resultstr[64];
    struct sockaddr rin;
    int address_size;
	
    if(_gshowErr || _gDebug) {
	printf("ddns_cli_receive got data before LOCK\n");
    }
	
    /*
     * 绑定失败。
     */
    if (-1 == s) {
	goto ret;
    }

#if 0
    /*
     * socket错误。
     */
    if ( socket != s && (_gshowErr||_gDebug)) {// some thing is very wrong 
	printf(" ddns: wrong UDP socket \n" );
	goto ret;
    }
    if (event_type != EV_UDP_RECV && (_gshowErr||_gDebug) ) {
	printf(" ddns: wrong UDP event \n" );
	goto ret;
    }
#endif
    
    //resplen = udp_recvfrom(s, &packet, &from_ip, &from_port1, NULL);
    resplen = recvfrom(s, packet, 1500, 0, (struct sockaddr *)&rin, &address_size);

    if(resplen <= 0) {
	if(_gDebug) {
	    printf("  %s %d: no response from %N, continue \n",
		__FUNCTION__,__LINE__,
		_ddns_confs[cmPa.index].server_ip[ns]);
	}
        return;
    }
    if (packet) {
	resplen = MIN(resplen, cmPa.anslen);
	memcpy(cmPa.answer, packet, resplen);
    }
    else {
   	if (_gshowErr)	printf(" %s %d: receive null",__FUNCTION__,__LINE__);
    	goto ret;
    }	

    if(_gDebug) {
	printf("response dns msg:\n");
	dumpDDNSmessage(cmPa.answer);
    }

    if (id != anhp->id) { //response from old query, ignore it
	if (_gshowErr||_gDebug)
	    printf("ddns: response from old query, ignore it. req's id=%d. res's id=%d\n",id,anhp->id);			      
	goto ret;
    }

    if (anhp->tc) {
	if (_gshowErr||_gDebug)
	    printf("ddns: the reponse was truncated .");
	goto quit;
    }
	
    if(_ddns_confs[cmPa.index].keyname[0] &&
	    (_ddns_confs[cmPa.index].key[0] || _ddns_confs[cmPa.index].enkey[0])) {//send request with TSIG
	int i, arcount = ntohs(anhp->arcount);
    	Uint8 *pRR;
    	
	printf("%s-%d, arcount = %d\n", __func__, __LINE__, arcount);
    	if(0 == arcount) {
	   if (_gshowErr||_gDebug)
    		printf("anhp->arcount == 0 , no tsig, rcode=%d\n",anhp->rcode);
	   goto quit;
    	}

    	locateSection(&pRR, cmPa.answer, AR_SEC);

    	for(i = 0; i < arcount; i++) {
	    if ((pRR =_skipCompName(pRR, cmPa.answer)) == NULL) {
		if (_gshowErr||_gDebug)
		    printf("%s %d parse tsig name of dns msg err  \n",__FUNCTION__,__LINE__);
		goto quit;
	    }

	    if (T_TSIG == getShort(pRR)) {
		if (i != (arcount - 1)) {
		    if (_gshowErr||_gDebug)
			printf("reponse is err form!\n");
		    goto quit;
		} else {// yes, TSIG is last RR. validate it
		   // Here, I just check the MAC with the key I send the query message			     				
		    hmacmd5_t hmd5;
		    Uint8 *key,*md5up_b,*md5up_e,querymaclen[2],*responseMAC;
		    Uint16	MAClen,t;
		    time_t ltime;
		    //int keylen = ( strlen(_ddns_confs[index].key) * 3 ) / 4;
     				
		   key = (Uint8 *)MALLOC(sizeof(_keyBuf));
		   memset(key, 0, sizeof(_keyBuf));
		    if (_ddns_confs[cmPa.index].key[0]) {
			deBase64( key, _ddns_confs[cmPa.index].key);
		    } else {
			int keylen = ( strlen(_ddns_confs[cmPa.index].enkey) * 3 ) / 4;
			struct rc4_key rc4key;
					
			deBase64( key, _ddns_confs[cmPa.index].enkey);
			prepare_key(_gKeyseed,KEYSEEDLEN,&rc4key);
			rc4(key,keylen,&rc4key);
		    }

		    if (_gDebug){
			printf("keylen:%d,key:",sizeof(_keyBuf));
			//monitorDump("key", key, sizeof(_keyBuf), sizeof(_keyBuf));
			//monitorDump("_gKeyseed", _gKeyseed, KEYSEEDLEN+1,KEYSEEDLEN+1);
			printf("\n");
		    }

		    hmacmd5_init(&hmd5, (const unsigned char *)key, sizeof(_keyBuf));	     			
		    FREE(key);
			     					
		    putShort(MD5_DIGEST_LEN, querymaclen);	
		    hmacmd5_update(&hmd5,querymaclen,sizeof(Uint16));	//request MAClen				     	
		    hmacmd5_update(&hmd5,cmPa.query_digest,MD5_DIGEST_LEN);  //request MAC
		    t = ntohs(anhp->arcount);
		    t--;	
		    anhp->arcount = htons(t);//adjust the arcount
		    hmacmd5_update(&hmd5,cmPa.answer,pRR-cmPa.answer);  // reponse + tsig.name
		    pRR += sizeof(Uint16);  //skip type
		    md5up_b = pRR;
		    pRR += sizeof(Uint16);	//skip class
		    pRR += sizeof(Uint32);  //skip ttl
		    md5up_e = pRR;
		    hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(class,ttl)
		    pRR += sizeof(Uint16);	//skip field of rdlen
		    md5up_b = pRR;
		    if ((pRR =_skipCompName(pRR, cmPa.answer)) == NULL) {
			if(_gshowErr||_gDebug){
			    printf("%s %d:parse the TSIG agri name of reponse  ERROR !\n",__FUNCTION__,__LINE__);
			}
			goto quit;
		    }

		    res_time = getShort(pRR);
		    res_time <<=32;
		    pRR += sizeof(Uint16);
		    res_time +=getLong(pRR);
		    pRR += sizeof(Uint32);
		    fudge = getShort(pRR);
		    pRR += sizeof(Uint16);
		    time( &ltime );
//		    ltime = localTime2EpochTime(ltime) + _tzseed(_ddns_confs[cmPa.index].offset);
		    ltime = ltime + 16*3600 - 28800;
		    if(_gTime) ltime = _gTime;

		    if (res_time > (ltime+fudge) || res_time < (ltime-fudge) ){
			if(_gshowErr||_gDebug)
			    printf("%s %d:the tsig RD time signed of reponse is err.  try again\n",__FUNCTION__,__LINE__);

			hmacmd5_invalidate(&hmd5);
			goto ret;
		    }								

		    md5up_e = pRR;
		    hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(algro name,time,fudge)
			
		    MAClen = getShort(pRR);
		    pRR += sizeof(Uint16);		// skip  MAClen
		    responseMAC = pRR;
		   pRR += MAClen;	//skip MAC	
		   pRR += sizeof(Uint16); //skip ori ID
		   md5up_b = pRR;
		   tsig_RD_Err = getShort(pRR);
		   pRR += sizeof(Uint16); // skip Error

		   if (0 == MAClen) {
			if(_gshowErr||_gDebug) {
			    if (TSIG_BADKEY == tsig_RD_Err )
				printf("%s %d:Tsig Key bad of request error\n",__FUNCTION__,__LINE__);
			    if (TSIG_BADMAC == tsig_RD_Err)
				printf("%s %d:Tsig  MAC of request error\n",__FUNCTION__,__LINE__);
     			}

			hmacmd5_invalidate(&hmd5);
     			goto ret;  //according to RFC2136 we should discard it and wait another one
		    }

		    if (TSIG_BADTIME == tsig_RD_Err) {       								
			if ( _gshowErr||_gDebug )
			    printf("%s %d:the time stamp of request err,   timeerr=%d\n",__FUNCTION__,__LINE__,timeerr);

			if ((timeerr++ < _ddns_confs[cmPa.index].update_retry) ||
			    (_ddns_confs[cmPa.index].update_retry ==0)) {
			    hmacmd5_invalidate(&hmd5);
			    goto ret; //if the time set error we still retry it for retrytime times
			} else {
			    goto quit;
			}
     					
		    } else {
			timeerr = 0;
		    }

		    otherlen = getShort(pRR);
		    pRR += sizeof(Uint16);
		    pRR += otherlen;
		    md5up_e = pRR;
		    hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(Error,otherlen , other data)

		    if (!hmacmd5_verify(&hmd5,responseMAC)) {
			if(_gshowErr||_gDebug) {
			    printf("%s %d:check the TSIG of reponse  ERROR !\n",__FUNCTION__,__LINE__);
			}

			goto ret;	//according to RFC2136 we should discard it and wait another one
		    }

		    break;  // tsig verify success, goto check if (anhp->rcode != NOERROR )
		}//if (i!= (arcount-1))
	    }//if (T_TSIG == getShort(pRR)
     		
	    pRR += sizeof(Uint16);  //skip type
	    pRR += sizeof(Uint16);	//skip class
	    pRR += sizeof(Uint32);  //skip ttl
	    pRR += getLong(pRR);		// skip rdlen - sizeof(Uint16)
	    pRR += sizeof(Uint16);	//skip the rest sizeof(Uint16)
     		
	}//for(i=0;i<anhp->arcount;i++)
	printf("%s-%d, arcount = %d\n", __func__, __LINE__, arcount);

    	if (arcount == i) {
    		if (_gshowErr||_gDebug)
    			printf("%s %d:lack of tsig RR\n",__FUNCTION__,__LINE__);
    		goto quit;
    	}
    }//if(_ddns_confs[index].keyname&&_ddns_confs[index].key)
		
    if (anhp->rcode != NOERROR) {
	fail_up_count++;
	sprintf(dateString,"Tot:%d, Succ:%d,Fail:%d.  \r\n",(succ_up_count+fail_up_count),succ_up_count,fail_up_count);
		
	switch (anhp->rcode){
	case NXDOMAIN:
	    if(_gshowErr||_gDebug)	printf("err: rcode:NXDOMAIN \n");
	    snprintf(resultstr,64,"err: rcode:NXDOMAIN. ");
	    break;										
	case SERVFAIL:
	    if(_gshowErr||_gDebug)	printf("err: rcode:SERVFAIL\n");
	    snprintf(resultstr,64,"err: rcode:SERVFAIL. ");
	    break;									
	case YXDOMAIN:
	case YXRRSET:
	case NXRRSET:
	    if(_gshowErr||_gDebug)	
	    printf("err: rcode:%d,YXDOMAIN or YXRRSET or NXRRSET \n",s_hp->rcode);
	    snprintf(resultstr,64,"err: rcode:%d,YXDOMAIN or YXRRSET or NXRRSET. ",s_hp->rcode);
	    break;
	case NOTAUTH:
	    if(_gshowErr||_gDebug)	printf("err: rcode:NOTAUTH \n");
	    snprintf(resultstr,64,"err: rcode:NOTAUTH. ");
	    break;
	case NOTZONE:
	    if(_gshowErr||_gDebug)	printf("err: rcode:NOTZONE \n");
	    snprintf(resultstr,64,"err: rcode:NOTZONE. ");
	    break;
	case BADSIG:
	case BADKEY:
	case BADTIME:
	    if(_gshowErr||_gDebug)	
		printf("err: rcode:%d,BADSIG or BADKEY or BADTIME \n",s_hp->rcode);
	    snprintf(resultstr,64,"err: rcode:%d,BADSIG or BADKEY or BADTIME. ",s_hp->rcode);
	    break;												
	case FORMERR:
	case NOTIMP:
	case REFUSED:
	default:
	    if(_gshowErr||_gDebug)	
		printf("err: rcode:%d,FORMERR or NOTIMP or REFUSED or others \n",s_hp->rcode);
	    snprintf(resultstr,64,"err: rcode:%d,FORMERR or NOTIMP or REFUSED or others. ",s_hp->rcode);
	    break;
	}
	
	strcat(dateString,resultstr);
//	logUpdateTime_f();
	goto quit;
    }//if (s_hp->rcode != NOERROR )
	
    if (_gshowErr)
	printf("ddns update success \n");
	
    succ_up_count++;
    sprintf(dateString,"Tot:%d, Succ:%d,Fail:%d, \r\n",(succ_up_count+fail_up_count),succ_up_count,fail_up_count);
	
    //snprintf(resultstr,64,"ddns update success .");
    //strcat(dateString,resultstr);

#if (DDNS_ADVANCE == FYES)
    logUpdateTime(interface);
#else
    logUpdateTime();
#endif	
	
quit:
    if (nextTimer){
//	untimeout2(nextTimer);
	nextTimer = 0;
    }

    /* by guan: for move initial to ddnsInit
    FREE( cmPa.buf );
    FREE( cmPa.answer );
    FREE( cmPa.query_digest );
    netrelease(s);
    s = -1;*/

ret:    
//    UNLOCK(sem_wk_cf);
    return;
}


/*
 * 构建DDNS请求数据包。
 */
static Int
_makeUpdate(Uint32 newip,
		Uint32 oldip,
		const char * fqdn, 
		int ttl,	
		Uint8		*data,
		Int		datalen,
		Uint8		*buf,
		Int		buflen,
		int *pindex)
{
#if 1
    DDNS_HDR 	*hp;
    Uint8  	*cp,*domain;
    char *sd=NULL;
    Int    	n;
    char	*dnptrs[10], **dpp, **lastdnptr;
    static Uint16 query_id=1;
    
    // to check if there is configuration
    for(*pindex = 0; *pindex<MAXCONFS; (*pindex)++){
    	if(_ddns_confs[*pindex].hostname&&
    		!strcmp(_ddns_confs[*pindex].hostname,fqdn))
   	    break;
    }
    
    if ( MAXCONFS == (*pindex)){ 
    	if(_gDebug) printf("MAXCONFS == (*pindex)");   	
    	return -1;
    }
    if (0 == _ddns_confs[*pindex].enable){
    	if(_gDebug) printf("0 == _ddns_confs[%d].enable", *pindex);
    	return -2;
    }
    /*	
     * Initialize header fields.
     */
    if ((buf == NULL) || ((size_t)buflen < sizeof(DDNS_HDR))){
    	if(_gDebug) printf("(buf == NULL) || ((size_t)buflen < sizeof(DDNS_HDR))");
      return (-1);
    }
    
    memset(buf, 0, sizeof(DDNS_HDR));
    hp = (DDNS_HDR *) buf;
    hp->id = htons(++query_id);
    hp->opcode = DNS_UPDATE;
    hp->qr = 0;
    hp->rd = 0;
    hp->rcode = NOERROR;
    cp = buf + sizeof(DDNS_HDR);
    buflen -= sizeof(DDNS_HDR);
    dpp = dnptrs;
    *dpp++ = (char *) buf;
    *dpp++ = NULL;
    lastdnptr = dnptrs + sizeof(dnptrs) / sizeof(dnptrs[0]);
    
    //add the zone section
    domain=(Uint8 *)MALLOC(256);
    sd = strchr(fqdn,'.');
    sd++;
    if(*sd){
    	strcpy(domain,sd);
    	if ((n = compressDnsName((Uint8 const *) domain, (Uint8 *) cp, buflen,
				 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
		if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
	    	return (-1);
	}
        cp += n;
        buflen -= n;
    }
    FREE(domain);
    
    putShort(T_SOA, (Uint8 *) cp); //type
    cp += sizeof(Uint16);
    buflen -= sizeof(Uint16);
	
    putShort(C_IN, (Uint8 *) cp);//class: IN
    cp += sizeof(Uint16);
    buflen -= sizeof(Uint16);
    hp->zocount = htons(1);
	
    if(_gDebug){
	    printf("%s %d: after add zone section . zocount=%d\n", 
	    	__FUNCTION__,__LINE__,ntohs(hp->zocount));
    }
    // add the delete RR/RRset
    if ((n = compressDnsName((Uint8 const *) fqdn, (Uint8 *) cp, buflen,
				 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
        if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
        return (-1);
    }
	cp += n;
	buflen -= n;
	
	putShort(T_A, (Uint8 *) cp); //type
	cp += sizeof(Uint16);
	buflen -= sizeof(Uint16);

	{//delete RRset
		putShort(C_ANY, (Uint8 *) cp);//class: any
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(0, (Uint8 *) cp);  //ttl: 0
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		
		putShort(0, (Uint8 *) cp);//rdlength
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
	}
	hp->upcount = htons(1);
	
	if(_gDebug){
	    printf("%s %d: after add delete RR/RRset \n", __FUNCTION__,__LINE__);
	}
	
	if(0 != newip){	//add the add RR
		if ((n = compressDnsName((Uint8 const *) fqdn, (Uint8 *) cp, buflen,
					 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);	
		}
		cp += n;
		buflen -= n;
		
		putShort(T_A, (Uint8 *) cp); //type
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putShort(C_IN, (Uint8 *) cp);//class 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(ttl, (Uint8 *) cp);  //ttl
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		
		putShort(4, (Uint8 *) cp);//rdlength
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		//inetAtoN(newip, &ip32);
		putLong(ntohl(newip), (Uint8 *) cp);  //rd  . now the ip is stored in network order
		cp += sizeof(Uint32);
		buflen -=  sizeof(Uint32);
		
		
		hp->upcount = htons(2);
	}
	if(_gDebug){
	    printf("%s %d: after add add arr\n", __FUNCTION__,__LINE__);
	}
	//add the tsig RR
	if(_ddns_confs[*pindex].keyname[0]&&(_ddns_confs[*pindex].key[0]||_ddns_confs[*pindex].enkey[0])){
		Uint8 *prdlen,*pMAC,*md5up_b,*md5up_e;
		//AbsoluteTime time;
		time_t _time;
		Uint16 t_time;
		unsigned long long ltime;
		hmacmd5_t hmd5;
		Uint8 *key;//,digest[MD5_DIGEST_LEN];
		//int i;//,keylen = ( strlen(_ddns_confs[*pindex].key) * 3 ) / 4;
		
		if(!data || datalen<MD5_DIGEST_LEN) {
			if(_gDebug) printf("!data || datalen<MD5_DIGEST_LEN");
			return -1;
		}
		
		key = (Uint8 *)MALLOC(sizeof(_keyBuf));
		memset(key,0,sizeof(_keyBuf));
		if (_ddns_confs[*pindex].key[0]){
			deBase64( key, _ddns_confs[*pindex].key);
		}else{
			int keylen = ( strlen(_ddns_confs[*pindex].enkey) * 3 ) / 4;
			struct rc4_key rc4key;
			
			deBase64( key, _ddns_confs[*pindex].enkey);
			prepare_key(_gKeyseed,KEYSEEDLEN,&rc4key);	
			rc4(key,keylen,&rc4key);
			
		}
		//for debug
		if (_gDebug){
			printf("keylen:%d,key:",sizeof(_keyBuf));
			//monitorDump("key", key, sizeof(_keyBuf), sizeof(_keyBuf));
			//monitorDump("_gKeyseed", _gKeyseed, KEYSEEDLEN+1,KEYSEEDLEN+1);
			printf("\n");
		}
		hmacmd5_init(&hmd5, (const unsigned char *)key, sizeof(_keyBuf));
		FREE(key);
		
		if ((n = compressDnsName((Uint8 const *) _ddns_confs[*pindex].keyname, (Uint8 *) cp, buflen,
					 NULL, NULL)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);
		}
		cp += n;
		buflen -= n;
		
		hmacmd5_update(&hmd5, (Uint8 *)hp, (cp-(Uint8 *)hp)); // dns message + tsig.(name)
		
		putShort(T_TSIG, (Uint8 *) cp); //type
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		md5up_b = cp;
		putShort(C_ANY, (Uint8 *) cp);//class 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(0, (Uint8 *) cp);  //ttl
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		md5up_e = cp;
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(class,ttl)
		
		
		prdlen = cp;
		putShort(0, (Uint8 *) cp);	//rdlength now to reserve a room and it will be filled again after fill RDATA
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		md5up_b = cp;
		if ((n = compressDnsName((Uint8 const *) "hmac-md5.sig-alg.reg.int", 
				(Uint8 *) cp, buflen, NULL, NULL)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);
		}
		cp += n;
		buflen -= n;
		
		GRT_Z(buflen,-1);
		
		//requestAbsoluteTime( &time );
		time( &_time );
		
		if((!_gTime) &&(_time < TIME_19_YEARS && (_gshowErr || _gDebug))){
			//I assume this box won't startup for around 5 years and still not get the time by sntp
			printf("%s %d:the time not set correctly !\n",__FUNCTION__,__LINE__);
		}
		//ltime = _gTime ? _gTime : localTime2EpochTime(time);(time + TIME90_70 + 3600*8);
		//ltime = _gTime ? _gTime : localTime2EpochTime(time)+_tzOffset;
		//ltime = _gTime ? _gTime : localTime2EpochTime(_time) + _tzseed(_ddns_confs[*pindex].offset);
		ltime = _gTime ? _gTime : _time + 16*3600 - 28800;
		if(_gDebug){
			printf("time=%x,ltime=%x,_gTime=%x%x\n",
			_time,ltime,_gTime);
		}

		t_time = (ltime>>32);
		putShort(t_time, (Uint8 *) cp);  //time signed 1-2 byte
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		putLong(ltime, (Uint8 *) cp);  //time signed 3-6 byte
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		GRT_Z(buflen,-1);

		putShort(TIME_FUDGE, (Uint8 *) cp);//Fudge
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		md5up_e = cp;
		GRT_Z(buflen,-1);
		
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(algo,time,fudge)
		
		putShort(MD5_DIGEST_LEN, (Uint8 *) cp);//MAC Size 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		
		pMAC = cp;
		cp += MD5_DIGEST_LEN;
		buflen -= MD5_DIGEST_LEN;
		
		putShort(ntohs(hp->id), (Uint8 *) cp);//original ID
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		
		md5up_b = cp;
		putShort(0, (Uint8 *) cp);//Error
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		putShort(0, (Uint8 *) cp);//other len
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		md5up_e = cp;
		
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(Error,other len,other data)
		hmacmd5_sign(&hmd5, data);
		
		putShort(md5up_e-(prdlen+2),prdlen);
		
		memcpy(pMAC,data,MD5_DIGEST_LEN);
		
		hp->arcount = htons(1);
		
	}
	
    return (cp - buf);
#else    
	DDNS_HDR 	*hp;
    Uint8  	*cp,*domain;
    char *sd=NULL;
    Int    	n;
    char	*dnptrs[10], **dpp, **lastdnptr;
    static Uint16 query_id=1;
    //Uint32 ip32;
    
    // to check if there is configuration
    for(*pindex = 0; *pindex<MAXCONFS; (*pindex)++){
    	if(_ddns_confs[*pindex].hostname&&
    		!strcmp(_ddns_confs[*pindex].hostname,fqdn))
    		break;
    }
    
    if ( MAXCONFS == (*pindex)){ 
    	if(_gDebug) printf("MAXCONFS == (*pindex)");   	
    	return -1;
    }
    if (0 == _ddns_confs[*pindex].enable){
    	if(_gDebug) printf("0 == _ddns_confs[%d].enable", *pindex);
    	return -2;
    }
    /*	
     * Initialize header fields.
     */
    if ((buf == NULL) || ((size_t)buflen < sizeof(DDNS_HDR))){
    	if(_gDebug) printf("(buf == NULL) || ((size_t)buflen < sizeof(DDNS_HDR))");
      return (-1);
    }
    
    memset(buf, 0, sizeof(DDNS_HDR));
    hp = (DDNS_HDR *) buf;
    hp->id = htons(++query_id);
    hp->opcode = DNS_UPDATE;
    hp->qr = 0;
    hp->rd = 0;
    hp->rcode = NOERROR;
    cp = buf + sizeof(DDNS_HDR);
    buflen -= sizeof(DDNS_HDR);
    dpp = dnptrs;
    *dpp++ = (char *) buf;
    *dpp++ = NULL;
    lastdnptr = dnptrs + sizeof(dnptrs) / sizeof(dnptrs[0]);
    
    //add the zone section
    domain=(Uint8 *)MALLOC(256);
    sd = strchr(fqdn,'.');
    sd++;
    if (*sd){
    	strcpy(domain,sd);
    	if ((n = compressDnsName((Uint8 const *) domain, (Uint8 *) cp, buflen,
				 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
	    	return (-1);
	    }
	    
	    cp += n;
		buflen -= n;
    }
    FREE(domain);
    
    putShort(T_SOA, (Uint8 *) cp); //type
	cp += sizeof(Uint16);
	buflen -= sizeof(Uint16);
	
	putShort(C_IN, (Uint8 *) cp);//class: IN
	cp += sizeof(Uint16);
	buflen -= sizeof(Uint16);
	hp->zocount = htons(1);
	
    if(_gDebug){
	    printf("%s %d: after add zone section . zocount=%d\n", 
	    	__FUNCTION__,__LINE__,ntohs(hp->zocount));
	}
    // add the delete RR/RRset
    if ((n = compressDnsName((Uint8 const *) fqdn, (Uint8 *) cp, buflen,
				 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
		if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
	    return (-1);
	}
	cp += n;
	buflen -= n;
	
	putShort(T_A, (Uint8 *) cp); //type
	cp += sizeof(Uint16);
	buflen -= sizeof(Uint16);

/* changed by hqguan on Jan 9th 2003 , for avoid duplicated ip address per hostname	
	if(oldip != 0){//delete RR
		putShort(C_NONE, (Uint8 *) cp);//class: none
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(0, (Uint8 *) cp);  //ttl: 0
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		
		putShort(4, (Uint8 *) cp);//rdlength
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		//inetAtoN(oldip, &ip32);
		putLong(oldip, (Uint8 *) cp);  //rd
		cp += sizeof(Uint32);
		buflen -=  sizeof(Uint32);
	}
	else*/
	{//delete RRset
		putShort(C_ANY, (Uint8 *) cp);//class: any
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(0, (Uint8 *) cp);  //ttl: 0
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		
		putShort(0, (Uint8 *) cp);//rdlength
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
	}
	hp->upcount = htons(1);
	
	if(_gDebug){
	    printf("%s %d: after add delete RR/RRset \n", __FUNCTION__,__LINE__);
	}
	
	if(0 != newip){	//add the add RR
		if ((n = compressDnsName((Uint8 const *) fqdn, (Uint8 *) cp, buflen,
					 (Uint8 **) dnptrs, (Uint8 **) lastdnptr)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);	
		}
		cp += n;
		buflen -= n;
		
		putShort(T_A, (Uint8 *) cp); //type
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putShort(C_IN, (Uint8 *) cp);//class 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(ttl, (Uint8 *) cp);  //ttl
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		
		putShort(4, (Uint8 *) cp);//rdlength
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		//inetAtoN(newip, &ip32);
		putLong(ntohl(newip), (Uint8 *) cp);  //rd  . now the ip is stored in network order
		cp += sizeof(Uint32);
		buflen -=  sizeof(Uint32);
		
		
		hp->upcount = htons(2);
	}
	if(_gDebug){
	    printf("%s %d: after add add arr\n", __FUNCTION__,__LINE__);
	}
	//add the tsig RR
	if(_ddns_confs[*pindex].keyname[0]&&(_ddns_confs[*pindex].key[0]||_ddns_confs[*pindex].enkey[0])){
		Uint8 *prdlen,*pMAC,*md5up_b,*md5up_e;
		time_t _time;
		Uint16 t_time;
		unsigned long long ltime;
		hmacmd5_t hmd5;
		Uint8 *key;//,digest[MD5_DIGEST_LEN];
		//int i;//,keylen = ( strlen(_ddns_confs[*pindex].key) * 3 ) / 4;
		
		if(!data || datalen<MD5_DIGEST_LEN) {
			if(_gDebug) printf("!data || datalen<MD5_DIGEST_LEN");
			return -1;
		}
		
		key = (Uint8 *)MALLOC(sizeof(_keyBuf));
		memset(key,0,sizeof(_keyBuf));
		if (_ddns_confs[*pindex].key[0]){
			deBase64( key, _ddns_confs[*pindex].key);
		}else{
			int keylen = ( strlen(_ddns_confs[*pindex].enkey) * 3 ) / 4;
			struct rc4_key rc4key;
			
			deBase64( key, _ddns_confs[*pindex].enkey);
			prepare_key(_gKeyseed,KEYSEEDLEN,&rc4key);	
			rc4(key,keylen,&rc4key);
			
		}
		//for debug
		if (_gDebug){
			printf("keylen:%d,key:",sizeof(_keyBuf));
			//monitorDump("key", key, sizeof(_keyBuf), sizeof(_keyBuf));
			//monitorDump("_gKeyseed", _gKeyseed, KEYSEEDLEN+1,KEYSEEDLEN+1);
			printf("\n");
		}
		hmacmd5_init(&hmd5, (const unsigned char *)key, sizeof(_keyBuf));
		FREE(key);
		
		if ((n = compressDnsName((Uint8 const *) _ddns_confs[*pindex].keyname, (Uint8 *) cp, buflen,
					 NULL, NULL)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);
		}
		cp += n;
		buflen -= n;
		
		hmacmd5_update(&hmd5, (Uint8 *)hp, (cp-(Uint8 *)hp)); // dns message + tsig.(name)
		
		putShort(T_TSIG, (Uint8 *) cp); //type
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		md5up_b = cp;
		putShort(C_ANY, (Uint8 *) cp);//class 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		putLong(0, (Uint8 *) cp);  //ttl
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		md5up_e = cp;
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(class,ttl)
		
		
		prdlen = cp;
		putShort(0, (Uint8 *) cp);	//rdlength now to reserve a room and it will be filled again after fill RDATA
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		
		md5up_b = cp;
		if ((n = compressDnsName((Uint8 const *) "hmac-md5.sig-alg.reg.int", 
				(Uint8 *) cp, buflen, NULL, NULL)) < 0){
			if(_gDebug) printf("%d,compressDnsName<0",__LINE__);
		    return (-1);
		}
		cp += n;
		buflen -= n;
		
		GRT_Z(buflen,-1);
		
		//requestAbsoluteTime( &time );
		time( &_time );
		
		if((!_gTime) &&(_time < TIME_19_YEARS && (_gshowErr || _gDebug))){
			//I assume this box won't startup for around 5 years and still not get the time by sntp
			printf("%s %d:the time not set correctly !\n",__FUNCTION__,__LINE__);
		}
		//ltime = _gTime ? _gTime : localTime2EpochTime(time);(time + TIME90_70 + 3600*8);
		//ltime = _gTime ? _gTime : localTime2EpochTime(time)+_tzOffset;
		//ltime = _gTime ? _gTime : localTime2EpochTime(time) + _tzseed(_ddns_confs[*pindex].offset);
		ltime = _gTime ? _gTime : _time + 16*3600 - 28600;
		if(_gDebug){
			printf("time=%x,ltime=%x,_gTime=%x%x\n",
			_time,ltime,_gTime);
		}

		t_time = (ltime>>32);
		putShort(t_time, (Uint8 *) cp);  //time signed 1-2 byte
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		putLong(ltime, (Uint8 *) cp);  //time signed 3-6 byte
		cp += sizeof(Uint32);
		buflen -= sizeof(Uint32);
		GRT_Z(buflen,-1);

		putShort(TIME_FUDGE, (Uint8 *) cp);//Fudge
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		md5up_e = cp;
		GRT_Z(buflen,-1);
		
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(algo,time,fudge)
		
		putShort(MD5_DIGEST_LEN, (Uint8 *) cp);//MAC Size 
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		
		pMAC = cp;
		cp += MD5_DIGEST_LEN;
		buflen -= MD5_DIGEST_LEN;
		
		putShort(ntohs(hp->id), (Uint8 *) cp);//original ID
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		
		md5up_b = cp;
		putShort(0, (Uint8 *) cp);//Error
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		putShort(0, (Uint8 *) cp);//other len
		cp += sizeof(Uint16);
		buflen -= sizeof(Uint16);
		GRT_Z(buflen,-1);
		md5up_e = cp;
		
		hmacmd5_update(&hmd5, md5up_b, md5up_e-md5up_b); // + tsig.(Error,other len,other data)
		hmacmd5_sign(&hmd5, data);
		
		putShort(md5up_e-(prdlen+2),prdlen);
		
		memcpy(pMAC,data,MD5_DIGEST_LEN);
		
		hp->arcount = htons(1);
		
	}
	
    return (cp - buf);
 
#endif
}


/*
 * DDNS 更新主函数
 */

#if (DDNS_ADVANCE == FYES)
int updateRR_A(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl,char *interface)
#else
int updateRR_A(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl)
#endif
{
    //Uint8 *pbuf,*answer, *query_digest;	
    int n,index;
//    SlotNumber	slot;
//    const SlotProfile	*slotprof;
//    AbsoluteTime time;	
    time_t _time;	
	
	
    time(&_time);
//    requestAbsoluteTime( &time );

#if 0
    if((!_gTime) && (time < TIME_19_YEARS)) {
	//I assume this box won't startup for around 5 years and still not get the time by sntp
	if (_gshowErr||_gDebug) {
	    printf("%s %d:the time not set correctly !\n",__FUNCTION__,__LINE__);
	}

	if (wtimeTimer) {
	    untimeout2(wtimeTimer);
	    wtimeTimer = 0;
	    oldip = upPa.oldip; // it is very important for update accumulated
	}

	upPa.newip = newip;
	upPa.oldip = oldip;
	strcpy(upPa.fqdn, fqdn);
	upPa.ttl = ttl;
		
	wtimeTimer = timeout((ClockCallback *) ddns_wtime_expire, 
		    		(unsigned int)&upPa,
				NULL,
				50  //wait 5 seconds
				); 
		 
	//UNLOCK(sem_wk_cf);	
	return 0;
    }
#endif
		
    /*
     * 设置更新参数
     */
    upPa.newip = newip;
    upPa.oldip = oldip;
    strcpy(upPa.fqdn, fqdn);
    upPa.ttl = ttl;
	
    /* by guan : moved to ddnsInit
	
    pbuf = (Uint8*)MALLOC( MAXPACKET );
    answer = (Uint8*)MALLOC( MAXPACKET );
    query_digest = (Uint8*)MALLOC(MD5_DIGEST_LEN);
	
    memset(pbuf,0,MAXPACKET);
    memset(answer,0,MAXPACKET);
    memset(query_digest,0,MD5_DIGEST_LEN);*/

    /*
     * here this parameter seemed not logical to the concept of my original design, 
     * but now in profile only one hostname and different kind address is support so it working
     * badly program structure !!
     */
#if 0
    if(NULL == fqdn) {
	slot.slotNumber = ETHER_SLOT_NUMBER;
	slotprof = requestSlotProfilePointer(slot);
	fqdn = slotprof->cardType.etherSlot.ext.ddns.Hostname;
	releaseSlotProfilePointer(slotprof);
    }
#endif
	
    if( 0 == oldip) {
	oldip = lastnewip;
    }

    lastnewip1 = newip;
	
#if 0
    if(_gDebug) {
	printf("%s %d: before _makeUpdate oldip:%N, newip:%N, fqdn:%s, ttl:%d\n",
		__FUNCTION__,__LINE__,
		oldip, newip, fqdn, ttl);
    }
#endif
	
    n = _makeUpdate(newip, oldip, fqdn, ttl, query_digest, MD5_DIGEST_LEN, pbuf, MAXPACKET, &index);
	
    if (n <= 0) {
	if(_gDebug) {
	    printf("%s %d: after _makeUpdate ret = %d \n", __FUNCTION__,__LINE__, n);
	}
	    return (n);
    }
	
    if(_gDebug) {
	printf("request dns msg:\n");
    }
    
    ddns_cli_request_ini(pbuf, n, answer, MAXPACKET, index, query_digest, MD5_DIGEST_LEN);
	
#if (DDNS_ADVANCE == FYES)
    n = ddns_cli_request_send(interface);    
#else
    n = ddns_cli_request_send();
#endif

    return n;	
}

#if (DDNS_ADVANCE == FYES)
int updateRR_A1(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl,char *interface)
#else
int updateRR_A1(Uint32 newip, Uint32 oldip, const char * fqdn, int ttl)
#endif
{
    int ret;
	
//    LOCK(sem_wk_cf);
	
#if (DDNS_ADVANCE == FYES)
    ret = updateRR_A(newip, oldip, fqdn, ttl,interface);
#else
    ret = updateRR_A(newip, oldip, fqdn, ttl);
#endif
	
//    UNLOCK(sem_wk_cf);	

    return ret;
}
#if 0
int updateRR_A2(const char *profile, Uint32 newip, Uint32 oldip, const char * fqdn, int ttl)
{
    SlotNumber	slot;
    const SlotProfile	*slotprof;
	
    slot.slotNumber = ETHER_SLOT_NUMBER;
    slotprof = requestSlotProfilePointer(slot);
	
    if(_gDebug) {
	printf("called from updateRR_A2 newip %N oldip %N fqdn %s  ttl %d\n",newip, oldip, fqdn, ttl);
    }

    if (slotprof) {
	if(!strcmp(slotprof->cardType.etherSlot.ext.ddns.Profile, profile)) {
	    updateRR_A1(newip, oldip, slotprof->cardType.etherSlot.ext.ddns.Hostname, ttl);
	}

	releaseSlotProfilePointer(slotprof);
    }

    return 0;
}
#endif

#if (DDNS_ADVANCE == FYES)
extern Int ddns_update_iplink(char *domain, Uint32 ipaddress,char *interface)
#else
extern Int ddns_update_iplink(char *domain, Uint32 ipaddress)
#endif
{
//    strcpy(dateString, "begin to update iplink ddns");
    //brwang
//    ddns_load_iplink_state(dateString, DDNS_UPDATE_DOING);
    /*
     * 64 is edited by brwang for wlan
     * old is 10
     */

#if (DDNS_ADVANCE == FYES)
    return updateRR_A1(ipaddress, 0, domain, 64,interface);
#else
    return updateRR_A1(ipaddress, 0, domain, 64);
#endif
}

//this function should be called in Critical Section
int ddns_configure(const char *hostname, 
	Uint32 server_ip0, int server_port0,
	Uint32 server_ip1, int server_port1,
	const char *keyname, const char *key, const char *enkey,
	int update_retry, int update_timeout, int update_interval, 
	Uint8 enable, Uint32 offset)
{
    int i,empty_i = 0;
	
    /*
     * 设置重试时间，最小为30，如果为0，则不重试。
     */
    if ((update_retry < MIN_DDNS_UPDATE_RETRY) && (update_retry != 0) ) { 
	update_retry = MIN_DDNS_UPDATE_RETRY;
    }

    /*
     * 设置超时时间，最小为5s
     */
    if (update_timeout < MIN_DDNS_UPDATE_TIMEOUT)  {
	update_timeout = MIN_DDNS_UPDATE_TIMEOUT;
    }
	
#if 0
    // lock in outer LOCK(sem_wk_cf);
    for(i = 0; i < MAXCONFS; i++) {
	if (_ddns_confs[i].hostname && !strcmp(_ddns_confs[i].hostname, hostname)) {
	    switch (op){
	    case add:
		//UNLOCK(sem_wk_cf);
		return -1;
	    case del:
		//int j;
					
		if (NULL != _ddns_confs[i].hostname) FREE(_ddns_confs[i].hostname);
		//for(j = 0; j<SERVERS_PER_IP; j++)
		//	if (NULL != _ddns_confs[i].server_ip[j])
		//		FREE(_ddns_confs[i].server_ip[j]);
					
		if (NULL != _ddns_confs[i].keyname) FREE(_ddns_confs[i].keyname);
		if (NULL != _ddns_confs[i].key)  FREE(_ddns_confs[i].key);
		if (NULL != _ddns_confs[i].enkey)  FREE(_ddns_confs[i].enkey);
					
		memset(&_ddns_confs[i],0,sizeof(struct ddns_conf));
			
		//UNLOCK(sem_wk_cf);
		return 0;
	    }
	}
	if (NULL == _ddns_confs[i].hostname) empty_i = i;
    }
    if (MAXCONFS == i) {// this if condition seems no need
	switch (op){
	case add:
	    if (MAXCONFS == empty_i) {
		//UNLOCK(sem_wk_cf);
		return -1; // no room 
	    }
#endif
	    _ddns_confs[empty_i].hostname = MALLOC(strlen(hostname)+1);
	    strcpy(_ddns_confs[empty_i].hostname, hostname);
				
	    _ddns_confs[empty_i].servers = 0;
		
	    _ddns_confs[empty_i].server_ip[0] = 0;
	    if (0 != server_ip0) {
		_ddns_confs[empty_i].servers++;
		_ddns_confs[empty_i].server_ip[0] = server_ip0;
		//strcpy(_ddns_confs[empty_i].server_ip[0], server_ip0);
	    }
				
	    _ddns_confs[empty_i].server_port[0] = server_port0;
				
	    _ddns_confs[empty_i].server_ip[1] =0;
	    if (0 != server_ip1) {
		_ddns_confs[empty_i].servers++;
		_ddns_confs[empty_i].server_ip[1] = server_ip1;					
	    }
				
	    _ddns_confs[empty_i].server_port[1] = server_port1;
				
	    _ddns_confs[empty_i].keyname = MALLOC(strlen(keyname) + 1);
	    strcpy(_ddns_confs[empty_i].keyname, keyname);
				
	    _ddns_confs[empty_i].key = MALLOC(strlen(key) + 1);
	    strcpy(_ddns_confs[empty_i].key, key);

	    _ddns_confs[empty_i].enkey = MALLOC(strlen(enkey) + 1);
	    strcpy(_ddns_confs[empty_i].enkey, enkey);
				
	    _ddns_confs[empty_i].update_retry = update_retry;
	    _ddns_confs[empty_i].update_timeout = update_timeout;
				
	    _ddns_confs[empty_i].update_interval = update_interval;
				
	    _ddns_confs[empty_i].enable = enable;
				
	    _ddns_confs[empty_i].offset = offset;
#if 0
	    //UNLOCK(sem_wk_cf);
	    return 0;
	case del:
	    //UNLOCK(sem_wk_cf);
	    return 0;  //no matter if it existed, now no
	}
    }
#endif

    //UNLOCK(sem_wk_cf);
    return 0;
}

int skipRR(Uint8 **pRR, Uint8 *msg)
{
	//Uint8 *p=(*pRR);
	int rdlen,type,MAClen,otherlen,i;
//	char f[20];
	
	if ((*pRR =_skipCompName(*pRR, msg)) == NULL)
      return (-1);	/* compression error */
    type=getShort(*pRR);
	//printf("TYPE: %d ",type);
	*pRR+=2;
	//printf("CLASS: %d ",getShort(p));
	*pRR+=2;		
	//printf("TTL: %d ",getLong(p));
	*pRR+=4;	
	rdlen = getShort(*pRR);
	*pRR+=2;	
	//printf("RDLENGTH: %d ",rdlen);		
	
	if(rdlen){
		switch(type){
		case T_A:
			//printf("ip:%N\n",getLong(p));
			*pRR+=4;
			break;
		case T_TSIG:
			//printf("Algorithm Name:");
			if ((*pRR = _printCompName(*pRR, msg)) == NULL)
      			return (-1);	/* compression error */
      		//printf(" Time Signed %d",getLong(p));
      		//p+=2;
      		//printf("%d ",getLong(p));
      		//p+=4;
      		*pRR += 6;
      		//printf("Fudge: %d",getShort(p));
      		*pRR +=2;
      		MAClen=getShort(*pRR);
      		//printf("MAC Size: %d MAC: ",MAClen);
      		*pRR+=2;
      		//the MAClen should be 16, so
      		for(i=0;i<MAClen/4;i++,*pRR+=4){
      			//printf("%x",getLong(p));
      		}
      		//printf("\n");
      		//printf("Original ID %d\n",getShort(p));
      		*pRR+=2;
      		//printf("Error %d\n",getShort(p));
      		*pRR+=2;
      		otherlen = getShort(*pRR);
      		//printf("Other len %d\n",otherlen);
      		*pRR+=2;      		
      		     
      		*pRR += otherlen;
      		 			
			break;
		default:
			//sprintf(f,"RD: %%ds\n",rdlen);	
			//printf(f,p);
			*pRR += rdlen;
			break;
		}
	}
	else{
		//printf("\n");
	}
		
	//*pRR = p;
	return 0;
}

int locateSection(Uint8 **p, Uint8 *msg, enum section_t t)
{
	DDNS_HDR *hd=(DDNS_HDR *)msg;
	//Uint8 *p=*pp;
	int i;
	
	(*p)=(Uint8 *)(hd+1);
	if (ZONE_SEC == t) return 0;
	for(i=0;i<ntohs(hd->zocount);i++){
		if (((*p) = _skipCompName(*p, msg)) == NULL)
      		return 0;	/* compression error */
		*p+=4;		//skip ztype zclass
	}
	if (PRE_SEC == t) return 0;
	//printf("PRE SECTION:");
	for(i=0;i<ntohs(hd->prcount);i++){
		skipRR(p,msg);
	}
	if (UP_SEC == t) return 0;
	//printf("UP SECTION:");
	for(i=0;i<ntohs(hd->upcount);i++){
		skipRR(p,msg);
	}
	if (AR_SEC == t) return 0;
	//printf("ADD SECTION:");
	for(i=0;i<ntohs(hd->arcount);i++){
		skipRR(p,msg);
	}
	printf("\n");
	return 0;
}

static void  ddns_get_localtime(char* tmp, int gmt)
{
    time_t _time;
    time(&_time);
    sprintf(tmp, "%d", (_time));
}

#if (DDNS_ADVANCE == FYES)
void ddns_load_iplink_state(char *dateString,char *interface)
#else
void ddns_load_iplink_state(char *dateString)
#endif
{
    FILE *fp;
#if (DDNS_ADVANCE == FYES)
    char logfile[35];
    sprintf(logfile,"/tmp/ddns/ddns_iplink_%s.log",interface);
    fp = fopen(logfile, "w+");
#else
    fp = fopen(DDNS_IPLINK_LOG, "w+");
#endif
    fprintf(fp, "%s", dateString);
    fclose(fp);
}
/* for fail */
static 
void logUpdateTime_f(void)
{
    char tmp[80];
    
    //ddns_get_localtime(tmp, TRUE);

    strcat(dateString,"Last update: ");
    strcat(dateString,tmp);		
    strcat(dateString,"\r\n Hostname: ");
    strcat(dateString,_ddns_confs[cmPa.index].hostname);
    strcat(dateString,"\r\n ddns update result : fail . " );		      
    strcat(dateString,"\r\n");
//    ddns_load_iplink_state(dateString);
}

/* for send req */

#if (DDNS_ADVANCE == FYES)
static void logSendTime(char *interface)
#else
static void logSendTime(void)
#endif
{
    char tmp[80];

    memset(dateString,0,sizeof(dateString));
    strcat(dateString,"start update\n");
    ddns_get_localtime(tmp, TRUE);
    strcat(dateString,tmp);			      
    strcat(dateString,"\n");	
//    ddns_get_localtime(tmp, TRUE);
//    strcpy(dateString, "Have sent request, Waiting reply...  ");
//    strcat(dateString, tmp);		
//    strcat(dateString,"\r\n Hostname: ");
//    strcat(dateString,_ddns_confs[cmPa.index].hostname);
//    strcat(dateString,"\r\n");

#if (DDNS_ADVANCE == FYES)
    ddns_load_iplink_state(dateString,interface);
#else
    ddns_load_iplink_state(dateString);
#endif
}

static 

#if (DDNS_ADVANCE == FYES)
int logUpdateTime(char *interface)
#else
int logUpdateTime(void)
#endif
{
    char tmp[80],tmp1[32];
    struct in_addr addr;
	
    lastnewip = lastnewip1;   // record the last success update ip address

    memset(dateString,0,sizeof(dateString));
    strcat(dateString,"success\n");			      
    ddns_get_localtime(tmp, TRUE);
    strcat(dateString,tmp);			      
    strcat(dateString,"\n");	
    addr.s_addr = lastnewip;
    snprintf(tmp1,32,"%s\n",inet_ntoa(addr));
    strcat(dateString,tmp1);
    strcat(dateString,_ddns_confs[cmPa.index].hostname);
    strcat(dateString,"\n");	

#if (DDNS_ADVANCE == FYES)
    ddns_load_iplink_state(dateString,interface);
#else
    ddns_load_iplink_state(dateString);
#endif

    return 0;
}

int set_usage()
{
    printf("Usage example: \n");
    printf("iplink wanIF domain enkey \n");
    printf("iplink eth2.2 utt.iplink.com.cn enkey \n");
    return 0;
}

int getIfIpInaddr(char *ifname, struct in_addr *ifaddr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("getIfIpInaddr: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfIpInaddr: ioctl SIOCGIFADDR error for %s"), ifname);
		return -1;
	}
        ifaddr->s_addr = (((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr).s_addr;
//	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

extern unsigned int getSystemSn(void);
extern int ProfInit(void);

#if (DDNS_ADVANCE == FYES)
static void create_pidfile(char *interface)
{
    FILE *pidfile;
    char pidfilename[128];
    sprintf(pidfilename, "/var/run/iplink_%s.pid", interface);
    
    if ((pidfile = fopen(pidfilename, "w")) != NULL) {
                fprintf(pidfile, "%d\n", getpid());
                (void) fclose(pidfile);
    } else {
                printf("Failed to create pid file %s: %m", pidfilename);
                pidfilename[0] = 0;
    }   
}
#endif
int main(int argc,char** argv)
{
    /*
     * need parse conf file
     */
    struct hostent *host = NULL;
    struct in_addr servaddr, oweraddr;
    int i=0;
    int fd;
#if (DDNS_ADVANCE == FYES)
    char interface[7];
#endif
#if 1
    ProfInit();
#else
#if ((CONFIG_X86_CPU) == 1)
    unsigned   int sn;
#else
    volatile unsigned   int *sn;
#endif
#endif
    void * map_base;
    if(argc != 4) {
        return set_usage();   
    }


    while(1){
	host = gethostbyname(IPLINK_HOST_DOMAIN);
	if (host == NULL) {
	    printf("host[NULL],wait 1 sec\n");
	    sleep(1);
	    i++;
	    if(i<10)
		continue;
	    else
		exit(0);
	}   
	break;
    }   
    //inet_aton("202.101.35.232",&servaddr);
    servaddr = *(struct in_addr *)(host->h_addr);
	
    lk = ulk = 0;
	
    _tzOffset = TIME_OFFSET;
#if (DDNS_ADVANCE == FYES)
    strcpy(interface,argv[1]);
    create_pidfile(interface);
#endif
    pbuf = (Uint8 *)MALLOC(MAXPACKET);
    answer = (Uint8 *)MALLOC(MAXPACKET);
    query_digest = (Uint8 *)MALLOC(MD5_DIGEST_LEN);
    
	
    while(1){
    if(getIfIpInaddr(argv[1], &oweraddr) == -1) {
        continue;
//        return 0;
    }
    if(lastnewip != oweraddr.s_addr)
    {
    memset(pbuf,0,MAXPACKET);
    memset(answer,0,MAXPACKET);
    memset(query_digest,0,MD5_DIGEST_LEN);
		
    memset(_ddns_confs, 0, MAXCONFS*sizeof(struct ddns_conf));
	
#if 0
    ddns_configure( "7344555.iplink.com.cn",
            servaddr.s_addr, 53,
            servaddr.s_addr, 53,
            "han", "", "zVWe0t0o9y5Xc/Q8uekau81fu9hWH/eL15VZ27E1U3IC",
            30, 5, 0, 1, 1);
#endif

    ddns_configure( argv[2],
            servaddr.s_addr, 53,
            servaddr.s_addr, 53,
            "han", "", argv[3],
            30, 5, 0, 1, 1);
    memset(_gKeyseed,0,KEYSEEDLEN);
#if 1
    snprintf((char *)_gKeyseed,KEYSEEDLEN,SSEED_KEY_FMT, 
	    getSystemSn(),getSystemSn() , _skseed(1), argv[2]);
#else
#if ((CONFIG_X86_CPU) == 1)
    fd = open("/dev/sda4", O_RDWR, 0666);
    if (fd < 0) {
        fprintf(stderr, "Could not open config file\n");
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    read(fd, &sn, 4);
    close(fd);
    snprintf((char *)_gKeyseed,KEYSEEDLEN,SSEED_KEY_FMT, 
		sn, sn, _skseed(1), argv[2]);
#else
    if((fd=open("/dev/mem",O_RDWR|O_SYNC))==-1){
        return(-1);
    }
    map_base=mmap(0,0xff,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xbf000000);
    sn = (volatile unsigned int *) (map_base+0x2c);
    close(fd);
    snprintf((char *)_gKeyseed,KEYSEEDLEN,SSEED_KEY_FMT, 
		*sn, *sn, _skseed(1), argv[2]);
#endif
#endif

#if (DDNS_ADVANCE == FYES)
    ddns_update_iplink(_ddns_confs[0].hostname,oweraddr.s_addr,interface);
#else
    ddns_update_iplink(_ddns_confs[0].hostname,oweraddr.s_addr);
#endif
    }
    sleep(10);
    }
}
