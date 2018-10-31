# Makefile for uClibc
#
# Copyright (C) 2000-2003 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General
# Public License along with this program; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place, Suite 330,
# Boston, MA 02111-1307 USA


#--------------------------------------------------------------
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------
noconfig_targets := menuconfig config oldconfig randconfig \
	defconfig allyesconfig allnoconfig clean distclean \
	release tags TAGS
TOPDIR=./
include Rules.mak
-include $(CONFIG_CONFIG)

DIRS = 
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
DIRS = ldso libc
ifeq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
    DIRS += libintl
endif
ifeq ($(CONFIG_LIB_CRYPT_FORCE),y)
    DIRS += libcrypt
endif
ifeq ($(CONFIG_LIB_RESOLV_FORCE),y)
    DIRS += libresolv
endif
ifeq ($(CONFIG_LIB_NSL_FORCE),y)
    DIRS += libnsl
endif
ifeq ($(CONFIG_LIB_UTIL_FORCE),y)
    DIRS += libutil
endif
ifeq ($(CONFIG_LIB_RT_FORCE),y)
    DIRS += librt
endif
ifeq ($(CONFIG_LIB_LIBGMP_FORCE),y)
    DIRS += libgmp
endif
ifeq ($(CONFIG_LIB_LIBM_FORCE),y)
    DIRS += libm
endif
ifeq ($(CONFIG_LIB_LIBPTHREAD_FORCE),y)
    DIRS += libpthread
endif
ifeq ($(CONFIG_LIB_LIBNVRAM_FORCE),y)
    DIRS += libnvram
endif

ifeq ($(CONFIG_LIB_LIBNTFS3G_FORCE),y)
    DIRS += libntfs-3g
endif
ifeq ($(CONFIG_LIB_LIBBT_FORCE),y)
    DIRS += libbluetooth
endif
ifeq ($(CONFIG_LIB_READLINE_FORCE),y)
    DIRS += libreadline
endif
ifeq ($(CONFIG_LIB_FLEX_FORCE),y)
    DIRS += libflex
endif
ifeq ($(CONFIG_LIB_LIBZEBRA_FORCE),y)
    DIRS += libzebra
endif
ifeq ($(CONFIG_LIB_USB0112_FORCE),y)
    DIRS += libusb-0.1.12
endif
ifeq ($(CONFIG_LIB_USB100_FORCE),y)
    DIRS += libusb-1.0.0
endif
ifeq ($(CONFIG_LIB_LIBUSB_USER_FORCE),y)
    DIRS += libusb-user
endif
endif

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
    DIRS += libswitch
endif

ifeq ($(CONFIG_LIB_LIBUPNP_FORCE),y)
    DIRS += libupnp
endif
ifeq ($(CONFIG_LIB_ZLIB_FORCE),y)
    DIRS += zlib
endif
ifeq ($(CONFIG_LIB_LIBCONFIG_FORCE),y)
    DIRS += mxml-2.6
    DIRS += libconfig
endif
ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
    DIRS += libiconv-1.14
endif
ifeq ($(CONFIG_USER_WIFIDOG),y)
    DIRS += libcJSON
else 
ifeq ($(CONFIG_LIB_LIBCJSON_FORCE),y)
    DIRS += libcJSON
endif
endif
ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
all: headers pregen subdirs shared finished
else
all: pregen subdirs shared finished
endif

# In this section, we need .config
-include .config.cmd

shared: subdirs
ifeq ($(strip $(HAVE_SHARED)),y)
	$(SECHO)
	$(SECHO) Building shared libraries ...
	$(SECHO)
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
	@$(MAKE) -C libc shared
	@$(MAKE) -C ldso shared

ifeq ($(CONFIG_LIB_CRYPT_FORCE),y)
	@$(MAKE) -C libcrypt shared
endif
ifeq ($(CONFIG_LIB_RESOLV_FORCE),y)
	@$(MAKE) -C libresolv shared
endif
ifeq ($(CONFIG_LIB_NSL_FORCE),y)
	@$(MAKE) -C libnsl shared
