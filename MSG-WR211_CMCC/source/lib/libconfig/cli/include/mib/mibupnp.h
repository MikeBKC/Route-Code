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
	char 	name[MAX_PROFILE_NAME_LENGTH + 1 ];		// ʵ����
	Boolean active;
	char 	profile[MAX_PROFILE_NAME_LENGTH + 1]; 	// �󶨵�����˿�
	Uint8	enable;
	Uint8	wanCtrl;								// ������Ƶ��Wan�ڲ���
	Uint8	disconnCtrl;							// ������Ƶ�ϵ�����������
	Uint8	natCtrl;								// ������Ƶ����ӡ�ɾ��NATӳ��
	Uint8	dhcpsCtrl;								// ������Ƶ��޸�DHCP Server����
}UpnpConfProfile;

#endif		// _MIBUPNP_H
