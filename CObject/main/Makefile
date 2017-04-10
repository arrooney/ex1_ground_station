CC := gcc
AR := ar

CFLAGS := -Wall -Wextra -pedantic -g -Os

# Build directory for executable
BUILDDIR := debug

# Name of binary executable
EXEC = main

# Path to all header files used
INCLUDES := -I../liba/Class -I../liba/tests -I../liba/util

# Path to all source files used
SOURCES := main.c

# All object files
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

# All libraries
STATIC_LIB_SRC := ../liba/Class ../liba/tests ../liba/util
STATIC_LIB_NAME := ctests cutil cclass pthread

STATIC_LIBS := $(addprefix -l,$(STATIC_LIB_NAME));
STATIC_LIB_BUILD := $(addsuffix /debug,$(addprefix -L,$(STATIC_LIB_SRC)))

all : MKDIR BUILD_LIBS $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(BUILDDIR)/$(EXEC) $(STATIC_LIB_BUILD) $(STATIC_LIBS)
	cp $(BUILDDIR)/$(EXEC) ./

$(BUILDDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

BUILD_LIBS :
	make -C ../liba/util/ all
	make -C ../liba/tests/ all
	make -C ../liba/Class all

MKDIR :
	mkdir -p $(dir $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%)))

clean :
	rm -rf $(BUILDDIR)
	make -C ../liba/util/ clean
	make -C ../liba/tests/ clean
	make -C ../liba/Class clean


run:
	./$(BUILDDIR)/$(EXEC)

crun: clean all run
