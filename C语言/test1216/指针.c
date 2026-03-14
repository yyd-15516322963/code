#include <stdio.h>

int main()
{
    int a = 100;

    int *p = &a;
    int *k = p + 2;
    int *q = (int)p + 2;
    printf("%d\n",p);
    printf("%d\n",k);
    printf("%d\n",q);

    printf("%d\n",a+2);
    printf("%d\n",(int *)a+2);
    return 0;
}