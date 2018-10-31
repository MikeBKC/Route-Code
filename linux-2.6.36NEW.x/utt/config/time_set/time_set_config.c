#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/netfilter_ipv4/time_set.h>

#include "mibShare.h"
#include "profacceShare.h"

#if 0 
#define TIME_SET_PRINT(fmt, args...)		printk(fmt, ## args)
#else
#define TIME_SET_PRINT(fmt, args...)
#endif
extern void registerForProfileUpdates(MibProfileType mibType,Pointer updateTag,ProfileUpdateFnc	updateFnc);

static void timeGrpProfileChange(Pointer tag, MibProfileType profileType, 
        ProfChangeType changeType, Uint32 index,
        const TimeGrpProfile *oldProfileDataPtr, const TimeGrpProfile *newProfileDataPtr )
{
    TIME_SET_PRINT("file:%s,func:%s,line:%d---changeType=%d\n",__FILE__,__func__,__LINE__,changeType);
    switch(changeType) {
        case PROF_CHANGE_ADD:
	    uttTimeSetCreat(newProfileDataPtr);/*创建内核时间组*/
	    break;
	case PROF_CHANGE_EDIT:
	    uttTimeSetModify(newProfileDataPtr);/*修改内核时间组*/
	    break;
        case PROF_CHANGE_DEL:
	    uttTimeSetDel(oldProfileDataPtr);
	    break;
        case PROF_CHANGE_DELALL:
	    uttTimeSetDelAll();
        default:
            break;
    };
}

static int __init time_set_config_init(void)
{
    registerForProfileUpdates(MIB_PROF_TIMEGRP, (Pointer)NULL, (ProfileUpdateFnc)timeGrpProfileChange);/*触发函数*/

    return 0;
}

static void __exit time_set_config_exit(void)
{
    uttTimeSetDelAll();
}
module_init(time_set_config_init);
module_exit(time_set_config_exit);
