// 题目 13：取地址与解引用运算符
// 定义int a=10，int *p=&a，通过指针p修改a的值为 20，输出a和*p的值（入门级指针，结合取地址运算符）。
#include<stdio.h>
int main()
{
    int a=10;int *p=&a;
    *p = 20;
    printf("%d %d\n",*p,a);

}