#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "linked-list/linked-list.h"
#include "database.h"

static int const MAX_LINE_SIZE = 100;
static int const FLIGHT_LINE_SIZE = 500;
static int const IATA_CODE = 3;
int max_dest = 0;
char *airport;

/* Method that deletes a given rb_tree
 * @param *tree
 */
void delete_database(rb_tree *tree) {
    /* Delete tree */
    delete_tree(tree);
    free(tree);
    tree = NULL;
}

/* Method that splits a line of the 'dades' table and returns the info in the specified column
 * @param *str char pointer to the designated line
 * @param pos number of the column we want to obtain
 * @param *size size of the info in the column
 * @return &str string with the selected column of the current line
 */
char *split (char *str, int pos, int *size) {
    int max = strlen(str); /* Max lenght of the line */
    int counter = 0; /* Counter to keep track of the number of periods */
    int start;
    int i;
    
    for (i = 0; i < max; i++) { /* We travel the line one char at a time */
        
        if (str[i] == ',' || str[i] == '\n') { /* If we find a period or reach the end of the line: increase counter */
            counter++;
            if (counter == pos) { /* If we arrive to the desired column, save the starting position */
                start = i;
            }
            else if (counter == pos+1) { /* If we arrive to the next period after 'start': */
                *size = i-start; /* Calculate size of string */
                return &str[start+1]; /* Return the string by reference */
            }
        }
    }
    return 0;
}

/* Method that fills the tree with nodes from a given file
 * @param *filename name of the file with the nodes
 * @return tree
 */
rb_tree *airport_tree(FILE *fp) {
    char str[MAX_LINE_SIZE];
    int line_num = 0;
    int i = 0;
    rb_tree *tree;
    node_data *n_data;
    
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
            n_data->key = (char *)malloc((IATA_CODE+1)*sizeof(char));
            strcpy(n_data->key, str);
            
            /* This is additional information that is stored in the tree */
            /* Initialize the list */
            n_data->flights = (list *)malloc(sizeof(list));
            init_list(n_data->flights);

            /* We insert the node in the tree */
            insert_node(tree, n_data);
        }
        i++;
    }
    return tree;
}

/* Method that fills the linked list of the tree nodes with the flights from a file
 * @param *tree pointer to the tree
 * @param *filename name of the file with the information about flights
 */