endif
ifeq ($(CONFIG_LIB_UTIL_FORCE),y)
	@$(MAKE) -C libutil shared
endif
ifeq ($(CONFIG_LIB_RT_FORCE),y)
	@$(MAKE) -C librt shared
endif
ifeq ($(CONFIG_LIB_LIBNVRAM_FORCE),y)
#	@$(MAKE) -C libnvram shared
endif

ifeq ($(CONFIG_LIB_LIBNTFS3G_FORCE),y)
	@$(MAKE) -C libntfs-3g shared
endif

ifeq ($(CONFIG_LIB_LIBBT_FORCE),y)
	@$(MAKE) -C libbluetooth shared
endif
ifeq ($(CONFIG_LIB_READLINE_FORCE),y)
	@$(MAKE) -C libreadline shared
endif
ifeq ($(CONFIG_LIB_FLEX_FORCE),y)
	@$(MAKE) -C libflex shared
endif
ifeq ($(CONFIG_LIB_LIBGMP_FORCE),y)
	@$(MAKE) -C libgmp shared
endif
ifeq ($(CONFIG_LIB_LIBM_FORCE),y)
	@$(MAKE) -C libm shared
endif
ifeq ($(CONFIG_LIB_LIBPTHREAD_FORCE),y)
	@$(MAKE) -C libpthread shared
endif
ifeq ($(CONFIG_LIB_LIBZEBRA_FORCE),y)
	@$(MAKE) -C libzebra shared
endif
ifeq ($(CONFIG_LIB_USB0112_FORCE),y)
	@$(MAKE) -C libusb-0.1.12 shared
endif
ifeq ($(CONFIG_LIB_USB100_FORCE),y)
	@$(MAKE) -C libusb-1.0.0 shared
endif
ifeq ($(CONFIG_LIB_LIBUSB_USER_FORCE),y)
	@$(MAKE) -C libusb-user shared
endif
ifeq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
	@$(MAKE) -C libintl shared
endif
endif

ifeq ($(CONFIG_LIB_LIBSWITCH_FORCE),y)
	@$(MAKE) -C libswitch shared
endif


ifeq ($(CONFIG_LIB_LIBUPNP_FORCE),y)
	@$(MAKE) -C libupnp shared
endif
ifeq ($(CONFIG_LIB_ZLIB_FORCE),y)
	@$(MAKE) -C zlib
endif
ifeq ($(CONFIG_LIB_LIBCONFIG_FORCE),y)
	@$(MAKE) -C mxml-2.6
	@$(MAKE) -C libconfig shared
endif
ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
	@$(MAKE) -C libiconv-1.14 shared
endif
ifeq ($(CONFIG_USER_WIFIDOG),y)
	@$(MAKE) -C libcJSON shared
else
ifeq ($(CONFIG_LIB_LIBCJSON_FORCE),y)
	@$(MAKE) -C libcJSON shared
endif
endif
ifeq ($(CONFIG_LIB_LIBGMP_5_0_4_FORCE), y)
	@$(MAKE) -C gmp-5.0.4 -f makefile.config
endif
else
	$(SECHO)
	$(SECHO) Not building shared libraries ...
	$(SECHO)
endif

finished: shared
	$(SECHO)
	$(SECHO) Finally finished compiling ...
	$(SECHO)

ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
include/bits/uClibc_config.h: .config
	@if [ ! -x ./extra/config/conf ] ; then \
	    $(MAKE) -C extra/config conf; \
	fi;
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.in
endif

