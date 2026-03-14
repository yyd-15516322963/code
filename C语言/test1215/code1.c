#include <stdio.h>

int main()
{
    // int a;
    // printf("a = %d\n", a);
    int b = 0, x = 1;
    if (x++)
        b = x + 1;
    printf("%d\n", b);
    printf("%d\n", x);
}
