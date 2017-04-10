CC := gcc
AR := ar
CFLAGS := -Wall -Wextra -pedantic -g -Os

# sources/includes/objects for static util lib
LIB_SRC := $(shell echo ./*.c)
LIB_INC := -I.
LIB_DIR := debug
LIB_NAME := cclass
LIB_OBJ := $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%.o))

all : MKDIR $(LIB_OBJ)
	$(AR) rcs $(LIB_DIR)/$(addprefix lib,$(LIB_NAME).a) $(LIB_OBJ)

clean :
	rm -rf $(LIB_DIR)

MKDIR :
	mkdir -p $(addprefix $(LIB_DIR)/,$(LIB_SRC:%.c=%))

$(LIB_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $(LIB_INC) $< -o $@
