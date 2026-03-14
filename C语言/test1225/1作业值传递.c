#include <stdio.h>

void f(int a, int b)
{
    int t;
    t = a;
    a = b;
    b = t;
}
int main()
{
    int x = 1, y = 3, z = 2;
    if (x > y)
        f(x, y);
    else if (y > z)
        f(x, z);
    else
        f(x, z);
    printf("%d,%d,%d\n", x, y, z);
}
