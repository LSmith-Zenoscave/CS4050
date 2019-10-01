.PHONY: all

all: quicksort

quicksort: *.c
	clang -Ofast -funroll-loops -fomit-frame-pointer -finline -o $@ $<
