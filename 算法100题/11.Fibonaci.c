#include <stdio.h>

int Fibonaci(int n)
{
    if (n >= 1)
    {

        if (n == 1 || n == 2)
        {
            return 1;
        }
        else
            return Fibonaci(n - 1) + Fibonaci(n - 2);
    }
    else
        return 0;
}

int main(int argc, char const *argv[])
{
    int sum = 10;
    for(int i = 1;i <= sum;i ++)
    {
        printf("%d   ", Fibonaci(i));
    }
    printf("\n");
    
    return 0;
}
