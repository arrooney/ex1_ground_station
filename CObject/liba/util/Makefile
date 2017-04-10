CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -pedantic -g -std=c99

# Set this to true if using iterators, ie LIB_USE_ITER := true
# otherwise, leave it empty.
LIB_USE_ITER := true

# Symbols required to compile in iterators and the source location
# of iterator code
LIB_USE_ITER_DEF := -DCIITERATOR_INCLUDED
LIB_ITER_SRC := src/CIIterator.c src/CCListIterator.c

# sources/includes/defines for static util lib
LIB_SRC := $(shell echo src/*.c)
LIB_INC := -I. -I../Class
LIB_DEF := -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
LIB_DIR := debug
LIB_NAME := cutil

# Determine if iterators are being compiled in or not
# If they're being used, defined symbols required for them
# If they're not used, remove iterator source from compilation
ifdef LIB_USE_ITER
	LIB_DEF += $(LIB_USE_ITER_DEF)
else
	LIB_SRC := $(filter-out $(LIB_ITER_SRC), $(LIB_SRC))
endif

# Get names to give object files for each source file
LIB_OBJ = $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%.o))

all : MKDIR $(LIB_OBJ)
	$(AR) rcs $(LIB_DIR)/$(addprefix lib,$(LIB_NAME).a) $(LIB_OBJ)

clean :
	rm -rf $(LIB_DIR)

rmbackup :
	rm -f *~ **/*~

MKDIR :
	mkdir -p $(dir $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%)))

$(LIB_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(LIB_INC) $(LIB_DEF) $< -o $@

syntax :
	$(CC) -fsyntax-only -c $(LIB_SRC)  $(LIB_INC)
