mingw=/usr/bin/x86_64-w64-mingw32-gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -g -std=c23
LDFLAGS=-lsolidc

.PHONY: build test

all: mkdir build test

mkdir:
	mkdir -p build

build:
	gcc $(CFLAGS) large_csv.c csvparser.c -o build/main $(LDFLAGS)
	# gcc $(CFLAGS) main.c csvparser.c -o build/main $(LDFLAGS)

test:
	gcc $(CFLAGS) tests/test_csvparser.c csvparser.c -o build/test_main $(LDFLAGS)
	./build/test_main

run: build
	./build/main ./data.csv

clean:
	rm -rf build
