#include <stdio.h>

int max(int a, int b)
{
    return a > b ? a : b;
}

int main(int argc, char const *argv[])
{
    int a = 3, b = 5, c = 8, d = 9;

    int max1 = max(a, b);


    printf("%d\n",max1);
    return 0;
}
