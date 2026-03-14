//结构体
#include<stdio.h>

struct student_info
{
    char name[10];
    int age;
    float score;
};

int main()
{
    struct student_info stu1;
    scanf("%s %d %f",stu1.name,&stu1.age,&stu1.score);

    printf("name=%s,age=%d,score=%.1f\n",stu1.name,stu1.age,stu1.score);

}