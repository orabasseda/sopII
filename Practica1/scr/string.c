#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int const line_size = 100;

int compara(const void *p1, const void *p2)
{
    char **str1, **str2;
    int size1, size2;
    
    str1 = (char **)p1;
    str2 = (char **)p2;

    size1 = strlen(*str1);
    size2 = strlen(*str2);
    
    if (size1 < size2)
        return -1;
    if (size1 > size2)
        return 1;
    return 0;
}

int main(void)
{
    FILE *fp;
    char element[line_size];
    int line_num = 0;
    int i = 0;
    int len;
    
    /* opening file for reading*/
    fp = fopen("./dades/strings.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return (-1);
    }
    
    if (fgets(element, line_size, fp) != NULL)
    {
        line_num = atoi(element);
    }
    
    char **vector = (char **)malloc(line_num*sizeof(char *));

    while (fgets(element, line_size, fp) != NULL && i < line_num)
    {
        len = strlen(element)-1;
        vector[i] = (char *)malloc(len*sizeof(char));
        memcpy(vector[i], element, len*sizeof(char));
        i++;
    }
    
    fclose(fp);

    qsort(vector, line_num, sizeof(char *), compara);
    
    printf("El vector ordenat és\n");
    
    for(i = 0; i < line_num; i++)
        printf("%s\n", vector[i]);
    
    printf("\n");

    for(i = 0; i < line_num; i++)
        free(vector[i]);
    free(vector);
    
    return 0;
}
