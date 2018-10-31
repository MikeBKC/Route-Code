/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

void FAST_FUNC bb_perror_msg(const char *s, ...)
{
	va_list p;

	va_start(p, s);
	/* Guard against "<error message>: Success" */
	bb_verror_msg(s, p, errno ? strerror(errno) : NULL);
	va_end(p);
}

void FAST_FUNC bb_simple_perror_msg(const char *s)
{
	bb_perror_msg("%s", s);
}

int dbck(const char *format, ...)
{
#if 0
	va_list valst;
	va_start(valst, format);

	int fd;
	char buf[128];
	memset(buf, 0, 128);
	fd = open("/dev/console", O_RDWR);

	vsnprintf(buf, 128, format, valst);

	write(fd, buf, sizeof(buf));
	close(fd);

	va_end(valst);

	return 0;
#endif
}
