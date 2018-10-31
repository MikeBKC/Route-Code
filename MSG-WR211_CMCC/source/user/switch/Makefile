


ifneq ($(CONFIG_IP1725), )
SW_DIRS    += acl
endif

ifeq ($(CONFIG_URCP_MANAGE),y)
ifeq ($(CONFIG_USER_CAPWAP_WTP)$(CONFIG_USER_CAPWAP_AC),)
SW_DIRS    += urcp
endif
ifeq ($(CONFIG_UTT_AC),y)
SW_DIRS    += urcpDb
endif
endif
#SW_DIRS    += easyNetbios

all:
	for i in $(SW_DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done
romfs:
	for i in $(SW_DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done
clean:
	for i in $(SW_DIRS) ; do $(MAKE) -C $$i $@; done

