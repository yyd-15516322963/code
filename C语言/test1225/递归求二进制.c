#include <stdio.h>

int erjinzhi(int i)
{
    if (i / 2 != 0)
    {
        erjinzhi(i / 2);
    }

    printf("%d ", i % 2);
}
int main(int argc, char const *argv[])
{
    int a;
    printf("你要算转化的十进制数：");
    int n = scanf("%d", &a);

    erjinzhi(a);
    
    printf("\n");
    return 0;
}
