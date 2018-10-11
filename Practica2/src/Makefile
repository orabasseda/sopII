# This is the makefile that generates the executable

# Exectuable to generate
TARGET = main

# Compilation options
CFLAGS = -Wall -Werror -g

# Linker options 
LFLAGS = -lm

# There is no need to change the instructions below this
# line. Change if you really know what you are doing.

FILES_O = red-black-tree.o linked-list.o main.o

$(TARGET): $(FILES_O) Makefile
	gcc $(FILES_O) -o $(TARGET) $(LFLAGS) 

red-black-tree.o: arbre-binari/red-black-tree.c Makefile
	gcc $(CFLAGS) -c arbre-binari/red-black-tree.c

linked-list.o: linked-list/linked-list.c Makefile
	gcc $(CFLAGS) -c linked-list/linked-list.c

main.o: main.c Makefile
	gcc $(CFLAGS) -c main.c

all: $(TARGET) 

clean:
	/bin/rm $(FILES_O) $(TARGET) 
