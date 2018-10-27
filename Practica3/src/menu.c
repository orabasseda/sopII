/**
 *
 * Practica 3 
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-base/database.h" /* Import functions from Practica2 */

#define MAXLINE      200
#define MAGIC_NUMBER 0x0133C8F9

static int const IATA_CODE = 3;

/* Recursive method that explores all tree nodes and finds the node with more destinations
 * @param *x node
 * @param *fp file
 */
void postorder_store(node *x, FILE *fp) {    
    if (x->right != NIL)
        postorder_store(x->right, fp);

    if (x->left != NIL)
        postorder_store(x->left, fp);

    node_data *n_data = x->data;
    list_data *l_data;
    list_item *l_item = n_data->flights->first;
    
    /* We write the key and the number of destinations in file */
    fwrite(n_data->key, sizeof(char), strlen(n_data->key), fp);
    fwrite(&n_data->flights->num_items, sizeof(int), 1, fp);
    
    /* We explore the linked list of each node and write the destination, number of flights and delay of each flight */
    while (l_item != NULL) {
        l_data = l_item->data;
        fwrite(l_data->key, sizeof(char), strlen(l_data->key), fp);
        fwrite(&l_data->n_flights, sizeof(int), 1, fp);
        fwrite(&l_data->delay, sizeof(int), 1, fp);
        l_item = l_item->next;
    }
}

/**
 * 
 *  Menu
 * 
 */

int menu() 
{
    char str[5];
    int opcio;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre\n");
    printf(" 2 - Emmagatzemar arbre a disc\n");
    printf(" 3 - Llegir arbre de disc\n");
    printf(" 4 - Consultar informacio de l'arbre\n");
    printf(" 5 - Sortir\n\n");
    printf("   Escull opcio: ");

    fgets(str, 5, stdin);
    opcio = atoi(str); 

    return opcio;
}

/**
 * 
 *  Main procedure
 *
 */

int main(int argc, char **argv)
{
    char str1[MAXLINE], str2[MAXLINE];
    int opcio;
    rb_tree *tree = NULL;

    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    do {
        opcio = menu();
        printf("\n\n");

       /* Feu servir aquest codi com a pantilla */

        switch (opcio) {
            case 1: /* Tree creation */
                printf("Introdueix fitxer que conte llistat d'aeroports: ");
                fgets(str1, MAXLINE, stdin);
                str1[strlen(str1)-1]=0;

                printf("Introdueix fitxer de dades: ");
                fgets(str2, MAXLINE, stdin);
                str2[strlen(str2)-1]=0;
                
                /* If we already have a tree, delete it */
                if (tree != NULL) {
                    delete_database(tree);
                }
                /* Create tree given both file locations */
                tree = build_database(str1, str2);

                break;

            case 2: /* Tree storing */
               if (tree != NULL) {
		    printf("Introdueix el nom de fitxer en el qual es desara l'arbre: ");
		    fgets(str1, MAXLINE, stdin);
		    str1[strlen(str1)-1]=0;
                    
                    FILE *fp = fopen(str1, "w"); /* We open empty file for writing */
                    
                    if (fp == NULL) /* If there's no file, print an error */
                    {
                        perror("Error opening file");
                        break;
                    }

                    int magic = MAGIC_NUMBER;
                    fwrite(&magic, sizeof(int), 1, fp); /* First of all, write magic number */
                    fwrite(&tree->num_nodes, sizeof(int), 1, fp); /* Write number of nodes in tree */
                    postorder_store(tree->root, fp); /* Explore tree in postorder and store each node */
		    fclose(fp); /* Close file */
                }
                else { /* If there's no tree, print an error */
                    perror("L'arbre no està inicialitzat");
                }
                break;

            case 3: /* Tree loading */
                printf("Introdueix nom del fitxer que conte l'arbre: ");
                fgets(str1, MAXLINE, stdin);
                str1[strlen(str1)-1]=0;

                if (tree != NULL) { /* If we already have a tree, delete it */
                    delete_database(tree);
                }
                
                FILE *fp = fopen(str1, "r"); /* Open a file for reading */
                if (fp == NULL) /* If there's no file, print an error */
		{
		    perror("Error opening file");
		    break;
		}
		
		int magic_aux;
                fread(&magic_aux, sizeof(int), 1, fp); /* Read first number in file */
                int magic = MAGIC_NUMBER;
                
                if (magic_aux == magic) { /* If the number matches MAGIC_NUMBER, continue */
                    int num_nodes;
                    int i;
                    char *origin;
                    int num_dest;
                    int j;
                    char *dest;
                    int num_vols;
                    int delay;
                    
                    fread(&num_nodes, sizeof(int), 1, fp); /* Read number of nodes */
                    tree = create_database(); /* Initialize empty tree */

                    /* We explore all nodes */
                    for (i = 0; i < num_nodes; i++) {
                        origin = (char *)malloc(sizeof(char)*(IATA_CODE+1));
                        fread(origin, sizeof(char), IATA_CODE, fp); /* Read flight origin */
                        origin[IATA_CODE] = '\0';
                        insert_node_tree(tree, origin); /* Insert airport in tree */
                        
                        fread(&num_dest, sizeof(int), 1, fp); /* Read number of destinations */
                        for (j = 0; j < num_dest; j++) {
                            dest = (char *)malloc(sizeof(char)*(IATA_CODE+1));
                            fread(dest, sizeof(char), IATA_CODE, fp); /* Read flight destination */
                            dest[IATA_CODE] = '\0';
                            fread(&num_vols, sizeof(int), 1, fp); /* Read number of flights */
                            fread(&delay, sizeof(int), 1, fp); /* Read delay */
                            insert_destination(tree, origin, dest, num_vols, delay); /* Insert flight in tree */
                        }
                    }
                }
                else { /* If there's no file, print an error */
                    perror("El fitxer donat no és vàlid");
                }
                fclose(fp); /* Close file */

                break;

            case 4: /* Tree search */
                if (tree != NULL) {
                    printf("Introdueix aeroport per cercar retard o polsa enter per saber l'aeroport amb mes destins: ");
                    fgets(str1, MAXLINE, stdin);
                    str1[strlen(str1)-1]=0;
                    
                    if (strlen(str1) == 0) {
                        max_destinations(tree); /* Find airport with more destinations */
                    }
                    else {
                        delay(tree, str1); /* Search delay of given airport */
                    }
                }
                else { /* If there's no tree, print an error */
                    perror("L'arbre no està inicialitzat");
                }
                break;

            case 5: /* Delete tree */

                if (tree != NULL) { /* If we have a tree, delete it */
                    delete_database(tree);
                }
                break;

            default:
                printf("Opcio no valida\n");

        } /* switch */
    }
    while (opcio != 5);

    return 0;
}

