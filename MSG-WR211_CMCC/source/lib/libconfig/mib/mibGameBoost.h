#if (GAME_BOOST == FYES)
#ifndef MIBGAMEBOOST_H
#define MIBGAMEBOOST_H
#include "mibunion.h"
#include "mibtype.h"

/* 英雄联盟:     0x1
 * 地下城与勇士: 0x2
 * 穿越火线:     0x4
 * 魔兽世界:     0x8
 * 梦幻西游:     0x10
 */

/* LOL()--判断是否开启*/
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
 * gamePolicy - 按位存取游戏开关
 */
typedef struct st_gameBoost_profile
{
    mibHead head;
    Uint32 gamePolicy;
}GameBoostProfile;
#endif
#endif
