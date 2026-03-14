// 输入两个正整数m和n，求其最大公约数和最小公倍数。
#include<stdio.h>
int main()
{
    int a,b;
    scanf("%d %d",&a,&b);
    
    int num_gy;
    for(int i = 1;i<=a;i++)
    {
        int x = a / i;
        for(int j = 1;j<=b;j++)
        {
            int y = b / j;
            if(x == y)
            {
                printf("%d ",x);
                break;
            }
            
        } 

    } 
    
}