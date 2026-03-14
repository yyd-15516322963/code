// 题目 9：斐波那契数列用循环输出前 20 项斐波那契数列（1,1,2,3,5,8...），
// 数列规则：第 n 项 = 第 n-1 项 + 第 n-2 项。
#include <stdio.h>
int main()
{
    int fbnq[20];
    fbnq[0] = 1;
    fbnq[1] = 1;
    printf("%d %d ", fbnq[0], fbnq[1]);
    for (int i = 2; i < 20; i++)
    {
        fbnq[i] = fbnq[i - 1] + fbnq[i - 2];
        printf("%d ", fbnq[i]);
    }
    printf("\n");
}