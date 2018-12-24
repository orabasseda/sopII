#pragma once

#include "arbre-binari/red-black-tree.h"

/* Struct with the information needed for a thread
 */
typedef struct cell_ {
    char **block; /* Current block stored in cell */
    int size; /* Size of the block */
} cell;

typedef struct buffer_ {
    cell **buffer; /* Vector of cells */
    int num_cells; /* Number of cells already occupied */
    int r; /* Position to read from */
    int w; /* Position to write to */
    int eof; /* Indicates if we've reached the end of file */
} buffer;

typedef struct producer_data_ {
    FILE *fp; /* Pointer to a file */
    buffer *buffer; /* Pointer to the cell buffer */
} producer_data;

typedef struct consumer_data_ {
    rb_tree *tree; /* Pointer to the tree */
    buffer *buffer; /* Pointer to the cell buffer */
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
