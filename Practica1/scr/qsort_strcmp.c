#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int const line_size = 100;

/* Method that compares 2 pointers and returns and integer
 * @param *p1 void pointer
 * @param *p2 void pointer
 * @return strcmp(p1, p2)
 */
int compara(const void *p1, const void *p2)
{
    char *str1, *str2;
    
    /* Cast from void to string pointer */
    str1 = *((char **)p1);
    str2 = *((char **)p2);
    
    /* Compare value of both strings */
    return strcmp(str1, str2);
}

int main(void)
{
    FILE *fp;
    char element[line_size];
    int line_num = 0;
    int i = 0;
    int len;
    
    /* Opening file for reading */
    fp = fopen("./dades/strings.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return (-1);
    }
    
    /* line_num = 1st line of file */
    if (fgets(element, line_size, fp) != NULL)
    {
        line_num = atoi(element);
    }
    
    /* We reserve memory for a char matrix to sort */
    char **vector = (char **)malloc(line_num*sizeof(char *));

    /* We fill the matrix with the lines from the file */
    while (fgets(element, line_size, fp) != NULL && i < line_num)
    {
        len = strlen(element);
        /* Overwrite '\n' with '\0'*/
        element[len-1] = '\0';
        /* For each row we reserve memory equal to the size of the string */
        vector[i] = (char *)malloc(len*sizeof(char));
        /* We copy each string to its position in the matrix */
        strcpy(vector[i], element);
        i++;
    }
    
    fclose(fp);

    /* quicksort of vector using function 'compara' */
    qsort(vector, line_num, sizeof(char *), compara);
    
    /* print sorted vector */
    printf("El vector ordenat és\n");
    for(i = 0; i < line_num; i++)
        printf("%s\n", vector[i]);
    
    printf("\n");

    for(i = 0; i < line_num; i++)
        free(vector[i]);
    free(vector);
    
    return 0;
}
