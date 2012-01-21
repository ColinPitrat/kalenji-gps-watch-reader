TARGET=kalenji_reader
INCPATH=$(shell pkg-config --cflags libusb-1.0) $(shell pkg-config --cflags libxml-2.0)
LIBS=$(shell pkg-config --libs libusb-1.0) $(shell pkg-config --libs libxml-2.0)
OBJECTS=$(shell find . -name \*.cc | sed 's/.cc/.o/')
HEADERS=$(shell find . -name \*.h)
CFLAGS=

debug: CFLAGS=-D DEBUG=1 -g

.PHONY: all debug clean check_deps

all: check_deps $(OBJECTS)
	ctags -R .
	g++ $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

check_deps:
	@pkg-config --libs libusb-1.0 >/dev/null 2>&1 || (echo "Error: missing dependency libusb-1.0" && false)
	@pkg-config --libs libxml-2.0 >/dev/null 2>&1 || (echo "Error: missing dependency libxml2" && false)

$(OBJECTS): %.o:%.cc $(HEADERS)
	g++ $(CFLAGS) -c $(INCPATH) -o $@ $<

debug: all

clean:
	rm -f $(TARGET) $(OBJECTS) tags core
