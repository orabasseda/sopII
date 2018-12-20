#pragma once

#include "arbre-binari/red-black-tree.h"

/* Struct with the information needed for a thread
 */
typedef struct cell_ {
    char **block;
    int *size;
} cell;

typedef struct buffer_ {
    cell **buffer;
    int *num_cells; //numero de cells amb read = 1
    int *r;
    int *w;
    int *eof;
} buffer;

typedef struct producer_data_ {
    FILE *fp;
    buffer *buffer;
} producer_data;

typedef struct consumer_data_ {
    rb_tree *tree;
    buffer *buffer;
} consumer_data;

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
