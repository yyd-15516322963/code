#include <stdio.h>

int main(int argc, char const *argv[])
{

    for(int i = 2;i <= 100;i++)
    {
        int n = 1;
        for(int j = 2;j<i;j++)
        {
            if(i % j == 0)
            {
                n = 0;
                break;
            }
        }
        if(n == 1)
        {
            printf("%d ",i);

        }
    }
    printf("\n");
    return 0;
}
