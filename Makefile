TARGET=kalenji_reader
INCPATH=-Isrc $(shell pkg-config --cflags libusb-1.0) $(shell pkg-config --cflags libxml-2.0) $(shell pkg-config --cflags libcurl)
LIBS=$(shell pkg-config --libs libusb-1.0) $(shell pkg-config --libs libxml-2.0) $(shell pkg-config --libs libcurl)
WINOBJECTS=$(shell find src -name \*.cc | sed 's/.cc/.os/')
OBJECTS=$(shell find src -name \*.cc | sed 's/.cc/.o/')
HEADERS=$(shell find src -name \*.h)
CFLAGS=-Wall -Wextra -Wno-unused-parameter -std=c++11
ADD_CFLAGS=-O2
DEBUG_ADD_CFLAGS=-D DEBUG=1 -D _GLIBCXX_DEBUG -O0 -g -coverage -pthread
ARCH ?= linux
ifeq ($(ARCH),win64)
MINGW_PATH=/usr/x86_64-w64-mingw32
WINCXX=x86_64-w64-mingw32-g++
else ifeq($(ARCH),win32)
MINGW_PATH=/usr/i686-w64-mingw32
WINCXX=i686-w64-mingw32-g++
else ifeq($(ARCH),linux)
MINGW_PATH=/usr/i686-w64-mingw32
WINCXX=i686-w64-mingw32-g++
else
$(error Unknown ARCH. Supported ones are linux, win32 and win64.)
endif
WININCPATH=-I$(MINGW_PATH)/include/libusb-1.0/ -I$(MINGW_PATH)/include/libxml2/
WINLIBS=$(MINGW_PATH)/lib/libusb-1.0.dll.a $(MINGW_PATH)/lib/libxml2.dll.a $(MINGW_PATH)/lib/libcurl.dll.a
WINCFLAGS=-DWINDOWS
GTEST_DIR=googletest/googletest/
GMOCK_DIR=googletest/googlemock/
TEST_CFLAGS=-I$(GTEST_DIR)/include -I$(GTEST_DIR) -I$(GMOCK_DIR)/include -I$(GMOCK_DIR) -I.
TEST_TARGET=test/unit/unit_tester
TEST_OBJECTS=$(shell find test/unit -name \*.cc | sed 's/.cc/.o/') $(GTEST_DIR)/src/gtest-all.o $(GMOCK_DIR)/src/gmock-all.o
TESTED_OBJECTS=$(shell find src -name \*.cc | grep -v main.cc | sed 's/.cc/.o/')
LAST_BUILD_IN_DEBUG=$(shell [ -e .debug ] && echo 1 || echo 0)
ifndef CXX
CXX=g++
endif
ifndef COV
COV=gcov
endif
SOURCES:=$(shell find src test -name \*.cc)
CLANG_TIDY=clang-tidy-3.8
CLANG_CHECKS=modernize-*,-modernize-pass-by-value,-modernize-use-using,performance-*,misc-*,-misc-unused-parameters,readability-*,-readability-braces-around-statements,-readability-implicit-bool-cast

debug: ADD_CFLAGS=$(DEBUG_ADD_CFLAGS)

.PHONY: unit_test build debug clean check_deps

ifeq ($(LAST_BUILD_IN_DEBUG), 1)
DUMMY:=$(shell echo -e '\n!!!!!!!!!!!\n!! Warning: previous build was in debug - rebuilding in debug.\n!! Use make clean before running make to rebuild in release.\n!!!!!!!!!!!\n' >&2)
ADD_CFLAGS=$(DEBUG_ADD_CFLAGS)
all: debug
else
all: build
endif

# the (read ...) part sorts the output except for the first line. The tac between and after make it sort the output except for the last line.
debug: debug_flag gtest build unit_test test tags
	$(COV) -p -r `find src -name \*.cc` | sed '/File/N;s/\n/ - /g' | grep -v Creating | grep -v "^$$" | tac | (read line; printf "%s\n" "$$line"; sort -k 2 -t : -g) | tac

