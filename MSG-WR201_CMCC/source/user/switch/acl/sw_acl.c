
/*
 *
 *配置及读取ACL的命令
 *
 *
 *KeHuatao
 *
 *2011-09-21
 *
 */
#include  <stdio.h>
#include  <string.h>
#include  <netinet/in.h>
#include  <arpa/inet.h> /*inet_addr*/
#include  <getopt.h>
#include  <linux/switch/swioc_common.h>
#include  <linux/switch/switch.h>
#include  <linux/types.h>
#include  <sys/types.h>

#include    <mib.h>
#include    <profacce.h>
#include    <a2n.h>
#include    <sw_ioctl.h>


#if (FEATURE_ACL == FYES) 
#define   OPT_BIT_A        0x01 
#define   OPT_BIT_D        0x02
#define   OPT_BIT_H        0x04
#define   OPT_BIT_SIP      0x08
#define   OPT_BIT_DIP      0x10
#define   OPT_BIT_SPORT    0x20
#define   OPT_BIT_DPORT    0x40
#define   OPT_BIT_ACTION   0x80
#define   OPT_BIT_SOURCE   0x100
#define   OPT_BIT_SHOW     0x200
#define   OPT_BIT_ENABLE   0x400
#define   OPT_BIT_DISABLE  0x800

#define   SOURCE_ALL_PORTS 32


static int is_digit(char *str);
static void usage(const char *p);
static void print_acl(sw_acl_t *acl);
static void print_space(int ct);
static void show_acl(int index);
static void print_head(void);
static void acl_profile_add(sw_acl_t *acl);
static void acl_profile_del(int id);
static void acl_status_profile(int status);
static int check_optmask(unsigned int optmask);
static int check_actions(char *str);
static void active_acl(unsigned int optmask, sw_acl_t *acl);

#if 0
LcdParamEnumValue acl_actions[MAX_ACL_ACTION+2]={
    {"NONE", ACL_NONE},
    {"forward", ACL_FORWARD},
    {"cpu", ACL_CPU},
    {"drop", ACL_DROP},
    {"queue0", ACL_QUEUE0},
    {"queue1", ACL_QUEUE1},
    {"queue2", ACL_QUEUE2},
    {"queue3", ACL_QUEUE3},
    {"QinQTag1", ACL_QINQ1},
    {"QinQTag2", ACL_QINQ2},
    {"QinQTag3", ACL_QINQ3},
    {"QinQTag4", ACL_QINQ4},
    {NULL, 0},
};
#endif

