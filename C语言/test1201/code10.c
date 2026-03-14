// 将一个正整数分解质因数。例如：输入 90,打印出 90=2*3*3*5。
#include<stdio.h>
int main()
{
    int n,i;
    scanf("%d",&n);
    printf("%d=",n);
    for(i = 2;i<=n;i++)
    {
        while(n % i == 0)
        {
            printf("%d",i);
            n /= i;
            if(n != 1)
            {
                printf("*");
            }
            
        }
    }
    printf("\n");
}