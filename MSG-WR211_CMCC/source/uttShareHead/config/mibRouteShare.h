#ifndef MIBROUTE_SHARE_H
#define MIBROUTE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"

/*
 * ��̬·������
 * head		    -   �û���������
 * dstAddr	    -	Ŀ������
 * netmask	    -	��������
 * gwAddr	    -	����
 * preference	    -   ���ȼ�
 * interfaceName    -	·�ɰ󶨵Ľӿ�����
 */
typedef struct st_staticRouteProfile {
    mibHead		head;
    IPADDR		dstAddr;
    IPADDR		netmask;
    IPADDR		gwAddr;
    Uint32		preference;
    char		interfaceName[UTT_INTERFACE_LEN + 1];
} StaticRouteProfile;

#endif
