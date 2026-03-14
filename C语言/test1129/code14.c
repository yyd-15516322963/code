// 题目 4：存款利息计算器
// 需求：输入本金、年利率、存款年数，计算复利（本息和 = 本金 ×(1 + 年利率)^ 年数），
// 保留两位小数输出。考点：浮点型、循环（计算幂次）、算术运算符。
#include <stdio.h>
int main()
{
    float a, b;
    int c;
    float sum_year = 1;
    printf("输入本金、年利率、存款年数:");
    scanf("%f %f %d", &a, &b, &c);

    float one_year = (1 + b);
    float sum = 0;
    for (int i = 0; i <c; i++)
    {
        sum_year *= one_year;

    }
    sum = a * sum_year;
    printf("%.2f\n", sum);
}