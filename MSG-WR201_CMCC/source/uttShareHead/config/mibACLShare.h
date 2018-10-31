#ifndef _MIB_ACL_SHARE_H_
#define _MIB_ACL_SHARE_H_

#include "mibunionShare.h"
#include "mibtypeShare.h"
#include <linux/switch/swioc_common.h>

typedef struct _sw_acl_status_profile{
    mibHead head;
}AclStatusProfile;
typedef struct _sw_acl_profile{
    mibHead head;

    sw_acl_action_t      action_type;    /* ACE action */

    sw_acl_source_t      source;    /* ACE source entry*/

    sw_acl_ip_t          sip;       /* Source IP address */
    sw_acl_ip_mask_t     sip_mask;       /* Source IP address */
    sw_acl_ip_t          dip;       /* Destination IP address */
    sw_acl_ip_mask_t     dip_mask;       /* Source IP address */
    sw_acl_udp_tcp_t     sport;     /* UDP/TCP: Source port */
    sw_acl_udp_tcp_t     dport;     /* UDP/TCP: Destination port */
}AclProfile;

#endif
