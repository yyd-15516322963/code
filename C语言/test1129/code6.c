// 题目 1：成绩等级判断输入一个 0-100 的成绩，用 if-else 判断等级：
// 90-100 为 A，80-89 为 B，70-79 为 C，60-69 为 D，0-59 为 E，输入非法则提示错误。
#include<stdio.h>
int main()
{
    int n;
    printf("请输入成绩：");
    scanf("%d",&n);

    if(n >= 90){
        printf("A\n");
    }else if(n >= 80)
    {
        printf("B\n");
    }else if(n >= 70)
    {
        printf("C\n");
    }else if(n >= 60)
    {
        printf("D\n");
    }else
    {
        printf("E\n");
    }
}