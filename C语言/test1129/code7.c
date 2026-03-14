// 题目 4：三角形类型判断输入三个边长，
// 先判断能否构成三角形（任意两边之和大于第三边），再判断是等边、等腰还是普通三角形。
#include <stdio.h>
int main()
{
    float a, b, c;
    printf("输入长宽高：");
    scanf("%f %f %f", &a, &b, &c);

    if (a + b <= c || a + c <= b || b + c <= a)
    {
        printf("不是三角形，重新输入！\n");
    }else
        {
        if (a == b == c)
        {
            printf("等边三角形\n");
        }
        else if (a == b || b == c || a == c)
        {
            printf("等腰三角形\n");
        }else
            printf("普通三角形\n");
    }
}