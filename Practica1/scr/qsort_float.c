#include <stdio.h>
#include <stdlib.h>

static int const line_size = 100;

/* Method that compares 2 pointers and returns and integer
 * @param *p1 void pointer
 * @param *p2 void pointer
 * @return -1 if p1 < p2; 1 if p1 > p2; 0 if p1 == p2
 */
int compara(const void *p1, const void *p2)
{
    float *num1, *num2;
    
    /* cast from void to float pointer */
    num1 = (float *)p1;
    num2 = (float *)p2;
    
    if (*num1 < *num2)
        return -1;
    if (*num1 > *num2)
        return 1;
    return 0;
}

int main(void)
{
    FILE *fp;
    char element[line_size];
    int line_num = 0;
    int i = 0;
    
    /* Opening file for reading */
    fp = fopen("./dades/float.txt", "r");
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
    
    /* We reserve memory for a float vector to sort */
    float *vector = (float *)malloc(line_num*sizeof(float));

    /* We fill the vector with the lines from the file */
    while (fgets(element, line_size, fp) != NULL && i < line_num)
    {
        vector[i] = atof(element);
        i++;
    }
    
    fclose(fp);
    
    /* quicksort of vector using function 'compara' */
    qsort(vector, line_num, sizeof(float), compara);
    
    /* print sorted vector */
    printf("El vector ordenat és\n");
    for(i = 0; i < line_num; i++)
        printf("%f\n", vector[i]);
    
    printf("\n");

    free(vector);
    
    return 0;
}
