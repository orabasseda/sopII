# This is the makefile that generates the executable

# Files to compile
FILES_C = database.c

TARGET = database.o

# Compilation options
CFLAGS = -Wall -Werror -g

$(TARGET): $(FILES_C)
	$(MAKE) -C arbre-binari
	$(MAKE) -C linked-list
	gcc $(CFLAGS) -c $<

all:
	$(TARGET)

clean:
	/bin/rm $(TARGET)
	$(MAKE) -C arbre-binari clean
	$(MAKE) -C linked-list clean
