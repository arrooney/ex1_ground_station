# Source name and output executable name of ram script
RAM_EXEC := ram.o
RAM_SRC := ram.c

# Uncomment this symbol to include debug printing in the code
#RAM_DEBUG := true

# Uncomment this symbol to stop the code after putting the nanomind
# into safemode. No image will be uploaded if this is uncommented.
RAM_SAFEMODE_ONLY := true


# Don't change anything beyond this point
RAM_DEF := -D_BSD_SOURCE
ifdef RAM_DEBUG
	RAM_DEF += -DDEBUG
endif
ifdef RAM_SAFEMODE_ONLY
	RAM_DEF += -DIMAGE_INHIBIT
endif

all : mram

mram : clean
	gcc -Wall -Wextra -std=c99 $(RAM_DEF) $(RAM_SRC) -o $(RAM_EXEC)

ram:
	./$(RAM_EXEC)

clean:
	rm -f $(RAM_EXEC) *~ ./**/*~
