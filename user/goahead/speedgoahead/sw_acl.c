
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/autoconf.h>

#include "uttMachine.h"
#include "typedef.h"
#include "mib.h"
#include "mibunion.h"
#include "profacce.h"
#if (FEATURE_ACL == FYES)
#include <linux/switch/swioc_common.h>
#include <sw_ioctl.h>
#define DEBUG_ACL 0
#if DEBUG_ACL
#define ACL_DEBUG(fmt, argx...) printf(fmt, ##argx)
#else
#define ACL_DEBUG(fmt, argx...)
#endif




static void acl_rule_init(int index, int is_del);
static void acl_init(int is_del);
static void acl_status_init(void);

/*
 *profile changed
 *
 *KeHuatao
 */
static void aclProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType1, Uint32 index,
         Pointer*oldProfileDataPtr,  Pointer*newProfileDataPtr )
{
    ACL_DEBUG("%s\n", __func__);
    switch(changeType1) {
        case PROF_CHANGE_ADD:/*add acl*/
        case PROF_CHANGE_EDIT:/*edit acl*/
            acl_rule_init((int)index, 0);/*init*/
	    break;
        case PROF_CHANGE_DEL:/*delete*/
            acl_rule_init((int)index, 1);
	    break;
        case PROF_CHANGE_DELALL:/*delete all*/
	    acl_init(1);
	    break;
        default:/*unknow*/
	    break;
    }
}
/*
 *acl status profile changed
 *
 *
 */
static void aclStatusProfileChange(Pointer tag, MibProfileType profileType,
        ProfChangeType changeType1, Uint32 index,
         Pointer*oldProfileDataPtr,  Pointer*newProfileDataPtr)
{
    ACL_DEBUG("%s\n", __func__);
    switch(changeType1) {
        case PROF_CHANGE_EDIT:/*change status*/
            acl_status_init();/*init*/
	    break;
        default:/*unknow*/
	    break;
    }

}

/*
 *inital acl status
 *
 */
static void acl_status_init(void)
{
    AclStatusProfile *prof;
    MibProfileType mibType = MIB_PROF_ACL_STATUS;

    ACL_DEBUG("%s\n",__func__);
    /*get pointer*/
    prof = (AclStatusProfile*)ProfGetInstPointByIndex(mibType, 0);
    if(prof != NULL)
    {
	/*free or not*/
        if(ProfInstIsFree(((Pointer*)prof)) == 0)
	{
	    if(prof->head.active == TRUE) {
		/*enable*/
		sw_acl_enable(1);
	    }
	    else {
		/*disable*/
		sw_acl_enable(0);
	    }
	}
    }
}

/*
 *inital acl 
 *KeHuatao
 *
 */
static void acl_rule_init(int index, int is_del)
{
    int max1, min1;
    sw_acl_t acl;
    AclProfile *prof;
    MibProfileType mibType = MIB_PROF_ACL;

	/*get min and max*/
    ProfInstLowHigh(mibType, &max1, &min1);

    if((index>=min1) && (index<max1))
    {
	if(is_del == 1)/*delete profile*/
	{
	    sw_acl_del(index+1);
	}
	else/*delete one acl*/
	{
	    /*get pointer*/
	    prof = (AclProfile*)ProfGetInstPointByIndex(mibType, index);
	    if(prof != NULL)
	    {
		if((ProfInstIsFree(((Pointer*)prof)) == 0) && (prof->head.active == TRUE))
		{
		    /*valid profile*/
		    memset((void*)(&acl), 0, sizeof(sw_acl_t));
		    acl.idx = index+1;
		    if((prof->source>PORT_COUNT) || (prof->source==0u))
		    {
			/*all ports*/
			acl.source_entry = 32u;
		    }
		    else {
			/*any port*/
			acl.source_entry = prof->source - 1u;
		    }
		    /*action*/
		    acl.action = prof->action_type;
		    /*source ip*/
		    acl.ipv4.sip = ntohl(prof->sip);
		    /*mask*/
		    acl.ipv4.sip_mask_width = prof->sip_mask;
		    acl.ipv4.dip = ntohl(prof->dip);
		    acl.ipv4.dip_mask_width = prof->dip_mask;
		    /*tcp/udp source port*/
		    acl.ipv4.sport = prof->sport;
		    acl.ipv4.dport = prof->dport;
		    /*write to registers*/
		    sw_acl_add(&acl);
		}
	    }
	}
    }

    return;
}
/*
 *
 *≥ı ºªØacl
 *KeHuatao
 *2011-09-10
 */
static void acl_init(int is_del)
{
    int max1, min1, i;

    ACL_DEBUG("%s\n", __func__);
    ProfInstLowHigh(MIB_PROF_ACL, &max1, &min1);
    /*all the acl*/
    for(i= min1; i<max1; i++)
    {
	/*init*/
	acl_rule_init(i, is_del);
    }
}
/*
 *acl init
 *
 */
void funInitAcl(void)
{
    ACL_DEBUG("%s\n", __func__);
    /*registger call back function*/
    registerForProfileUpdates(MIB_PROF_ACL, (Pointer)NULL, aclProfileChange);
    registerForProfileUpdates(MIB_PROF_ACL_STATUS, (Pointer)NULL, aclStatusProfileChange);
    /*acl init*/
    acl_init(0);
    /*acl status*/
    acl_status_init();
}
#endif
