// 跳转语句（break/continue/goto）
// 题目 13：break 跳出循环用 for 循环遍历 1-20，当遇到能被 7 整除的数时，跳出循环并输出该数。

// 题目 14：continue 跳过循环用 for 循环遍历 1-10，跳过偶数，输出所有奇数。
#include<stdio.h>
int main()
{
    for(int i = 1;i<= 20;i++)
    {
        // if( i % 7 == 0)
        // {
        //     printf("%d\n",i);
        //     continue;
        // }

        if( i % 2 == 0)
        {
            continue;
        }
        printf("%d\n",i);
    }
}