int main(int argc, char* argv[])
{
    int ch = 0, longindex = 0, value = 0;
    unsigned int optmask=0u;
    char *short_option="a:d:s:h";
    in_addr_t ip;
    sw_acl_t acl;
    struct option long_option[] = {  
	{"sip", 1, NULL, '1'},  
	{"dip", 1, NULL, '2'},  
	{"sport", 1, NULL, '3'},  
	{"dport", 1, NULL, '4'},  
	{"action", 1, NULL, '5'},  
	{"enable", 0, NULL, '6'},  
	{"disable", 0, NULL, '7'},  
	{"show", 1, NULL, '8'},  
	{NULL, 0, NULL, 0}
    };

    ProfInit();
    memset(&acl, 0, sizeof(sw_acl_t));
    opterr = 0; // prevent error information to print for unrecognized options   
    while( ( ch = getopt_long(argc, argv, short_option, long_option, &longindex) ) != -1 )  
    {
	switch(ch)
	{
	    case 'd':/*add*/
		optmask |= OPT_BIT_D;
		/*delete all*/
		if(strcmp(optarg, "all")==0)
		{
		    acl.idx = -1;
		    break;
		}
	    case 'a':/*del*/
		if(ch=='a') {
		    optmask |= OPT_BIT_A;
		}
		/*必须是第一个参数*/
		if(optind!=3) {
		    /*参数不对*/
		    goto out;
		}
		if(is_digit(optarg)==1)
		{
		    value = strtoul(optarg, NULL, 10);
		    if((value>0) && (value<=SW_MAX_ACL))
		    {
			acl.idx = value;
		    }
		    else 
		    {
			goto out;
		    }
		}
		else{
		    goto out;
		}
		break;
	    case 's':/*source entry*/
		optmask |= OPT_BIT_SOURCE;
		if(is_digit(optarg)==1)
		{
		    value = strtoul(optarg, NULL, 10);
		    if((value>0) && (value<=PORT_COUNT))
		    {
			acl.source_entry = value;
		    }
		    else {
			goto out;
		    }
		}
		else {
		    goto out;
		}
		break;
	    case '1':/*sip*/
	    case '2':/*dip*/
		ip = inet_addr(optarg);
		if(ip != -1)
		{
		    if(ch=='1') {
			optmask |= OPT_BIT_SIP;
			acl.ipv4.sip = (unsigned int)ip;
		    }
		    else{
			optmask |= OPT_BIT_DIP;
			acl.ipv4.dip = (unsigned int)ip;
		    }
		    if(optind<argc)
		    {
			/*shoud be digit*/
			if(is_digit(argv[optind])==1)
			{
			    /*should be 0,4,8,12,16,20,24,32*/
			    value = strtoul(argv[optind], NULL, 10);
			    if( ((value%4)!=0) || (value<0)
				    || ( ((value/4)>6) && (value!=32) )
			      )
			    {
				goto out;
			    }
			    else
			    {
				if(ch=='1'){
				    acl.ipv4.sip_mask_width = value;
				}
				else{
				    acl.ipv4.dip_mask_width = value;
				}
			    }
			}
			else{
			    goto out;
			}
			optind++;
		    }
		    else{
			goto out;
		    }
		}
		else{
		    goto out;
		}
		break;
	    case '3':/*sport*/
	    case '4':/*dport*/
		if(is_digit(optarg)==1)
		{
		    value = strtoul(optarg, NULL, 10);
		    if(value<=65535) {
			if(ch=='3')
			{
			    optmask |= OPT_BIT_SPORT;
			    acl.ipv4.sport = value;
			}
			else{
			    optmask |= OPT_BIT_DPORT;
			    acl.ipv4.dport = value;
			}
		    }
		    else{
			goto out;
		    }
		}
		else{
		    goto out;
		}
		break;
	    case '5':/*action*/
		optmask |= OPT_BIT_ACTION;
		value = check_actions(optarg);
		if(value!=-1)
		{
		    acl.action = value;
		}
		else{
		    goto out;
		}
		break;
	    case '6':/*enable*/
		optmask |= OPT_BIT_ENABLE;
		break;
	    case '7':/*disable*/
		optmask |= OPT_BIT_DISABLE;
		break;
	    case '8':/*show*/
		optmask |= OPT_BIT_SHOW;
		if(strcmp(optarg, "all")==0)
		{
		    value = SOURCE_ALL_PORTS;
		}
		else
		{
		    if(is_digit(optarg)==1)
		    {
			value = strtoul(optarg, NULL, 10);
			if((value<=0) || (value>SW_MAX_ACL))
			{
			    goto out;
			}
		    }
		    else{
			goto out;
		    }
		}
		break;
	    default:
		printf("unknow option %s\n", argv[optind-1]);
		break;
	}
    }
    if(check_optmask(optmask)<0)
    {
	goto out;
    }

    if((optmask & OPT_BIT_SHOW)>0)
    {
	show_acl(value);
    }
    else
    {
	active_acl(optmask, &acl);
    }

    ProfDetach();
    return 0; 
out:
    usage("sw_acl");
    ProfDetach();

    return 0;
}

/*
 *是否全为数字
 *
 */
static int is_digit(char *str)
{
    int ret=1;
    char *p = str;

    if(p!=NULL)
    {
	/*str*/
	while(*p!='\0') 
	{
	    /*range*/
	    if((*p<'0') || (*p>'9'))
	    {
		/*not digit*/
		ret = 0;
		break;
	    }
	    p++;
	}
    }

    return ret;
}

/*
 *获取action
 *
 *
 */
static int check_actions(char *str)
{
    int i, ret=-1;

    /*action*/
    for(i=1;i<MAX_ACL_ACTION;i++)
    {
	/*compare*/
	if(strcmp(str, acl_actions[i].enumName)==0)
	{
	    /*get it*/
	    ret = acl_actions[i].enumValue;
	    break; 
	}
    }

    return ret;
}
/*
 *check the optmask
 *
 *
 */
