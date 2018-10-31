.EXPORT_ALL_VARIABLES:

DIRS = src


version =     -DVERSION=\"2.76\"
CFLAGS += $(version)

all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do make -C $$i $@ ; done

clean:
	for i in $(DIRS) ; do make -C $$i $@ ; done
