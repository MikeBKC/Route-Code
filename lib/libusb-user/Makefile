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

CFLAGS+=-I$(LINUXDIR)/include 

LIBUSBUSER=libusb-user.a
LIBUSBUSER_SHARED=libusb-user.so
LIBUSBUSER_SHARED_FULLNAME=libusb-user-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


#OBJS	:= switch.o sw_cfg.o sw_ioctl.o
OBJS	:= usb-user.o

HEADERS := usb-user.o

all:	$(LIBUSBUSER)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBUSBUSER_SHARED).$(MAJOR_VERSION) \
		-o $(LIBUSBUSER_SHARED_FULLNAME) --whole-archive $(LIBUSBUSER) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSBUSER_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUSBUSER_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBUSBUSER_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBUSBUSER_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUSBUSER_SHARED)
	$(LN) -sf $(LIBUSBUSER_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUSBUSER_SHARED).$(MAJOR_VERSION)

$(LIBUSBUSER) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBUSBUSER) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSBUSER)
	$(INSTALL) -m 644 $(LIBUSBUSER) $(TOPDIR)lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] $(LIBUSBUSER_SHARED)* $(LIBUSBUSER_SHARED_FULLNAME)*

