SHELL := /bin/bash
all:
		gcc -Wall -Werror -g -fpic -c denbu_mem.c
		gcc -shared -o libdenbu_mem.so *.o
		gcc -I. -L. main.c -o test -ldenbu_mem
clean:
		rm libdenbu_mem.so
		rm *.o