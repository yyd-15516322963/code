#include <stdio.h>
#include <stdlib.h>

char get_memory()
{
    char p[] = "hello"; 
    return p;
}
int main(int argc, char const *argv[])
{
    char *str = NULL;
    str = get_memory();

    printf("%s\n",str);

    return 0;
}
