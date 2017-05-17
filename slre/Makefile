clean:
	rm -rf debug
all:
	mkdir -p debug
	gcc -Wall -Wextra -g -c slre.c -o debug/slre.o
	ar rcs debug/libslre.a debug/slre.o
