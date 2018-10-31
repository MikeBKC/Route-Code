/*
 * $Id: strerror.c,v 1.1.1.1 2011/08/17 11:10:40 brwang Exp $
 *
 * Copyright (C) 1996 Lars Fenneberg and Christian Graefe
 *
 * This file is provided under the terms and conditions of the GNU general
 * public license, version 2 or any later version, incorporated herein by
 * reference.
 *
 */

#include "config.h"
#include "includes.h"

/*
 * if we're missing strerror, these are mostly not defined either
 */
extern int sys_nerr;
extern char *sys_errlist[];

/*
 * Function: strerror
 *
 * Purpose:  implements strerror for systems which lack it. if these
 *			 systems even lack sys_errlist, you loose...
 *
 */


char *strerror(int err)
{
	static char buf[32];

	if (err >= 0 && err < sys_nerr)
		return sys_errlist[err];
	else {
		sprintf(buf, "unknown error: %d", errno);
		return buf;
	}
}
