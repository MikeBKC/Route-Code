/*** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***
 ***                                                               ***
 ***                  CONFIDENTIAL INFORMATION                     ***
 ***                  ------------------------                     ***
 ***    This Document contains Confidential Information or         ***
 ***    Trade Secrets, or both, which are the property of UTT      ***
 ***    Technologies, Inc. This document may not be  copied,       ***
 ***    reproduced, reduced to any electronic medium or machine    ***
 ***    readable form or otherwise duplicated and the information  ***
 ***    herein may not be used, disseminated or otherwise          ***
 ***    disclosed, except with the prior written consent of UTT    ***
 ***    Technologies, Inc.                                         ***
 ***                                                               ***
 *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE *** NOTICE ***/
 
/* 
    Copyright 1991-2002 UTT Technologies, Inc.
    All rights reserved.
    Use of copyright notice does not imply publication.
 */

/*
 * 
 * File:  
 * 
 * Overview:
 *  
 *
*/

#if ! defined ( _MIBUPNP_H )
#define _MIBUPNP_H

#include "mibglob.h"

typedef struct _Upnp_Config {
	char 	name[MAX_PROFILE_NAME_LENGTH + 1 ];		// 实例名
	Boolean active;
	char 	profile[MAX_PROFILE_NAME_LENGTH + 1]; 	// 绑定的网络端口
	Uint8	enable;
	Uint8	wanCtrl;								// 允许控制点对Wan口操作
	Uint8	disconnCtrl;							// 允许控制点断掉因特网连接
	Uint8	natCtrl;								// 允许控制点增加、删除NAT映射
	Uint8	dhcpsCtrl;								// 允许控制点修改DHCP Server配置
}UpnpConfProfile;

#endif		// _MIBUPNP_H
