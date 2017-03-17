all:
	gcc -Wall -o main main.c
	gcc -Wall -D_GNU_SOURCE -fPIC -shared -o IOHook.so IOHook.c -ldl

run:
	$ LD_PRELOAD="./IOHook.so" ./main
