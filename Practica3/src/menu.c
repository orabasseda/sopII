/**
 *
 * Practica 3 
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-base/database.h"

#define MAXLINE      200
#define MAGIC_NUMBER 0x0133C8F9

static int const IATA_CODE = 3;

/* Recursive method that explores all tree nodes and finds the node with more destinations
 * @param *x node
 */
void postorder_store(node *x, FILE *fp) {    
    if (x->right != NIL)
        postorder_store(x->right, fp);

    if (x->left != NIL)
        postorder_store(x->left, fp);

    node_data *n_data = x->data;
    list_data *l_data;
    list_item *l_item = n_data->flights->first;
                
    fwrite(n_data->key, sizeof(char), strlen(n_data->key), fp);
    fwrite(&n_data->flights->num_items, sizeof(int), 1, fp);
    
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
            case 1:
                printf("Introdueix fitxer que conte llistat d'aeroports: ");
                fgets(str1, MAXLINE, stdin);
                str1[strlen(str1)-1]=0;

                printf("Introdueix fitxer de dades: ");
                fgets(str2, MAXLINE, stdin);
                str2[strlen(str2)-1]=0;
                
                if (tree != NULL) {
                    delete_database(tree);
                }
                tree = build_database(str1, str2);

                break;

            case 2:
               if (tree != NULL) {
		    printf("Introdueix el nom de fitxer en el qual es desara l'arbre: ");
		    fgets(str1, MAXLINE, stdin);
		    str1[strlen(str1)-1]=0;
                    /* Falta codi */
                    FILE *fp = fopen(str1, "w");
                    
                    if (fp == NULL) /* If there's no file, print an error */
                    {
                        perror("Error opening file");
                        break;
                    }

                    int magic = MAGIC_NUMBER;
                    fwrite(&magic, sizeof(int), 1, fp); 
                    fwrite(&tree->num_nodes, sizeof(int), 1, fp);
                    postorder_store(tree->root, fp);
		    fclose(fp);
                }
                else {
                    perror("L'arbre no està inicialitzat");
                }
                break;

            case 3:
                printf("Introdueix nom del fitxer que conte l'arbre: ");
                fgets(str1, MAXLINE, stdin);
                str1[strlen(str1)-1]=0;

                /* Falta codi */
                if (tree != NULL) {
                    delete_database(tree);
                }
                
                FILE *fp = fopen(str1, "r");
                if (fp == NULL) /* If there's no file, print an error */
		{
		    perror("Error opening file");
		    break;
		}
		
		int magic_aux;
                fread(&magic_aux, sizeof(int), 1, fp);
                int magic = MAGIC_NUMBER;
                
                if (magic_aux == magic) {
                    int num_nodes;
                    int i;
                    char *origin;
                    int num_dest;
                    int j;
                    char *dest;
                    int num_vols;
                    int delay;
                    
                    fread(&num_nodes, sizeof(int), 1, fp);
                    
                    tree = create_database();
                    for (i = 0; i < num_nodes; i++) {
                        origin = (char *)malloc(sizeof(char)*(IATA_CODE+1));
                        fread(origin, sizeof(char), IATA_CODE, fp);
                        origin[IATA_CODE] = '\0';
                        insert_node_tree(tree, origin);
                        
                        fread(&num_dest, sizeof(int), 1, fp);
                        for (j = 0; j < num_dest; j++) {
                            dest = (char *)malloc(sizeof(char)*(IATA_CODE+1));
                            fread(dest, sizeof(char), IATA_CODE, fp);
                            dest[IATA_CODE] = '\0';
                            fread(&num_vols, sizeof(int), 1, fp);
                            fread(&delay, sizeof(int), 1, fp);
                            insert_destination(tree, origin, dest, num_vols, delay);
                        }
                    }
                }
                else {
                    perror("El fitxer donat no és vàlid");
                }
                fclose(fp);

                break;

            case 4:
                if (tree != NULL) {
                    printf("Introdueix aeroport per cercar retard o polsa enter per saber l'aeroport amb mes destins: ");
                    fgets(str1, MAXLINE, stdin);
                    str1[strlen(str1)-1]=0;
                    
                    if (strlen(str1) == 0) {
                        max_destinations(tree);
                    }
                    else {
                        delay(tree, str1);
                    }
                }
                else {
                    perror("L'arbre no està inicialitzat");
                }
                break;

            case 5:

                /* Falta codi */
                if (tree != NULL) {
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

