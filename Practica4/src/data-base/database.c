#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "linked-list/linked-list.h"
#include "database.h"

#define BLOCK_SIZE 10
#define NUM_THREADS 8
#define MAX_LINE_SIZE 100
#define FLIGHT_LINE_SIZE 500
#define IATA_CODE 3

int max_dest = 0;
char *airport;
int eof = 0;
pthread_mutex_t f_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Method that deletes a given rb_tree
 * @param *tree
 */
void delete_database(rb_tree *tree) {
    /* Delete tree */
    delete_tree(tree);
    free(tree);
    tree = NULL;
    airport = NULL;
}

/* Method that splits a line of the 'dades' table and returns the info in the specified column
 * @param *str char pointer to the designated line
 * @param pos number of the column we want to obtain
 * @param *size size of the info in the column
 * @return &str string with the selected column of the current line
 */
char *split (char *str, int pos, int *size) {
    int max_size = strlen(str);
    int counter = 0; /* Counter to keep track of the number of periods */
    int start;
    int i = 0;
    
    for(i = 0; i < max_size; i++) { /* We travel the line one char at a time */
        
        if (str[i] == ',' || str[i] == '\0') { /* If we find a period or reach the end of the line: increase counter */
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

            pthread_mutex_init(&(n_data->mutex), NULL);

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
void flight_list(rb_tree *tree, char *str) {
    char *delay;
    char *origin;
    char *destination;
    char *aux;
    node_data *n_data;
    list *l;
    int size;
    list_data *l_data;
        
    aux = split(str, 14, &size);
    delay = (char *)malloc(size*sizeof(char));
    memcpy(delay, aux, size);
    delay[size-1] = '\0';
    
    aux = split(str, 16, &size);
    origin = (char *)malloc(size*sizeof(char));
    memcpy(origin, aux, size);
    origin[size-1] = '\0';
    
    aux = split(str, 17, &size);
    destination = (char *)malloc(size*sizeof(char));
    memcpy(destination, aux, size);
    destination[size-1] = '\0';
    
    /* Search if the key is in the tree */
    n_data = find_node(tree, origin);
    
    if (n_data != NULL) {
        pthread_mutex_lock(&(n_data->mutex));
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
        pthread_mutex_unlock(&(n_data->mutex));
    }
    free(origin);
    free(delay);
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
    char str[FLIGHT_LINE_SIZE];
    int i;
    
    while(!eof) {
        i = 0;
        char **block = (char **)malloc(sizeof(char *)*BLOCK_SIZE);
        
        pthread_mutex_lock(&f_mutex);
        while (fgets(str, FLIGHT_LINE_SIZE, t_data->fp) != NULL && i < BLOCK_SIZE) {
            block[i] = (char *)malloc(sizeof(char)*strlen(str));
            memcpy(block[i], str, strlen(str)-1);
	    block[i][strlen(str)-1] = '\0';
            i++;
        }
        if (i < BLOCK_SIZE) {
            eof = 1;
        }
        pthread_mutex_unlock(&f_mutex);

        int j;
        for (j = 0; j < i; j++) {
            flight_list(t_data->tree, block[j]);
        }

        for (j = 0; j < i; j++) {
            free(block[j]);
        }
        free(block);
    }
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
    pthread_t ntid[NUM_THREADS];
    char str[FLIGHT_LINE_SIZE];

    if (fgets(str, FLIGHT_LINE_SIZE, fp2) == NULL) {
        perror("This error I don't understand");
        return 0;
    }
    
    thread_data *t_data = (thread_data *)malloc(sizeof(thread_data));
    t_data->fp = fp2;
    t_data->tree = tree;
    int i, err;

    for (i = 0; i < NUM_THREADS; i++) {
        err = pthread_create(&ntid[i], NULL, thread_ini, (void *)t_data);
        if (err != 0) {
            perror("Impossible to create thread");
            exit(1);
        }   
    }
    
    for (i = 0; i < NUM_THREADS; i++) {
        err = pthread_join(ntid[i], NULL);
        if (err != 0) {
            perror("Error in thread");
            exit(1);
        }
    }
    eof = 0;

    fclose(fp2);
    free(t_data);
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
