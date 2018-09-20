#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int const line_size = 100;

int compara(const void *p1, const void *p2)
{
    char *str1, *str2;
    
    str1 = *((char **)p1);
    str2 = *((char **)p2);
    
    return strcmp(str1, str2);
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
        len = strlen(element);
        element[len-1] = '\0';
        vector[i] = (char *)malloc(len*sizeof(char));
        strcpy(vector[i], element);
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
