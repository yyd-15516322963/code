#include <stdio.h>

int main()
{
    int age;
    int gender;
    printf("请输入你的年龄：\n");
    scanf("%d", &age);
    printf("请输入你的性别（男1，女0）：\n");
    scanf("%d", &gender);
    if (age >= 18)
    {
        if ((age >= 20 && gender == 0) || (age >= 22 && gender == 1))
        {
            printf("可以结婚\n");
        }
        else
        {
            printf("不能结婚\n");
        }
        printf("成年\n");
    }
    else
    {
        printf("未成年\n");
    }

    return 0;
}