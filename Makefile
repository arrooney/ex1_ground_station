# Edit compile time settings here           #
#############################################
# Compiler
CC := gcc
AR := ar

# Compile and link flags
CFLAGS := -Wall -Wextra -pedantic -g -std=c99 -O0
LFLAGS := $(CFLAGS) -Wl,-rpath=../IOHook/debug

# Output name (ie, the thing you execute in the terminal)
PROG := main

# Location of include files
INC_PATH := ./ ../../CObject/liba/Class ../../CObject/liba/util ../../CObject/liba/tests ../IOHook

# Defines
DEFINES := _POSIX_C_SOURCE=199309L _GNU_SOURCE

# Directories of static libraries
LIB_PATH := ../../CObject/liba/Class/debug ../../CObject/liba/util/debug ../IOHook/debug

# Static libraries to link
LIB_NAME := IOHook cutil cclass pthread

# Directory to put all object files in
BUILD_DIR := debug
#############################################
# Stop editting  compile time settings here #

# Source files and include directories. $(shell echo ./**/*.c)
SRC := $(shell echo ./*.c) $(shell echo ./src/*.c) 
INC := $(addprefix -I,$(INC_PATH))
OBJ := $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%.o))
DEF := $(addprefix -D,$(DEFINES))
LIB_SRC := $(addprefix -L,$(LIB_PATH))
LIB := $(addprefix -l,$(LIB_NAME))

inspect:
	$(CC) -Wall -E $(INC) $(DEF) main.c -o main.e

all: MKDIR $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) -o $(BUILD_DIR)/$(PROG) $(DEF) $(LIB_SRC) $(LIB)

# Note, $< is the first pre-req, in this case, %.c
# Note, $@ is the call, in this case, $(BUILD_DIR)/%.o
$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(INC) $(DEF) $< -o $@

MKDIR:
	mkdir -p $(dir $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%)))

run:
	$ LD_PRELOAD="../IOHook/debug/libIOHook.so" ./$(BUILD_DIR)/$(PROG)

clean:
	rm -rf $(BUILD_DIR)

print-%:
	@echo $* = $($*)
