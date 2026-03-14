// 题目 15：浮点型的关系运算陷阱
// 定义float a=0.1，double b=0.1，判断a==b的结果，分析浮点型精度导致的关系运算问题及解决方案。
#include<stdio.h>
int main()
{
    float a=0.1;double b=0.1;
    if(a == b)
    {
        printf("相等\n");
    }else
    printf("不相等\n");

}