#pragma once

#include "arbre-binari/red-black-tree.h"
#define NUM_CELLS 100

/* Struct with the information needed for a thread
 */
typedef struct producer_data_ {
    FILE *fp;
    int *eof;
    buffer *buffer;
} producer_data;

typedef struct consumer_data_ {
    rb_tree *tree;
    buffer *buffer;
} consumer_data;

typedef struct cell_ {
    char **block;
    int read = 0;
} cell;

typedef struct buffer_ {
    cell *buffer[NUM_CELLS];
    int num_cells = 0;
    int r = 0;
    int w = 0;
} buffer;

/**
 *
 * Function heders we want to make visible so that they
 * can be called from any other file.
 *
 */
char *split (char *str, int pos, int *size);
void delete_database(rb_tree *tree);
rb_tree *build_database(char *filename1, char *filename2, rb_tree *tree);
void max_destinations(rb_tree *tree);
void delay(rb_tree *tree, char *origin);
rb_tree *create_database();
void insert_node_tree(rb_tree *tree, char *origin);
void insert_destination(rb_tree *tree, char *origin, char *destination, int num_vols, int delay);
