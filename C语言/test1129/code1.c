// 题目 1：简易学生成绩管理
// 需求：定义整型数组存储 5 名学生的成绩，计算平均分、最高分、最低分，统计及格（≥60）人数。
// 考点：数组（数据类型）、循环、算术运算符、关系运算符。
#include <stdio.h>
int main()
{
    int stu[5] = {59, 69, 79, 88, 96};

    int max = stu[0];
    int min = stu[0];
    int sum = 0;
    int num = 0;
    int n = 0;


    for (int i = 0; i < 5; i++)
    {
        sum += stu[i];

        if (stu[i] > max)
        {
            max = stu[i];
        }
        if (stu[i] < min)
        {
            min = stu[i];
        }
        if (stu[i] >= 60)
        {
            num++;
        }
       n++;
        
    }
        float ave = (float)sum / n;

    printf("最高分=%d\n", max);
    printf("最低分=%d\n", min);
    printf("总分=%d\n", sum);
    printf("平均分=%.2f\n", ave);
    printf("及格人数=%d\n", num);

    return 0;
}