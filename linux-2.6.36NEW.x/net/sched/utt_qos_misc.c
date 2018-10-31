
/*
 * added by brwang for qos
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <linux/ip.h>
#include <net/sch_utt.h>

extern struct utt_wan_if_info   uttEtherIfInfo[UTT_MAX_IF_NUM];
void statisPerSecPerUserFlowSta(struct smart_hash_node *hashNode, 
        int ifIndex, enum en_uttQosDirection dir, unsigned int len)
{
    hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].txByte 
        = hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpTxByte;
    hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].rxByte 
        = hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpRxByte;

    if((hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].rxByte 
                > (uttEtherIfInfo[ifIndex].perSecPerIpRxBytes/2))
            || (hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].txByte 
                > (uttEtherIfInfo[ifIndex].perSecPerIpTxBytes/2))) {
        if(hashNode->lesslimitCnt > 0) {
            hashNode->lesslimitCnt--; 
#if 0
            printk("--------------------- %x\n", hashNode->sip);
#endif
        }
    } else {
        if(hashNode->lesslimitCnt < 10) {
            hashNode->lesslimitCnt++; 
#if 0
            printk("+++++++++++++++ %x\n", hashNode->sip);
#endif
        }
    }
    if(dir == UTT_SMART_QOS_RX) {
        hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpRxByte = len;
        hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpTxByte = 0;
    } else {
        hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpTxByte = len;
        hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].tmpRxByte = 0;
    }
    hashNode->preSecFlowStatus->usrIfFlowSta[ifIndex].lastClrJiffies
        = jiffies;
}
EXPORT_SYMBOL(statisPerSecPerUserFlowSta);

struct netFlowStatus* uttPerIpPerSecFlowSta[MAX_HASH_TABLE] = {0};
EXPORT_SYMBOL(uttPerIpPerSecFlowSta);
void decUserFlowStatusQuoteCnt(struct smart_hash_node *hashNode)
{
    unsigned int hash;
    struct netFlowStatus *headFlowStatus;
    headFlowStatus = hashNode->preSecFlowStatus;
    (headFlowStatus->quoteCnt)--;
    if(headFlowStatus->quoteCnt == 0) {
        if(headFlowStatus->prev == NULL) {
            hash = hashNode->sip & 0xff;
            uttPerIpPerSecFlowSta[hash] = headFlowStatus->next;
            if(uttPerIpPerSecFlowSta[hash] != NULL) {
                uttPerIpPerSecFlowSta[hash]->prev = NULL;
            }
        } else {
            headFlowStatus->prev->next = headFlowStatus->next;
            if(headFlowStatus->next != NULL) {
                headFlowStatus->next->prev = headFlowStatus->prev;
            }
        }
        kfree(headFlowStatus);
    }
}
EXPORT_SYMBOL(decUserFlowStatusQuoteCnt);
