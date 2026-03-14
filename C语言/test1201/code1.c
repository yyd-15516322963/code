// 题目 9：位运算之按位异或
// 输入两个整数，计算并输出它们的按位异或（^）结果，利用异或实现两个变量值的交换（不使用临时变量）。
#include<stdio.h>
int main()
{
    int a = 2;
    int b = 3;
    int c = a ^ b;
    printf("a ^ b = %d\n",c);

    printf("a,b 换位：a = %d\n",a ^ c);
    printf("a,b 换位：b = %d\n",b ^ c);

}