static int check_optmask(unsigned int optmask)
{
    int ret = 0;

    if( (((optmask & OPT_BIT_ENABLE)>0) && (optmask != OPT_BIT_ENABLE))
	    || (((optmask & OPT_BIT_DISABLE)>0) && (optmask != OPT_BIT_DISABLE))
	    || (((optmask & OPT_BIT_SHOW)>0) && (optmask != OPT_BIT_SHOW))
	    || (((optmask & OPT_BIT_D)>0) && (optmask != OPT_BIT_D))
      )
    {
	ret = -1;
    }
    else
    {
	if( ((optmask & OPT_BIT_ENABLE)==0) && ((optmask & OPT_BIT_DISABLE)==0)
		&& ((optmask & OPT_BIT_SHOW)==0) && ((optmask & OPT_BIT_D)==0)
	  )
	{
	    if( ((optmask & OPT_BIT_ACTION)==0) || ((optmask & OPT_BIT_A)==0))
	    {
		ret = -1;
	    }
	}
    }

    return ret;
}
/*
 *show acl
 *
 */
static void show_acl(int index)
{
    int i, enable=0;
    sw_acl_t acl;

    /*show status*/
    sw_acl_status_get(&enable);
    print_space(4);
    printf("ACL status:  ");
    if(enable==0)
    {
	printf("disable\n");
    }
    else
    {
	printf("enable\n");
    }

    /*print head*/
    print_head();

    /*show all*/
    if(index==SOURCE_ALL_PORTS)
    {
	for(i=1;i<=SW_MAX_ACL;i++)
	{
	    memset(&acl, 0, sizeof(sw_acl_t));
	    acl.idx = i;
	    sw_acl_get(&acl);
	    print_acl(&acl);
	    printf("\n");
	}
    }
    else
    {
	/*show one acl*/
	if((index>0) && (index<=SW_MAX_ACL))
	{
	    memset(&acl, 0, sizeof(sw_acl_t));
	    acl.idx = index;
	    sw_acl_get(&acl);
	    print_acl(&acl);
	    printf("\n");
	}
    }
}


static void usage(const char *p)
{
    printf("Usage:\
	    %s  [--enable>]\n\
	            [--disable]\n\
	            [-a <index>]\n\
	            [-d <index | all>]\n\
	            [-s <source port>]\n\
	            [--sip <source IP> <netmask width>]\n\
	            [--dip <destination IP> <netmask width>]\n\
	            [--sport <TCP/UDP source port>]\n\
	            [--dport <TCP/UDP destination port>]\n\
	            [--action <drop | forward>]\n\
	            [--show <index | all>]\n\
	                netmask width should be 0|4|8|12|16|20|24|32\n\
	                source port should be 1~24\n\
	                index should be 1~16\n\n", p);

    return;
}
/*
 *print some space
 *
 */
static void print_space(int ct)
{
    int i;

    for(i=0;i<ct;i++)
    {
	/*printf*/
	printf(" ");
    }

    return;
}
/*
 *print acl
 *
 *
 */
static void print_acl(sw_acl_t *acl)
{
    char str[64];
    struct in_addr inaddr;

    print_space(4);
    printf("%-9d", acl->idx);

    if(acl->source_entry==SOURCE_ALL_PORTS)
    {
	/*all ports*/
	printf("all");
    }
    else
    {
	printf("%-3d", acl->source_entry+1);
    }
    print_space(5);

    /*print sip and mask*/
    memset(str, 0, sizeof(str));
    inaddr.s_addr = htonl(acl->ipv4.sip); 
    sprintf(str, "%s/%d", inet_ntoa(inaddr), acl->ipv4.sip_mask_width);
    printf("%-22s", str);

    /*print dip and mask*/
    memset(str, 0, sizeof(str));
    inaddr.s_addr = htonl(acl->ipv4.dip); 
    sprintf(str, "%s/%d", inet_ntoa(inaddr), acl->ipv4.dip_mask_width);
    printf("%-22s", str);

    /*port*/
    printf("%-5d", acl->ipv4.sport);
    print_space(4);
    printf("%-5d", acl->ipv4.dport);
    print_space(4);

    /*action*/
    if((acl->action>=0) && (acl->action<=MAX_ACL_ACTION))
    {
	printf("%s", acl_actions[acl->action]);
    }
    else
    {
	printf("unknow");
    }

    return;
}

