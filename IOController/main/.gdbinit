set environment LD_LIBRARY_PATH=../IOHook/debug
set environment LD_PRELOAD=../IOHook/debug
b main
run
set scheduler-locking on
