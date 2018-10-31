SCRIPTS = $(shell ls ./ap/ )
SRCDIR = $(shell pwd)
all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/sbin/lua/AC
	for i in $(SCRIPTS) ; do \
	     chmod 755 $$i ; \
		cp $(SRCDIR)/ap/* $(ROMFSDIR)/ -rf ; \
	done

