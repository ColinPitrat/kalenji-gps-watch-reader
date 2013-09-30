TARGET=kalenji_reader
INCPATH=$(shell pkg-config --cflags libusb-1.0) $(shell pkg-config --cflags libxml-2.0) $(shell pkg-config --cflags libcurl)
LIBS=$(shell pkg-config --libs libusb-1.0) $(shell pkg-config --libs libxml-2.0) $(shell pkg-config --libs libcurl)
WINOBJECTS=$(shell find src -name \*.cc | sed 's/.cc/.os/')
OBJECTS=$(shell find src -name \*.cc | sed 's/.cc/.o/')
HEADERS=$(shell find src -name \*.h)
CFLAGS=
WININCPATH=-I/usr/i486-mingw32/include/libusb-1.0/ -I/usr/i486-mingw32/include/libxml2/
WINLIBS=/usr/i486-mingw32/lib/libusb-1.0.dll.a /usr/i486-mingw32/lib/libxml2.dll.a /usr/i486-mingw32/lib/libcurl.dll.a
WINCFLAGS=-DWINDOWS

debug: CFLAGS=-D DEBUG=1 -g

.PHONY: all debug clean check_deps

all: check_deps $(OBJECTS)
	ctags -R .
	g++ $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

windows: $(WINOBJECTS)
	mkdir -p win
	i486-mingw32-g++ $(WINCFLAGS) -o win/$(TARGET).exe $(WINOBJECTS) $(WINLIBS)
	cp /usr/i486-mingw32/bin/libcurl-4.dll win/
	cp /usr/i486-mingw32/bin/libeay32.dll win/
	cp /usr/i486-mingw32/bin/libiconv-2.dll win/
	cp /usr/i486-mingw32/bin/libidn-11.dll win/
	cp /usr/i486-mingw32/bin/libintl-8.dll win/
	cp /usr/i486-mingw32/bin/libusb-1.0.dll win/
	cp /usr/i486-mingw32/bin/libxml2-2.dll win/
	cp /usr/i486-mingw32/bin/ssleay32.dll win/
	cp /usr/i486-mingw32/bin/zlib1.dll win/
	cp /usr/i486-mingw32/lib/libgcc_s_sjlj-1.dll win/
	cp /usr/i486-mingw32/lib/libstdc++-6.dll win/

check_deps:
	@pkg-config --libs libusb-1.0 >/dev/null 2>&1 || (echo "Error: missing dependency libusb-1.0" && false)
	@pkg-config --libs libxml-2.0 >/dev/null 2>&1 || (echo "Error: missing dependency libxml2" && false)
	@pkg-config --libs libcurl >/dev/null 2>&1 || (echo "Error: missing dependency libcurl" && false)

$(OBJECTS): %.o:%.cc $(HEADERS)
	g++ $(CFLAGS) -c $(INCPATH) -o $@ $<

$(WINOBJECTS): %.os:%.cc $(HEADERS)
	i486-mingw32-g++ $(WINCFLAGS) -c $(WININCPATH) -o $@ $<

debug: all

test: all
	cd test && ./run.sh && cd ..

clean:
	rm -rf $(TARGET) $(OBJECTS) $(WINOBJECTS) tags core win
