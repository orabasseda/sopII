#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbre-binari/red-black-tree.h"
#include "linked-list/linked-list.h"

static int const MAX_LINE_SIZE = 100;
static int const FLIGHT_LINE_SIZE = 500;
static int const IATA_CODE = 3;

rb_tree *airport_tree(void) {
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
        return 0;
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
        
        if (n_data == NULL) {
            /* If the key is not in the tree, allocate memory for the data
            * and insert in the tree */
            n_data = (node_data *)malloc(sizeof(node_data));
            
            /* This is the key by which the node is indexed in the tree */
            n_data->key = (char *)malloc(IATA_CODE*sizeof(char));
            strcpy(n_data->key, str);
            
            /* This is additional information that is stored in the tree */
            /* Initialize the list */
            n_data->flights = (list *) malloc(sizeof(list));
            init_list(n_data->flights);

            /* We insert the node in the tree */
            insert_node(tree, n_data);
        }
        i++;
    }
    
    fclose(fp);
    return tree;
}

char *split (char *str, int pos, int *size) {
    int max = len(str);
    int counter = 0;
    int start;
    
    for (int i = 0; i < max; i++) {
        
        if (str[i] == ',' || str[i] == '\n') {
            counter++;
            if (counter == pos) {
                start = i;
            }
            else if (counter == pos+1) {
                *size = i-start;
                return &str[start+1];
            }
        }
    }    
}

void flight_list(rb_tree *tree) {
    FILE *fp;
    char str[FLIGHT_LINE_SIZE];
    char *delay;
    char *origin;
    char *destination;
    char *aux;
    node_data *n_data;
    list *l;
    int size;
    list_data *l_data;
    
    
    /* Opening file for reading */
    fp = fopen("./data/data.csv", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
    }
    
    /* We fill the matrix with the lines from the file */
    while (fgets(str, FLIGHT_LINE_SIZE, fp) != NULL) {
        
        aux = split(&str, 14, &size);
        delay = (char *)malloc(sizeof(char)*(*size));
        strcpy(delay, aux);
        delay[*size-1] = '\0';
        
        aux = split(&str, 16, &size);
        origin = (char *)malloc(sizeof(char)*(*size));
        strcpy(origin, aux);
        origin[*size-1] = '\0';
        
        aux = split(&str, 17, &size);
        destination = (char *)malloc(sizeof(char)*(*size));
        strcpy(destination, aux);
        destination[*size-1] = '\0';
        
        /* Search if the key is in the tree */
        n_data = find_node(tree, origin);
        
        if (n_data != NULL) {
            l = n_data->flights;
            
            /* Search if the key is in the tree */
            l_data = find_list(l, destination);
            
            if (l_data != NULL) {

                /* We increment the number of times current item has appeared */
                l_data->n_flights++;
                l_data->delay += atoi(delay);
            }
            else {
                /* If the key is not in the list, allocate memory for the data and
                * insert it in the list */

                l_data = malloc(sizeof(list_data));
                l_data->key = destination;
                l_data->n_flights = 1;
                l_data->delay = atoi(delay);

                insert_list(l, l_data);
            }
                
        }
    }
}

int main(void)
{
    rb_tree *tree = airport_tree();
    
    delete_tree(tree);
    free(tree);
    
    return 0;
}