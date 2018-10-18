# This is the makefile that generates the executable

# Files to compile
FILES_C = data-base/arbre-binari/red-black-tree.c data-base/linked-list/linked-list.c menu.c

# Exectuable to generate
TARGET = menu

# Compilation options
CFLAGS = -Wall -Werror -g

# Linker options 
LFLAGS = -lm

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.

FILES_O = red-black-tree.o linked-list.o menu.o

$(TARGET): $(FILES_O) Makefile
	gcc $(FILES_O) -o $(TARGET) $(LFLAGS) 

%.o: %.c Makefile
	gcc $(CFLAGS) -c $<

all: $(TARGET) 

clean:
	/bin/rm $(FILES_O) $(TARGET) 
