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

CFLAGS += $(SSP_ALL_CFLAGS) -DUSE_HOSTCC
#CFLAGS+=-I$(TOPDIR)zlib
CFLAGS += -I$(ROOTDIR)
CFLAGS += -I$(ROOTDIR)/uttShareHead/config
#CFLAGS+=-I$(TOPDIR)libconfig/profacc -I$(TOPDIR)libconfig/cli -I$(TOPDIR)libconfig/mib

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/include 

LDFLAGS += -lm

#MAJOR_VERSION=0
#MINOR_VERSION=0
#SUBLEVEL=0

LIBCJSON=libcJSON.a
LIBCJSON_SHARED=libcJSON.so
LIBCJSON_SHARED_FULLNAME=libcJSON-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


#OBJS	:= switch.o sw_cfg.o sw_ioctl.o
OBJS	:= cJSON.o

HEADERS := cJSON.h 

all:	$(LIBCJSON)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBCJSON_SHARED).$(MAJOR_VERSION) \
		-o $(LIBCJSON_SHARED_FULLNAME) --whole-archive $(LIBCJSON) \
		--no-whole-archive  \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBCJSON_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCJSON_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBCJSON_SHARED_FULLNAME) $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBCJSON)
	$(LN) -sf $(LIBCJSON_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCJSON_SHARED)
	$(LN) -sf $(LIBCJSON_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBCJSON_SHARED).$(MAJOR_VERSION)

$(LIBCJSON) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBCJSON) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBCJSON)
	$(INSTALL) -m 644 $(LIBCJSON) $(TOPDIR)lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] $(LIBCJSON_SHARED)* $(LIBCJSON_SHARED_FULLNAME)*

