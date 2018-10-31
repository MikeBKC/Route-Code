/*
 *	wireless.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wireless.h,v 1.1.1.1 2011/08/17 11:10:35 brwang Exp $
 */

void formDefineWireless(void);
void restart8021XDaemon(int nvram);
void updateFlash8021x(int nvram);
void Security(int nvram, webs_t wp, char_t *path, char_t *query);
void confWPAGeneral(int nvram, webs_t wp, int mbssid);
void confWEP(int nvram, webs_t wp, int mbssid);
void conf8021x(int nvram, webs_t wp, int mbssid);
void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query);
void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query);
