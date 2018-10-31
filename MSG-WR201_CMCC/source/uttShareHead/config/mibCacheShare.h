#ifndef MIBCACHE_SHARE_H
#define MIBCACHE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

/*
 *
 * 缓存功能结构体
 */
typedef struct st_cacheProfile
{
	mibHead head;
	Uint32 size;
	FunEnableEnum cacheEnable;
	FunEnableEnum youkuVideoEnable;
	FunEnableEnum tudouVideoEnable;
	FunEnableEnum tcVideoEnable;
	FunEnableEnum sinaVideoEnable;
	FunEnableEnum ifengVideoEnable;
	FunEnableEnum letvVideoEnable;
	FunEnableEnum v56VideoEnable;
	FunEnableEnum funshionVideoEnable;
	FunEnableEnum baiduMp3Enable;
	FunEnableEnum sinaPageEnable;
}CacheProfile;

#endif	
	
