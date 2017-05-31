CC := clang

all: get-pfn

clean:
	rm -f get-pfn

get-pfn: get-pfn.c
	$(CC) -o $@ get-pfn.c

install: get-pfn
	cp get-pfn /usr/local/bin/get-pfn
	chmod a+x /usr/local/bin/get-pfn
	setcap cap_sys_admin+ep /usr/local/bin/get-pfn

.PHONY: all clean install

