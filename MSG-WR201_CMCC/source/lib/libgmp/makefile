TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# gmp
LIBGMP_A= libgmp.a
LIBGMP_SHARED=libgmp.so
LIBGMP_SHARED_FULLNAME=libgmp-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


all: Makefile
	make -f Makefile all
	cp .libs/$(LIBGMP_A) .

Makefile:
	AR=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ar AS=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-as CC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-gcc LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ld OBJCOPY=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-objcopy RANLIB=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ranlib STRIP=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-strip ./configure $(CONFOPTS)

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a 
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBGMP_SHARED).$(MAJOR_VERSION) \
         -o $(LIBGMP_SHARED_FULLNAME) --whole-archive $(LIBGMP_A) \
         --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
         -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED)
	$(LN) -sf $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED).$(MAJOR_VERSION)
