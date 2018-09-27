#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbre-binari/red-black-tree.h"

static int const MAX_LINE_SIZE = 100;
static int const IATA_CODE = 3;

int main(void)
{
    FILE *fp;
    char str[MAX_LINE_SIZE];
    int line_num = 0;
    int i = 0;
    rb_tree *tree;
    node_data *n_data;
    
    /* Opening file for reading */
    fp = fopen("./aeroports/aeroports.csv", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return (-1);
    }
    
    /* line_num = 1st line of file */
    if (fgets(str, MAX_LINE_SIZE, fp) != NULL)
    {
        line_num = atoi(str);
    }
    
    /* Allocate memory for tree */
    tree = (rb_tree *)malloc(sizeof(rb_tree));
    
    /* Initialize the tree */
    init_tree(tree);

    /* We fill the matrix with the lines from the file */
    while (fgets(str, MAX_LINE_SIZE, fp) != NULL && i < line_num) {
        
        /* Overwrite '\n' with '\0'*/
        str[IATA_CODE] = '\0';
        
        /* Search if the key is in the tree */
        n_data = find_node(tree, str);
        
        if (n_data != NULL) {

            /* If the key is in the tree increment 'num' */
            n_data->num_vegades++;
        }
        else {

            /* If the key is not in the tree, allocate memory for the data
            * and insert in the tree */
            n_data = (node_data *)malloc(sizeof(node_data));
            
            /* This is the key by which the node is indexed in the tree */
            n_data->key = (char *)malloc(IATA_CODE*sizeof(char));
            strcpy(n_data->key, str);
            
            /* This is additional information that is stored in the tree */
            n_data->num_vegades = 1;

            /* We insert the node in the tree */
            insert_node(tree, n_data);
        }
        i++;
    }
    
    fclose(fp);

    delete_tree(tree);
    free(tree);
    
    return 0;
}
