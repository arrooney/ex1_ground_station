CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -pedantic -g

# sources/includes/objects for static util lib
LIB_SRC := $(shell echo ./*.c) $(shell echo ./**/*.c)
LIB_INC := -I. -I../Class -I../util
LIB_DEF := -DCIITERATOR_INCLUDED -D_GNU_SOURCE
LIB_DIR := debug
LIB_NAME := ctests
LIB_OBJ := $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%.o))

all : MKDIR $(LIB_OBJ)
	$(AR) rcs $(LIB_DIR)/$(addprefix lib,$(LIB_NAME).a) $(LIB_OBJ)

clean :
	rm -rf $(LIB_DIR)

rmbackup:
	rm -f ./*~ ./**/*~

MKDIR :
	mkdir -p $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%))

$(LIB_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(LIB_INC) $(LIB_DEF) $< -o $@

call: clean all
