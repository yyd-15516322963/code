#include <stdio.h>
int main()
{
    int a, b;
    printf("请输入两个整数 \n");
    int n = scanf("%d %d", &a, &b);

    while (n != 2)
    {
        char c[50];
        fgets(c, 50, stdin);
        printf("请重新输入两个整数 \n");
        n = scanf("%d %d", &a, &b);
    }
    return 0;
}