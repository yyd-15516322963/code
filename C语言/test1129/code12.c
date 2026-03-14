// 题目 12：水仙花数查找用循环找出 100-999 之间的所有水仙花数
// （各位数字的立方和等于自身，如 153=1³+5³+3³）。
#include<stdio.h>
int main()
{
    int a,b,c;
    for(int i = 100;i<=999;i++)
    {
        a = i / 100;
        b = i / 10 % 10;
        c = i % 10;
        if(i == a*a*a + b*b*b + c*c*c)
        {
            printf("%d ",i);
        }
    }
    printf("\n");
    return 0;
}