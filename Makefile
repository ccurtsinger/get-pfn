CC ?= clang
CFLAGS ?= -fPIC
PREFIX ?= /usr/local

all: get-pfn libpfn.so

clean:
	rm -f get-pfn

get-pfn: get-pfn.c
	$(CC) $(CFLAGS) -o $@ $<

libpfn.so: libpfn.c
	$(CC) $(CFLAGS) -shared -o $@ $<

install: get-pfn libpfn.so
	cp get-pfn $(PREFIX)/bin/get-pfn
	chmod a+x $(PREFIX)/bin/get-pfn
	setcap cap_sys_admin+ep $(PREFIX)/bin/get-pfn
	cp libpfn.so $(PREFIX)/lib/libpfn.so

.PHONY: all clean install