gtest:
	[ -d googletest ] || git clone https://github.com/google/googletest.git

debug_flag:
	@touch .debug

tags:
	@ctags -R . || echo -e "!!!!!!!!!!!\n!! Warning: ctags not found - if you are a developer, you might want to install it.\n!!!!!!!!!!!"

build: $(TARGET)

$(TARGET): check_deps $(OBJECTS)
	$(CXX) $(CFLAGS) $(ADD_CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

win:
	mkdir -p win

$(TARGET).exe: $(WINOBJECTS) win
	$(WINCXX) $(CFLAGS) $(WINCFLAGS) -o win/$(TARGET).exe $(WINOBJECTS) $(WINLIBS)

windows: $(TARGET).exe win
	cp $(MINGW_PATH)/bin/libcurl-4.dll win/
	cp $(MINGW_PATH)/bin/libeay32.dll win/ || true
	cp $(MINGW_PATH)/bin/libiconv-2.dll win/
	cp $(MINGW_PATH)/bin/libidn-11.dll win/ || true
	cp $(MINGW_PATH)/bin/libintl-8.dll win/
	cp $(MINGW_PATH)/bin/libusb-1.0.dll win/
	cp $(MINGW_PATH)/bin/libxml2-2.dll win/
	cp $(MINGW_PATH)/bin/ssleay32.dll win/
	cp $(MINGW_PATH)/bin/zlib1.dll win/
	cp $(MINGW_PATH)/bin/libgcc_s_sjlj-1.dll win/
	cp $(MINGW_PATH)/bin/libstdc++-6.dll win/

check_deps:
	@pkg-config --libs libusb-1.0 >/dev/null 2>&1 || (echo "Error: missing dependency libusb-1.0. Try installing libusb development package (e.g: libusb libusb-1 libusb-1.0.0-dev ...)" && false)
	@pkg-config --libs libxml-2.0 >/dev/null 2>&1 || (echo "Error: missing dependency libxml2. Try installing libxml2 development package (e.g: libxml2 libxml2-dev ...)" && false)
	@pkg-config --libs libcurl >/dev/null 2>&1 || (echo "Error: missing dependency libcurl. Try installing libcurl development package (e.g: libcurl-dev libcurl4-gnutls-dev ...)" && false)

$(OBJECTS): %.o:%.cc $(HEADERS)
	$(CXX) $(CFLAGS) $(ADD_CFLAGS) -c $(INCPATH) -o $@ $<

$(TEST_OBJECTS): %.o:%.cc $(HEADERS)
	$(CXX) $(CFLAGS) $(ADD_CFLAGS) $(TEST_CFLAGS) -c $(INCPATH) -o $@ $<

$(WINOBJECTS): %.os:%.cc $(HEADERS)
	$(WINCXX) $(CFLAGS) $(WINCFLAGS) -c $(WININCPATH) -o $@ $<

unit_test: $(TEST_OBJECTS) $(TESTED_OBJECTS)
	$(CXX) $(CFLAGS) $(ADD_CFLAGS) $(TEST_CFLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TESTED_OBJECTS) $(LIBS)
	./$(TEST_TARGET) --gtest_shuffle

test: $(TARGET)
	rm -f /tmp/20[0-9][0-9][0-9][0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9][0-9].* /tmp/E9HG*.GHR
	cd test/integrated/ && ./run.sh && cd ..

clang-tidy:
	if $(CLANG_TIDY) -checks=-*,$(CLANG_CHECKS) $(SOURCES) -header-filter=src/* -- $(CFLAGS) $(ADD_CFLAGS) $(INCPATH) $(TEST_CFLAGS) | grep "."; then false; fi

cleancov:
	find . '(' -name \*.gcda -or -name \*.gcov ')' -exec rm '{}' \;

clean: cleancov
	find . -name \*.gcno -exec rm '{}' \;
	rm -rf $(TARGET) $(OBJECTS) $(TEST_OBJECTS) $(WINOBJECTS) tags core win .debug
