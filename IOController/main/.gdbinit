set environment LD_LIBRARY_PATH=../IOHook/debug
set environment LD_PRELOAD=../IOHook/debug
b main
b gomshell_thread
run
set scheduler-locking on
