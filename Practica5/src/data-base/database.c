#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "linked-list/linked-list.h"
#include "database.h"

#define BLOCK_SIZE 10
#define NUM_CONSUMERS 8 /* <- F */
#define MAX_LINE_SIZE 100
#define FLIGHT_LINE_SIZE 500
#define IATA_CODE 3
#define NUM_CELLS 100

int max_dest = 0;
char *airport;
pthread_mutex_t f_mutex = PTHREAD_MUTEX_INITIALIZER; /* Static inizialization of mutex for airport file */
pthread_mutex_t b_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_cond = PTHREAD_COND_INITIALIZER;

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
    int start = 0;
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

            pthread_mutex_init(&(n_data->mutex), NULL); /* Initialize mutex for each node to NULL */

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
    
    /* If node exists, continue */
    if (n_data != NULL) {
        pthread_mutex_lock(&(n_data->mutex)); /* We lock the mutex of the current node for writing */
        l = n_data->flights;
        
        /* Search if the key is in the tree */
        l_data = find_list(l, destination);
        
        if (l_data != NULL) {

            /* We increment the number of times current item has appeared */
            l_data->n_flights++;
            /* We increase the delay for the node accordingly */
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
        pthread_mutex_unlock(&(n_data->mutex)); /* Unlock mutex of current node */
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

/* Method that gets called when initializing a new thread, uses blocks of given size to read a filename1
 * @param *arg: struct thread_data
 */
void *producer_ini(void *arg) {
    producer_data *p_data = (producer_data *)arg; /* Struct with the information needed for the thread to read the file */
    char str[FLIGHT_LINE_SIZE];
    int err;
    cell *cell_aux;
    buffer *buffer_aux = p_data->buffer;
    
    
    /* If file2 is empty, exit */
    if (fgets(str, FLIGHT_LINE_SIZE, p_data->fp) == NULL) {
        perror("Flights file is empty");
        return 0;
    }
    
    /* While there's still lines to read, continue */
    while(!*(buffer_aux->eof)) {
        int i = 0;
        cell_aux = malloc(sizeof(cell));
        cell_aux->block = (char **)malloc(sizeof(char *)*BLOCK_SIZE); /* We allocate space for a block of a given size */
        
        /* We lock the section for reading lines from the main file */
        pthread_mutex_lock(&f_mutex);
        /* We keep reading lines from the given file until reaching the block size limit or the end of file */
        while (fgets(str, FLIGHT_LINE_SIZE, p_data->fp) != NULL && i < BLOCK_SIZE) { 
            (cell_aux->block)[i] = (char *)malloc(sizeof(char)*strlen(str)); /* For each block line we allocate memory for the current line */
            memcpy((cell_aux->block)[i], str, strlen(str)-1); /* We copy the current line to the block */
	        (cell_aux->block)[i][strlen(str)-1] = '\0';
            i++;
        }
        pthread_mutex_unlock(&f_mutex); /* Unlock main file for reading */
        
        cell_aux->size = &i;
        
        pthread_mutex_lock(&b_mutex);
        if (i < BLOCK_SIZE) { /* If we run out of lines to read, we've reached the end of file */
            *(buffer_aux->eof) = 1;
        }
        
        while (*(buffer_aux->num_cells) == NUM_CELLS) {
            err = pthread_cond_wait(&p_cond, &b_mutex);
            if (err != 0) {
                perror("Impossible to create thread"); /* If can't create a thread, print an error */
                exit(0);
            }
        }

        buffer_aux->buffer[*(buffer_aux->w)] = cell_aux;
        //*(buffer_aux->w) = (*(buffer_aux->w)+1)%NUM_CELLS;
        *(buffer_aux->num_cells) = *(buffer_aux->num_cells) + 1;
        
        err = pthread_cond_signal(&c_cond);
        if (err != 0) {
            perror("Impossible to create thread"); /* If can't create a thread, print an error */
            exit(0);
        }
        pthread_mutex_unlock(&b_mutex);
    }
    return NULL;
}

/* Method that gets called when initializing a new thread, uses blocks of given size to read a filename1
 * @param *arg: struct thread_data
 */
void *consumer_ini(void *arg) {
    consumer_data *c_data = (consumer_data *)arg; /* Struct with the information needed for the thread to read the file */
    int i, err;
    
    cell *cell_aux;
    buffer *buffer_aux = c_data->buffer;
    
    /* While there's still lines to read, continue */
    while(!*(buffer_aux->eof) || *(buffer_aux->num_cells) != 0) {
        
        pthread_mutex_lock(&b_mutex);
        while (*(buffer_aux->num_cells) == 0) {
            err = pthread_cond_wait(&c_cond, &b_mutex);
            if (err != 0) {
                perror("Impossible to create thread"); /* If can't create a thread, print an error */
                exit(0);
            }
        }
        
        cell_aux = buffer_aux->buffer[*(buffer_aux->r)];

        //*(buffer_aux->r) = (*(buffer_aux->r)+1)%NUM_CELLS;
        //*(buffer_aux->num_cells) = *(buffer_aux->num_cells) - 1;
        
        err = pthread_cond_signal(&p_cond);
        if (err != 0) {
            perror("Impossible to create thread"); /* If can't create a thread, print an error */
            exit(0);
        }
        pthread_mutex_unlock(&b_mutex);
        
        for (i = 0; i < *(cell_aux->size); i++) { /* For each block, add all read lines to the tree */
            flight_list(c_data->tree, cell_aux->block[i]);
        }
        
        /* Free block */
        for (i = 0; i < *(cell_aux->size); i++) {
            //free(cell_aux->block[i]);
        }
        //free(cell_aux->block);
        //free(cell_aux->size);
        //free(cell_aux);
    }
    return NULL;
}

/* Method that creates a new rb_tree given 2 file locations
 * This method build a tree, even though its called 'database' for some reason...
 * @param *filename1 file of airports
 * @param *filename2 file of flights
 * @param *tree pointer to current tree
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
    fclose(fp1); /* With the rb-tree initialized, we don't need this file anymore */
    pthread_t producer;
    pthread_t consumers[NUM_CONSUMERS]; /* We create a list of threads to use */
    
    int i, err;
    /* We allocate space for a thread_data struct */
    producer_data *p_data = (producer_data *)malloc(sizeof(producer_data)); 
    p_data->fp = fp2;
    p_data->buffer = (buffer *)malloc(sizeof(buffer));
    
    int num_cells = 0, r = 0, w = 0, eof = 0;
    
    p_data->buffer->num_cells = &num_cells;
    p_data->buffer->r = &r;
    p_data->buffer->w = &w;
    p_data->buffer->eof = &eof;
    
    err = pthread_create(&producer, NULL, producer_ini, (void *)p_data);
    if (err != 0) {
        perror("Impossible to create thread"); /* If can't create a thread, print an error */
        exit(0);
    }
    
    consumer_data *c_data = (consumer_data *)malloc(sizeof(consumer_data)); 
    c_data->tree = tree;
    c_data->buffer = p_data->buffer;
    
    /* We create the decided number of threads to use, with the start routine 'thread_ini' and the thread_data struct */
    for (i = 0; i < NUM_CONSUMERS; i++) {
        err = pthread_create(&consumers[i], NULL, consumer_ini, (void *)c_data);
        if (err != 0) {
            perror("Impossible to create thread"); /* If can't create a thread, print an error */
            exit(0);
        }
    }
    
    err = pthread_join(producer, NULL);
    if (err != 0) {
        perror("Error in thread");
        exit(0);
    }
    /* We wait for each thread to finish */
    for (i = 0; i < NUM_CONSUMERS; i++) {
        err = pthread_join(consumers[i], NULL);
        if (err != 0) {
            perror("Error in thread");
            exit(0);
        }
    }

    /* Close file 2, free the thread_data struct and return the tree */
    fclose(fp2);
    
    free(p_data->buffer->buffer);
    free(p_data->buffer->num_cells);
    free(p_data->buffer->r);
    free(p_data->buffer->w);
    free(p_data->buffer->eof);
    free(p_data->buffer);
    
    free(p_data);
    free(c_data);
    return tree;
}

/* Method that initializes a new rb_tree
 * @return rb_tree *tree
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
