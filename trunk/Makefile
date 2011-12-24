all:
	ctags -R .
	g++ main.cc -o kalenji_reader -lusb-1.0 -I/usr/include/libusb-1.0/

debug:
	g++ -g -D DEBUG=1 main.cc -o kalenji_reader -lusb-1.0 -I/usr/include/libusb-1.0/

clean:
	rm kalenji_reader tags
