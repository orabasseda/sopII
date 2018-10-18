#pragma once

#include "arbre-binari/red-black-tree.h"

/**
 *
 * Function heders we want to make visible so that they
 * can be called from any other file.
 *
 */

char *split (char *str, int pos, int *size);
void delete_database(rb_tree *tree);
rb_tree *build_database(char *filename1, char *filename2);
char *max_destinations(rb_tree *tree, int *number);
