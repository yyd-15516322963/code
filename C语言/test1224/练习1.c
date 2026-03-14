#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_memory(char **p,int num)
{
    *p = (char*)malloc(num);
}
int main(int argc, char const *argv[])
{
    char *str = NULL;
    get_memory(&str,100);
    printf("%d\n",str);

    strcpy(str,"hello");

    printf("%s\n",str);

    return 0;
}
