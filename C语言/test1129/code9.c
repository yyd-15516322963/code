// 题目 7：1 到 100 的奇数和 / 偶数和用循环计算 1 到 100 的奇数和与偶数和，分别输出。

#include<stdio.h>
int main()
{
    int sum_o = 0;
    int sum_j = 0;
    for(int i = 1;i <=100;i++)
    {
        if(i % 2 == 0)
        {
            sum_o += i;
        }
        if(i % 2 != 0)
        {
            sum_j += i;
        }
    }
    printf("偶数和为%d\n",sum_o);
    printf("偶数和为%d\n",sum_j);
}