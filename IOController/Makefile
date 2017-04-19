# Edit compile time settings here           #
#############################################
# Compiler
CC := gcc
AR := ar

# Compile flags
CFLAGS := -Wall -Wextra -pedantic -g -std=c99

# Output name
PROG := main
IOHOOK := IOHook

# Location of custom include files
INC_PATH := ./

# Directory to put all object files in
BUILD_DIR := debug
#############################################
# Stop editting  compile time settings here #

# Path to CObject libraries
COBJ_CLASS_PATH = ../CObject/liba/Class
COBJ_UTIL_PATH = ../CObject/liba/util

# Source files and include directories
SRC := $(shell echo ./*.c) $(shell echo ./**/*.c)
INC := -I$(COBJ_CLASS_PATH) -I$(COBJ_UTIL_PATH) $(addprefix -I,$(INC_PATH))

all: MKDIR
	make -C $(COBJ_CLASS_PATH) all
	make -C $(COBJ_UTIL_PATH) all
	$(CC) $(CFLAGS) -o $(PROG) main.c
	gcc -Wall -D_GNU_SOURCE -fPIC -shared -o $(IOHOOK).so IOHook.c -ldl

MKDIR:
	mkdir -p $(dir $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%)))

run:
	$ LD_PRELOAD="./IOHook.so" ./main

clean:
	make -C $(COBJ_CLASS_PATH) clean
	make -C $(COBJ_UTIL_PATH) clean
	rm -rf $(BUILD_DIR)
