#pragma once

#include "arbre-binari/red-black-tree.h"

typedef struct thread_data_ {
    rb_tree *tree;
    FILE *fp;
} thread_data;

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
