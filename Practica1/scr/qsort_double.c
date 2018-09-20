#include <stdio.h>
#include <stdlib.h>

static int const line_size = 100;

int compara(const void *p1, const void *p2)
{
    double *num1, *num2;
    
    num1 = (double *)p1;
    num2 = (double *)p2;
    
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
    int line_num;
    int i = 0;
    
    /* opening file for reading*/
    fp = fopen("./dades/float.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return (-1);
    }
    
    if (fgets(element, line_size, fp) != NULL)
    {
        line_num = atoi(element);
    }
    
    double *vector = (double *)malloc(line_num*sizeof(double));

    while (fgets(element, line_size, fp) != NULL && i < line_num)
    {
        vector[i] = atof(element);
        i++;
    }
    
    fclose(fp);
    
    qsort(vector, line_num, sizeof(double), compara);
    
    printf("El vector ordenat és\n");
    
    for(i = 0; i < line_num; i++)
        printf("%f\n", vector[i]);
    
    printf("\n");

    free(vector);
    
    return 0;
}
