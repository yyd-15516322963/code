#include <stdio.h>

int Swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int main()
{
    int x = 10, y = 20;
    Swap(&x, &y);
    printf("%d %d \n", x, y);
}
