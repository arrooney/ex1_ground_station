# Edit compile time settings here           #
#############################################
# Compiler
CC := gcc

# Compile flags
CFLAGS := -Wall -Wextra -pedantic -g -std=c99

# defines
DEFINE := _POSIX_C_SOURCE=199309L _GNU_SOURCE

# Output name
LIB := IOHook

# Location of custom include files
INC_PATH := ./ ../../CObject/liba/Class ../../CObject/liba/util ../../CObject/liba/tests ../IOHook

# Directory to put all object files in
BUILD_DIR := debug
#############################################
# Stop editting  compile time settings here #

# Path to CObject libraries
COBJ_CLASS_PATH = ../../CObject/liba/Class
COBJ_UTIL_PATH = ../../CObject/liba/util

# Source files and include directories
# add this if putting source files in directories #$(shell echo ./**/*.c)
SRC := $(shell echo ./*.c) 
INC := -I$(COBJ_CLASS_PATH) -I$(COBJ_UTIL_PATH) $(addprefix -I,$(INC_PATH))

all: MKDIR
	$(CC) $(CFLAGS) $(addprefix -D,$(DEFINE)) $(INC) -fPIC -shared -o $(BUILD_DIR)/lib$(LIB).so $(SRC) -ldl

MKDIR:
	mkdir -p $(dir $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%)))

clean:
	rm -rf $(BUILD_DIR)
