#ifndef _XT_UTTHOSTSTA_H
#define _XT_UTTHOSTSTA_H

#include <linux/types.h>

/* Ӧ�ô���ͳ�������±� */
typedef enum {
    UTT_APP_IM_ARR = 0,
    UTT_APP_P2P_ARR = 1,
    UTT_APP_STOCK_ARR = 2, 
    UTT_APP_VIDEO_ARR = 3,
    UTT_APP_GAME_ARR = 4,
    UTT_APP_SHOP_ARR = 5,
    UTT_APP_SNS_ARR = 6,
    UTT_APP_WEBGAME_ARR = 7,
    UTT_APP_MAIL_ARR = 8,
    UTT_APP_FORUM_ARR = 9,
    UTT_APP_OTHER_ARR = 10,

    UTT_APP_MAX_ARR
} UttAppFTypeEnum;

/* ͳ�ƽṹ��
 * bytes: �ֽ���
 * conns: ����
 * Ŀǰ�������壺һ��ʱ��������û��й�ʶ��������������һ��
 */ 
typedef struct st_uttappsta {
    __u64 bytes;
    __u32 conns;
} UttAppSta;


#endif /* _XT_UTTHOSTSTA_H */
