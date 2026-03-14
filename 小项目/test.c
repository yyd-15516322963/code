#include <stdio.h>
int main(int argc, char const *argv[])
{

    int a = 10;
    int *p = &a;
    int *q = p;
    printf("%d\n",*q);
    printf("%d\n",*p);

    return 0;
}

