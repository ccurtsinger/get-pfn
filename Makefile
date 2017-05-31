CC := clang

all: get-pfn

clean:
	rm -f get-pfn

get-pfn: get-pfn.c
	$(CC) -o $@ get-pfn.c

grant-access: get-pfn
	chmod a+x get-pfn
	sudo setcap cap_sys_admin+ep get-pfn

.PHONY: all clean grant-access

