#include "utt_debug.h"
#if UTT_RT3883_DEBUG
#include "rtk_api_ext.h"
/**
 * display all entry from look table
 */
void rtkSwMacDump(void)
{
    rtk_l2_addr_table_t l2_entry;
    int i = 0, ret = 0;

    printk("Index   MacAddr       MemeberPort Fid Auth Block  Static Age\n");

    for(i=1; i<=8256 ;i++)
    {
	memset(&l2_entry , 0, sizeof(l2_entry));
	l2_entry.index = i;
	ret = rtk_l2_entry_get(&l2_entry);
	if(ret == RT_ERR_OK)
	{
	    if(l2_entry.is_ipmul)
	    {
		printk("\nIndex SourceIp    DestinationIP  MemeberPort    State\n");
		printk("%4d ", l2_entry.index);
		printk(NIPQUAD_FMT, NIPQUAD(l2_entry.sip));
		printk(NIPQUAD_FMT, NIPQUAD(l2_entry.dip));
		printk("%04x", l2_entry.portmask);
		printk("%s ", l2_entry.is_static? "Static" : "Auto");
	    }
	    else if(l2_entry.mac.octet[0]&0x01)
	    {
		printk("%4d %02x:%02x:%02x:%02x:%02x:%02x %04x %4d %4s %5s %s %d\n",
			l2_entry.index, l2_entry.mac.octet[0],
			l2_entry.mac.octet[1],
			l2_entry.mac.octet[2],
			l2_entry.mac.octet[3],
			l2_entry.mac.octet[4],
			l2_entry.mac.octet[5],
			l2_entry.portmask,
			l2_entry.fid,
			l2_entry.auth?"Auth":"x",
			l2_entry.sa_block?"Block":"x",
			l2_entry.is_static? "Static" : "Auto",
			l2_entry.age

		      );
	    }
	    else if((l2_entry.age != 0) || (l2_entry.is_static ==1))
		printk("%4d %02x:%02x:%02x:%02x:%02x:%02x %04x %4d %4s %5s %s %d\n",
			l2_entry.index, l2_entry.mac.octet[0],
			l2_entry.mac.octet[1],
			l2_entry.mac.octet[2],
			l2_entry.mac.octet[3],
			l2_entry.mac.octet[4],
			l2_entry.mac.octet[5],
			l2_entry.portmask,
			l2_entry.fid,
			l2_entry.auth?"Auth":"x",
			l2_entry.sa_block?"Block":"x",
			l2_entry.is_static? "Static" : "Auto",
			l2_entry.age

		      );  
	}
    }
}



#endif
