#if (GAME_BOOST == FYES)
#ifndef MIBGAMEBOOST_H
#define MIBGAMEBOOST_H
#include "mibunion.h"
#include "mibtype.h"

/* Ӣ������:     0x1
 * ���³�����ʿ: 0x2
 * ��Խ����:     0x4
 * ħ������:     0x8
 * �λ�����:     0x10
 */

/* LOL()--�ж��Ƿ���*/
#define LOL(x)	    ((x) & 0x1)
#define DNF(x)	    ((x) & 0x2)
#define CF(x)	    ((x) & 0x4)
#define WOW(x)	    ((x) & 0x8)
#define MHXY(x)	    ((x) & 0x10)
#define QQYY(x)      ((x) & 0x20)
#define CCTV(x)       ((x) & 0x40)
#define SINA(x)      ((x) & 0x80)
#define QQZB(x)     ((x) & 0x100)

/*
 * gamePolicy - ��λ��ȡ��Ϸ����
 */
typedef struct st_gameBoost_profile
{
    mibHead head;
    Uint32 gamePolicy;
}GameBoostProfile;
#endif
#endif
