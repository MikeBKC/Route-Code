#
# (C) Copyright 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=$(SSP_ALL_CFLAGS) -DUSE_HOSTCC
CFLAGS+=-I$(TOPDIR)zlib
CFLAGS+=-I$(ROOTDIR)
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
CFLAGS+=-I$(TOPDIR)libconfig/profacc -I$(TOPDIR)libconfig/cli -I$(TOPDIR)libconfig/mib

CFLAGS+=-I$(ROOTDIR)/$(LINUXDIR)/include 
CFLAGS+=-I$(ROOTDIR)/config
CFLAGS+=-I$(ROOTDIR)/uttShareHead/Config

LIBSWITCH=libswitch.a
LIBSWITCH_SHARED=libswitch.so
LIBSWITCH_SHARED_FULLNAME=libswitch-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


#OBJS	:= switch.o sw_cfg.o sw_ioctl.o
OBJS	:= switch.o sw_ioctl.o sw_cfg.o

HEADERS := switch.h 

all:	$(LIBSWITCH)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBSWITCH_SHARED).$(MAJOR_VERSION) \
		-o $(LIBSWITCH_SHARED_FULLNAME) --whole-archive $(LIBSWITCH) \
		--no-whole-archive  \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBSWITCH_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSWITCH_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBSWITCH_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBSWITCH_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSWITCH_SHARED)
	$(LN) -sf $(LIBSWITCH_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSWITCH_SHARED).$(MAJOR_VERSION)

$(LIBSWITCH) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBSWITCH) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBSWITCH)
	$(INSTALL) -m 644 $(LIBSWITCH) $(TOPDIR)lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] $(LIBSWITCH_SHARED)* $(LIBSWITCH_SHARED_FULLNAME)*

