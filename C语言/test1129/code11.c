// 题目 10：九九乘法表用双层 for 循环输出九九乘法表（正三角 / 倒三角形式）。
#include<stdio.h>
int main()
{
    for(int i =1;i<=9;i++)
    {
        for(int j = 1;j<=i;j++)
        {
            printf("%d*%d=%d\t",j,i,j*i);
        }
        printf("\n");
    }
    //倒三角
    for(int i =9;i>0;i--)
    {
        for(int j = i;j>0;j--)
        {
            printf("%d*%d=%d\t",j,i,j*i);
        }
        printf("\n");
    }
}