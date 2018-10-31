/*
 * $Id: memcmp.c,v 1.1.1.1 2011/08/17 11:10:40 brwang Exp $
 *
 * Taken from the Linux kernel. GPL applies.
 * Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include "config.h"
#include "includes.h"

int memcmp(const void * cs,const void * ct,size_t count)
{
	const unsigned char *su1, *su2;
	signed char res = 0;

	for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}
