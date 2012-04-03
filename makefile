BUILDTYPE ?= Release
PYTHON ?= python

all: out/Makefile sorrow

.PHONY: sorrow

test: all
	./sorrow test/driver.js

install: all
	./sorrow tools/install.js

sorrow: config.gypi
	$(MAKE) -C out BUILDTYPE=Release
	ln -fs out/Release/sorrow sorrow

config.gypi: configure
	./configure

out/Makefile: common.gypi deps/v8/build/common.gypi deps/v8/tools/gyp/v8.gyp sorrow.gyp config.gypi
	tools/sorrow_gyp -f make

clean:
	-rm -rf out/Makefile sorrow out/$(BUILDTYPE)/sorrow
	-find out/ -name '*.o' -o -name '*.a' | xargs rm -rf

distclean:
	-rm -rf out
	-rm -f config.gypi
	-rm -f config.mk

dependencies:
	svn co http://v8.googlecode.com/svn/trunk@10833 deps/v8 
	svn co http://gyp.googlecode.com/svn/trunk@1249 tools/gyp 

	
