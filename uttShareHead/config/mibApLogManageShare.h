#ifndef MIBAPLOGMANAGE_SHARE_H
#define MIBAPLOGMANAGE_SHARE_H
#include "mibunionShare.h"
#include "mibtypeShare.h"
#include "uttMachine.h"

#define  MAX_TYPE_NUM  6u   //最多支持的报警种类，目前设置6，如果不够再加大，修改这cliaaa.c中也要修改

typedef enum {

	APLINKLOG = 0u,

	APUSERLOG = 1u,

	APUSERNUM = 2u,

	MAX_LOG_NUM = MAX_TYPE_NUM,

} ap_log_list;

typedef struct st_apLogManageProfile {
	mibHead head;

	LogEnableEnum apLogType[MAX_TYPE_NUM];
} ApLogManageProfile;
#endif
