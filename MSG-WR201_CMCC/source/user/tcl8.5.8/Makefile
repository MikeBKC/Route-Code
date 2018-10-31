
curdir = $(shell pwd)
all:
	$(MAKE) -C $(curdir)/unix all
	$(MAKE) -C $(curdir)/unix DESTDIR=$(curdir)/ipkg-install install

romfs:
	cp -fpR $(curdir)/ipkg-install/usr/bin/tclsh8.5  ../../romfs/usr/bin
	cp -fpR $(curdir)/ipkg-install/usr/lib/*.so*  ../../romfs/lib
	mkdir -p ../../romfs/usr/lib/tcl8.5
	cp -fpR $(curdir)/ipkg-install/usr/lib/tcl8.5/init.tcl ../../romfs/usr/lib/tcl8.5/
	

clean:
	$(MAKE) -C $(curdir)/unix clean
	rm -rf ipkg-install


