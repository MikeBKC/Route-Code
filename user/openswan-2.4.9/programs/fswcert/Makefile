# Makefile for the KLIPS interface utilities
# Copyright (C) 2004 Michael Richardson <mcr@xelerance.com>
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# RCSID $Id: Makefile,v 1.1.1.1 2011/08/17 11:10:43 brwang Exp $

###################################################
# Adjust SSLDIR before make ...
SSLDIR=/usr

###################################################

OPENSWANSRCDIR?=$(shell cd ../..; pwd)
include ${OPENSWANSRCDIR}/Makefile.inc

PROGRAM=fswcert
CFLAGS += -DVERSION=\"$(IPSECVERSION)\" -I$(SSLDIR)/include
LDFLAGS = -L$(SSLDIR)/lib
LDLIBS = -lcrypto

fswcert.8: fswcert.pod
	pod2man fswcert.pod >fswcert.8

include ${srcdir}../Makefile.program

#
# $Log: Makefile,v $
# Revision 1.1.1.1  2011/08/17 11:10:43  brwang
# 3520G start
#
# Revision 1.1.1.1  2011/03/15 05:24:28  lei.minghuan
# import ralink
#
# Revision 1.1.1.1  2009/12/14 12:44:47  brwang
# ralink wlan
#
# Revision 1.1.1.1  2007-10-08 08:01:06  steven
#
#
# Revision 1.3  2004/12/18 18:13:39  mcr
# 	adjusted Makefiles so that ${OBJDIR} builds work.
#
# Revision 1.2  2004/04/10 03:36:10  mcr
# 	merged in BSD-compat changes.
#
#

