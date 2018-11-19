# This is the makefile that generates the executable

# Files to compile
FILES_C = red-black-tree.c

# Compilation options
CFLAGS = -Wall -Werror -O

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.

FILES_O = $(subst .c,.o,$(FILES_C))

%.o: %.c Makefile
	gcc $(CFLAGS) -c $<

all: $(FILES_O) 

clean:
	/bin/rm $(FILES_O)