# For the moment, we have to keep re-running this target 
# because the fix includes scripts rely on pre-processers 
# in order to generate the headers correctly :(.  That 
# means we can't use the $(HOSTCC) in order to get the 
# correct output.
ifeq ($(strip $(ARCH_HAS_MMU)),y)
export header_extra_args = 
else
export header_extra_args = -n
endif

ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
headers: include/bits/uClibc_config.h
	@$(SHELL_SET_X); \
	./extra/scripts/fix_includes.sh \
		-k $(KERNEL_SOURCE) -t $(TARGET_ARCH) \
		$(header_extra_args)
	@cd include/bits; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/bits/*.h` ; do \
		$(LN) -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/bits/*.h` ; do \
			$(LN) -fs $$i .; \
		done; \
	fi
	@cd include/sys; \
	set -e; \
	for i in `ls ../../libc/sysdeps/linux/common/sys/*.h` ; do \
		$(LN) -fs $$i .; \
	done; \
	if [ -d ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `ls ../../libc/sysdeps/linux/$(TARGET_ARCH)/sys/*.h` ; do \
			$(LN) -fs $$i .; \
		done; \
	fi
	@cd $(TOPDIR); \
	set -e; \
	$(SHELL_SET_X); \
	TOPDIR=. CC="$(CC)" /bin/sh extra/scripts/gen_bits_syscall_h.sh > include/bits/sysnum.h.new; \
	if cmp include/bits/sysnum.h include/bits/sysnum.h.new >/dev/null 2>&1; then \
		$(RM) include/bits/sysnum.h.new; \
	else \
		mv -f include/bits/sysnum.h.new include/bits/sysnum.h; \
	fi
	$(MAKE) -C libc/sysdeps/linux/common headers
	$(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) headers
endif

# Command used to download source code
WGET:=wget --passive-ftp

LOCALE_DATA_FILENAME:=uClibc-locale-030818.tgz
 
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
pregen: headers
else
pregen:
endif

ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
ifeq ($(strip $(UCLIBC_DOWNLOAD_PREGENERATED_LOCALE_DATA)),y)
	(cd extra/locale; \
	if [ ! -f $(LOCALE_DATA_FILENAME) ] ; then \
	$(WGET) http://www.uclibc.org/downloads/$(LOCALE_DATA_FILENAME) ; \
	fi );
endif
ifeq ($(strip $(UCLIBC_PREGENERATED_LOCALE_DATA)),y)
	(cd extra/locale; zcat $(LOCALE_DATA_FILENAME) | tar -xvf -)
	$(MAKE) -C extra/locale pregen
endif
endif


subdirs: $(patsubst %, _dir_%, $(DIRS))
ifeq ($(CONFIG_USER_ALINK),y)
	tar zxvf lib-alink.tar.gz
endif
ifeq ($(CONFIG_DEFAULTS_ATHEROS_AR7100),y)
	tar xzvf lib-ar7100.tar.gz
endif
ifeq ($(CONFIG_DEFAULTS_INTEL_ATOM),y)
	tar xzvf lib-x86-atom.tar.gz
endif
ifeq ($(CONFIG_DEFAULTS_FREESCALE_P1010),y)
	tar xzvf lib-ppc.tar.gz
endif
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
$(patsubst %, _dir_%, $(DIRS)): headers
else
$(patsubst %, _dir_%, $(DIRS)):
endif
	$(MAKE) -C $(patsubst _dir_%, %, $@)

tags:
	ctags -R

install: install_runtime install_dev finished2

#added by Steven Liu
romfs: install_runtime
#--


RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB=$(shell extra/scripts/relative_path.sh $(DEVEL_PREFIX)lib $(RUNTIME_PREFIX)lib)

# Installs header files and development library links.
install_dev:
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)lib
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
	$(INSTALL) -d $(PREFIX)$(DEVEL_PREFIX)include
	-$(INSTALL) -m 644 lib/*.[ao] $(PREFIX)$(DEVEL_PREFIX)lib/
	if [ "$(KERNEL_SOURCE)" == "$(DEVEL_PREFIX)" ] ; then \
		extra_exclude="--exclude include/linux --exclude include/asm'*'" ; \
	else \
		extra_exclude="" ; \
	fi ; \
	tar -chf - include --exclude .svn --exclude CVS $$extra_exclude \
		| tar -xf - -C $(PREFIX)$(DEVEL_PREFIX)
endif
ifneq ($(strip $(UCLIBC_HAS_FLOATS)),y)
	# Remove floating point related headers since float support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/complex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/fpu_control.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/ieee754.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/math.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/tgmath.h
endif
ifneq ($(strip $(UCLIBC_HAS_WCHAR)),y)
	# Remove wide char headers since wide char support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wctype.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wchar.h
endif
ifneq ($(strip $(UCLIBC_HAS_LOCALE)),y)
	# Remove iconv header since locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/iconv.h
endif
ifneq ($(strip $(UCLIBC_HAS_GLIBC_CUSTOM_PRINTF)),y)
	# Remove printf header since custom print specifier support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/printf.h
endif
ifneq ($(strip $(UCLIBC_HAS_XLOCALE)),y)
	# Remove xlocale header since extended locale support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/xlocale.h
endif
ifneq ($(strip $(UCLIBC_HAS_GETTEXT_AWARENESS)),y)
	# Remove libintl header since gettext support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/libintl.h
endif
ifneq ($(strip $(UCLIBC_HAS_REGEX)),y)
	# Remove regex headers since regex support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/regex.h
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/regexp.h
endif
ifneq ($(strip $(UCLIBC_HAS_WORDEXP)),y)
	# Remove wordexp header since wordexp support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/wordexp.h
endif
ifneq ($(strip $(UCLIBC_HAS_FTW)),y)
	# Remove ftw header since ftw support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/ftw.h
endif
ifneq ($(strip $(UCLIBC_HAS_GLOB)),y)
	# Remove glob header since glob support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/glob.h
endif
ifneq ($(strip $(UCLIBC_HAS_GNU_GETOPT)),y)
	# Remove getopt header since gnu getopt support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/getopt.h
endif
ifneq ($(strip $(HAS_SHADOW)),y)
	# Remove getopt header since shadow password support is disabled.
	$(RM) $(PREFIX)$(DEVEL_PREFIX)include/shadow.h
endif
	-@for i in `find  $(PREFIX)$(DEVEL_PREFIX) -type d` ; do \
	    chmod 755 $$i; chmod 644 $$i/*.h > /dev/null 2>&1; \
	done;
	-find $(PREFIX)$(DEVEL_PREFIX) -name .svn | xargs $(RM) -r;
	-chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$$/\1:\2/'` $(PREFIX)$(DEVEL_PREFIX)
ifeq ($(strip $(HAVE_SHARED)),y)
	for i in `find lib/ -type l -name 'lib[a-zA-Z]*.so' | \
	sed -e 's/lib\///'` ; do \
		$(LN) -sf $(RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB)$$i.$(MAJOR_VERSION) \
		$(PREFIX)$(DEVEL_PREFIX)lib/$$i; \
	done;
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
	$(LN) -sf $(RUNTIME_PREFIX_LIB_FROM_DEVEL_PREFIX_LIB)libthread_db.so.1 \
		$(PREFIX)$(DEVEL_PREFIX)lib/libthread_db.so
endif
#	# If we build shared libraries then the static libs are PIC...
#	# Make _pic.a symlinks to make mklibs.py and similar tools happy.
	for i in `find lib/  -type f -name '*.a' | sed -e 's/lib\///'` ; do \
		$(LN) -sf $$i $(PREFIX)$(DEVEL_PREFIX)lib/`echo $$i \
			| sed -e 's/\.a$$/_pic.a/'`; \
	done;
	# Ugh!!! Remember that libdl.a and libdl_pic.a are different.  Since
	# libdl is pretty small, and not likely to benefit from mklibs.py and
	# similar, lets just remove libdl_pic.a and avoid the issue
	rm -f $(PREFIX)$(DEVEL_PREFIX)lib/libdl_pic.a
endif


# Installs run-time libraries
install_runtime:
ifeq ($(strip $(HAVE_SHARED)),y)
	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)lib
#	$(INSTALL) -m 644 lib/lib*-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
#		$(PREFIX)$(RUNTIME_PREFIX)lib
	cp -dRf lib/*.so* $(PREFIX)$(RUNTIME_PREFIX)lib
	@if [ -x lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so ] ; then \
	    set -e; \
		$(SHELL_SET_X); \
	    $(INSTALL) -m 755 lib/ld-uClibc-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so \
	    		$(PREFIX)$(RUNTIME_PREFIX)lib; \
	fi;
endif
ifeq ($(strip $(OPTIMIZED_SHARED)),y)
	utils/liboptim.sh $(CROSS) ; \
	if [ -e liboptim/libc.so.0 ]; then cp liboptim/libc.so.0 $(PREFIX)$(RUNTIME_PREFIX)lib/libuClibc-0.9.28.so; fi ; \
	if [ -e liboptim/libiw.so.29 ]; then cp liboptim/libiw.so.29 $(PREFIX)$(RUNTIME_PREFIX)lib/libiw.so.29; fi ; \
	for i in `find liboptim/ -name lib[a-zA-Z]*.so.0 | sed -e 's/liboptim\///'` ; do \
		if [ "$$i" == libc.so.0 ]; then continue; fi ; \
		cp liboptim/$$i $(PREFIX)$(RUNTIME_PREFIX)lib/`echo $$i | sed -e 's/.so.0/-0.9.28.so/'` ; \
	done;
endif

.PHONY: utils
ifeq ($(strip $(HAVE_SHARED)),y)
utils:
	$(MAKE) CROSS="$(CROSS)" CC="$(CC)" -C utils
else
utils: dummy
endif

# Installs helper applications, such as 'ldd' and 'ldconfig'
install_utils: utils
	$(MAKE) CROSS="$(CROSS)" CC="$(CC)" -C utils install
#ifeq ($(strip $(UCLIBC_HAS_LOCALE)),y)
#	@$(MAKE) -C libc/misc/wchar iconv.target
#	$(INSTALL) -d $(PREFIX)$(RUNTIME_PREFIX)/usr/bin;
#	$(INSTALL) -m 755 libc/misc/wchar/iconv.target $(PREFIX)$(RUNTIME_PREFIX)/usr/bin/iconv
#endif

finished2:
	$(SECHO)
	$(SECHO) Finished installing ...
	$(SECHO)

else # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

all: menuconfig

# configuration
# ---------------------------------------------------------------------------
ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
extra/config/conf:
	$(MAKE) -C extra/config conf

extra/config/mconf:
	$(MAKE) -C extra/config ncurses mconf

menuconfig: extra/config/mconf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/mconf extra/Configs/Config.in

config: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf extra/Configs/Config.in

oldconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -o extra/Configs/Config.in

randconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -r extra/Configs/Config.in

allyesconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -y extra/Configs/Config.in
	sed -i -e "s/^DODEBUG=.*/# DODEBUG is not set/" .config
	sed -i -e "s/^DOASSERTS=.*/# DOASSERTS is not set/" .config
	sed -i -e "s/^SUPPORT_LD_DEBUG_EARLY=.*/# SUPPORT_LD_DEBUG_EARLY is not set/" .config
	sed -i -e "s/^SUPPORT_LD_DEBUG=.*/# SUPPORT_LD_DEBUG is not set/" .config
	sed -i -e "s/^UCLIBC_MJN3_ONLY=.*/# UCLIBC_MJN3_ONLY is not set/" .config
	@./extra/config/conf -o extra/Configs/Config.in

allnoconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -n extra/Configs/Config.in

defconfig: extra/config/conf
	$(RM) -r include/bits
	$(INSTALL) -d include/bits
	@./extra/config/conf -d extra/Configs/Config.in
endif

clean:
	- find . \( -name \*.o -o -name \*.a -o -name \*.so -o -name core -o -name .\#\* \) -exec $(RM) {} \;
ifneq ("$(CONFIG_DEFAULTS_FREESCALE_P1010)", "")
	$(RM) -r include
endif
	@$(RM) -r tmp lib include/bits libc/tmp _install lib_optim
	$(RM) libc/obj.* headers lib_diff
	$(MAKE) -C test clean
	$(MAKE) -C ldso clean
	$(MAKE) -C libc/misc/internals clean
	$(MAKE) -C libc/misc/wchar clean
	$(MAKE) -C libc/unistd clean
	$(MAKE) -C libc/sysdeps/linux/common clean
	$(MAKE) -C extra/locale clean
	$(MAKE) -C utils clean
	$(MAKE) -C extra clean

ifeq ($(CONFIG_LIB_LIBGMP_FORCE),y)
	$(MAKE) -C libgmp clean
endif
ifeq ($(CONFIG_LIB_LIBUPNP_FORCE),y)
	$(MAKE) -C libupnp clean
endif
ifeq ($(CONFIG_LIB_LIBBT_FORCE),y)
	$(MAKE) -C libbluetooth clean
endif
ifeq ($(CONFIG_LIB_READLINE_FORCE),y)
	$(MAKE) -C libreadline clean
endif
ifeq ($(CONFIG_LIB_FLEX_FORCE),y)
	$(MAKE) -C libflex clean
endif
ifeq ($(CONFIG_LIB_USB0112_FORCE),y)
	$(MAKE) -C libusb-0.1.12 clean
endif
ifeq ($(CONFIG_LIB_USB100_FORCE),y)
	$(MAKE) -C libusb-1.0.0 clean
endif
ifeq ($(CONFIG_LIB_LIBUSB_USER_FORCE),y)
	$(MAKE) -C libusb-user clean
endif
ifeq ($(CONFIG_LIB_LIBICONV_FORCE),y)
	$(MAKE) -C libiconv-1.14 clean
endif
ifeq ($(CONFIG_USER_WIFIDOG),y)
	$(MAKE) -C libcJSON clean
else
ifeq ($(CONFIG_LIB_LIBCJSON_FORCE),y)
	$(MAKE) -C libcJSON clean
endif
endif
ifeq ($(CONFIG_LIB_LIBGMP_5_0_4_FORCE), y)
	$(MAKE) -C gmp-5.0.4 distclean
endif

ifneq ("$(CONFIG_DEFAULTS_RALINK_RT2880)$(CONFIG_DEFAULTS_RALINK_RT3052)$(CONFIG_DEFAULTS_RALINK_RT3883)$(CONFIG_DEFAULTS_RALINK_RT5350)$(CONFIG_DEFAULTS_RALINK_MT7620)$(CONFIG_DEFAULTS_RALINK_MT7621)$(CONFIG_DEFAULTS_RALINK_MT7628)","")
	@set -e; \
	for i in `(cd $(TOPDIR)/libc/sysdeps/linux/common/sys; ls *.h)` ; do \
		$(RM) include/sys/$$i; \
	done; \
	if [ -d libc/sysdeps/linux/$(TARGET_ARCH)/sys ] ; then \
		for i in `(cd libc/sysdeps/linux/$(TARGET_ARCH)/sys; ls *.h)` ; do \
			$(RM) include/sys/$$i; \
		done; \
	fi;
	@$(RM) include/linux include/asm*
	@if [ -d libc/sysdeps/linux/$(TARGET_ARCH) ]; then		\
	    $(MAKE) -C libc/sysdeps/linux/$(TARGET_ARCH) clean;		\
	fi;
endif

distclean: clean
	$(RM) .config .config.old .config.cmd

release: distclean
	cd ..;					\
	$(RM) -r uClibc-$(VERSION);		\
	cp -dRf uClibc uClibc-$(VERSION);	\
	find uClibc-$(VERSION)/ -type f		\
	    -name .\#* -exec $(RM) -r {} \; ;	\
	find uClibc-$(VERSION)/ -type d		\
	    -name .svn -exec $(RM) -r {} \; ;	\
						\
	tar -cvzf uClibc-$(VERSION).tar.gz uClibc-$(VERSION)/;

endif # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

check:
	$(MAKE) -C test

.PHONY: dummy subdirs release distclean clean config oldconfig menuconfig
