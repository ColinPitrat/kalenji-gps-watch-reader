TARGET=kalenji_reader
INCPATH=-I/usr/include/libusb-1.0/
LIBS=-lusb-1.0
OBJECTS=$(shell find . -name \*.cc | sed 's/.cc/.o/')
HEADERS=$(shell find . -name \*.h)
CFLAGS=

debug: CFLAGS=-D DEBUG=1 -g

.PHONY: all debug clean

all: $(OBJECTS)
	ctags -R .
	g++ $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o:%.cc $(HEADERS)
	g++ $(CFLAGS) -c $(INCPATH) -o $@ $<

debug: all

clean:
	rm -f $(TARGET) $(OBJECTS) tags core