/*
 *print the head
 *
 *
 */
static void print_head(void)
{
    print_space(4);

    printf("index");
    print_space(4);

    printf("port");
    print_space(4);

    printf("sip");
    print_space(19);

    printf("dip");
    print_space(19);

    printf("sport");
    print_space(4);

    printf("dport");
    print_space(4);

    printf("action");

    printf("\n");

    return;
}

/*
 *根据输入参数配置acl
 *
 */
static void active_acl(unsigned int optmask, sw_acl_t *acl)
{
    /*enable*/
    if((optmask & OPT_BIT_ENABLE)>0)
    {
	acl_status_profile(1);
    }
    else
    {
	/*disable*/
	if((optmask & OPT_BIT_DISABLE)>0)
	{
	    acl_status_profile(0);
	}
    }

    /*add or del*/
    if((optmask & OPT_BIT_A)>0)
    {
	/*add*/
	acl_profile_add(acl);
    }
    else
    {
	if((optmask & OPT_BIT_D)>0)
	{
	    /*del*/
	    acl_profile_del(acl->idx);
	}
    }

    return;
}

/*
 *add acl to profile
 *
 *
 */
static void acl_profile_add(sw_acl_t *acl)
{
    int i, min=0, max=0;
    char name[UTT_INST_NAME_LEN+1];
    AclProfile *prof;
    MibProfileType mibType = MIB_PROF_ACL;
    struProfAlloc *profList= NULL;

    ProfInstLowHigh(mibType, &max, &min);
    i = acl->idx-1;
    if(i>=min && (i<max))
    {
	/*get point*/
	prof = ProfGetInstPointByIndex(mibType, i);
	if(prof!=NULL)
	{
	    /*backup*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_ADD, i, &profList);

	    memset(name, 0, sizeof(name));
	    sprintf(name, "acl%d", acl->idx);
	    ProfSetNameByPoint(prof, name);
	    /*add*/
	    prof->head.active = TRUE;
	    prof->action_type = acl->action;
	    prof->source = acl->source_entry;
	    prof->sip = acl->ipv4.sip;
	    prof->sip_mask = acl->ipv4.sip_mask_width;
	    prof->dip = acl->ipv4.dip;
	    prof->dip_mask = acl->ipv4.dip_mask_width;
	    prof->sport = acl->ipv4.sport;
	    prof->dport = acl->ipv4.dport;
	    /*update*/
	    ProfUpdate(profList);
	    ProfFreeAllocList(profList);
	    /*write*/
	    nvramWriteCommit();
	}
    }

    return;
}

/*
 *del acl
 *
 */
static void acl_profile_del(int idx)
{
    int i, min=0, max=0;
    MibProfileType mibType = MIB_PROF_ACL;
    struProfAlloc *profList= NULL;

    /*range*/
    if((idx>0) && (idx<=SW_MAX_ACL))
    {
	/*delete one acl*/
	min = idx-1;
	max = idx;
    }
    else
    {
	if(idx==-1)
	{
	    /*delete all*/
	    ProfInstLowHigh(mibType, &max, &min);
	}
    }

    /*delete*/
    for(i=min;i<max;i++)
    {
	/*backup*/
	ProfBackupByIndex(mibType, PROF_CHANGE_DEL, i, &profList);
	ProfDelInstByIndex(mibType, i);
    }

    /*update*/
    ProfUpdate(profList);
    ProfFreeAllocList(profList);
    /*write to flash*/
    nvramWriteCommit();

    return;
}
/*
 *acl status
 *
 *KeHuatao
 */
static void acl_status_profile(int status)
{
    AclStatusProfile *prof;
    MibProfileType mibType = MIB_PROF_ACL_STATUS;
    struProfAlloc *profList= NULL;

    /*get point*/
    prof = (AclStatusProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
	/*changed*/
	if(prof->head.active!=status)
	{
	    /*backup*/
	    ProfBackupByIndex(mibType, PROF_CHANGE_EDIT, 0, &profList);
	    prof->head.active=status;

	    /*update*/
	    ProfUpdate(profList);
	    ProfFreeAllocList(profList);
	    nvramWriteCommit();
	}
    }   

    return;
}
#endif
