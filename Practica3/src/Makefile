# This is the makefile that generates the executable

# Files to compile
FILES_C = menu.c

# Exectuable to generate
TARGET = menu

# Compilation options
CFLAGS = -Wall -Werror -g

# Linker options 
LFLAGS = -lm

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.

FILES_O = data-base/arbre-binari/red-black-tree.o data-base/linked-list/linked-list.o data-base/database.o

EXTRA_O = menu.o

$(TARGET): $(FILES_O) $(EXTRA_O) Makefile
	gcc $(FILES_O) $(EXTRA_O) -o $(TARGET) $(LFLAGS) 

$(EXTRA_O): $(FILES_C) Makefile
	gcc $(CFLAGS) -c $<

$(FILES_O):
	$(MAKE) -C data-base

all:
	$(TARGET)

clean:
	/bin/rm $(TARGET) $(EXTRA_O)
	$(MAKE) -C data-base clean
