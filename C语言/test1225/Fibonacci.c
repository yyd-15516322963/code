#include <stdio.h>

int fibonacci(int i)
{
    if (i > 0)
    {

        if (i == 1 || i == 2)
        {
            return 1;
        }
        else
        {
            return fibonacci(i - 1) + fibonacci(i - 2);
        }
    }
    else
        return -1;
}
int main(int argc, char const *argv[])
{
    int a;
    printf("你要算出斐波那契的第几位：");
    int n = scanf("%d",&a);
    printf("%d \n", fibonacci(a));
    return 0;
}
