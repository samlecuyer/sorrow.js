CC = g++
JSC = tools/js2c.py
JSMIN = java -jar tools/yuicompressor-2.4.7.jar
ARCH = -arch i386
V8ARCH = ia32.release
INCS = -Ideps/v8/include/
LIBS = -Ldeps/v8/out/$(V8ARCH) -lv8_base -lv8_snapshot
CCFILES = src/sorrow.cpp src/sorrow_binary.cpp

all: sorrow

sorrow: libv8 $(CCFILES) src/gen/sorrow_natives.h
	$(CC) $(ARCH) $(CCFILES)  $(INCS) $(LIBS) -o sorrow

src/gen/sorrow_natives.h: src/gen/sorrow.js lib/*.js
	$(JSC) src/gen/sorrow_natives.h src/gen/sorrow.js lib/*.js

src/gen/sorrow.js: src/sorrow.js v8
	$(JSMIN) src/sorrow.js -o src/gen/sorrow.js

libv8: v8
	cd deps/v8 && export GYP_GENERATORS=make && make dependencies && make $(V8ARCH) -j2

v8:
	svn co http://v8.googlecode.com/svn/trunk@10833 deps/v8 

clean:
	rm sorrow src/gen/*.h src/gen/*.js