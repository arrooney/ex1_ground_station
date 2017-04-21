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
#INC_PATH := ./ ../../CObject/liba/Class ../../CObject/liba/util ../../CObject/liba/tests ../IOHook
INC_PATH := include client

# Defines
#DEFINES := _POSIX_C_SOURCE=199309L _GNU_SOURCE
DEFINES :=

# Directories of static libraries
#LIB_PATH := ../../CObject/liba/Class/debug ../../CObject/liba/util/debug ../IOHook/debug


# Static libraries to link
LIB_NAME := IOHook cutil cclass pthread

# Directory to put all object files in
BUILD_DIR := debug
#############################################
# Stop editting  compile time settings here #

# Source files and include directories. $(shell echo ./**/*.c) $(shell echo ./*.c) 
SRC := $(shell echo ./src/*.c) 
INC := $(addprefix -I,$(INC_PATH))
OBJ := $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%.o))
DEF := $(addprefix -D,$(DEFINES))
LIB_SRC := $(addprefix -L,$(LIB_PATH))
LIB := $(addprefix -l,$(LIB_NAME))

#['lib/libcsp', 'lib/libparam', 'lib/libutil', 'lib/libgosh', 'lib/libftp', 'lib/liblog']

inspect:
	$(CC) -Wall -E $(INC) $(DEF) main.c -o main.e

configure:
	./waf configure

all: MKDIR #$(OBJ)
	./waf build
#	$(CC) $(LFLAGS) $(OBJ) -o $(BUILD_DIR)/$(PROG) $(DEF) $(LIB_SRC) $(LIB)

# Note, $< is the first pre-req, in this case, %.c
# Note, $@ is the call, in this case, $(BUILD_DIR)/%.o
$(BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(INC) $(DEF) $< -o $@

MKDIR:
	mkdir -p $(dir $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%)))

run:
	$ LD_PRELOAD="../IOHook/debug/libIOHook.so" ./$(BUILD_DIR)/$(PROG)

clean:
	(cd lib/libcsp && ./waf clean)
	rm -rf $(BUILD_DIR)

print-%:
	@echo $* = $($*)
