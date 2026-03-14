#include <stdio.h>

void demo(int n)
{
    if (n < 0)
    {
        return;
    }
    printf("%d 递\n", n);
    demo(n - 1);
    printf("%d 归\n", n);
}
int main(int argc, char const *argv[])
{
    demo(5);
    return 0;
}