void flight_list(rb_tree *tree, FILE *fp) {
    char str[FLIGHT_LINE_SIZE];
    char *delay;
    char *origin;
    char *destination;
    char *aux;
    node_data *n_data;
    list *l;
    int size;
    list_data *l_data;
    
    
      /* We skip the 1st line of the file */
    if (fgets(str, FLIGHT_LINE_SIZE, fp) != NULL){
    
        /* We fill the matrix with the lines from the file */
        while (fgets(str, FLIGHT_LINE_SIZE, fp) != NULL) {
            
            aux = split(&str[0], 14, &size);
            delay = (char *)malloc(size*sizeof(char));
            memcpy(delay, aux, size);
            delay[size-1] = '\0';
            
            aux = split(&str[0], 16, &size);
            origin = (char *)malloc(size*sizeof(char));
            memcpy(origin, aux, size);
            origin[size-1] = '\0';
            
            aux = split(&str[0], 17, &size);
            destination = (char *)malloc(size*sizeof(char));
            memcpy(destination, aux, size);
            destination[size-1] = '\0';
            
            /* Search if the key is in the tree */
            n_data = find_node(tree, origin);
            
            if (n_data != NULL) {
                l = n_data->flights;
                
                /* Search if the key is in the tree */
                l_data = find_list(l, destination);
                
                if (l_data != NULL) {

                    /* We increment the number of times current item has appeared */
                    l_data->n_flights++;
                    if (strcmp(delay, "NA")) {
                    l_data->delay += 0;
                    }
                    else {
                    l_data->delay += atoi(delay);
                    }
                        free(destination);
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
            free(origin);
            free(delay);
        }
    }
}

/* Recursive method that explores all tree nodes and finds the node with more destinations
 * @param *x node
 */
void postorder(node *x) {
    int center = x->data->flights->num_items;
    
    if (x->right != NIL)
        postorder(x->right);

    if (x->left != NIL)
        postorder(x->left);

    /* If our current node has more destinations than max_dest, save current node */
    if (center > max_dest) {
        max_dest = center;
        airport = x->data->key;
    }
}

void *thread_ini(void *arg) {
    thread_data *t_data = (thread_data *)arg;
    flight_list(t_data->tree, t_data->fp); /* List with the info about different flights */
    
    return NULL;
}

/* Method that creates a new rb_tree given 2 file locations
 * This method build a tree, even though its called 'database' for some reason...
 * @param *filename1 file of airports
 * @aram *filename2 file of flights
 */
rb_tree *build_database(char *filename1, char *filename2, rb_tree *tree){
    /* Opening file for reading */
    FILE *fp1 = fopen(filename1, "r");
    FILE *fp2 = fopen(filename2, "r");
    if (fp1 == NULL || fp2 == NULL) /* If there's no file, print an error */
    {
        perror("Error opening file");
        return 0;
    }   
    
    /* If we already have a tree, delete it */
    if (tree != NULL) {
        delete_database(tree);
    }

    tree = airport_tree(fp1); /* Tree of the different airports */
    fclose(fp1);
    
    thread_data *t_data = (thread_data *)malloc(sizeof(thread_data));
    t_data->fp = fp2;
    t_data->tree = tree;
    
    pthread_t secondary;
    pthread_create(&secondary, NULL, thread_ini, (void *)t_data);
    
    void *tret;
    pthread_join(secondary, &tret);

    fclose(fp2);
    return tree;
}

/* Method that initializes a new rb_tree
 */
rb_tree *create_database() {
    /* Allocate memory for tree */
    rb_tree *tree = (rb_tree *)malloc(sizeof(rb_tree));
    
    /* Initialize the tree */
    init_tree(tree);
    return tree;
}

/* Method that inserts a new node in a tree
 * @param *tree
 * @param *origin node to insert
 */
void insert_node_tree(rb_tree *tree, char *origin) {
    node_data *n_data = find_node(tree, origin);
    if (n_data == NULL) {
	/* If the key is not in the tree, allocate memory for the data
	* and insert in the tree */
	n_data = (node_data *)malloc(sizeof(node_data));
	
	/* This is the key by which the node is indexed in the tree */
	n_data->key = origin;
	
	/* This is additional information that is stored in the tree */
	/* Initialize the list */
	n_data->flights = (list *)malloc(sizeof(list));
	init_list(n_data->flights);

	/* We insert the node in the tree */
	insert_node(tree, n_data);
    }
    else {
	perror("Origin already in tree");
    }
    
}

/* Method that inserts a flight into the tree
 * @param *tree
 * @param *origin 
 * @param *destination
 * @param num_vols number of flights
 * @param delay
 */
void insert_destination(rb_tree *tree, char *origin, char *destination, int num_vols, int delay) {
    node_data *n_data = find_node(tree, origin); /* Find origin node in tree */
    list_data *l_data;
        
    if (n_data == NULL) { /* If origin not in tree, print an error */
        perror("Origin not in tree");
    }
    else {
        /* Create element to add to linked list */
        l_data = malloc(sizeof(list_data));
        l_data->key = destination;
        l_data->n_flights = num_vols;
        l_data->delay = delay;

        insert_list(n_data->flights, l_data); /* Insert flight in list */
    }
}

/* Method that returns the airport with more destinations in a tree 
 */
void max_destinations(rb_tree *tree) {
    /* Find and print the airport with more destinations */
    postorder(tree->root);
    printf("Airport: %s\tDestinations: %d\n", airport, max_dest);
    max_dest = 0;
}

/* Method that calculates the average delay for a given airport
 */
void delay(rb_tree *tree, char *origin) {
    node_data *n_data = find_node(tree, origin); /* Find given node in tree */
    
    if (n_data != NULL) { /* If the node exists, get the node's flight list */
        list_data *l_data;
        float avg_delay;
        list_item *l_item = n_data->flights->first;
        
        if (l_item == NULL) { /* If there's no list, print message */
            printf("There are no flights from this airport\n");
        }
        else {
            while (l_item != NULL) { /* Print list of the current node */
                l_data = l_item->data;
                avg_delay = (float)l_data->delay/l_data->n_flights;
                printf("Destination: %s\tAverage delay: %f\n", l_data->key, avg_delay);
                l_item = l_item->next;
            }
        }
    }
    else { /* If the node doesn't exist, print an error */
        perror("Origin not valid\n");
    }
}
