CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -pedantic -g -std=c99

# Set this to true if using iterators, ie LIB_USE_ITER := true
# otherwise, leave it empty.
LIB_USE_ITER := true

# sources/includes/defines for static util lib
LIB_SRC := sayhi.c
LIB_INC := -I.
LIB_DEF :=
LIB_DIR := debug
LIB_NAME := sayhi

# Get names to give object files for each source file
LIB_OBJ = $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%.o))

all : MKDIR $(LIB_OBJ)
	$(AR) rcs $(LIB_DIR)/$(addprefix lib,$(LIB_NAME).a) $(LIB_OBJ)

clean :
	rm -rf $(LIB_DIR)

MKDIR :
	mkdir -p $(dir $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%)))

$(LIB_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(LIB_INC) $(LIB_DEF) $< -o $@

