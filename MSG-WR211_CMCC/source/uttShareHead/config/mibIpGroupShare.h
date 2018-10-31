#ifndef MIB_IP_GROUP_SHARE_H
#define MIB_IP_GROUP_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"
#if (IP_GRP == FYES)
/**
 * ip��ַ���ǻ���ip�������
 * ���ݿ�����ip �� �û���
 * Ϊipʱֱ��չ����ipset��
 * Ϊ�û���ʱ�������û� ���� ����ʱ������Ӧ�� ipset��
 */
#define UTT_IPGRP_NAME_LEN 31 /*ip���Ա�����ȡ�������Ӧ���� ���� pppoe�û��� �Լ� web��֤�û��������еĽϳ���*/
#define MAX_IPGRP_DEP 2  /*ip������Ƕ�� ���*/
#define IPGRP_IP_ENTRYS 10 /*IPGRP_IP��������Ŀ��*/
#define IPGRP_USER_ENTRYS 100 /*IPGRP_USER��������Ŀ��*/
#define IPGRP_ENTRYS 100 /*һ��ip���������Ŀ���� Ϊ IPGRP_IP �� IPGRP_USER����Ŀ�еĽϴ��ߡ�*/
#define MAX_IPGRP_IP_NUM 65535 /*ip��ַ������*/

/*ip��ַ������*/
typedef enum{
    IPGRP_IP,/*���������Ϊ�̶�ip*/
    IPGRP_USER/*���ip��Ϊ�̶�ip�������û�����Ӧip�ı仯���仯*/
}ipGrpType;

/*ip����Ŀ����*/
typedef enum{
    IPENT_NONE = 0,/*����Ŀû��*/

    /*start�������ͽ���ӦIPGRP_IP������*/
    IPENT_RANGE,/*ip��ַ�� */
    /*end*/

#if (PPPOE_SERVER == FYES)
    /*start�������ͽ���ӦIPGRP_USER������*/
    IPENT_POE_ACC,/*pppoe�˺� */
#endif

#if (WEB_AUTH == FYES)
    IPENT_WEB_AUTH,/*web��֤�û�*/
#endif
    /*end*/

    IPENT_GRP/*��ip��*/
}ipGrpEntryType;

/*ip�鷶Χ*/
typedef struct st_ipRange{
    IPADDR ipFrom;
    IPADDR ipTo;
}ipRange;

/*ip����Ŀ����*/
typedef union{
    ipRange range;/*ip��*/
    char name[UTT_IPGRP_NAME_LEN + 1];/*���� ����Ϊ������ �û���*/
}ipGrpEntryVal;

/*ip����Ŀ*/
typedef struct st_ipGrpEntry{
    ipGrpEntryType entryType;/*����*/
    ipGrpEntryVal  entryVal;/*ֵ*/
} ipGrpEntry;

/*ʵ������һ��ip��ַ��*/
typedef struct st_ipGrpProfile{
    mibHead head;
    ipGrpType grpType;/*������*/
    ipGrpEntry ipEntry[IPGRP_ENTRYS];/*�˴�Ҳ���Զ�̬����*/
}IpGrpProfile;

#endif
#endif
