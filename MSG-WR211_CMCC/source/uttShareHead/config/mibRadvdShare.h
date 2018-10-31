#ifndef MIBRADVD_SHARE_H
#define MIBRADVD_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "typedefShare.h"

#if (IPV6_RADVD == FYES)
typedef struct ipv6_st_radvd {
    mibHead head;
    char DesPrefix[40];   /*ͨ��ǰ׺*/
    int manageEnabled;
    int otherEnabled;
    int DesPrefixLength;  /*ͨ��ǰ׺����*/
    int PrefixstaleTime;  /*ǰ׺��������*/
    int noticeInterval;//���ͨ��
    int routerPreferences;//·������ѡ��
    int MTU_config;//MTU����
    int routerLifetime;//·����������
    }RadvdProfile;
#endif
#endif
