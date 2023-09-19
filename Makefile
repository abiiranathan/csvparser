mingw=/usr/bin/x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Werror -pedantic -O2 -std=c11 -I.

.PHONY: build win32 test

all: mkdir build win32 test

mkdir:
	mkdir -p build

build:
	gcc $(CFLAGS) main.c csvparser.c -o build/main

win32:
	$(mingw) $(CFLAGS) main.c csvparser.c -o build/main.exe

test:
	gcc $(CFLAGS) tests/test_csvparser.c csvparser.c -o build/test_main
	./build/test_main

run: build
	./build/main ./data.csv

clean:
	rm -rf build
