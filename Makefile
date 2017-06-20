CC       := clang
CXX      := clang++
CFLAGS   := -O2
CXXFLAGS := $(CFLAGS) --std=c++11
PREFIX   ?= /usr/local

all: get-pfn example

clean:
	rm -f get-pfn example

get-pfn: get-pfn.c
	$(CC) $(CFLAGS) -o $@ $<

example: example.cc pagemap.hh
	$(CXX) $(CXXFLAGS) -o $@ $<

install: get-pfn
	cp get-pfn $(PREFIX)/bin/get-pfn
	chmod a+x $(PREFIX)/bin/get-pfn
	setcap cap_sys_admin+ep $(PREFIX)/bin/get-pfn
	cp pagemap.hh $(PREFIX)/include/

.PHONY: all clean install

